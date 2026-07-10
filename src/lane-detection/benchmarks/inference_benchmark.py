import numpy as np
import cv2
import threading
import queue
import time
import sys
import argparse
from picamera2.picamera2 import Picamera2
from hailo_platform import (HEF, VDevice, HailoStreamInterface,
                             InferVStreams, ConfigureParams,
                             InputVStreamParams, OutputVStreamParams,
                             FormatType)

# ── Configurações dos 4 testes ────────────────────────────────────────────────
CONFIGS = {
    "A": {
        "desc":        "HEF 1600x320 | Câmara 1640x1232",
        "hef":         "/data/culane_res34_pool.hef",
        "input_stream":  "culane_res34_pool/input_layer1",
        "output_stream": "culane_res34_pool/conv37",
        "train_w":     1600,
        "train_h":     320,
        "full_w":      1640,
        "full_h":      1232,
    },
    "B": {
        "desc":        "HEF 800x160 | Câmara 1640x1232",
        "hef":         "/data/culane_res34_pool_800x160.hef",
        "input_stream":  "culane_res34_pool_800x160/input_layer1",
        "output_stream": "culane_res34_pool_800x160/resize1",
        "train_w":     800,
        "train_h":     160,
        "full_w":      1640,
        "full_h":      1232,
    },
    "C": {
        "desc":        "HEF 1600x320 | Câmara 820x616",
        "hef":         "/data/culane_res34_pool.hef",
        "input_stream":  "culane_res34_pool/input_layer1",
        "output_stream": "culane_res34_pool/conv37",
        "train_w":     1600,
        "train_h":     320,
        "full_w":      820,
        "full_h":      616,
    },
    "D": {
        "desc":        "HEF 800x160 | Câmara 820x616",
        "hef":         "/data/culane_res34_pool_800x160.hef",
        "input_stream":  "culane_res34_pool_800x160/input_layer1",
        "output_stream": "culane_res34_pool_800x160/resize1",
        "train_w":     800,
        "train_h":     160,
        "full_w":      820,
        "full_h":      616,
    },
}

WEIGHTS_PATH = "/data/culane_postprocess_weights_mini.npz"
ROI_RATIO    = 0.4
CAM_FPS      = 30
NUM_CELL_ROW = 200
NUM_ROW      = 72
NUM_LANES    = 4
LANE_SIZE    = NUM_CELL_ROW * NUM_ROW
LN_EPS       = 1e-5
ROW_ANCHOR   = np.linspace(0.42, 1, NUM_ROW)
ANCHOR_MIN   = ROW_ANCHOR[0]
ANCHOR_RANGE = ROW_ANCHOR[-1] - ROW_ANCHOR[0]
LANE_COLORS  = [(0, 255, 0), (0, 0, 255)]


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


# ── Câmara ────────────────────────────────────────────────────────────────────
class PiCamera2Capture:
    def __init__(self, width, height, fps=CAM_FPS):
        self._picam2 = Picamera2()
        config = self._picam2.create_preview_configuration(
            main={"format": "RGB888", "size": (width, height)},
            controls={"FrameRate": fps})
        self._picam2.configure(config)
        self._latest      = None
        self._frame_count = 0
        self._lock        = threading.Lock()
        self._stop        = threading.Event()
        self._ready       = threading.Event()
        self._picam2.start()
        threading.Thread(target=self._loop, daemon=True).start()
        print("A aguardar câmara CSI...")
        if not self._ready.wait(timeout=10):
            raise RuntimeError("Timeout a inicializar a câmara")
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
        #self._picam2.stop()


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


# ── Funções do pipeline ───────────────────────────────────────────────────────
def preprocess(frame, roi_y, train_w, train_h):
    roi = frame[roi_y:, :, :]
    img = cv2.resize(roi, (train_w, train_h))
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    return img.astype(np.uint8)


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


def pred2coords(loc_lane1, loc_lane2, exist_row, full_w, full_h, roi_y, train_w):
    roi_h     = full_h - roi_y
    coords    = []
    valid_row = exist_row.argmax(0)

    for loc, i in [(loc_lane1, 1), (loc_lane2, 2)]:
        tmp = []
        if valid_row[:, i].sum() > NUM_ROW / 4:
            max_idx = loc.argmax(0)
            for k in range(NUM_ROW):
                if valid_row[k, i]:
                    lo      = max(0, max_idx[k] - train_w)
                    hi      = min(NUM_CELL_ROW - 1, max_idx[k] + train_w)
                    all_ind = np.arange(lo, hi + 1)
                    scores  = loc[all_ind, k]
                    scores  = np.exp(scores - scores.max())
                    scores /= scores.sum()
                    out_x   = (scores * all_ind).sum() + 0.5
                    x_full  = out_x / (NUM_CELL_ROW - 1) * full_w
                    anchor  = ROW_ANCHOR[k]
                    y_full  = (anchor - ANCHOR_MIN) / ANCHOR_RANGE * roi_h + roi_y
                    tmp.append((int(x_full), int(y_full)))
        coords.append(tmp)
    return coords


def draw_lanes(frame, coords, roi_y, full_w):
    for lane_idx, lane in enumerate(coords):
        if not lane:
            continue
        color = LANE_COLORS[lane_idx % len(LANE_COLORS)]
        for j in range(len(lane) - 1):
            cv2.line(frame, lane[j], lane[j+1], color, 3)
        for pt in lane:
            cv2.circle(frame, pt, 5, color, -1)
    cv2.line(frame, (0, roi_y), (full_w, roi_y), (100, 100, 100), 1)
    return frame


# ── Pipeline principal ────────────────────────────────────────────────────────
def run_test(cfg_key, duration_seconds=60, save_video=False):
    cfg      = CONFIGS[cfg_key]
    full_w   = cfg["full_w"]
    full_h   = cfg["full_h"]
    train_w  = cfg["train_w"]
    train_h  = cfg["train_h"]
    roi_y    = int(full_h * ROI_RATIO)
    roi_h    = full_h - roi_y
    out_path = f"/data/teste_{cfg_key}.mp4"

    print(f"\n{'='*60}")
    print(f"  Teste {cfg_key}: {cfg['desc']}")
    print(f"{'='*60}")
    print(f"  Câmara:    {full_w}×{full_h}")
    print(f"  Modelo:    {train_w}×{train_h}")
    print(f"  ROI:       y={roi_y} a y={full_h} ({roi_h}px)")
    print(f"  HEF:       {cfg['hef'].split('/')[-1]}")

    cam = PiCamera2Capture(full_w, full_h)

    for _ in range(10):
        ret, frame, _ = cam.read()
        if ret:
            break
        time.sleep(0.1)
    if not ret:
        raise RuntimeError("Não foi possível ler frame da câmara")

    async_writer = None
    if save_video:
        real_fps = 15  # estimativa conservadora
        async_writer = AsyncVideoWriter(out_path, real_fps, full_w, full_h)
        print(f"  Gravação:  {out_path}")

    hef = HEF(cfg["hef"])
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
                print(f"\n  {'Frame':<7} {'Pre':>6} {'Hailo':>7} {'Post':>7} "
                      f"{'Coords':>8} {'Total':>7} {'Lanes':>6}")
                print(f"  {'─'*58}")

                frame_idx      = 0
                t_start        = time.time()
                last_cam_count = -1
                times          = {"pre": [], "hailo": [], "post": [],
                                  "coords": [], "total": []}

                try:
                    while (time.time() - t_start) < duration_seconds:
                        ret, frame, cam_count = cam.read()
                        if not ret or cam_count == last_cam_count:
                            time.sleep(0.005)
                            continue
                        last_cam_count = cam_count

                        t0    = time.time()
                        img   = preprocess(frame, roi_y, train_w, train_h)
                        t_pre = (time.time() - t0) * 1000

                        t0         = time.time()
                        input_data = {cfg["input_stream"]: img[np.newaxis]}
                        output     = pipeline.infer(input_data)
                        t_hailo    = (time.time() - t0) * 1000

                        t0                              = time.time()
                        hailo_out                       = output[cfg["output_stream"]][0]
                        loc_lane1, loc_lane2, exist_row = postprocess(hailo_out)
                        t_post = (time.time() - t0) * 1000

                        t0     = time.time()
                        coords = pred2coords(loc_lane1, loc_lane2, exist_row,
                                             full_w, full_h, roi_y, train_w)
                        t_coords = (time.time() - t0) * 1000

                        t_total   = t_pre + t_hailo + t_post + t_coords
                        num_lanes = sum(1 for lane in coords if lane)

                        times["pre"].append(t_pre)
                        times["hailo"].append(t_hailo)
                        times["post"].append(t_post)
                        times["coords"].append(t_coords)
                        times["total"].append(t_total)

                        print(f"  {frame_idx:<7} "
                              f"{t_pre:>5.1f}ms "
                              f"{t_hailo:>6.1f}ms "
                              f"{t_post:>6.1f}ms "
                              f"{t_coords:>7.1f}ms "
                              f"{t_total:>6.1f}ms "
                              f"{num_lanes:>6}")

                        if save_video and async_writer is not None:
                            frame_out = draw_lanes(frame.copy(), coords, roi_y, full_w)
                            async_writer.write(frame_out)

                        frame_idx += 1

                except KeyboardInterrupt:
                    print("\n  Interrompido.")

    cam.release()
    if async_writer is not None:
        print("  A finalizar vídeo...")
        async_writer.release()
        print(f"  ✅ Vídeo guardado: {out_path}")

    # ── Resumo ────────────────────────────────────────────────────────────────
    elapsed  = time.time() - t_start
    real_fps = frame_idx / elapsed if elapsed > 0 else 0

    def stats(arr):
        a = np.array(arr)
        return a.mean(), np.percentile(a, 50), np.percentile(a, 95)

    print(f"\n  📊 Resumo Teste {cfg_key} — {cfg['desc']}")
    print(f"  {'─'*56}")
    print(f"  {'Etapa':<12} {'mean':>8} {'p50':>8} {'p95':>8}")
    print(f"  {'─'*56}")
    for etapa in ["pre", "hailo", "post", "coords", "total"]:
        m, p50, p95 = stats(times[etapa])
        print(f"  {etapa:<12} {m:>7.1f}ms {p50:>7.1f}ms {p95:>7.1f}ms")
    print(f"  {'─'*56}")
    print(f"  FPS real: {real_fps:.1f}   Frames: {frame_idx}   Duração: {elapsed:.1f}s")

    return {
        "cfg":      cfg_key,
        "desc":     cfg["desc"],
        "fps":      real_fps,
        "frames":   frame_idx,
        "pre":      np.mean(times["pre"]),
        "hailo":    np.mean(times["hailo"]),
        "post":     np.mean(times["post"]),
        "coords":   np.mean(times["coords"]),
        "total":    np.mean(times["total"]),
    }


# ── Entry point ───────────────────────────────────────────────────────────────
if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="SEAME — Benchmark de inferência (4 configurações)")
    parser.add_argument("config",
        choices=["A", "B", "C", "D", "all"],
        help="A=HEF1600+CAM1640 | B=HEF800+CAM1640 | C=HEF1600+CAM820 | D=HEF800+CAM820 | all=todos")
    parser.add_argument("--duration", type=int, default=30,
        help="Duração de cada teste em segundos (default: 30)")
    parser.add_argument("--save", action="store_true",
        help="Gravar vídeo anotado")
    args = parser.parse_args()

    configs_to_run = ["A", "B", "C", "D"] if args.config == "all" else [args.config]

    print(f"SEAME Lane Detection | Benchmark de Configurações")
    print(f"Pesos: {WEIGHTS_PATH}")
    print(f"Testes: {configs_to_run} | Duração: {args.duration}s cada")

    resultados = []
    for cfg_key in configs_to_run:
        r = run_test(cfg_key, args.duration, args.save)
        resultados.append(r)
        if len(configs_to_run) > 1 and cfg_key != configs_to_run[-1]:
            print("\n  A aguardar 8s antes do próximo teste...")
            time.sleep(8)

    # ── Tabela comparativa final ───────────────────────────────────────────────
    if len(resultados) > 1:
        print(f"\n{'='*72}")
        print(f"  TABELA COMPARATIVA FINAL")
        print(f"{'='*72}")
        print(f"  {'Teste':<4} {'Descrição':<32} {'Pre':>6} {'Hailo':>7} "
              f"{'Post':>7} {'Total':>7} {'FPS':>6}")
        print(f"  {'─'*70}")
        for r in resultados:
            print(f"  {r['cfg']:<4} {r['desc']:<32} "
                  f"{r['pre']:>5.1f}ms "
                  f"{r['hailo']:>6.1f}ms "
                  f"{r['post']:>6.1f}ms "
                  f"{r['total']:>6.1f}ms "
                  f"{r['fps']:>5.1f}")
        print(f"  {'─'*70}")
        melhor = min(resultados, key=lambda x: x["total"])
        print(f"\n  Melhor configuração: Teste {melhor['cfg']} — {melhor['desc']}")
        print(f"  Latência média: {melhor['total']:.1f}ms | FPS: {melhor['fps']:.1f}")
