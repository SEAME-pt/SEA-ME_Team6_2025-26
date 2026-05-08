import numpy as np
import cv2
import threading
import queue
import time
import sys
from picamera2.picamera2 import Picamera2
from hailo_platform import (HEF, VDevice, HailoStreamInterface,
                             InferVStreams, ConfigureParams,
                             InputVStreamParams, OutputVStreamParams,
                             FormatType)
import grpc
from kuksa.val.v2 import val_pb2, val_pb2_grpc, types_pb2

# ── Configuração ──────────────────────────────────────────────────────────────
HEF_PATH     = "/data/tusimple_res34_cut.hef"
WEIGHTS_PATH = "/data/tusimple_postprocess_weights.npz"
OUTPUT_PATH  = "/data/demo_tusimple_cut.mp4"

SENSOR_W  = 4608
SENSOR_H  = 2592
ROI_RATIO = 0.6

FULL_W  = 820
FULL_H  = 616
CAM_FPS = 30
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

MODEL_NAME    = "tusimple_res34_cut"
INPUT_STREAM  = f"{MODEL_NAME}/input_layer1"
OUTPUT_STREAM = f"{MODEL_NAME}/conv37"

# Modo '2row2col': lanes 1,2 por row; lanes 0,3 por col (igual ao eval do UFLDv2)
ROW_LANE_LIST = [1, 2]
COL_LANE_LIST = [0, 3]

EXIST_THRESHOLD = 0.5
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


# ── Carregar pesos ────────────────────────────────────────────────────────────
print("A carregar pesos...")
weights = np.load(WEIGHTS_PATH)
W1 = weights["W1"]  # (2048, 2000)
b1 = weights["b1"]  # (2048,)
W2 = weights["W2"]  # (39576, 2048)
b2 = weights["b2"]  # (39576,)
print(f"✅ Pesos carregados — W2: {W2.shape}")


# ── Câmara ────────────────────────────────────────────────────────────────────
class PiCamera2Capture:
    def __init__(self, width=FULL_W, height=FULL_H, fps=CAM_FPS):
        self._picam2 = Picamera2()
        config = self._picam2.create_preview_configuration(
            main={"format": "RGB888", "size": (width, height)},
            controls={"FrameRate": fps})
        self._picam2.configure(config)
        crop_top  = int(SENSOR_H * ROI_RATIO)
        crop_rect = (0, crop_top, SENSOR_W, SENSOR_H - crop_top)
        self._picam2.set_controls({"ScalerCrop": crop_rect})
        self._latest      = None
        self._frame_count = 0
        self._lock        = threading.Lock()
        self._stop        = threading.Event()
        self._ready       = threading.Event()
        self._picam2.start()
        threading.Thread(target=self._loop, daemon=True).start()
        print("A aguardar câmara CSI...")
        if not self._ready.wait(timeout=10):
            raise RuntimeError("Timeout a inicializar câmara")
        print(f"✅ Câmara iniciada — {width}×{height} @ {fps}fps")

    def _loop(self):
        while not self._stop.is_set():
            frame = self._picam2.capture_array()
            frame_bgr = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
            with self._lock:
                self._latest      = frame_bgr
                self._frame_count += 1
            self._ready.set()

    def read(self):
        with self._lock:
            if self._latest is None:
                return False, None, -1
            return True, self._latest.copy(), self._frame_count

    def release(self):
        self._stop.set()
        time.sleep(0.5)
        try:
            self._picam2.stop()
            self._picam2.close()
        except Exception:
            pass
        time.sleep(1.0)


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
def postprocess(hailo_output):
    fea = hailo_output.astype(np.float32).transpose(2, 0, 1).flatten()  # (8,10,25) → 2000
    fc1 = np.maximum(W1 @ fea + b1, 0)
    fc2 = W2 @ fc1 + b2

    s1 = fc2[    0:22400]
    s2 = fc2[22400:38800]
    s3 = fc2[38800:39248]
    s4 = fc2[39248:39576]
    return s1, s2, s3, s4


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
    Decodifica os 4 slices para coordenadas (x, y) no frame original.

    Modo 2row2col:
      - Lanes 1, 2 → row-based (slice1 + slice3)
      - Lanes 0, 3 → col-based (slice2 + slice4)

    Retorna lista de 4 lanes (índices 0-3), cada uma lista de (x, y).
    """
    loc_row   = slice1.astype(np.float32).reshape(NUM_CELL_ROW, NUM_ROW,  NUM_LANES)
    loc_col   = slice2.astype(np.float32).reshape(NUM_CELL_COL, NUM_COL,  NUM_LANES)
    exist_row = slice3.astype(np.float32).reshape(2, NUM_ROW, NUM_LANES)
    exist_col = slice4.astype(np.float32).reshape(2, NUM_COL, NUM_LANES)

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

    x_left  = bottom_mean_x(lanes[1])
    x_right = bottom_mean_x(lanes[2])

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
def run_demo(duration_seconds=60, save_video=False):
    threading.Thread(target=_kuksa_worker, args=(_kuksa_queue,), daemon=True).start()

    cam = PiCamera2Capture()
    for _ in range(10):
        ret, frame, _ = cam.read()
        if ret:
            break
        time.sleep(0.1)
    if not ret:
        raise RuntimeError("Não foi possível ler frame da câmara")

    print(f"\nFrame câmara:     {FULL_W}×{FULL_H}")
    print(f"Modelo recebe:    {TRAIN_WIDTH}×{TRAIN_HEIGHT} (crop_ratio={CROP_RATIO})")
    print(f"Modo:             2row2col (lanes 1,2=row | lanes 0,3=col)")
    print(f"EXIST_THRESHOLD:  {EXIST_THRESHOLD}")
    print(f"W1:               {W1.shape} | W2: {W2.shape}")

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
                print("-" * 78)

                frame_idx      = 0
                t_start        = time.time()
                last_cam_count = -1
                times_all      = []
                fps_acc        = []

                try:
                    while (time.time() - t_start) < duration_seconds:
                        ret, frame, cam_count = cam.read()
                        if not ret or cam_count == last_cam_count:
                            time.sleep(0.005)
                            continue
                        last_cam_count = cam_count

                        t0    = time.time()
                        img   = preprocess(frame)
                        t_pre = (time.time() - t0) * 1000

                        t0         = time.time()
                        input_data = {INPUT_STREAM: img[np.newaxis]}
                        output     = pipeline.infer(input_data)
                        t_hailo    = (time.time() - t0) * 1000

                        t0 = time.time()
                        hailo_out        = output[OUTPUT_STREAM][0]  # (10, 25, 8)
                        print("shape:", hailo_out.shape)
                        print("min/max:", hailo_out.min(), hailo_out.max())
                        fea = hailo_out.astype(np.float32).flatten()
                        print("fea shape:", fea.shape)
                        fc1 = np.maximum(W1 @ fea + b1, 0)
                        fc2 = W2 @ fc1 + b2
                        print("fc2 min/max:", fc2.min(), fc2.max())
                        s1 = fc2[0:22400].reshape(100, 56, 4)
                        print("s1 argmax sample:", s1[:, 28, 1].argmax())

                        s1, s2, s3, s4   = postprocess(hailo_out)
                        lanes            = decode_lanes(s1, s2, s3, s4)
                        t_post           = (time.time() - t0) * 1000

                        t0                = time.time()
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

    cam.release()
    if async_writer is not None:
        print("\nA aguardar gravação...")
        async_writer.release()
        print(f"Vídeo guardado: {OUTPUT_PATH}")

    elapsed  = time.time() - t_start
    real_fps = frame_idx / elapsed if elapsed > 0 else 0
    avg      = sum(times_all) / len(times_all) if times_all else 0
    sorted_t = sorted(times_all)
    p50      = sorted_t[int(len(sorted_t) * 0.50)]
    p95      = sorted_t[int(len(sorted_t) * 0.95)]

    print("-" * 78)
    print(f"\nResumo ({frame_idx} frames em {elapsed:.1f}s):")
    print(f"   FPS real:          {real_fps:.1f}")
    print(f"   FPS pipeline puro: {1000/avg:.1f}")
    print(f"   Latência média:    {avg:.1f}ms")
    print(f"   P50:               {p50:.1f}ms")
    print(f"   P95:               {p95:.1f}ms")


# ── Entry point ───────────────────────────────────────────────────────────────
if __name__ == "__main__":
    duration   = int(sys.argv[1]) if len(sys.argv) > 1 else 60
    save_video = "--save" in sys.argv
    print(f"SEAME Lane Detection | TuSimple 2row2col | {TRAIN_WIDTH}x{TRAIN_HEIGHT}")
    print(f"Camara: {FULL_W}x{FULL_H} | Lanes 1,2=row | Lanes 0,3=col\n")
    run_demo(duration_seconds=duration, save_video=save_video)
