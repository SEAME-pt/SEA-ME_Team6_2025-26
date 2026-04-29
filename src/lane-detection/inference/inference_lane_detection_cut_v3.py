import numpy as np
import cv2
import threading
import queue
import time
import sys
from collections import deque
from hailo_platform import (HEF, VDevice, HailoStreamInterface,
                             InferVStreams, ConfigureParams,
                             InputVStreamParams, OutputVStreamParams,
                             FormatType)
import grpc
from kuksa.val.v2 import val_pb2, val_pb2_grpc, types_pb2

# ── Configuração ──────────────────────────────────────────────────────────────
HEF_PATH     = "/data/tusimple_res34_cut_v2.hef"
WEIGHTS_PATH = "/data/tusimple_postprocess_weights.npz"
OUTPUT_PATH  = "/data/demo_tusimple_v6_cut.mp4"

SENSOR_W  = 4608
SENSOR_H  = 2592
ROI_RATIO = 0.6

FULL_W  = 820
FULL_H  = 616
CAM_FPS = 10
ROI_Y   = 0
ROI_H   = FULL_H

TRAIN_WIDTH  = 800
TRAIN_HEIGHT = 320
CROP_RATIO   = 0.8

# UFLDv2 TuSimple
NUM_ROW      = 56
NUM_COL      = 41
NUM_LANES    = 4
GRIDING_NUM  = 100
NUM_CELL_ROW = GRIDING_NUM
NUM_CELL_COL = GRIDING_NUM

# Row anchors: y=160→710 normalizado para 720px
ROW_ANCHOR = np.linspace(160, 710, NUM_ROW) / 720.0
# Col anchors: x=0→1 normalizado
COL_ANCHOR = np.linspace(0, 1, NUM_COL)

MODEL_NAME   = "tusimple_res34_cut_v2"
INPUT_STREAM = f"{MODEL_NAME}/input_layer1"
OUTPUT_LAYER = f"{MODEL_NAME}/conv37"

# Dequantização do output Hailo (qp_scale e qp_zp extraídos do HEF)
QUANT_SCALE = 0.271904319524765
QUANT_ZP    = 157.0

# Ego-lanes apenas: lanes 1 (esquerda) e 2 (direita)
ROW_LANE_LIST = [1, 2]
COL_LANE_LIST = [0, 3]

EXIST_THRESHOLD  = 0.5
LOCAL_WIDTH      = 14
MIN_LANE_PTS     = 3

TEMPORAL_HISTORY  = 7
TEMPORAL_MIN_HITS = 4
SPATIAL_WINDOW    = 9

CAMERA_OFFSET_NORM = 0.142

LANE_COLORS = [
    (0,   0,   0),   # lane 0 (col) — não usada
    (0,   255, 0),   # lane 1 (row) — ego esquerda (verde)
    (0,   0,   255), # lane 2 (row) — ego direita (vermelho)
    (0,   0,   0),   # lane 3 (col) — não usada
]

# ── KUKSA ─────────────────────────────────────────────────────────────────────
KUKSA_HOST         = "10.21.220.191"
KUKSA_PORT         = 55555
KUKSA_CA_CERT      = "/etc/kuksa/tls/ca.crt"
KUKSA_TOKEN        = "/etc/kuksa/jwt/publisher.jwt"
LKA_DEVIATION_PATH = "Vehicle.ADAS.LaneKeepAssist.LateralDeviation"
LKA_STATUS_PATH    = "Vehicle.ADAS.LaneKeepAssist.LaneStatus"

_kuksa_queue = queue.Queue(maxsize=1)


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


# ── Pesos de post-processing ──────────────────────────────────────────────────
def load_weights(path):
    w  = np.load(path)
    W1 = w["W1"]  # (2048, 2000)
    b1 = w["b1"]  # (2048,)
    W2 = w["W2"]  # (39576, 2048)
    b2 = w["b2"]  # (39576,)
    print(f"[Weights] W1={W1.shape} b1={b1.shape} W2={W2.shape} b2={b2.shape}")
    return W1, b1, W2, b2


# ── Writer assíncrono ─────────────────────────────────────────────────────────
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


# ── Pré-processamento ─────────────────────────────────────────────────────────
def preprocess(frame):
    """
    BGR FULL_W×FULL_H → UINT8 (320, 800, 3)
    Resize 800×400 → crop inferior 800×320 → RGB
    """
    resize_h = int(TRAIN_HEIGHT / CROP_RATIO)  # 400
    img = cv2.resize(frame, (TRAIN_WIDTH, resize_h))
    top = resize_h - TRAIN_HEIGHT               # 80
    img = img[top:, :]
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    return img.astype(np.uint8)


# ── Post-processing CPU ───────────────────────────────────────────────────────
def postprocess(conv37_uint8, W1, b1, W2, b2):
    """
    conv37_uint8: (10, 25, 8) UINT8 — output do Hailo
    Dequantiza, transpõe para alinhar com ONNX (CHW), e corre FC1→ReLU→FC2.
    Devolve os 4 slices flat prontos para decode_lanes.
    """
    # Dequantização + transpose HWC→CHW para alinhar com ONNX (8,10,25)
    x = (conv37_uint8.astype(np.float32) - QUANT_ZP) * QUANT_SCALE
    x = x.transpose(2, 0, 1).flatten()  # (10,25,8)→(8,10,25)→(2000,)

    # FC1 → ReLU
    x = np.dot(W1, x) + b1   # (2048,)
    x = np.maximum(x, 0)

    # FC2
    x = np.dot(W2, x) + b2   # (39576,)

    # Split nos 4 slices
    # loc_row:   4 × 56 × 100 = 22400
    # loc_col:   4 × 41 × 100 = 16400
    # exist_row: 4 × 56 × 2   = 448
    # exist_col: 4 × 41 × 2   = 328
    loc_row_flat   = x[:22400]
    loc_col_flat   = x[22400:38800]
    exist_row_flat = x[38800:39248]
    exist_col_flat = x[39248:39576]

    return loc_row_flat, loc_col_flat, exist_row_flat, exist_col_flat


# ── Post-processing ───────────────────────────────────────────────────────────
def _softmax_local(arr, max_idx, local_width):
    """Softmax local em torno do argmax."""
    lo     = max(0, max_idx - local_width)
    hi     = min(len(arr) - 1, max_idx + local_width)
    ind    = np.arange(lo, hi + 1)
    scores = arr[ind]
    scores = np.exp(scores - scores.max())
    scores /= scores.sum()
    return float((scores * ind).sum()) + 0.5


def decode_lanes(loc_row_flat, loc_col_flat, exist_row_flat, exist_col_flat):
    """
    Decodifica os 4 slices do FC2 para coordenadas (x, y) no frame original.
    Lanes 1, 2 → row-based.
    Retorna lista de 4 lanes (índices 0-3), cada uma lista de (x, y).
    """
    loc_row   = loc_row_flat.reshape(NUM_CELL_ROW, NUM_ROW,  NUM_LANES)
    loc_col   = loc_col_flat.reshape(NUM_CELL_COL, NUM_COL,  NUM_LANES)
    exist_row = exist_row_flat.reshape(2, NUM_ROW, NUM_LANES)
    exist_col = exist_col_flat.reshape(2, NUM_COL, NUM_LANES)

    def exist_scores(exist):
        exp_e = np.exp(exist - exist.max(axis=0, keepdims=True))
        return exp_e / exp_e.sum(axis=0, keepdims=True)

    row_score = exist_scores(exist_row)  # (2, NUM_ROW, NUM_LANES)
    col_score = exist_scores(exist_col)  # (2, NUM_COL, NUM_LANES)

    lanes = [[] for _ in range(NUM_LANES)]

    # ── Row-based lanes (1, 2) ─────────────────────────────────────────────
    for lane_idx in ROW_LANE_LIST:
        pts = []
        for row_idx in range(NUM_ROW):
            threshold = EXIST_THRESHOLD if row_idx > 20 else 0.8
            if row_score[1, row_idx, lane_idx] < threshold:
                continue
            max_idx = int(loc_row[:, row_idx, lane_idx].argmax())
            out_x   = _softmax_local(loc_row[:, row_idx, lane_idx],
                                     max_idx, LOCAL_WIDTH)
            x_full  = int(out_x / (NUM_CELL_ROW - 1) * FULL_W)
            y_full  = int(ROW_ANCHOR[row_idx] * FULL_H)
            pts.append((x_full, y_full))
        lanes[lane_idx] = pts

    # Lanes 0 e 3 (col-based) não são calculadas — só as ego-lanes 1 e 2 são usadas

    return lanes


# ── Desvio lateral ────────────────────────────────────────────────────────────
def calc_lateral_deviation(lanes):
    """
    Usa as lanes 1 (esquerda) e 2 (direita) como ego-lanes primárias.
    """
    frame_center = FULL_W / 2.0

    def bottom_mean_x(pts, n=10): #aumentei de 5 para 10 = mais pontos para o calculo
        if len(pts) < MIN_LANE_PTS:
            return None
        sorted_pts = sorted(pts, key=lambda p: p[1], reverse=True)
        return float(np.mean([p[0] for p in sorted_pts[:n]]))

    x_left  = bottom_mean_x(lanes[1])
    x_right = bottom_mean_x(lanes[2])

    has_left  = x_left  is not None
    has_right = x_right is not None

    if has_left and has_right:
        lane_width = x_right - x_left
        print(f"lane_width={lane_width:.0f}px  x_left={x_left:.0f}  x_right={x_right:.0f}")

    if not has_left and not has_right:
        return None, "none"

    if has_left and has_right:
        lane_width = x_right - x_left
        if lane_width < 50:
            return None, "none"
        lane_center = (x_left + x_right) / 2.0
        deviation   = (lane_center - frame_center) / (lane_width / 2.0)
        #return float(np.clip(deviation, -1.0, 1.0)), "both"
        return float(np.clip(deviation - CAMERA_OFFSET_NORM, -1.0, 1.0)), "both"

    if has_left:
        deviation = -(frame_center - x_left) / frame_center
        return float(np.clip(deviation, -1.0, 1.0)), "left"

    deviation = (x_right - frame_center) / frame_center
    return float(np.clip(deviation, -1.0, 1.0)), "right"


def publish_deviation(deviation, status):
    try:
        _kuksa_queue.put_nowait((deviation if deviation is not None else 0.0, status))
    except queue.Full:
        pass


# ── Visualização ──────────────────────────────────────────────────────────────
def draw_lanes(frame, lanes):
    MAX_Y_GAP = 40  # só liga pontos com diferença de y < 40px (evita linhas entre gaps)
    for lane_idx in ROW_LANE_LIST:
        lane = lanes[lane_idx]
        if not lane:
            continue
        color = LANE_COLORS[lane_idx]
        for j in range(len(lane) - 1):
            if abs(lane[j+1][1] - lane[j][1]) < MAX_Y_GAP:
                cv2.line(frame, lane[j], lane[j+1], color, 3)
        for pt in lane:
            cv2.circle(frame, pt, 4, color, -1)
    return frame


class TemporalLaneSmoother:
    """
    Suavização temporal: mantém histórico de x por lane/row_idx.
    Um ponto é emitido se aparecer em >= min_hits dos últimos `history` frames.
    O x emitido é a média dos valores válidos no histórico.
    """
    def __init__(self, history=TEMPORAL_HISTORY, min_hits=TEMPORAL_MIN_HITS):
        self.history  = history
        self.min_hits = min_hits
        self._hist = [[deque(maxlen=history) for _ in range(NUM_ROW)]
                      for _ in range(NUM_LANES)]
        self._y = [int(ROW_ANCHOR[r] * FULL_H) for r in range(NUM_ROW)]

    def update(self, lanes):
        smoothed = [[] for _ in range(NUM_LANES)]
        for lane_idx in ROW_LANE_LIST:
            y_to_x = {y: x for (x, y) in lanes[lane_idx]}
            for row_idx in range(NUM_ROW):
                self._hist[lane_idx][row_idx].append(y_to_x.get(self._y[row_idx]))
            pts = []
            for row_idx in range(NUM_ROW):
                valid = [x for x in self._hist[lane_idx][row_idx] if x is not None]
                if len(valid) >= self.min_hits:
                    pts.append((int(np.mean(valid)), self._y[row_idx]))
            smoothed[lane_idx] = pts
        return smoothed


def smooth_lane(pts, window=SPATIAL_WINDOW):
    """Suavização espacial: moving average no eixo x entre pontos adjacentes."""
    if len(pts) < 3:
        return pts
    xs = np.array([p[0] for p in pts], dtype=np.float32)
    ys = np.array([p[1] for p in pts], dtype=np.float32)
    pad       = window // 2
    xs_padded = np.pad(xs, pad, mode='edge')
    kernel    = np.ones(window) / window
    xs_smooth = np.convolve(xs_padded, kernel, mode='valid')
    return [(int(x), int(y)) for x, y in zip(xs_smooth, ys)]

def smooth_lanes(lanes):
    return [smooth_lane(lane) for lane in lanes]

def draw_overlay(frame, fps, frame_idx, t_pre, t_hailo, t_post, t_coords,
                 deviation, status):
    h, w = frame.shape[:2]
    overlay = frame.copy()
    cv2.rectangle(overlay, (0, 0), (215, 145), (0, 0, 0), -1)
    cv2.addWeighted(overlay, 0.5, frame, 0.5, 0, frame)
    dev_str   = f"{deviation:+.3f}" if deviation is not None else "N/A"
    dev_color = (0, 255, 0) if status == "both" else (0, 165, 255)
    lines = [
        (f"FPS:    {fps:.1f}",              (0, 255, 0)),
        (f"Pre:    {t_pre:.1f}ms",          (200, 200, 200)),
        (f"Hailo:  {t_hailo:.1f}ms",        (200, 200, 200)),
        (f"Post:   {t_post:.1f}ms",         (200, 200, 200)),
        (f"Coords: {t_coords:.1f}ms",       (200, 200, 200)),
        (f"Frame:  {frame_idx}",            (255, 255, 255)),
        (f"Thr:    {EXIST_THRESHOLD}",      (255, 200, 0)),
        (f"Dev:    {dev_str} [{status}]",   dev_color),
    ]
    for i, (text, color) in enumerate(lines):
        cv2.putText(frame, text, (5, 16 + i * 14),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.35, color, 1)
    cv2.putText(frame, f"SEAME | TuSimple cut v2 | {TRAIN_WIDTH}x{TRAIN_HEIGHT}",
                (w - 260, h - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (200, 200, 200), 1)
    return frame


# ── Pipeline principal ────────────────────────────────────────────────────────
def run_demo(frame_queue, duration_seconds=60, save_video=False):
    threading.Thread(target=_kuksa_worker, args=(_kuksa_queue,), daemon=True).start()

    W1, b1, W2, b2 = load_weights(WEIGHTS_PATH)

    print(f"\nFrame câmara:     {FULL_W}×{FULL_H} (via CameraBroker)")
    print(f"Modelo recebe:    {TRAIN_WIDTH}×{TRAIN_HEIGHT} (crop_ratio={CROP_RATIO})")
    print(f"HEF cortado:      /pool/Conv → conv37 (10,25,8) UINT8")
    print(f"Post-processing:  CPU (dequant + FC1 + ReLU + FC2)")
    print(f"EXIST_THRESHOLD:  {EXIST_THRESHOLD}")
    print(f"Temporal smoother: history={TEMPORAL_HISTORY} min_hits={TEMPORAL_MIN_HITS}")

    async_writer = None
    if save_video:
        async_writer = AsyncVideoWriter(OUTPUT_PATH, CAM_FPS, FULL_W, FULL_H)
        print(f"A gravar: {OUTPUT_PATH}")

    hef = HEF(HEF_PATH)
    with VDevice() as target:
        configure_params = ConfigureParams.create_from_hef(
            hef, interface=HailoStreamInterface.PCIe)
        network_groups = target.configure(hef, configure_params)
        network_group  = network_groups[0]

        input_params  = InputVStreamParams.make(network_group,
                            format_type=FormatType.UINT8)
        output_params = OutputVStreamParams.make(network_group,
                            format_type=FormatType.UINT8)

        with network_group.activate():
            with InferVStreams(network_group, input_params, output_params) as pipeline:
                print(f"\nA correr durante {duration_seconds}s — Ctrl+C para parar\n")
                print(f"{'Frame':<7} {'Pre':>6} {'Hailo':>7} {'Post':>7} "
                      f"{'Coords':>8} {'Total':>7} {'Lanes':>6} {'Dev':>8} {'Status'}")
                print("─" * 78)

                frame_idx         = 0
                t_start           = time.time()
                times_all         = []
                fps_acc           = []
                temporal_smoother = TemporalLaneSmoother()

                try:
                    while (time.time() - t_start) < duration_seconds:
                        try:
                            frame = frame_queue.get(timeout=0.1)
                        except queue.Empty:
                            continue

                        t0    = time.time()
                        img   = preprocess(frame)
                        t_pre = (time.time() - t0) * 1000

                        t0         = time.time()
                        input_data = {INPUT_STREAM: img[np.newaxis]}
                        output     = pipeline.infer(input_data)
                        t_hailo    = (time.time() - t0) * 1000

                        t0     = time.time()
                        conv37 = output[OUTPUT_LAYER][0]  # (10, 25, 8) UINT8
                        loc_row_flat, loc_col_flat, exist_row_flat, exist_col_flat = \
                            postprocess(conv37, W1, b1, W2, b2)
                        lanes  = decode_lanes(loc_row_flat, loc_col_flat,
                                              exist_row_flat, exist_col_flat)
                        t_post = (time.time() - t0) * 1000

                        t0                = time.time()
                        lanes             = temporal_smoother.update(lanes)
                        lanes             = smooth_lanes(lanes)
                        deviation, status = calc_lateral_deviation(lanes)
                        t_coords          = (time.time() - t0) * 1000

                        publish_deviation(deviation, status)

                        t_total   = t_pre + t_hailo + t_post + t_coords
                        num_lanes = sum(1 for l in lanes if l)
                        times_all.append(t_total)

                        fps_acc.append(1000.0 / (t_total + 1e-9))
                        if len(fps_acc) > 30:
                            fps_acc.pop(0)
                        fps = sum(fps_acc) / len(fps_acc)

                        dev_str = f"{deviation:+.3f}" if deviation is not None else "   N/A"
                        print(f"{frame_idx:<7} "
                              f"{t_pre:>5.1f}ms "
                              f"{t_hailo:>6.1f}ms "
                              f"{t_post:>6.1f}ms "
                              f"{t_coords:>7.1f}ms "
                              f"{t_total:>6.1f}ms "
                              f"{num_lanes:>6} "
                              f"{dev_str:>8} "
                              f"{status}")

                        if save_video and async_writer is not None:
                            frame_out = draw_lanes(frame.copy(), lanes)
                            frame_out = draw_overlay(
                                frame_out, fps, frame_idx,
                                t_pre, t_hailo, t_post, t_coords,
                                deviation, status)
                            async_writer.write(frame_out)

                        frame_idx += 1

                except KeyboardInterrupt:
                    print("\nInterrompido.")

    if async_writer is not None:
        print("\nA aguardar gravação...")
        async_writer.release()
        print(f"Vídeo guardado: {OUTPUT_PATH}")

    elapsed  = time.time() - t_start
    real_fps = frame_idx / elapsed if elapsed > 0 else 0
    avg      = sum(times_all) / len(times_all) if times_all else 0
    sorted_t = sorted(times_all)
    p50      = sorted_t[int(len(sorted_t) * 0.50)] if sorted_t else 0
    p95      = sorted_t[int(len(sorted_t) * 0.95)] if sorted_t else 0

    print("─" * 78)
    print(f"\nResumo ({frame_idx} frames em {elapsed:.1f}s):")
    print(f"   FPS real:          {real_fps:.1f}")
    print(f"   FPS pipeline puro: {1000/avg:.1f}" if avg > 0 else "   FPS pipeline puro: N/A")
    print(f"   Latência média:    {avg:.1f}ms")
    print(f"   P50:               {p50:.1f}ms")
    print(f"   P95:               {p95:.1f}ms")


if __name__ == "__main__":
    import sys
    sys.path.insert(0, "/opt/seame/adas")
    from camera_broker import CameraBroker

    duration   = int(sys.argv[1]) if len(sys.argv) > 1 else 60
    save_video = "--save" in sys.argv

    print(f"SEAME Lane Detection | TuSimple cut v2 | {TRAIN_WIDTH}×{TRAIN_HEIGHT}")
    print(f"HEF cortado em /pool/Conv | Post-processing CPU\n")

    broker = CameraBroker(width=FULL_W, height=FULL_H, fps=CAM_FPS)
    q = broker.register("tusimple_cut")
    broker.start()

    run_demo(q, duration_seconds=duration, save_video=save_video)

    broker.stop()
