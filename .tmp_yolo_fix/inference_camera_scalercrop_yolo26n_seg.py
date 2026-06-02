#!/usr/bin/env python3
"""
YOLO26n-seg com host-side NMS — ScalerCrop Camera
Experimental variant: HEF compilado SEM NMS
→ Host aplica NMS (conf=0.5, iou=0.6)
→ Máscara de segmentação renderizada

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
HEF_PATH    = "/data/yolo26n_seg_320_h8_no_nms.hef"  # SEM NMS — host faz
OUTPUT_PATH = "/data/demo_yolo26n_seg_scalercrop.mp4"

# Câmara CSI — ScalerCrop
SENSOR_W    = 4608
SENSOR_H    = 2592
ROI_RATIO   = 0.6   # cortar 40% do topo no sensor

FULL_W  = 820
FULL_H  = 616
CAM_FPS = 30

ROI_Y = 0
ROI_H = FULL_H

# YOLO26n-seg — input do modelo
MODEL_W = 320  # NOTA: 320 para YOLO26n (mais rápido)
MODEL_H = 320

# Streams do HEF (SEM NMS)
INPUT_STREAM   = "yolo26n_seg/input_layer1"
OUTPUT_STREAM  = "yolo26n_seg/output0"     # detections [x, y, w, h, conf, class_0, ..., class_79, mask_0, ..., mask_31]
MASK_STREAM    = "yolo26n_seg/output1"     # masks (se existir; verificar com hailortcli parse-hef)

# Thresholds (fixados conforme protocolo)
CONF_THRESH = 0.30  # conf mínima
IOU_THRESH  = 0.6   # NMS iou
MASK_THRESH = 0.5   # limiar para máscara
PRE_NMS_TOPK = 600
MAX_DETECTIONS = 80

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


# ── Pré-processamento YOLO26n-seg ─────────────────────────────────────────────
def preprocess(frame):
    """
    Frame BGR 820×616 → UINT8 (320, 320, 3) para o Hailo.
    Sem crop por software — ScalerCrop já entregou só a estrada.
    """
    img = cv2.resize(frame, (MODEL_W, MODEL_H))
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    return img.astype(np.uint8)


# ── NMS (implementação simples em Python) ─────────────────────────────────────
def nms(boxes, scores, thresh=IOU_THRESH):
    """
    Aplicar NMS (Non-Maximum Suppression) aos boxes.
    
    Args:
        boxes: np.array shape (N, 4) [x1, y1, x2, y2]
        scores: np.array shape (N,) confidence scores
        thresh: IOU threshold para suprimir
    
    Returns:
        indices: np.array dos boxes a manter
    """
    if len(boxes) == 0:
        return np.array([])

    x1 = boxes[:, 0]
    y1 = boxes[:, 1]
    x2 = boxes[:, 2]
    y2 = boxes[:, 3]

    area = (x2 - x1 + 1) * (y2 - y1 + 1)
    idxs = np.argsort(scores)[::-1]  # ordenar por score decrescente

    keep = []
    while len(idxs) > 0:
        last = len(idxs) - 1
        i = idxs[last]
        keep.append(i)

        if len(idxs) == 1:
            break

        ixs = idxs[:last]
        xx1 = np.maximum(x1[i], x1[ixs])
        yy1 = np.maximum(y1[i], y1[ixs])
        xx2 = np.minimum(x2[i], x2[ixs])
        yy2 = np.minimum(y2[i], y2[ixs])

        w = np.maximum(0, xx2 - xx1 + 1)
        h = np.maximum(0, yy2 - yy1 + 1)
        inter = w * h

        union = area[i] + area[ixs] - inter
        iou = inter / union

        idxs = idxs[np.where(iou <= thresh)[0]]

    return np.array(keep)


# ── Post-processing YOLO26n-seg (HEF real) ───────────────────────────────────
def _sigmoid(x):
    return 1.0 / (1.0 + np.exp(-x))


def _xywh_to_xyxy(boxes):
    x = boxes.copy()
    x[:, 0] = boxes[:, 0] - boxes[:, 2] / 2.0
    x[:, 1] = boxes[:, 1] - boxes[:, 3] / 2.0
    x[:, 2] = boxes[:, 0] + boxes[:, 2] / 2.0
    x[:, 3] = boxes[:, 1] + boxes[:, 3] / 2.0
    return x


def _nms_numpy(boxes, scores, thresh=IOU_THRESH):
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
        order = order[1:][iou <= thresh]
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


def _extract_yolo26_tensors(hailo_output):
    """Extract proto and flattened prediction tensor from HEF outputs."""
    if isinstance(hailo_output, dict):
        items = list(hailo_output.items())
    else:
        items = [(f"out_{i}", arr) for i, arr in enumerate(hailo_output)]

    proto = None
    pred = None

    for _, arr in items:
        a = np.array(arr)
        if a.ndim != 4:
            continue
        _, h, w, c = a.shape
        if h == 80 and w == 80 and c == 32:
            proto = a
        elif h == 1 and w == 116 and c == 2100:
            pred = a

    return proto, pred


def yolo26n_seg_postprocess(hailo_output, conf_thresh=CONF_THRESH, iou_thresh=IOU_THRESH):
    """Parse HEF YOLO26n-seg where output is (1,1,116,2100)."""
    proto, pred = _extract_yolo26_tensors(hailo_output)
    if proto is None or pred is None:
        return np.empty((0, 4)), np.empty((0,)), np.empty((0,)), None

    # (1,1,116,2100) -> (2100,116)
    pred_flat = pred[0, 0].T.astype(np.float32)
    if pred_flat.shape[1] < 116:
        return np.empty((0, 4)), np.empty((0,)), np.empty((0,)), None

    box_xywh = pred_flat[:, :4]
    cls_logits = pred_flat[:, 4:84]
    coeffs = pred_flat[:, 84:116]

    boxes = _xywh_to_xyxy(box_xywh)
    class_scores = _sigmoid(cls_logits)
    cls_id = np.argmax(class_scores, axis=1)
    cls_score = class_scores[np.arange(class_scores.shape[0]), cls_id]

    keep = cls_score >= conf_thresh
    boxes = boxes[keep]
    scores_kept = cls_score[keep]
    classes = cls_id[keep]
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

    keep_idx = _nms_numpy(boxes, scores_kept, thresh=iou_thresh)
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
    """Converte boxes de 320×320 para 820×616."""
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


def decode_mask(mask_proto, mask_values, frame_h, frame_w, threshold=MASK_THRESH):
    """
    Decodificar máscara proto a partir dos coefficients.
    (Implementação simplificada; adaptar conforme output do HEF)
    """
    # Placeholder: retornar None por agora
    # Em produção, isto seria: mask = mask_proto @ mask_values → resize → threshold
    return None


def draw_detections(frame, boxes, scores, classes, masks=None):
    """Desenhar boxes e máscaras (se disponível)."""
    overlay = frame.copy()
    for idx, (box, score, cls) in enumerate(zip(boxes, scores, classes)):
        x1, y1, x2, y2 = box
        cv2.rectangle(frame, (x1, y1), (x2, y2), BOX_COLOR, 2)
        label = f"C{cls}:{score:.2f}"
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
    cv2.rectangle(overlay, (0, 0), (240, 105), (0, 0, 0), -1)
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
    cv2.putText(frame, f"NMS:    {IOU_THRESH}",
                (5, 100), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (255, 200, 0), 1)
    cv2.putText(frame, "SEAME | ScalerCrop | YOLO26n-seg 320×320 | Host-NMS",
                (w - 280, h - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (200, 200, 200), 1)
    return frame


def percentile_ms(values, percentile):
    if not values:
        return 0.0
    ordered = sorted(values)
    idx = int(np.ceil((percentile / 100.0) * len(ordered))) - 1
    idx = max(0, min(idx, len(ordered) - 1))
    return float(ordered[idx])


def write_frame_stats_csv(csv_path, rows):
    if not rows:
        return
    with open(csv_path, "w", newline="", encoding="utf-8") as csv_file:
        writer = csv.DictWriter(
            csv_file,
            fieldnames=[
                "frame_idx",
                "pre_ms",
                "hailo_ms",
                "post_ms",
                "total_ms",
                "num_detections",
                "pipeline_fps",
            ],
        )
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
    print(f"Modo:             Host-side NMS (conf={CONF_THRESH}, iou={IOU_THRESH})")
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
                dropped_frames = 0
                times_all      = []
                fps_acc        = []
                frame_stats    = []

                try:
                    while True:
                        if source.is_live and (time.time() - t_start) >= duration_seconds:
                            break

                        ret, frame, cam_count = source.read()
                        if not ret:
                            break
                        if source.is_live:
                            if cam_count == last_cam_count:
                                time.sleep(0.005)
                                continue
                            if last_cam_count >= 0 and cam_count > (last_cam_count + 1):
                                dropped_frames += (cam_count - last_cam_count - 1)
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
                        boxes, scores, classes, masks = yolo26n_seg_postprocess(output)
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
    print(f"   Quedas de frame:      {dropped_frames}")
    print(f"   Thresholds:           conf={CONF_THRESH}, iou={IOU_THRESH}")
    print(f"   Stats CSV:            {stats_csv_path}")
    if save_video:
        print(f"   Vídeo guardado:       {output_path}")


if __name__ == "__main__":
    parser = demo_io.build_arg_parser("SEAME YOLO26n-seg | ScalerCrop | Host-side NMS")
    args = parser.parse_args()
    output_path = args.output or OUTPUT_PATH

    print(f"SEAME YOLO26n-seg | ScalerCrop | HEF 320×320 | Host-side NMS")
    print(f"Câmara: {FULL_W}×{FULL_H} (sensor crop 60% topo) | "
          f"Modelo: {MODEL_W}×{MODEL_H} | Thresholds: conf={CONF_THRESH}, iou={IOU_THRESH}\n")
    run_demo(
        duration_seconds=args.duration,
        save_video=args.save,
        images_dir=args.images_dir,
        loop=args.loop,
        output_path=output_path,
    )
