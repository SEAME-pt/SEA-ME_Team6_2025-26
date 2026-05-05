#!/usr/bin/env python3
"""
SEAME — Dual Inference Pipeline
  • UFLDv2 (TuSimple)  — Lane Detection  — Hailo-8 PCIe
  • YOLOv8s            — Object Detection — Hailo-8 PCIe

Cada modelo corre na sua própria thread/VDevice.
O vídeo final sobrepõe os dois resultados no mesmo frame.
"""

import numpy as np
import cv2
import threading
import queue
import time
import sys

from hailo_platform import (HEF, VDevice, HailoStreamInterface,
                             InferVStreams, ConfigureParams,
                             InputVStreamParams, OutputVStreamParams,
                             FormatType)
import grpc
from kuksa.val.v2 import val_pb2, val_pb2_grpc, types_pb2


# ══════════════════════════════════════════════════════════════════════════════
# Configuração geral
# ══════════════════════════════════════════════════════════════════════════════
OUTPUT_PATH = "/data/demo_dual.mp4"

FULL_W  = 820
FULL_H  = 616
CAM_FPS = 10

# ── UFLDv2 ────────────────────────────────────────────────────────────────────
LANE_HEF_PATH  = "/data/tusimple_res34_finetune_seame.hef"
TRAIN_WIDTH    = 800
TRAIN_HEIGHT   = 320
CROP_RATIO     = 0.8

NUM_ROW      = 56
NUM_COL      = 41
NUM_LANES    = 4
GRIDING_NUM  = 100
NUM_CELL_ROW = GRIDING_NUM
NUM_CELL_COL = GRIDING_NUM

ROW_ANCHOR = np.linspace(160, 710, NUM_ROW) / 720.0
COL_ANCHOR = np.linspace(0, 1, NUM_COL)

LANE_MODEL_NAME = "tusimple_res34_finetune_seame"
LANE_INPUT      = f"{LANE_MODEL_NAME}/input_layer1"
OUT_SLICE1      = f"{LANE_MODEL_NAME}/slice1"
OUT_SLICE2      = f"{LANE_MODEL_NAME}/slice2"
OUT_SLICE3      = f"{LANE_MODEL_NAME}/slice3"
OUT_SLICE4      = f"{LANE_MODEL_NAME}/slice4"

ROW_LANE_LIST    = [1, 2]
COL_LANE_LIST    = [0, 3]
EXIST_THRESHOLD  = 0.7
LOCAL_WIDTH      = 14
MIN_LANE_PTS     = 3

LANE_COLORS = [
    (0,   0,   0),   # lane 0 — não usada
    (0,   255, 0),   # lane 1 — ego esquerda (verde)
    (0,   0,   255), # lane 2 — ego direita (vermelho)
    (0,   0,   0),   # lane 3 — não usada
]

# ── YOLOv8s ───────────────────────────────────────────────────────────────────
YOLO_HEF_PATH = "/data/yolov8s.hef"
MODEL_W       = 640
MODEL_H       = 640
YOLO_INPUT    = "yolov8s/input_layer1"
YOLO_OUTPUT   = "yolov8s/yolov8_nms_postprocess"
CONF_THRESH   = 0.25

BOX_COLOR  = (0, 255, 255)   # amarelo-ciano para não colidir com verde das lanes
TEXT_COLOR = (0, 255, 255)

# ── KUKSA ─────────────────────────────────────────────────────────────────────
KUKSA_HOST         = "10.21.220.191"
KUKSA_PORT         = 55555
KUKSA_CA_CERT      = "/etc/kuksa/tls/ca.crt"
KUKSA_TOKEN        = "/etc/kuksa/jwt/publisher.jwt"
LKA_DEVIATION_PATH = "Vehicle.ADAS.LaneKeepAssist.LateralDeviation"
LKA_STATUS_PATH    = "Vehicle.ADAS.LaneKeepAssist.LaneStatus"

_kuksa_queue = queue.Queue(maxsize=1)


# ══════════════════════════════════════════════════════════════════════════════
# KUKSA worker
# ══════════════════════════════════════════════════════════════════════════════
def _kuksa_worker(q):
    token    = open(KUKSA_TOKEN).read().strip()
    ca_certs = open(KUKSA_CA_CERT, "rb").read()
    creds    = grpc.ssl_channel_credentials(root_certificates=ca_certs)
    channel  = grpc.secure_channel(f"{KUKSA_HOST}:{KUKSA_PORT}", creds)
    stub     = val_pb2_grpc.VALStub(channel)
    metadata = [("authorization", f"Bearer {token}")]
    print("[KUKSA] Thread iniciada (gRPC v2)")
    while True:
        deviation, status = q.get()
        try:
            req_dev = val_pb2.PublishValueRequest()
            req_dev.signal_id.path = LKA_DEVIATION_PATH
            req_dev.data_point.value.float = float(deviation)
            stub.PublishValue(req_dev, metadata=metadata)
            req_st = val_pb2.PublishValueRequest()
            req_st.signal_id.path = LKA_STATUS_PATH
            req_st.data_point.value.string = status
            stub.PublishValue(req_st, metadata=metadata)
        except Exception as e:
            print(f"[KUKSA] Erro: {e}")


def publish_deviation(deviation, status):
    try:
        _kuksa_queue.put_nowait((deviation if deviation is not None else 0.0, status))
    except queue.Full:
        pass


# ══════════════════════════════════════════════════════════════════════════════
# Writer assíncrono
# ══════════════════════════════════════════════════════════════════════════════
class AsyncVideoWriter:
    def __init__(self, path, fps, width, height):
        self._queue  = queue.Queue(maxsize=60)
        self._writer = cv2.VideoWriter(
            path, cv2.VideoWriter_fourcc(*"mp4v"), fps, (width, height))
        if not self._writer.isOpened():
            raise RuntimeError(f"Não foi possível criar: {path}")
        self._stop   = threading.Event()
        self._thread = threading.Thread(target=self._loop, daemon=True)
        self._thread.start()

    def _loop(self):
        while not self._stop.is_set() or not self._queue.empty():
            try:
                frame = self._queue.get(timeout=0.1)
                self._writer.write(frame)
            except Exception:
                continue

    def write(self, frame):
        try:
            self._queue.put_nowait(frame)
        except queue.Full:
            pass

    def release(self):
        self._stop.set()
        self._thread.join(timeout=10)
        self._writer.release()


# ══════════════════════════════════════════════════════════════════════════════
# Pré-processamento
# ══════════════════════════════════════════════════════════════════════════════
def preprocess_lane(frame):
    """BGR FULL_W×FULL_H → UINT8 (320, 800, 3) — crop inferior 80%."""
    resize_h = int(TRAIN_HEIGHT / CROP_RATIO)   # 400
    img = cv2.resize(frame, (TRAIN_WIDTH, resize_h))
    top = resize_h - TRAIN_HEIGHT                # 80
    img = img[top:, :]
    return cv2.cvtColor(img, cv2.COLOR_BGR2RGB).astype(np.uint8)


def preprocess_yolo(frame):
    """BGR FULL_W×FULL_H → UINT8 (640, 640, 3) — letterbox-free resize."""
    img = cv2.resize(frame, (MODEL_W, MODEL_H))
    return cv2.cvtColor(img, cv2.COLOR_BGR2RGB).astype(np.uint8)


# ══════════════════════════════════════════════════════════════════════════════
# Post-processing — UFLDv2
# ══════════════════════════════════════════════════════════════════════════════
def _softmax_local(arr, max_idx, local_width):
    lo     = max(0, max_idx - local_width)
    hi     = min(len(arr) - 1, max_idx + local_width)
    ind    = np.arange(lo, hi + 1)
    scores = arr[ind]
    scores = np.exp(scores - scores.max())
    scores /= scores.sum()
    return float((scores * ind).sum()) + 0.5


def decode_lanes(slice1, slice2, slice3, slice4):
    loc_row   = slice1.astype(np.float32).reshape(NUM_CELL_ROW, NUM_ROW,  NUM_LANES)
    loc_col   = slice2.astype(np.float32).reshape(NUM_CELL_COL, NUM_COL,  NUM_LANES)
    exist_row = slice3.astype(np.float32).reshape(2, NUM_ROW, NUM_LANES)
    exist_col = slice4.astype(np.float32).reshape(2, NUM_COL, NUM_LANES)

    def exist_scores(exist):
        exp_e = np.exp(exist - exist.max(axis=0, keepdims=True))
        return exp_e / exp_e.sum(axis=0, keepdims=True)

    row_score = exist_scores(exist_row)
    col_score = exist_scores(exist_col)
    lanes = [[] for _ in range(NUM_LANES)]

    # ── Row-based ego-lanes apenas (1 = esquerda, 2 = direita) ───────────────
    for lane_idx in ROW_LANE_LIST:
        pts = []
        for row_idx in range(NUM_ROW):
            if row_score[1, row_idx, lane_idx] < EXIST_THRESHOLD:
                continue
            max_idx = int(loc_row[:, row_idx, lane_idx].argmax())
            out_x   = _softmax_local(loc_row[:, row_idx, lane_idx], max_idx, LOCAL_WIDTH)
            x_full  = int(out_x / (NUM_CELL_ROW - 1) * FULL_W)
            y_full  = int(ROW_ANCHOR[row_idx] * FULL_H)
            pts.append((x_full, y_full))
        lanes[lane_idx] = pts

    # Lanes 0 e 3 (col-based) não são calculadas
    return lanes


def calc_lateral_deviation(lanes):
    frame_center = FULL_W / 2.0

    def bottom_mean_x(pts, n=5):
        if len(pts) < MIN_LANE_PTS:
            return None
        sorted_pts = sorted(pts, key=lambda p: p[1], reverse=True)
        return float(np.mean([p[0] for p in sorted_pts[:n]]))

    # Apenas ego-lanes 1 (esquerda) e 2 (direita) — sem fallback para exteriores
    x_left  = bottom_mean_x(lanes[1])
    x_right = bottom_mean_x(lanes[2])

    has_left  = x_left  is not None
    has_right = x_right is not None

    if not has_left and not has_right:
        return None, "none"
    if has_left and has_right:
        lane_width = x_right - x_left
        if lane_width < 50:
            return None, "none"
        deviation = ((x_left + x_right) / 2.0 - frame_center) / (lane_width / 2.0)
        return float(np.clip(deviation, -1.0, 1.0)), "both"
    if has_left:
        return float(np.clip(-(frame_center - x_left) / frame_center, -1.0, 1.0)), "left"
    return float(np.clip((x_right - frame_center) / frame_center, -1.0, 1.0)), "right"


def smooth_lane(pts, window=5):
    if len(pts) < window:
        return pts
    xs = np.array([p[0] for p in pts], dtype=np.float32)
    ys = np.array([p[1] for p in pts], dtype=np.float32)
    kernel   = np.ones(window) / window
    xs_smooth = np.convolve(xs, kernel, mode='same')
    trim = window // 2
    return [(int(x), int(y)) for x, y in zip(xs_smooth[trim:-trim], ys[trim:-trim])]


def smooth_lanes(lanes):
    return [smooth_lane(lane) for lane in lanes]


# ══════════════════════════════════════════════════════════════════════════════
# Post-processing — YOLOv8s
# ══════════════════════════════════════════════════════════════════════════════
def yolo_postprocess(hailo_output, conf_thresh=CONF_THRESH):
    all_dets = []
    for cls_id, dets in enumerate(hailo_output):
        if dets is None or len(dets) == 0:
            continue
        dets = np.array(dets, dtype=np.float32)
        cls_col = np.full((dets.shape[0], 1), cls_id, dtype=np.float32)
        dets = np.concatenate([dets, cls_col], axis=1)
        all_dets.append(dets)

    if not all_dets:
        return np.empty((0, 4)), np.empty((0,)), np.empty((0,), dtype=int)

    out     = np.vstack(all_dets)
    out     = out[out[:, 4] >= conf_thresh]
    boxes   = out[:, 0:4]
    scores  = out[:, 4]
    classes = out[:, 5].astype(int)
    return boxes, scores, classes


def scale_boxes(boxes):
    if boxes.size == 0:
        return boxes
    b = boxes.copy()
    b[:, 0] *= FULL_W / MODEL_W;  b[:, 2] *= FULL_W / MODEL_W
    b[:, 1] *= FULL_H / MODEL_H;  b[:, 3] *= FULL_H / MODEL_H
    return b.astype(int)


# ══════════════════════════════════════════════════════════════════════════════
# Visualização combinada
# ══════════════════════════════════════════════════════════════════════════════
def draw_lanes(frame, lanes):
    for lane_idx in ROW_LANE_LIST:   # só lanes 1 e 2
        lane = lanes[lane_idx]
        if not lane:
            continue
        color = LANE_COLORS[lane_idx]
        for j in range(len(lane) - 1):
            cv2.line(frame, lane[j], lane[j + 1], color, 3)
        for pt in lane:
            cv2.circle(frame, pt, 4, color, -1)
    return frame


def draw_boxes(frame, boxes, scores, classes):
    for box, score, cls in zip(boxes, scores, classes):
        x1, y1, x2, y2 = box
        cv2.rectangle(frame, (x1, y1), (x2, y2), BOX_COLOR, 2)
        label = f"{cls}:{score:.2f}"
        cv2.putText(frame, label, (x1, max(0, y1 - 5)),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.4, TEXT_COLOR, 1)
    return frame


def draw_overlay(frame, fps, frame_idx,
                 t_lane_pre, t_lane_hailo, t_lane_post,
                 t_yolo_pre, t_yolo_hailo, t_yolo_post,
                 num_lanes, num_dets, deviation, status):
    h, w = frame.shape[:2]
    overlay = frame.copy()
    cv2.rectangle(overlay, (0, 0), (230, 200), (0, 0, 0), -1)
    cv2.addWeighted(overlay, 0.5, frame, 0.5, 0, frame)

    dev_str   = f"{deviation:+.3f}" if deviation is not None else "N/A"
    dev_color = (0, 255, 0) if status == "both" else (0, 165, 255)

    lines = [
        (f"FPS:           {fps:.1f}",                    (0, 255, 0)),
        ("── Lane ─────────────────",                    (180, 180, 180)),
        (f"  Pre:         {t_lane_pre:.1f}ms",           (200, 200, 200)),
        (f"  Hailo:       {t_lane_hailo:.1f}ms",         (200, 200, 200)),
        (f"  Post:        {t_lane_post:.1f}ms",          (200, 200, 200)),
        (f"  Lanes:       {num_lanes}",                  (255, 255, 255)),
        (f"  Dev:         {dev_str} [{status}]",         dev_color),
        ("── YOLO ─────────────────",                    (180, 180, 180)),
        (f"  Pre:         {t_yolo_pre:.1f}ms",           (200, 200, 200)),
        (f"  Hailo:       {t_yolo_hailo:.1f}ms",         (200, 200, 200)),
        (f"  Post:        {t_yolo_post:.1f}ms",          (200, 200, 200)),
        (f"  Detec.:      {num_dets}",                   (255, 255, 255)),
        (f"Frame: {frame_idx}",                          (255, 255, 255)),
    ]
    for i, (text, color) in enumerate(lines):
        cv2.putText(frame, text, (5, 16 + i * 14),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.33, color, 1)

    cv2.putText(frame, "SEAME | Dual: UFLDv2 + YOLOv8s",
                (w - 230, h - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (200, 200, 200), 1)
    return frame


# ══════════════════════════════════════════════════════════════════════════════
# Inferência dual — único VDevice, dois network groups, inferência sequencial
#
# O Hailo-8 é um único dispositivo físico: só pode existir um VDevice de cada
# vez. Os dois HEFs são carregados e configurados dentro do mesmo VDevice;
# cada um é ativado, inferido e desativado sequencialmente por frame.
# ══════════════════════════════════════════════════════════════════════════════

def run_inference(frame_queue, duration_seconds=60, save_video=False):
    """
    Loop de inferência principal. Corre numa única thread com um único VDevice.
    Sequência por frame:
      1. Pré-processa para lane + yolo
      2. Ativa lane_ng → infer → desativa
      3. Ativa yolo_ng → infer → desativa
      4. Post-processa ambos
      5. Coloca frame anotado no compositor_queue para gravação
    """
    threading.Thread(target=_kuksa_worker, args=(_kuksa_queue,), daemon=True).start()

    async_writer = None
    if save_video:
        async_writer = AsyncVideoWriter(OUTPUT_PATH, CAM_FPS, FULL_W, FULL_H)
        print(f"🎥 A gravar em {OUTPUT_PATH}")

    hef_lane = HEF(LANE_HEF_PATH)
    hef_yolo = HEF(YOLO_HEF_PATH)

    with VDevice() as target:
        # Configura os dois modelos no mesmo VDevice
        cfg_lane = ConfigureParams.create_from_hef(hef_lane, interface=HailoStreamInterface.PCIe)
        cfg_yolo = ConfigureParams.create_from_hef(hef_yolo, interface=HailoStreamInterface.PCIe)

        lane_ng = target.configure(hef_lane, cfg_lane)[0]
        yolo_ng = target.configure(hef_yolo, cfg_yolo)[0]

        lane_inp  = InputVStreamParams.make(lane_ng, format_type=FormatType.UINT8)
        lane_outp = OutputVStreamParams.make(lane_ng, format_type=FormatType.UINT8)
        yolo_inp  = InputVStreamParams.make(yolo_ng, format_type=FormatType.UINT8)
        yolo_outp = OutputVStreamParams.make(yolo_ng, format_type=FormatType.FLOAT32)

        print(f"\nDual inference a correr durante {duration_seconds}s — Ctrl+C para parar\n")
        print(f"{'Frame':<7} {'LPre':>6} {'LHailo':>7} {'LPost':>7} "
              f"{'YPre':>6} {'YHailo':>7} {'YPost':>7} "
              f"{'Total':>7} {'Lanes':>6} {'Dets':>5} {'Dev':>8}")
        print("─" * 95)

        frame_idx = 0
        t_start   = time.time()
        times_all = []
        fps_acc   = []

        try:
            while (time.time() - t_start) < duration_seconds:
                try:
                    frame = frame_queue.get(timeout=0.1)
                except queue.Empty:
                    continue

                # ── Pré-processamento (CPU, paralelo ao Hailo) ─────────────
                t0        = time.time()
                img_lane  = preprocess_lane(frame)
                t_lane_pre = (time.time() - t0) * 1000

                t0        = time.time()
                img_yolo  = preprocess_yolo(frame)
                t_yolo_pre = (time.time() - t0) * 1000

                # ── Inferência Lane (UFLDv2) ───────────────────────────────
                t0 = time.time()
                with lane_ng.activate():
                    with InferVStreams(lane_ng, lane_inp, lane_outp) as pipe:
                        lane_out = pipe.infer({LANE_INPUT: img_lane[np.newaxis]})
                t_lane_hailo = (time.time() - t0) * 1000

                # ── Inferência YOLO ────────────────────────────────────────
                t0 = time.time()
                with yolo_ng.activate():
                    with InferVStreams(yolo_ng, yolo_inp, yolo_outp) as pipe:
                        yolo_out = pipe.infer({YOLO_INPUT: img_yolo[np.newaxis]})
                t_yolo_hailo = (time.time() - t0) * 1000

                # ── Post-processing Lane ───────────────────────────────────
                t0    = time.time()
                lanes = decode_lanes(
                    lane_out[OUT_SLICE1][0], lane_out[OUT_SLICE2][0],
                    lane_out[OUT_SLICE3][0], lane_out[OUT_SLICE4][0])
                lanes             = smooth_lanes(lanes)
                deviation, status = calc_lateral_deviation(lanes)
                t_lane_post       = (time.time() - t0) * 1000

                publish_deviation(deviation, status)

                # ── Post-processing YOLO ───────────────────────────────────
                t0                     = time.time()
                boxes, scores, classes = yolo_postprocess(yolo_out[YOLO_OUTPUT][0])
                boxes                  = scale_boxes(boxes)
                t_yolo_post            = (time.time() - t0) * 1000

                # ── Métricas ───────────────────────────────────────────────
                t_total   = (t_lane_pre + t_lane_hailo + t_lane_post +
                             t_yolo_hailo + t_yolo_post)
                num_lanes = sum(1 for l in lanes if l)
                num_dets  = len(boxes)
                times_all.append(t_total)

                fps_acc.append(1000.0 / (t_total + 1e-9))
                if len(fps_acc) > 30:
                    fps_acc.pop(0)
                fps = sum(fps_acc) / len(fps_acc)

                dev_str = f"{deviation:+.3f}" if deviation is not None else "   N/A"
                print(f"{frame_idx:<7} "
                      f"{t_lane_pre:>5.1f}ms "
                      f"{t_lane_hailo:>6.1f}ms "
                      f"{t_lane_post:>6.1f}ms "
                      f"{t_yolo_pre:>5.1f}ms "
                      f"{t_yolo_hailo:>6.1f}ms "
                      f"{t_yolo_post:>6.1f}ms "
                      f"{t_total:>6.1f}ms "
                      f"{num_lanes:>6} "
                      f"{num_dets:>5} "
                      f"{dev_str:>8}")

                if save_video and async_writer is not None:
                    frame_out = frame.copy()
                    frame_out = draw_lanes(frame_out, lanes)
                    frame_out = draw_boxes(frame_out, boxes, scores, classes)
                    frame_out = draw_overlay(
                        frame_out, fps, frame_idx,
                        t_lane_pre, t_lane_hailo, t_lane_post,
                        t_yolo_pre, t_yolo_hailo, t_yolo_post,
                        num_lanes, num_dets, deviation, status)
                    async_writer.write(frame_out)

                frame_idx += 1

        except KeyboardInterrupt:
            print("\nInterrompido.")

    if async_writer is not None:
        print("\nA aguardar gravação do vídeo...")
        async_writer.release()
        print(f"✅ Vídeo guardado: {OUTPUT_PATH}")

    elapsed  = time.time() - t_start
    real_fps = frame_idx / elapsed if elapsed > 0 else 0
    avg      = sum(times_all) / len(times_all) if times_all else 0
    sorted_t = sorted(times_all) if times_all else [0]
    p50 = sorted_t[int(len(sorted_t) * 0.50)]
    p95 = sorted_t[int(len(sorted_t) * 0.95)]

    print("─" * 95)
    print(f"\n📊 Resumo ({frame_idx} frames em {elapsed:.1f}s):")
    print(f"   FPS real:              {real_fps:.1f}")
    print(f"   FPS pipeline puro:     {1000/avg:.1f}" if avg > 0 else "")
    print(f"   Latência média total:  {avg:.1f}ms")
    print(f"   P50:                   {p50:.1f}ms")
    print(f"   P95:                   {p95:.1f}ms")
    if save_video:
        print(f"   Vídeo guardado:        {OUTPUT_PATH}")


# ══════════════════════════════════════════════════════════════════════════════
# Entry point
# ══════════════════════════════════════════════════════════════════════════════
if __name__ == "__main__":
    sys.path.insert(0, "/opt/seame/adas")
    from camera_broker import CameraBroker

    duration   = int(sys.argv[1]) if len(sys.argv) > 1 else 60
    save_video = "--save" in sys.argv

    print("SEAME | Dual Inference: UFLDv2 (lane) + YOLOv8s (objects)")
    print(f"Câmara: {FULL_W}×{FULL_H} @ {CAM_FPS}fps")
    print(f"Modo: único VDevice, inferência sequencial por frame\n")

    broker = CameraBroker(width=FULL_W, height=FULL_H, fps=CAM_FPS)
    q = broker.register("dual")
    broker.start()

    run_inference(q, duration_seconds=duration, save_video=save_video)

    broker.stop()
