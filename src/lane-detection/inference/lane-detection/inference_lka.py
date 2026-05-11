import numpy as np
import cv2
import threading
import queue
import time
import sys
from picamera2.picamera2 import Picamera2
from libcamera import Rectangle
from hailo_platform import (HEF, VDevice, HailoStreamInterface,
                             InferVStreams, ConfigureParams,
                             InputVStreamParams, OutputVStreamParams,
                             FormatType)
import grpc
from kuksa.val.v2 import val_pb2, val_pb2_grpc, types_pb2

# ── Configuração ──────────────────────────────────────────────────────────────
HEF_PATH     = "/data/culane_res34_pool_800x160.hef"
WEIGHTS_PATH = "/data/culane_postprocess_weights_mini.npz"
OUTPUT_PATH  = "/data/demo_lka_test.mp4"

# Câmara CSI — captura só o ROI via ScalerCrop
# O sensor IMX708 tem resolução máxima de 4608×2592
SENSOR_W    = 4608
SENSOR_H    = 2592
ROI_RATIO   = 0.6   # cortar 40% do topo no sensor

# Resolução de output da câmara — representa só a zona da estrada
FULL_W  = 820
FULL_H  = 616
CAM_FPS = 30

# Com ScalerCrop, o frame entregue já é só o ROI — ROI_Y=0
ROI_Y = 0
ROI_H = FULL_H

# Resolução de input do modelo
TRAIN_WIDTH  = 800
TRAIN_HEIGHT = 160

# UFLDv2 — constantes do modelo
NUM_CELL_ROW = 200
NUM_ROW      = 72
NUM_LANES    = 4
LANE_SIZE    = NUM_CELL_ROW * NUM_ROW
LN_EPS       = 1e-5

ROW_ANCHOR   = np.linspace(0.42, 1.0, NUM_ROW)
ANCHOR_MIN   = ROW_ANCHOR[0]
ANCHOR_RANGE = ROW_ANCHOR[-1] - ROW_ANCHOR[0]

LANE_COLORS = [
    (0,   255, 0),   # verde    — lane esquerda
    (0,   0,   255), # vermelho — lane direita
]

INPUT_STREAM  = "culane_res34_pool_800x160/input_layer1"
OUTPUT_STREAM = "culane_res34_pool_800x160/resize1"

# ── Parâmetros de detecção ────────────────────────────────────────────────────
NEAR_ANCHORS    = 20    # quantos anchors inferiores usar (zona próxima do veículo)
EXIST_THRESHOLD = 0.7   # threshold de confiança de existência da lane

# ── KUKSA ─────────────────────────────────────────────────────────────────────
KUKSA_HOST         = "10.21.220.191"
KUKSA_PORT         = 55555
KUKSA_CA_CERT      = "/etc/kuksa/tls/ca.crt"
KUKSA_TOKEN        = "/etc/kuksa/jwt/publisher.jwt"
LKA_DEVIATION_PATH = "Vehicle.ADAS.LaneKeepAssist.LateralDeviation"
LKA_STATUS_PATH    = "Vehicle.ADAS.LaneKeepAssist.LaneStatus"

_kuksa_queue = queue.Queue(maxsize=1)


def _kuksa_worker(q):
    """
    Thread separada — publica desvio lateral no KUKSA sem bloquear o pipeline.
    Usa gRPC v2 directamente (kuksa.val.v2), compatível com o C++ publisher.
    Mantém ligação aberta e reutiliza-a a cada publicação.
    maxsize=1 na queue garante que o Ruben recebe sempre o valor mais recente.
    """
    token    = open(KUKSA_TOKEN).read().strip()
    ca_certs = open(KUKSA_CA_CERT, "rb").read()

    creds   = grpc.ssl_channel_credentials(root_certificates=ca_certs)
    channel = grpc.secure_channel(f"{KUKSA_HOST}:{KUKSA_PORT}", creds)
    stub    = val_pb2_grpc.VALStub(channel)
    metadata = [("authorization", f"Bearer {token}")]
    print("[KUKSA] Thread de publicação iniciada (gRPC v2)")

    while True:
        deviation, status = q.get()
        try:
            # Publicar LateralDeviation (float)
            req_dev = val_pb2.PublishValueRequest()
            req_dev.signal_id.path = LKA_DEVIATION_PATH
            req_dev.data_point.value.float = float(deviation)
            stub.PublishValue(req_dev, metadata=metadata)

            # Publicar LaneStatus (string)
            req_st = val_pb2.PublishValueRequest()
            req_st.signal_id.path = LKA_STATUS_PATH
            req_st.data_point.value.string = status
            stub.PublishValue(req_st, metadata=metadata)
        except Exception as e:
            print(f"[KUKSA] Erro: {e}")


# ── Carregar pesos ────────────────────────────────────────────────────────────
print("A carregar pesos...")
weights   = np.load(WEIGHTS_PATH)
ln_weight = weights["ln_weight"]
ln_bias   = weights["ln_bias"]
W1        = weights["W1"]
b1        = weights["b1"]
W2        = weights["W2"]
b2        = weights["b2"]
print(f"✅ Pesos carregados — W2: {W2.shape}")


# ── Câmara com ScalerCrop ─────────────────────────────────────────────────────
class PiCamera2Capture:
    def __init__(self, width=FULL_W, height=FULL_H, fps=CAM_FPS):
        self._picam2 = Picamera2()
        config = self._picam2.create_preview_configuration(
            main={"format": "RGB888", "size": (width, height)},
            controls={"FrameRate": fps})
        self._picam2.configure(config)

        # ScalerCrop — diz ao sensor para capturar só os 60% inferiores
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
        print("A aguardar câmara CSI com ScalerCrop...")
        if not self._ready.wait(timeout=10):
            raise RuntimeError("Timeout a inicializar a câmara")
        print(f"✅ Câmara iniciada — {width}×{height} @ {fps}fps")
        print(f"   ScalerCrop: sensor crop top={crop_top}px "
              f"({ROI_RATIO*100:.0f}% cortado no sensor)")
        print(f"   Frame entregue representa apenas a zona da estrada")

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
    Frame BGR 820×616 → UINT8 (160, 800, 3) para o Hailo.
    Sem crop — o ScalerCrop já entregou só a zona da estrada.
    Só resize e conversão de cor.
    """
    img = cv2.resize(frame, (TRAIN_WIDTH, TRAIN_HEIGHT))
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    return img.astype(np.uint8)


# ── Post-processing ───────────────────────────────────────────────────────────
def postprocess(hailo_output):
    fea      = hailo_output.astype(np.float32).transpose(2, 0, 1).flatten()
    mean     = fea.mean()
    var      = fea.var()
    fea_norm = (fea - mean) / np.sqrt(var + LN_EPS)
    fea_norm = fea_norm * ln_weight + ln_bias
    fc1_out  = np.maximum(W1 @ fea_norm + b1, 0)
    fc2_out  = W2 @ fc1_out + b2
    loc_lane1 = fc2_out[:LANE_SIZE].reshape(NUM_CELL_ROW, NUM_ROW)
    loc_lane2 = fc2_out[LANE_SIZE:2*LANE_SIZE].reshape(NUM_CELL_ROW, NUM_ROW)
    exist_row = fc2_out[2*LANE_SIZE:].reshape(2, NUM_ROW, NUM_LANES)
    return loc_lane1, loc_lane2, exist_row


# ── pred2coords ───────────────────────────────────────────────────────────────
def pred2coords(loc_lane1, loc_lane2, exist_row):
    """
    Converte predições em coordenadas no frame 820×616.
    Aplica EXIST_THRESHOLD para filtrar detecções com baixa confiança.
    Usa só os NEAR_ANCHORS inferiores (zona mais próxima do veículo).
    """
    coords = []

    exp_exist    = np.exp(exist_row - exist_row.max(axis=0, keepdims=True))
    exist_scores = exp_exist / exp_exist.sum(axis=0, keepdims=True)

    for loc, i in [(loc_lane1, 1), (loc_lane2, 2)]:
        tmp = []

        valid_mask = exist_scores[1, :, i] > EXIST_THRESHOLD

        near_mask = np.zeros(NUM_ROW, dtype=bool)
        near_mask[NUM_ROW - NEAR_ANCHORS:] = True
        valid_mask = valid_mask & near_mask

        if valid_mask.sum() > NUM_ROW / 4:
            max_idx = loc.argmax(0)
            for k in range(NUM_ROW - NEAR_ANCHORS, NUM_ROW):
                if valid_mask[k]:
                    lo      = max(0, max_idx[k] - TRAIN_WIDTH)
                    hi      = min(NUM_CELL_ROW - 1, max_idx[k] + TRAIN_WIDTH)
                    all_ind = np.arange(lo, hi + 1)
                    scores  = loc[all_ind, k]
                    scores  = np.exp(scores - scores.max())
                    scores /= scores.sum()
                    out_x   = (scores * all_ind).sum() + 0.5

                    x_full = int(out_x / (NUM_CELL_ROW - 1) * FULL_W)
                    anchor = ROW_ANCHOR[k]
                    y_full = int((anchor - ANCHOR_MIN) / ANCHOR_RANGE * ROI_H + ROI_Y)
                    tmp.append((x_full, y_full))
        coords.append(tmp)
    return coords


# ── Desvio lateral ────────────────────────────────────────────────────────────
def calc_lateral_deviation(coords):
    """
    Calcula o desvio lateral normalizado entre -1.0 e +1.0.

      0.0  = veículo centrado entre as duas lanes
     +1.0  = veículo desviado para a direita
     -1.0  = veículo desviado para a esquerda

    Usa a média dos 5 pontos mais baixos de cada lane (mais próximos do veículo).
    Retorna (None, "none") se não houver lanes suficientes.
    Retorna (deviation, "both" | "left" | "right") conforme lanes detectadas.
    """
    frame_center = FULL_W / 2.0
    left_pts     = coords[0]  # lane esquerda
    right_pts    = coords[1]  # lane direita

    has_left  = len(left_pts)  >= 3
    has_right = len(right_pts) >= 3

    if not has_left and not has_right:
        return None, "none"

    def bottom_x(pts, n=5):
        sorted_pts = sorted(pts, key=lambda p: p[1], reverse=True)
        return float(np.mean([p[0] for p in sorted_pts[:n]]))

    if has_left and has_right:
        x_left  = bottom_x(left_pts)
        x_right = bottom_x(right_pts)
        lane_width = x_right - x_left

        if lane_width < 50:
            # Lanes demasiado próximas — detecção provavelmente errada
            return None, "none"

        lane_center = (x_left + x_right) / 2.0
        deviation   = (lane_center - frame_center) / (lane_width / 2.0)
        return float(np.clip(deviation, -1.0, 1.0)), "both"

    if has_left:
        # Só lane esquerda — estima desvio pela distância ao centro
        x_left    = bottom_x(left_pts)
        deviation = (frame_center - x_left) / frame_center
        return float(np.clip(-deviation, -1.0, 1.0)), "left"

    # Só lane direita
    x_right   = bottom_x(right_pts)
    deviation = (x_right - frame_center) / frame_center
    return float(np.clip(deviation, -1.0, 1.0)), "right"


# ── Publicar no KUKSA (não bloqueante) ───────────────────────────────────────
def publish_deviation(deviation, status):
    """Envia para a queue KUKSA. Descarta silenciosamente se a queue estiver cheia."""
    payload = (deviation if deviation is not None else 0.0, status)
    try:
        _kuksa_queue.put_nowait(payload)
    except queue.Full:
        pass


# ── Desenhar lanes ────────────────────────────────────────────────────────────
def draw_lanes(frame, coords):
    for lane_idx, lane in enumerate(coords):
        if not lane:
            continue
        color = LANE_COLORS[lane_idx % len(LANE_COLORS)]
        for j in range(len(lane) - 1):
            cv2.line(frame, lane[j], lane[j+1], color, 3)
        for pt in lane:
            cv2.circle(frame, pt, 4, color, -1)
    return frame


# ── Overlay ───────────────────────────────────────────────────────────────────
def draw_overlay(frame, fps, frame_idx, t_pre, t_hailo, t_post, t_coords,
                 deviation, status):
    h, w = frame.shape[:2]
    overlay = frame.copy()
    cv2.rectangle(overlay, (0, 0), (210, 130), (0, 0, 0), -1)
    cv2.addWeighted(overlay, 0.5, frame, 0.5, 0, frame)
    cv2.putText(frame, f"FPS:    {fps:.1f}",
                (5, 16), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (0, 255, 0), 1)
    cv2.putText(frame, f"Pre:    {t_pre:.1f}ms",
                (5, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (200, 200, 200), 1)
    cv2.putText(frame, f"Hailo:  {t_hailo:.1f}ms",
                (5, 44), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (200, 200, 200), 1)
    cv2.putText(frame, f"Post:   {t_post:.1f}ms",
                (5, 58), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (200, 200, 200), 1)
    cv2.putText(frame, f"Coords: {t_coords:.1f}ms",
                (5, 72), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (200, 200, 200), 1)
    cv2.putText(frame, f"Thr:    {EXIST_THRESHOLD}",
                (5, 86), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (255, 200, 0), 1)
    # Desvio lateral — cor indica direcção
    dev_str   = f"{deviation:+.3f}" if deviation is not None else "N/A"
    dev_color = (0, 255, 0) if status == "both" else (0, 165, 255)
    cv2.putText(frame, f"Dev:    {dev_str} [{status}]",
                (5, 100), cv2.FONT_HERSHEY_SIMPLEX, 0.35, dev_color, 1)
    cv2.putText(frame, f"Frame:  {frame_idx}",
                (5, 114), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (255, 255, 255), 1)
    cv2.putText(frame, "SEAME | ScalerCrop | 800x160 | LKA",
                (w - 210, h - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (200, 200, 200), 1)
    return frame


# ── Pipeline principal ────────────────────────────────────────────────────────
def run_demo(duration_seconds=60, save_video=False):
    # Arrancar thread KUKSA antes do pipeline
    threading.Thread(target=_kuksa_worker, args=(_kuksa_queue,),
                     daemon=True).start()

    cam = PiCamera2Capture()

    for _ in range(10):
        ret, frame, _ = cam.read()
        if ret:
            break
        time.sleep(0.1)
    if not ret:
        raise RuntimeError("Não foi possível ler frame da câmara")

    print(f"\nFrame da câmara:  {FULL_W}×{FULL_H} (já cropado no sensor)")
    print(f"Modelo recebe:    {TRAIN_WIDTH}×{TRAIN_HEIGHT}")
    print(f"ROI_Y:            {ROI_Y} (ScalerCrop — sem crop por software)")
    print(f"EXIST_THRESHOLD:  {EXIST_THRESHOLD}")
    print(f"NEAR_ANCHORS:     {NEAR_ANCHORS}")
    print(f"KUKSA:            {KUKSA_HOST}:{KUKSA_PORT} (TLS+JWT)")

    async_writer = None
    if save_video:
        async_writer = AsyncVideoWriter(OUTPUT_PATH, CAM_FPS, FULL_W, FULL_H)
        print(f"🎥 A gravar em {OUTPUT_PATH}")

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

                        # Pré-processamento
                        t0    = time.time()
                        img   = preprocess(frame)
                        t_pre = (time.time() - t0) * 1000

                        # Inferência Hailo-8
                        t0         = time.time()
                        input_data = {INPUT_STREAM: img[np.newaxis]}
                        output     = pipeline.infer(input_data)
                        t_hailo    = (time.time() - t0) * 1000

                        # Post-processing
                        t0                              = time.time()
                        hailo_out                       = output[OUTPUT_STREAM][0]
                        loc_lane1, loc_lane2, exist_row = postprocess(hailo_out)
                        t_post = (time.time() - t0) * 1000

                        # Coordenadas
                        t0       = time.time()
                        coords   = pred2coords(loc_lane1, loc_lane2, exist_row)
                        t_coords = (time.time() - t0) * 1000

                        # Desvio lateral + publicação KUKSA
                        deviation, status = calc_lateral_deviation(coords)
                        publish_deviation(deviation, status)

                        t_total   = t_pre + t_hailo + t_post + t_coords
                        num_lanes = sum(1 for lane in coords if lane)
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
                            frame_out = draw_lanes(frame.copy(), coords)
                            frame_out = draw_overlay(frame_out, fps, frame_idx,
                                                     t_pre, t_hailo, t_post,
                                                     t_coords, deviation, status)
                            async_writer.write(frame_out)

                        frame_idx += 1

                except KeyboardInterrupt:
                    print("\nInterrompido.")

    cam.release()
    if async_writer is not None:
        print("\nA aguardar gravação do vídeo...")
        async_writer.release()
        print(f"✅ Vídeo guardado: {OUTPUT_PATH}")

    # ── Resumo ────────────────────────────────────────────────────────────────
    elapsed  = time.time() - t_start
    real_fps = frame_idx / elapsed if elapsed > 0 else 0
    avg      = sum(times_all) / len(times_all) if times_all else 0
    mn       = min(times_all) if times_all else 0
    mx       = max(times_all) if times_all else 0
    sorted_t = sorted(times_all)
    p50 = sorted_t[int(len(sorted_t) * 0.50)]
    p95 = sorted_t[int(len(sorted_t) * 0.95)]

    print("─" * 78)
    print(f"\n📊 Resumo ({frame_idx} frames em {elapsed:.1f}s):")
    print(f"   Câmara:               {FULL_W}×{FULL_H} (ScalerCrop)")
    print(f"   Modelo:               {TRAIN_WIDTH}×{TRAIN_HEIGHT}")
    print(f"   EXIST_THRESHOLD:      {EXIST_THRESHOLD}")
    print(f"   NEAR_ANCHORS:         {NEAR_ANCHORS}")
    print(f"   FPS real:             {real_fps:.1f}")
    print(f"   FPS pipeline puro:    {1000/avg:.1f}")
    print(f"   Latência média:       {avg:.1f}ms")
    print(f"   Latência mín:         {mn:.1f}ms")
    print(f"   Latência máx:         {mx:.1f}ms")
    print(f"   P50:                  {p50:.1f}ms")
    print(f"   P95:                  {p95:.1f}ms")
    if save_video:
        print(f"   Vídeo guardado:       {OUTPUT_PATH}")


# ── Entry point ───────────────────────────────────────────────────────────────
if __name__ == "__main__":
    duration   = int(sys.argv[1]) if len(sys.argv) > 1 else 60
    save_video = "--save" in sys.argv

    print(f"SEAME Lane Detection | ScalerCrop | HEF 800×160 | LKA")
    print(f"Câmara: {FULL_W}×{FULL_H} (sensor crop 40% topo) | Modelo: {TRAIN_WIDTH}×{TRAIN_HEIGHT}\n")
    run_demo(duration_seconds=duration, save_video=save_video)
