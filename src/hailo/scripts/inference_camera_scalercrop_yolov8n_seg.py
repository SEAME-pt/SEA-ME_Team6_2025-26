#!/usr/bin/env python3
import os
"""
YOLOv8n-seg com device-side NMS — ScalerCrop Camera
HEF compilado COM NMS integrado no device
→ Host apenas desenha resultados

Baseado em: inference_camera_scalercrop_yolov8s.py
Adaptado por: João (SEAME, 2026-04-21)
"""

import numpy as np
import cv2
import threading
import queue
import time
import sys
import csv
import statistics
from pathlib import Path
from hailo_platform import (HEF, VDevice, HailoStreamInterface,
                            InferVStreams, ConfigureParams,
                            InputVStreamParams, OutputVStreamParams,
                            FormatType)

import hailo_demo_common as demo_io

# ── Configuração ──────────────────────────────────────────────────────────────
HEF_PATH    = os.environ.get("HEF_PATH", "/data/yolo_benchmark/models/hef/yolov8n_seg_h8.hef")
OUTPUT_PATH = os.environ.get("OUTPUT_PATH", "/data/yolo_benchmark/results/demo_yolov8n_seg_scalercrop.mp4")

# Câmara CSI — ScalerCrop
SENSOR_W    = 4608
SENSOR_H    = 2592
ROI_RATIO   = 0.6   # cortar 40% do topo no sensor

FULL_W  = 820
FULL_H  = 616
CAM_FPS = 30

ROI_Y = 0
ROI_H = FULL_H

# YOLOv8n-seg — input do modelo
MODEL_W = 640  # NOTA: 640 standard para YOLOv8
MODEL_H = 640

# Streams do HEF (COM NMS integrado)
INPUT_STREAM   = "yolov8n_seg/input_layer1"
OUTPUT_STREAM  = "yolov8n_seg/output0"     # detections (NMS já aplicado)
MASK_STREAM    = "yolov8n_seg/output1"     # masks (se disponível)

# Thresholds: subir a confiança para reduzir falsos positivos
CONF_THRESH = 0.55  # conf mínima
MASK_THRESH = 0.5   # limiar para máscara
PRE_NMS_TOPK = 120
MAX_DETECTIONS = 15

ROAD_CLASS_NAMES = [
    "center_continuous_lane",
    "center_dashed_lane",
    "crosswalk",
    "left_lane",
    "right_lane",
]

BOX_COLOR   = (0, 255, 0)
MASK_COLOR  = (0, 255, 0)  # verde para máscara
TEXT_COLOR  = (0, 255, 0)


# ── Câmara com ScalerCrop ─────────────────────────────────────────────────────
class PiCamera2Capture:
    def __init__(self, width=FULL_W, height=FULL_H, fps=CAM_FPS):
        from picamera2.picamera2 import Picamera2

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
        print(f"   ScalerCrop: sensor crop top={crop_top}px ({ROI_RATIO*100:.0f}% cortado)")
        print(f"   Frame entregue representa a zona da estrada")

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


# ── Pré-processamento YOLOv8n-seg ─────────────────────────────────────────────
def preprocess(frame):
    """
    Frame BGR 820×616 → UINT8 (640, 640, 3) para o Hailo.
    Sem crop por software — ScalerCrop já entregou só a estrada.
    """
    img = cv2.resize(frame, (MODEL_W, MODEL_H))
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    return img.astype(np.uint8)


# ── Post-processing YOLOv8n-seg (format real do HEF) ──────────────────────────
def _sigmoid(x):
    return 1.0 / (1.0 + np.exp(-x))


def _softmax(x, axis=-1):
    x = x - np.max(x, axis=axis, keepdims=True)
    ex = np.exp(x)
    return ex / (np.sum(ex, axis=axis, keepdims=True) + 1e-9)


def _xywh_to_xyxy(boxes):
    x = boxes.copy()
    x[:, 0] = boxes[:, 0] - boxes[:, 2] / 2.0
    x[:, 1] = boxes[:, 1] - boxes[:, 3] / 2.0
    x[:, 2] = boxes[:, 0] + boxes[:, 2] / 2.0
    x[:, 3] = boxes[:, 1] + boxes[:, 3] / 2.0
    return x


def _nms_numpy(boxes, scores, iou_thresh=0.45):
    if len(boxes) == 0:
        return np.array([], dtype=int)
    x1 = boxes[:, 0]
    y1 = boxes[:, 1]
    x2 = boxes[:, 2]
    y2 = boxes[:, 3]
    areas = (x2 - x1) * (y2 - y1)
    order = scores.argsort()[::-1]
    keep = []
    while order.size > 0:
        i = order[0]
        keep.append(i)
        if order.size == 1:
            break
        xx1 = np.maximum(x1[i], x1[order[1:]])
        yy1 = np.maximum(y1[i], y1[order[1:]])
        xx2 = np.minimum(x2[i], x2[order[1:]])
        yy2 = np.minimum(y2[i], y2[order[1:]])
        w = np.maximum(0.0, xx2 - xx1)
        h = np.maximum(0.0, yy2 - yy1)
        inter = w * h
        iou = inter / (areas[i] + areas[order[1:]] - inter + 1e-6)
        order = order[1:][iou <= iou_thresh]
    return np.array(keep, dtype=int)


def _decode_masks(proto, coeffs, boxes, out_h=FULL_H, out_w=FULL_W, mask_thresh=MASK_THRESH):
    if proto is None or coeffs is None or len(coeffs) == 0:
        return None
    proto = np.array(proto)
    coeffs = np.array(coeffs)
    if proto.ndim == 4:
        proto = proto[0]
    if coeffs.ndim == 3:
        coeffs = coeffs[0]
    if proto.shape[-1] != coeffs.shape[-1]:
        return None
    mask = proto.reshape(-1, proto.shape[-1]) @ coeffs.T
    mask = _sigmoid(mask).reshape(proto.shape[0], proto.shape[1], -1)
    mask = np.transpose(mask, (2, 0, 1))
    masks_out = []
    for i, box in enumerate(boxes):
        m = cv2.resize(mask[i].astype(np.float32), (out_w, out_h))
        x1, y1, x2, y2 = box.astype(int)
        crop = np.zeros_like(m, dtype=np.uint8)
        x1 = max(0, min(out_w, x1))
        x2 = max(0, min(out_w, x2))
        y1 = max(0, min(out_h, y1))
        y2 = max(0, min(out_h, y2))
        crop[y1:y2, x1:x2] = (m[y1:y2, x1:x2] > mask_thresh).astype(np.uint8)
        masks_out.append(crop)
    return masks_out


def _decode_dfl_to_xyxy(box_logits, stride):
    """Decode YOLOv8 DFL bbox head (4x16 bins) to xyxy in model pixels."""
    b, h, w, c = box_logits.shape
    if c != 64:
        return np.empty((0, 4), dtype=np.float32)

    logits = box_logits.reshape(b, h, w, 4, 16)
    probs = _softmax(logits, axis=-1)
    bins = np.arange(16, dtype=np.float32)
    dist = np.sum(probs * bins, axis=-1) * float(stride)  # (b, h, w, 4)

    gy, gx = np.meshgrid(np.arange(h), np.arange(w), indexing="ij")
    cx = (gx.astype(np.float32) + 0.5) * float(stride)
    cy = (gy.astype(np.float32) + 0.5) * float(stride)

    l = dist[0, :, :, 0]
    t = dist[0, :, :, 1]
    r = dist[0, :, :, 2]
    btm = dist[0, :, :, 3]

    x1 = cx - l
    y1 = cy - t
    x2 = cx + r
    y2 = cy + btm

    return np.stack([x1, y1, x2, y2], axis=-1).reshape(-1, 4)


def _class_name(cls_id):
    if 0 <= int(cls_id) < len(ROAD_CLASS_NAMES):
        return ROAD_CLASS_NAMES[int(cls_id)]
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


def _collect_yolov8_seg_tensors(hailo_output):
    """Collect (bbox, cls, coeff) tensors by spatial scale, ignoring dict order."""
    if isinstance(hailo_output, dict):
        items = list(hailo_output.items())
    else:
        items = [(f"out_{i}", arr) for i, arr in enumerate(hailo_output)]

    proto = None
    levels = {}

    for name, arr in items:
        a = np.array(arr)
        if a.ndim != 4:
            continue
        _, h, w, ch = a.shape

        if h == 160 and w == 160 and ch == 32:
            proto = a
            continue

        if h not in (20, 40, 80) or w != h:
            continue

        levels.setdefault(h, {})
        if ch == 64:
            levels[h]["box"] = a
        elif ch == 80:
            levels[h]["cls"] = a
        elif ch == 32:
            levels[h]["coeff"] = a

    return levels, proto


def yolov8n_seg_postprocess(hailo_output, conf_thresh=CONF_THRESH, iou_thresh=0.45):
    """Parse YOLOv8n-seg raw heads (DFL + class + mask coeff + proto)."""
    if hailo_output is None:
        return np.empty((0, 4)), np.empty((0,)), np.empty((0,)), None
    levels, proto = _collect_yolov8_seg_tensors(hailo_output)
    if proto is None:
        return np.empty((0, 4)), np.empty((0,)), np.empty((0,)), None

    boxes_all = []
    cls_id_all = []
    cls_score_all = []
    coeff_all = []

    for h in sorted(levels.keys()):
        part = levels[h]
        if not {"box", "cls", "coeff"}.issubset(part.keys()):
            continue

        stride = MODEL_W // h
        boxes = _decode_dfl_to_xyxy(part["box"], stride)
        cls_logits = part["cls"].reshape(-1, part["cls"].shape[-1])
        coeff = part["coeff"].reshape(-1, part["coeff"].shape[-1])

        class_scores = _sigmoid(cls_logits)
        cls_id = np.argmax(class_scores, axis=1)
        cls_score = class_scores[np.arange(class_scores.shape[0]), cls_id]

        boxes_all.append(boxes)
        cls_id_all.append(cls_id)
        cls_score_all.append(cls_score)
        coeff_all.append(coeff)

    if not boxes_all:
        return np.empty((0, 4)), np.empty((0,)), np.empty((0,)), None

    boxes = np.concatenate(boxes_all, axis=0)
    classes = np.concatenate(cls_id_all, axis=0)
    cls_score = np.concatenate(cls_score_all, axis=0)
    coeffs = np.concatenate(coeff_all, axis=0)

    keep = cls_score >= conf_thresh
    boxes = boxes[keep]
    scores_kept = cls_score[keep]
    classes = classes[keep]
    coeffs = coeffs[keep]

    if len(boxes) == 0:
        return np.empty((0, 4)), np.empty((0,)), np.empty((0,)), None

    if len(scores_kept) > PRE_NMS_TOPK:
        order = np.argsort(scores_kept)[::-1][:PRE_NMS_TOPK]
        boxes = boxes[order]
        scores_kept = scores_kept[order]
        classes = classes[order]
        coeffs = coeffs[order]

    boxes[:, [0, 2]] = np.clip(boxes[:, [0, 2]], 0, MODEL_W - 1)
    boxes[:, [1, 3]] = np.clip(boxes[:, [1, 3]], 0, MODEL_H - 1)

    keep_idx = _nms_numpy(boxes, scores_kept, iou_thresh=iou_thresh)
    boxes = boxes[keep_idx]
    scores_kept = scores_kept[keep_idx]
    classes = classes[keep_idx]
    coeffs = coeffs[keep_idx]

    if len(scores_kept) > MAX_DETECTIONS:
        order = np.argsort(scores_kept)[::-1][:MAX_DETECTIONS]
        boxes = boxes[order]
        scores_kept = scores_kept[order]
        classes = classes[order]
        coeffs = coeffs[order]

    masks = _decode_masks(proto, coeffs, boxes, MODEL_H, MODEL_W)
    return boxes, scores_kept, classes, masks


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


def scale_masks_to_full_frame(masks):
    if not masks:
        return masks
    scaled = []
    for m in masks:
        if m is None:
            scaled.append(None)
        else:
            scaled.append(cv2.resize(m.astype(np.uint8), (FULL_W, FULL_H), interpolation=cv2.INTER_NEAREST))
    return scaled


def draw_detections(frame, boxes, scores, classes, masks=None):
    """Desenhar boxes e máscaras (se disponível)."""
    overlay = frame.copy()
    for idx, (box, score, cls) in enumerate(zip(boxes, scores, classes)):
        x1, y1, x2, y2 = box
        cv2.rectangle(frame, (x1, y1), (x2, y2), BOX_COLOR, 2)
        label = f"{_class_name(cls)} {score:.2f}"
        cv2.putText(frame, label, (x1, max(0, y1 - 5)),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.4, TEXT_COLOR, 1)

        if masks and idx < len(masks) and masks[idx] is not None:
            mask = masks[idx].astype(bool)
            overlay[mask] = (0.3 * overlay[mask] + 0.7 * np.array(MASK_COLOR)).astype(np.uint8)

    frame[:] = cv2.addWeighted(overlay, 0.45, frame, 0.55, 0)
    return frame


def draw_overlay(frame, fps, frame_idx, t_pre, t_hailo, t_post, num_dets):
    """Overlay com métricas."""
    h, w = frame.shape[:2]
    overlay = frame.copy()
    cv2.rectangle(overlay, (0, 0), (250, 105), (0, 0, 0), -1)
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
    cv2.putText(frame, f"Device-NMS",
                (5, 100), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (255, 200, 0), 1)
    cv2.putText(frame, "SEAME | ScalerCrop | YOLOv8n-seg 640×640 | Device-NMS",
                (w - 280, h - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (200, 200, 200), 1)
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
    print(f"Modo:             Device-side NMS (integrado no HEF)")
    if images_dir:
        print(f"Fonte offline:    {images_dir}")

    async_writer = None
    if save_video:
        async_writer = demo_io.AsyncVideoWriter(output_path, CAM_FPS, FULL_W, FULL_H)
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
                        input_data = {INPUT_STREAM: img[np.newaxis]}
                        output     = pipeline.infer(input_data)
                        t_hailo    = (time.time() - t0) * 1000

                        t0            = time.time()
                        # Este postprocess precisa do dicionário completo para encontrar
                        # box/cls/coeff/proto em qualquer ordem de saída do HEF.
                        boxes, scores, classes, masks = yolov8n_seg_postprocess(output)
                        boxes_full    = scale_boxes_to_full_frame(boxes)
                        masks_full    = scale_masks_to_full_frame(masks)
                        t_post        = (time.time() - t0) * 1000

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
                            frame_out = draw_detections(frame.copy(), boxes_full, scores, classes, masks_full)
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
    print(f"   NMS:                  Device-side (integrado)")
    print(f"   Stats CSV:            {stats_csv_path}")
    if save_video:
        print(f"   Vídeo guardado:       {output_path}")


if __name__ == "__main__":
    parser = demo_io.build_arg_parser("SEAME YOLOv8n-seg | ScalerCrop | Device-side NMS")
    args = parser.parse_args()
    output_path = args.output or OUTPUT_PATH

    print(f"SEAME YOLOv8n-seg | ScalerCrop | HEF 640×640 | Device-side NMS")
    print(f"Câmara: {FULL_W}×{FULL_H} (sensor crop 60% topo) | "
          f"Modelo: {MODEL_W}×{MODEL_H} | NMS: Device-side\n")
    run_demo(
        duration_seconds=args.duration,
        save_video=args.save,
        images_dir=args.images_dir,
        loop=args.loop,
        output_path=output_path,
    )
