#!/usr/bin/env python3
import os
import numpy as np
import cv2
import threading
import queue
import time
import sys
import csv
import statistics
from pathlib import Path
import hailo_demo_common as demo_io

# Import Picamera2 apenas se não estamos em modo offline
import argparse
_temp_parser = argparse.ArgumentParser(add_help=False)
_temp_parser.add_argument('--images-dir', type=str, default=None)
_temp_args, _ = _temp_parser.parse_known_args()

if _temp_args.images_dir is None:
    from picamera2.picamera2 import Picamera2
else:
    # Mock Picamera2 para modo offline
    class Picamera2:
        pass
from hailo_platform import (HEF, VDevice, HailoStreamInterface,
                            InferVStreams, ConfigureParams,
                            InputVStreamParams, OutputVStreamParams,
                            FormatType)

# ── Configuração ──────────────────────────────────────────────────────────────
MODEL_LABEL = os.environ.get("MODEL_LABEL", "YOLOv8s")
HEF_PATH    = os.environ.get("HEF_PATH", "/data/yolo_benchmark/models/hef/yolov8s_baseline_h8.hef")
OUTPUT_PATH = os.environ.get("OUTPUT_PATH", "/tmp/demo_yolov8s_scalercrop.mp4")

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

# Streams do HEF YOLOv8-style detection
INPUT_STREAM  = os.environ.get("INPUT_STREAM", "yolov8s/input_layer1")
OUTPUT_STREAM = os.environ.get("OUTPUT_STREAM", "yolov8s/yolov8_nms_postprocess")

CONF_THRESH = 0.25  # HEF já usa 0.2, isto é filtro extra

COCO_CLASS_NAMES = [
    "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
    "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
    "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
    "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket", "bottle",
    "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange",
    "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch", "potted plant", "bed",
    "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave", "oven",
    "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush",
]

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
def _normalize_yolov8_nms_output(raw_output):
    """Normalize Hailo NMS output into an iterable of per-class detections."""
    if isinstance(raw_output, dict):
        raw_output = list(raw_output.values())

    if isinstance(raw_output, np.ndarray):
        if raw_output.ndim >= 1 and raw_output.shape[0] == 1:
            raw_output = raw_output[0]
        if raw_output.ndim == 3 and raw_output.shape[-1] == 5:
            return [raw_output[i] for i in range(raw_output.shape[0])]
        return raw_output


def _class_name(cls_id):
    if 0 <= int(cls_id) < len(COCO_CLASS_NAMES):
        return COCO_CLASS_NAMES[int(cls_id)]
    return f"class_{int(cls_id)}"


def _pick_output_value(output, preferred_key):
    if isinstance(output, dict):
        value = output.get(preferred_key)
        if value is not None:
            return value
        for key, candidate in output.items():
            if candidate is not None:
                print(f"⚠️ Output '{preferred_key}' vazio; a usar '{key}'")
                return candidate
        return None
    return output

    if isinstance(raw_output, (list, tuple)) and len(raw_output) == 1:
        first = raw_output[0]
        if isinstance(first, np.ndarray) and first.ndim >= 1 and first.shape[0] == 1:
            return first[0]
        return first

    return raw_output


def yolo_postprocess(hailo_output, conf_thresh=CONF_THRESH):
    # hailo_output é uma LISTA de arrays, um por classe
    if hailo_output is None:
        return np.empty((0,4)), np.empty((0,)), np.empty((0,))
    hailo_output = _normalize_yolov8_nms_output(hailo_output)
    if hailo_output is None:
        return np.empty((0,4)), np.empty((0,)), np.empty((0,))
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
        label = f"{_class_name(cls)} {score:.2f}"
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
    cv2.putText(frame, f"SEAME | ScalerCrop | {MODEL_LABEL} 640x640",
                (w - 220, h - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (200, 200, 200), 1)
    return frame


def percentile_ms(values, percentile):
    if not values:
        return 0.0
    arr = sorted(values)
    idx = min(len(arr) - 1, int(len(arr) * percentile / 100.0))
    return arr[idx]


def write_frame_stats_csv(csv_path, rows):
    if not rows:
        return
    fieldnames = [
        "frame_idx",
        "pre_ms",
        "hailo_ms",
        "post_ms",
        "total_ms",
        "num_detections",
        "pipeline_fps",
    ]
    with open(csv_path, "w", newline="", encoding="utf-8") as fp:
        writer = csv.DictWriter(fp, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


# ── Pipeline principal ────────────────────────────────────────────────────────
def run_demo(duration_seconds=60, save_video=False, images_dir=None, loop=False, output_path=OUTPUT_PATH):
    source = demo_io.make_frame_source(
        FULL_W, FULL_H, CAM_FPS, SENSOR_W, SENSOR_H, ROI_RATIO, images_dir, loop
    )

    for _ in range(10):
        ret, frame, _ = source.read()
        if ret:
            break
        time.sleep(0.1)
    if not ret:
        raise RuntimeError("Não foi possível ler frame da fonte selecionada")

    if not source.is_live:
        frame = demo_io.resize_for_output(frame, FULL_W, FULL_H)

    print(f"\nFrame da câmara:  {FULL_W}×{FULL_H} (já cropado no sensor)")
    print(f"Modelo recebe:    {MODEL_W}×{MODEL_H}")
    print(f"ROI_Y:            {ROI_Y} (ScalerCrop — sem crop por software)")
    if images_dir:
        print(f"Fonte offline:    {images_dir}")

    async_writer = None
    if save_video:
        async_writer = AsyncVideoWriter(output_path, CAM_FPS, FULL_W, FULL_H)
        print(f"🎥 A gravar em {output_path}")

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
                input_stream_name = INPUT_STREAM
                available_input_names = list(
                    getattr(pipeline, "_input_name_to_network_name", {}).keys()
                )
                if available_input_names and INPUT_STREAM not in available_input_names:
                    input_stream_name = available_input_names[0]
                    print(
                        f"⚠️ INPUT_STREAM '{INPUT_STREAM}' não encontrado no HEF; "
                        f"a usar '{input_stream_name}'"
                    )

                print(f"\nA correr durante {duration_seconds}s — Ctrl+C para parar\n")
                print(f"{'Frame':<7} {'Pre':>6} {'Hailo':>7} {'Post':>7} "
                      f"{'Total':>7} {'Detec':>7}")
                print("─" * 60)

                frame_idx      = 0
                t_start        = time.time()
                last_cam_count = -1
                times_all      = []
                fps_acc        = []
                frame_stats    = []

                try:
                    while True:
                        if (time.time() - t_start) >= duration_seconds:
                            break

                        ret, frame, cam_count = source.read()
                        if not ret:
                            break
                        if source.is_live:
                            if cam_count == last_cam_count:
                                time.sleep(0.005)
                                continue
                            last_cam_count = cam_count
                        else:
                            frame = demo_io.resize_for_output(frame, FULL_W, FULL_H)

                        t0    = time.time()
                        img   = preprocess(frame)
                        t_pre = (time.time() - t0) * 1000

                        t0         = time.time()
                        input_data = {input_stream_name: img[np.newaxis]}
                        output     = pipeline.infer(input_data)
                        t_hailo    = (time.time() - t0) * 1000

                        t0            = time.time()
                        hailo_out     = _pick_output_value(output, OUTPUT_STREAM)
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

                        frame_stats.append({
                            "frame_idx": frame_idx,
                            "pre_ms": round(t_pre, 4),
                            "hailo_ms": round(t_hailo, 4),
                            "post_ms": round(t_post, 4),
                            "total_ms": round(t_total, 4),
                            "num_detections": int(num_dets),
                            "pipeline_fps": round(1000.0 / (t_total + 1e-9), 4),
                        })

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

    source.release()
    if async_writer is not None:
        print("\nA aguardar gravação do vídeo...")
        async_writer.release()
        print(f"✅ Vídeo guardado: {output_path}")

    elapsed  = time.time() - t_start
    real_fps = frame_idx / elapsed if elapsed > 0 else 0
    avg      = sum(times_all) / len(times_all) if times_all else 0
    mn       = min(times_all) if times_all else 0
    mx       = max(times_all) if times_all else 0
    p50      = percentile_ms(times_all, 50)
    p95      = percentile_ms(times_all, 95)
    p99      = percentile_ms(times_all, 99)
    jitter   = statistics.pstdev(times_all) if len(times_all) > 1 else 0.0
    stats_csv_path = str(Path(output_path).with_suffix("")) + "_stats.csv"
    write_frame_stats_csv(stats_csv_path, frame_stats)

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
    print(f"   P99:                  {p99:.1f}ms")
    print(f"   Jitter (std):         {jitter:.3f}ms")
    print(f"   Stats CSV:            {stats_csv_path}")
    if save_video:
        print(f"   Vídeo guardado:       {output_path}")


if __name__ == "__main__":
    parser = demo_io.build_arg_parser(f"SEAME {MODEL_LABEL} | ScalerCrop")
    args = parser.parse_args()
    output_path = args.output or OUTPUT_PATH

    print(f"SEAME {MODEL_LABEL} | ScalerCrop | HEF 640×640")
    print(f"Câmara: {FULL_W}×{FULL_H} (sensor crop 40% topo) | Modelo: {MODEL_W}×{MODEL_H}\n")
    run_demo(
        duration_seconds=args.duration,
        save_video=args.save,
        images_dir=args.images_dir,
        loop=args.loop,
        output_path=output_path,
    )

