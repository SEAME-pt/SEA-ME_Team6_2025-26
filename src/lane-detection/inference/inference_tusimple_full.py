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

# ── Configuração ──────────────────────────────────────────────────────────────
HEF_PATH    = "/data/tusimple_res34_finetune_seame.hef"
OUTPUT_PATH = "/data/demo_tusimple_v3_new.mp4"

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

MODEL_NAME   = "tusimple_res34_finetune_seame"
INPUT_STREAM = f"{MODEL_NAME}/input_layer1"
OUT_SLICE1   = f"{MODEL_NAME}/slice1"   # loc_row   NC(22400) = 100×56×4
OUT_SLICE2   = f"{MODEL_NAME}/slice2"   # loc_col   NC(16400) = 100×41×4
OUT_SLICE3   = f"{MODEL_NAME}/slice3"   # exist_row NC(448)   = 2×56×4
OUT_SLICE4   = f"{MODEL_NAME}/slice4"   # exist_col NC(328)   = 2×41×4

# Modo '2row2col': lanes 1,2 por row; lanes 0,3 por col (igual ao eval do UFLDv2)
ROW_LANE_LIST = [1, 2]
COL_LANE_LIST = [0, 3]

EXIST_THRESHOLD = 0.7
LOCAL_WIDTH     = 14
MIN_LANE_PTS    = 3

LANE_COLORS = [
    (255, 165, 0),   # laranja  — lane 0 (col)
    (0,   255, 0),   # verde    — lane 1 (row) — ego esquerda
    (0,   0,   255), # vermelho — lane 2 (row) — ego direita
    (255, 0,   255), # magenta  — lane 3 (col)
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


def decode_lanes(slice1, slice2, slice3, slice4):
    """
    Decodifica os 4 outputs do HEF para coordenadas (x, y) no frame original.

    Modo 2row2col:
      - Lanes 1, 2 → row-based (slice1 + slice3)
      - Lanes 0, 3 → col-based (slice2 + slice4)

    Retorna lista de 4 lanes (índices 0-3), cada uma lista de (x, y).
    """
    # Dequantize e reshape
    loc_row   = slice1.astype(np.float32).reshape(NUM_CELL_ROW, NUM_ROW,  NUM_LANES)
    loc_col   = slice2.astype(np.float32).reshape(NUM_CELL_COL, NUM_COL,  NUM_LANES)
    exist_row = slice3.astype(np.float32).reshape(2, NUM_ROW, NUM_LANES)
    exist_col = slice4.astype(np.float32).reshape(2, NUM_COL, NUM_LANES)

    # Scores de existência (softmax sobre dim 0)
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
            if row_score[1, row_idx, lane_idx] < EXIST_THRESHOLD:
                continue
            max_idx = int(loc_row[:, row_idx, lane_idx].argmax())
            out_x   = _softmax_local(loc_row[:, row_idx, lane_idx],
                                     max_idx, LOCAL_WIDTH)
            x_full  = int(out_x / (NUM_CELL_ROW - 1) * FULL_W)
            y_full  = int(ROW_ANCHOR[row_idx] * FULL_H)
            pts.append((x_full, y_full))
        lanes[lane_idx] = pts

    # ── Col-based lanes (0, 3) ─────────────────────────────────────────────
    for lane_idx in COL_LANE_LIST:
        pts = []
        for col_idx in range(NUM_COL):
            if col_score[1, col_idx, lane_idx] < EXIST_THRESHOLD:
                continue
            max_idx = int(loc_col[:, col_idx, lane_idx].argmax())
            out_y   = _softmax_local(loc_col[:, col_idx, lane_idx],
                                     max_idx, LOCAL_WIDTH)
            x_full  = int(COL_ANCHOR[col_idx] * FULL_W)
            y_full  = int(out_y / (NUM_CELL_COL - 1) * FULL_H)
            pts.append((x_full, y_full))
        lanes[lane_idx] = pts

    return lanes


# ── Desvio lateral ────────────────────────────────────────────────────────────
def calc_lateral_deviation(lanes):
    """
    Usa as lanes 1 (esquerda) e 2 (direita) como ego-lanes primárias.
    Fallback para lanes 0 e 3 se as primárias não tiverem pontos suficientes.
    """
    frame_center = FULL_W / 2.0

    def bottom_mean_x(pts, n=5):
        if len(pts) < MIN_LANE_PTS:
            return None
        sorted_pts = sorted(pts, key=lambda p: p[1], reverse=True)
        return float(np.mean([p[0] for p in sorted_pts[:n]]))

    # Tenta ego-lanes primárias (1 = esquerda, 2 = direita)
    x_left  = bottom_mean_x(lanes[1])
    x_right = bottom_mean_x(lanes[2])

    # Fallback para lanes exteriores se necessário
    if x_left is None:
        x_left = bottom_mean_x(lanes[0])
    if x_right is None:
        x_right = bottom_mean_x(lanes[3])

    has_left  = x_left  is not None
    has_right = x_right is not None

    if not has_left and not has_right:
        return None, "none"

    if has_left and has_right:
        lane_width = x_right - x_left
        if lane_width < 50:
            return None, "none"
        lane_center = (x_left + x_right) / 2.0
        deviation   = (lane_center - frame_center) / (lane_width / 2.0)
        return float(np.clip(deviation, -1.0, 1.0)), "both"

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
    for lane_idx, lane in enumerate(lanes):
        if not lane:
            continue
        color = LANE_COLORS[lane_idx % len(LANE_COLORS)]
        for j in range(len(lane) - 1):
            cv2.line(frame, lane[j], lane[j+1], color, 3)
        for pt in lane:
            cv2.circle(frame, pt, 4, color, -1)
    return frame

def smooth_lane(pts, window=5):
    if len(pts) < window:
        return pts
    xs = np.array([p[0] for p in pts], dtype=np.float32)
    ys = np.array([p[1] for p in pts], dtype=np.float32)
    kernel = np.ones(window) / window
    xs_smooth = np.convolve(xs, kernel, mode='same')
    # Corta as extremidades afectadas pelo kernel
    trim = window // 2
    xs_trim = xs_smooth[trim:-trim]
    ys_trim = ys[trim:-trim]
    return [(int(x), int(y)) for x, y in zip(xs_trim, ys_trim)]

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
    cv2.putText(frame, f"SEAME | TuSimple 2row2col | {TRAIN_WIDTH}x{TRAIN_HEIGHT}",
                (w - 260, h - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (200, 200, 200), 1)
    return frame


# ── Pipeline principal ────────────────────────────────────────────────────────
def run_demo(frame_queue, duration_seconds=60, save_video=False):
    """
    frame_queue: queue.Queue fornecida pelo CameraBroker.
                 Chamar broker.register("tusimple") antes de passar aqui.
    """
    threading.Thread(target=_kuksa_worker, args=(_kuksa_queue,), daemon=True).start()

    print(f"\nFrame câmara:     {FULL_W}×{FULL_H} (via CameraBroker)")
    print(f"Modelo recebe:    {TRAIN_WIDTH}×{TRAIN_HEIGHT} (crop_ratio={CROP_RATIO})")
    print(f"Modo:             2row2col (lanes 1,2=row | lanes 0,3=col)")
    print(f"EXIST_THRESHOLD:  {EXIST_THRESHOLD}")

    async_writer = None
    if save_video:
        async_writer = AsyncVideoWriter(OUTPUT_PATH, CAM_FPS, FULL_W, FULL_H)
        print(f"🎥 A gravar: {OUTPUT_PATH}")

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

                frame_idx      = 0
                t_start        = time.time()
                times_all      = []
                fps_acc        = []

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

                        t0    = time.time()
                        lanes = decode_lanes(
                            output[OUT_SLICE1][0],
                            output[OUT_SLICE2][0],
                            output[OUT_SLICE3][0],
                            output[OUT_SLICE4][0])
                        t_post = (time.time() - t0) * 1000
                        t0              = time.time()
                        lanes = smooth_lanes(lanes)
                        deviation, status = calc_lateral_deviation(lanes)
                        t_coords        = (time.time() - t0) * 1000

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
        print(f"✅ Vídeo guardado: {OUTPUT_PATH}")

    elapsed  = time.time() - t_start
    real_fps = frame_idx / elapsed if elapsed > 0 else 0
    avg      = sum(times_all) / len(times_all) if times_all else 0
    sorted_t = sorted(times_all)
    p50      = sorted_t[int(len(sorted_t) * 0.50)]
    p95      = sorted_t[int(len(sorted_t) * 0.95)]

    print("─" * 78)
    print(f"\n📊 Resumo ({frame_idx} frames em {elapsed:.1f}s):")
    print(f"   FPS real:          {real_fps:.1f}")
    print(f"   FPS pipeline puro: {1000/avg:.1f}")
    print(f"   Latência média:    {avg:.1f}ms")
    print(f"   P50:               {p50:.1f}ms")
    print(f"   P95:               {p95:.1f}ms")


if __name__ == "__main__":
    import sys
    sys.path.insert(0, "/opt/seame/adas")
    from camera_broker import CameraBroker

    duration   = int(sys.argv[1]) if len(sys.argv) > 1 else 60
    save_video = "--save" in sys.argv

    print(f"SEAME Lane Detection | TuSimple 2row2col | {TRAIN_WIDTH}×{TRAIN_HEIGHT}")
    print(f"Câmara: {FULL_W}×{FULL_H} | Lanes 1,2=row | Lanes 0,3=col\n")

    broker = CameraBroker(width=FULL_W, height=FULL_H, fps=CAM_FPS)
    q = broker.register("tusimple")
    broker.start()

    run_demo(q, duration_seconds=duration, save_video=save_video)

    broker.stop()
