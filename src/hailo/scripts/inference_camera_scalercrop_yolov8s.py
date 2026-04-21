#!/usr/bin/env python3
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

# ── Configuração ──────────────────────────────────────────────────────────────
HEF_PATH    = "/data/yolov8s.hef"
OUTPUT_PATH = "/data/demo_yolov8s_scalercrop.mp4"

# Câmara CSI — ScalerCrop (igual ao teu script UFLDv2)
SENSOR_W    = 4608
SENSOR_H    = 2592
ROI_RATIO   = 0.6   # cortar 40% do topo no sensor

FULL_W  = 820
FULL_H  = 616
CAM_FPS = 30

ROI_Y = 0
ROI_H = FULL_H

# YOLOv8s — input do modelo
MODEL_W = 640
MODEL_H = 640

# Streams do HEF YOLOv8s
INPUT_STREAM  = "yolov8s/input_layer1"
OUTPUT_STREAM = "yolov8s/yolov8_nms_postprocess"

CONF_THRESH = 0.25  # HEF já usa 0.2, isto é filtro extra

BOX_COLOR  = (0, 255, 0)
TEXT_COLOR = (0, 255, 0)


# ── Câmara com ScalerCrop ─────────────────────────────────────────────────────
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


# ── Pré-processamento YOLOv8 ──────────────────────────────────────────────────
def preprocess(frame):
    """
    Frame BGR 820×616 → UINT8 (640, 640, 3) para o Hailo.
    Sem crop por software — ScalerCrop já entregou só a estrada.
    """
    img = cv2.resize(frame, (MODEL_W, MODEL_H))
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    return img.astype(np.uint8)


# ── Post-processing YOLOv8 (NMS já no HEF) ────────────────────────────────────
def yolo_postprocess(hailo_output, conf_thresh=CONF_THRESH):
    # hailo_output é uma LISTA de arrays, um por classe
    all_dets = []

    for cls_id, dets in enumerate(hailo_output):
        if dets is None or len(dets) == 0:
            continue

        # Cada det é [x1, y1, x2, y2, score]
        dets = np.array(dets, dtype=np.float32)

        # Adicionar coluna da classe
        cls_col = np.full((dets.shape[0], 1), cls_id, dtype=np.float32)
        dets = np.concatenate([dets, cls_col], axis=1)

        all_dets.append(dets)

    if len(all_dets) == 0:
        return np.empty((0,4)), np.empty((0,)), np.empty((0,))

    out = np.vstack(all_dets)

    # Filtrar por score
    mask = out[:, 4] >= conf_thresh
    out = out[mask]

    boxes   = out[:, 0:4]
    scores  = out[:, 4]
    classes = out[:, 5].astype(int)

    return boxes, scores, classes


def scale_boxes_to_full_frame(boxes):
    """Converte boxes de 640×640 para 820×616."""
    if boxes.size == 0:
        return boxes
    sx = FULL_W / float(MODEL_W)
    sy = FULL_H / float(MODEL_H)
    b = boxes.copy()
    b[:, 0] *= sx
    b[:, 2] *= sx
    b[:, 1] *= sy
    b[:, 3] *= sy
    return b.astype(int)


def draw_detections(frame, boxes, scores, classes):
    for (box, score, cls) in zip(boxes, scores, classes):
        x1, y1, x2, y2 = box
        cv2.rectangle(frame, (x1, y1), (x2, y2), BOX_COLOR, 2)
        label = f"{cls}:{score:.2f}"
        cv2.putText(frame, label, (x1, max(0, y1 - 5)),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.4, TEXT_COLOR, 1)
    return frame


def draw_overlay(frame, fps, frame_idx, t_pre, t_hailo, t_post, num_dets):
    h, w = frame.shape[:2]
    overlay = frame.copy()
    cv2.rectangle(overlay, (0, 0), (220, 90), (0, 0, 0), -1)
    cv2.addWeighted(overlay, 0.5, frame, 0.5, 0, frame)
    cv2.putText(frame, f"FPS:    {fps:.1f}",
                (5, 16), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (0, 255, 0), 1)
    cv2.putText(frame, f"Pre:    {t_pre:.1f}ms",
                (5, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (200, 200, 200), 1)
    cv2.putText(frame, f"Hailo:  {t_hailo:.1f}ms",
                (5, 44), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (200, 200, 200), 1)
    cv2.putText(frame, f"Post:   {t_post:.1f}ms",
                (5, 58), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (200, 200, 200), 1)
    cv2.putText(frame, f"Detec.: {num_dets}",
                (5, 72), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (255, 255, 255), 1)
    cv2.putText(frame, f"Frame:  {frame_idx}",
                (5, 86), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (255, 255, 255), 1)
    cv2.putText(frame, "SEAME | ScalerCrop | YOLOv8s 640x640",
                (w - 220, h - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (200, 200, 200), 1)
    return frame


# ── Pipeline principal ────────────────────────────────────────────────────────
def run_demo(duration_seconds=60, save_video=False):
    cam = PiCamera2Capture()

    for _ in range(10):
        ret, frame, _ = cam.read()
        if ret:
            break
        time.sleep(0.1)
    if not ret:
        raise RuntimeError("Não foi possível ler frame da câmara")

    print(f"\nFrame da câmara:  {FULL_W}×{FULL_H} (já cropado no sensor)")
    print(f"Modelo recebe:    {MODEL_W}×{MODEL_H}")
    print(f"ROI_Y:            {ROI_Y} (ScalerCrop — sem crop por software)")

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

        input_params  = InputVStreamParams.make(
            network_group, format_type=FormatType.UINT8)
        output_params = OutputVStreamParams.make(
            network_group, format_type=FormatType.FLOAT32)

        with network_group.activate():
            with InferVStreams(network_group, input_params, output_params) as pipeline:
                print(f"\nA correr durante {duration_seconds}s — Ctrl+C para parar\n")
                print(f"{'Frame':<7} {'Pre':>6} {'Hailo':>7} {'Post':>7} "
                      f"{'Total':>7} {'Detec':>7}")
                print("─" * 60)

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

                        t0            = time.time()
                        hailo_out     = output[OUTPUT_STREAM][0]
                        boxes, scores, classes = yolo_postprocess(hailo_out)
                        boxes_full     = scale_boxes_to_full_frame(boxes)
                        t_post         = (time.time() - t0) * 1000

                        t_total   = t_pre + t_hailo + t_post
                        num_dets  = len(boxes_full)
                        times_all.append(t_total)

                        fps_acc.append(1000.0 / (t_total + 1e-9))
                        if len(fps_acc) > 30:
                            fps_acc.pop(0)
                        fps = sum(fps_acc) / len(fps_acc)

                        print(f"{frame_idx:<7} "
                              f"{t_pre:>5.1f}ms "
                              f"{t_hailo:>6.1f}ms "
                              f"{t_post:>6.1f}ms "
                              f"{t_total:>6.1f}ms "
                              f"{num_dets:>7}")

                        if save_video and async_writer is not None:
                            frame_out = draw_detections(frame.copy(), boxes_full, scores, classes)
                            frame_out = draw_overlay(frame_out, fps, frame_idx,
                                                     t_pre, t_hailo, t_post, num_dets)
                            async_writer.write(frame_out)

                        frame_idx += 1

                except KeyboardInterrupt:
                    print("\nInterrompido.")

    cam.release()
    if async_writer is not None:
        print("\nA aguardar gravação do vídeo...")
        async_writer.release()
        print(f"✅ Vídeo guardado: {OUTPUT_PATH}")

    elapsed  = time.time() - t_start
    real_fps = frame_idx / elapsed if elapsed > 0 else 0
    avg      = sum(times_all) / len(times_all) if times_all else 0
    mn       = min(times_all) if times_all else 0
    mx       = max(times_all) if times_all else 0
    sorted_t = sorted(times_all) if times_all else []
    p50 = sorted_t[int(len(sorted_t) * 0.50)] if sorted_t else 0
    p95 = sorted_t[int(len(sorted_t) * 0.95)] if sorted_t else 0

    print("─" * 60)
    print(f"\n📊 Resumo ({frame_idx} frames em {elapsed:.1f}s):")
    print(f"   Câmara:               {FULL_W}×{FULL_H} (ScalerCrop)")
    print(f"   Modelo:               {MODEL_W}×{MODEL_H}")
    print(f"   FPS real:             {real_fps:.1f}")
    if avg > 0:
        print(f"   FPS pipeline puro:    {1000/avg:.1f}")
    print(f"   Latência média:       {avg:.1f}ms")
    print(f"   Latência mín:         {mn:.1f}ms")
    print(f"   Latência máx:         {mx:.1f}ms")
    print(f"   P50:                  {p50:.1f}ms")
    print(f"   P95:                  {p95:.1f}ms")
    if save_video:
        print(f"   Vídeo guardado:       {OUTPUT_PATH}")


if __name__ == "__main__":
    duration   = int(sys.argv[1]) if len(sys.argv) > 1 else 60
    save_video = "--save" in sys.argv

    print(f"SEAME YOLOv8s | ScalerCrop | HEF 640×640")
    print(f"Câmara: {FULL_W}×{FULL_H} (sensor crop 40% topo) | Modelo: {MODEL_W}×{MODEL_H}\n")
    run_demo(duration_seconds=duration, save_video=save_video)
