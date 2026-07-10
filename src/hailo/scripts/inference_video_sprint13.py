#!/usr/bin/env python3
"""
inference_video_sprint13.py  —  Sprint 13 offline video inference on Hailo-8

BYOM HEFs compiled WITHOUT device-side NMS.
Host-side: DFL box decode + sigmoid class scores + NMS.

Models supported:
  yolov8s_detect_sprint13.hef   (17 classes, detect)
  yolo26n_detect_sprint13.hef   (17 classes, detect)
  yolov8n_seg_sprint13.hef      (19 classes, detect-only; cv4/masks not in HEF)
  yolo26n_seg_sprint13.hef      (19 classes, detect-only; cv4/masks not in HEF)

Usage:
  python3 inference_video_sprint13.py \\
    --hef  /data/yolo_benchmark/models/hef/sprint13/yolov8s_detect_sprint13.hef \\
    --source /data/videos/teste1.mp4 \\
    --output /data/yolo_benchmark/results/teste1_yolov8s_detect.mp4 \\
    [--conf 0.45] [--iou 0.55] [--model-type detect|seg] [--model-name yolov8s]

Output:
  <output>.mp4      annotated video
  <output>_stats.json  FPS + latency stats (p50/p95/p99)
"""

import argparse
import json
import os
import queue
import statistics
import threading
import time
from pathlib import Path

import cv2
import numpy as np

try:
    from hailo_platform import (
        HEF, VDevice, HailoStreamInterface,
        InferVStreams, ConfigureParams,
        InputVStreamParams, OutputVStreamParams,
        FormatType,
    )
except ModuleNotFoundError:
    HEF = None
    VDevice = None
    HailoStreamInterface = None
    InferVStreams = None
    ConfigureParams = None
    InputVStreamParams = None
    OutputVStreamParams = None
    FormatType = None

# ── Class names ────────────────────────────────────────────────────────────────
DETECT_CLASSES = [
    "50_maxspeed", "80_maxspeed", "Crosswalk", "Gate",
    "Pedestrians_crossing", "Stop_sign", "Traffic_priority",
    "both_arrow", "car", "cars not allowed", "left_cross",
    "obstacle", "right_cross", "traffic_lights_green",
    "traffic_lights_off", "traffic_lights_red", "traffic_lights_yellow",
]  # 17 classes

SEG_CLASSES = [
    "50_maxspeed_sign", "80_maxspeed_sign", "arrow_left", "arrow_left_front",
    "arrow_right", "car", "crosswalk", "crosswalk_sign", "curve", "gate",
    "obstacle", "shadow", "stop_sign", "traffic_danger_sign",
    "traffic_light_green", "traffic_light_off", "traffic_light_red",
    "traffic_light_yellow", "traffic_priority_yield_sign",
]  # 19 classes

# Distinct BGR colors per class for clearer visualisation
_PALETTE = [
    (0, 255, 0),    (0, 200, 255), (255, 0, 0),    (255, 128, 0),
    (128, 0, 255),  (0, 255, 200), (200, 255, 0),   (255, 0, 128),
    (0, 128, 255),  (128, 255, 0), (255, 200, 0),   (0, 0, 255),
    (200, 0, 255),  (255, 255, 0), (0, 255, 128),   (128, 128, 255),
    (255, 0, 200),  (0, 200, 128), (200, 128, 0),
]

MODEL_W  = 640
MODEL_H  = 640
REG_MAX  = 16         # YOLOv8 DFL reg_max
STRIDES  = [8, 16, 32]
BOX_CHANNELS = 4 * REG_MAX  # 64


# ── DFL + Host-side decode ─────────────────────────────────────────────────────
def _softmax(x: np.ndarray) -> np.ndarray:
    e = np.exp(x - x.max(axis=-1, keepdims=True))
    return e / (e.sum(axis=-1, keepdims=True) + 1e-9)


def _dfl_boxes(box_raw: np.ndarray) -> np.ndarray:
    """
    YOLOv8 Distributed Focal Loss box decode.

    box_raw : [..., 4 * REG_MAX]   raw Conv logits from cv2.X.2/Conv
    Returns : [..., 4]             l, t, r, b  distances in feature-map grid units
    """
    shape = box_raw.shape
    box_raw = box_raw.reshape(*shape[:-1], 4, REG_MAX)
    weights = _softmax(box_raw)
    proj = np.arange(REG_MAX, dtype=np.float32)
    return (weights * proj).sum(axis=-1)  # [..., 4]


def _nms(boxes: np.ndarray, scores: np.ndarray, iou_thresh: float) -> np.ndarray:
    """Simple numpy NMS. Returns kept indices."""
    if boxes.shape[0] == 0:
        return np.array([], dtype=int)
    x1, y1, x2, y2 = boxes[:, 0], boxes[:, 1], boxes[:, 2], boxes[:, 3]
    areas = np.maximum(0.0, x2 - x1) * np.maximum(0.0, y2 - y1)
    order = scores.argsort()[::-1]
    keep = []
    while order.size > 0:
        i = order[0]
        keep.append(i)
        if order.size == 1:
            break
        rest = order[1:]
        xx1 = np.maximum(x1[i], x1[rest])
        yy1 = np.maximum(y1[i], y1[rest])
        xx2 = np.minimum(x2[i], x2[rest])
        yy2 = np.minimum(y2[i], y2[rest])
        w = np.maximum(0.0, xx2 - xx1)
        h = np.maximum(0.0, yy2 - yy1)
        inter = w * h
        iou = inter / (areas[i] + areas[rest] - inter + 1e-6)
        order = rest[iou <= iou_thresh]
    return np.array(keep, dtype=int)


def _to_hwc(arr: np.ndarray) -> np.ndarray:
    """
    Convert a rank-3 tensor to HWC.
    Hailo may return CHW (C, H, W) or HWC (H, W, C).
    Heuristic: if shape[0] < shape[1] == shape[2] → CHW → transpose.
    (Feature maps are square: H == W, channels C < H.)
    """
    if arr.ndim != 3:
        return arr
    c, h, w = arr.shape
    if h == w and c < h:
        return arr.transpose(1, 2, 0)  # CHW → HWC
    return arr


def decode_byom(output_dict: dict, num_classes: int,
                conf_thresh: float, iou_thresh: float,
                min_box_size: float = 0.0,
                global_nms_iou: float = 0.0):
    """
    Decode 6 (detect) or 7 (seg) raw BYOM output tensors.

    For each of 3 strides there should be a pair:
      box tensor   : [H, W, 64]  — DFL logits
      class tensor : [H, W, nc]  — class logits

    Returns:
      boxes   : np.ndarray [N, 4]  xyxy in model-input pixels (640×640)
      scores  : np.ndarray [N]     confidence
      classes : np.ndarray [N]     class index (int)
    """
    # ── 1. Normalise all tensors to HWC, drop batch ──────────────────────────
    tensors = []
    for name, arr in output_dict.items():
        a = np.asarray(arr, dtype=np.float32)
        if a.ndim == 4:
            a = a[0]              # drop batch dim
        a = _to_hwc(a)
        if a.ndim == 3:
            tensors.append((name, a))

    # ── 2. Group by spatial area (H*W) ───────────────────────────────────────
    # Expected groups for 640-input: 6400 (80×80), 1600 (40×40), 400 (20×20)
    # Proto (if present): 25600 (160×160) → skip
    expected_areas = {s * s for s in [MODEL_H // st for st in STRIDES]}
    groups: dict[int, list[np.ndarray]] = {}
    for name, a in tensors:
        h, w, _ = a.shape
        if h != w:
            continue     # skip non-square (e.g. proto at 160×160 if H≠W — but it is square, handle below)
        area = h * w
        if area not in expected_areas:
            continue     # skip proto (160×160 = 25600, not expected)
        groups.setdefault(area, []).append(a)

    # ── 3. Per-stride decode ──────────────────────────────────────────────────
    all_boxes, all_scores, all_classes = [], [], []

    for stride in STRIDES:
        grid = MODEL_H // stride   # 80, 40, 20
        area = grid * grid
        group = groups.get(area)
        if not group:
            continue

        # Identify box vs class tensor by channel count.
        # Sprint13 detect models can expose:
        # - 64-ch box head (DFL)
        # - 4-ch box head (direct regression)
        box_t = cls_t = None
        for t in group:
            if t.shape[-1] in (BOX_CHANNELS, 4):
                box_t = t
            elif t.shape[-1] == num_classes:
                cls_t = t

        if box_t is None or cls_t is None:
            # Fallback: sort by channels descending; largest = box (64), next = class
            group_sorted = sorted(group, key=lambda x: x.shape[-1], reverse=True)
            if len(group_sorted) >= 2:
                box_t, cls_t = group_sorted[0], group_sorted[1]
            else:
                continue

        H, W = box_t.shape[:2]

        box_ch = int(box_t.shape[-1])

        # Box decode:
        # - 64 channels: DFL logits
        # - 4 channels : direct l/t/r/b regression
        if box_ch == BOX_CHANNELS:
            box_flat = box_t.reshape(-1, BOX_CHANNELS)
            ltrb = _dfl_boxes(box_flat)      # [H*W, 4]
        elif box_ch == 4:
            box_flat = box_t.reshape(-1, 4)
            # Keep distances non-negative for dist2bbox-style decoding.
            ltrb = np.maximum(box_flat, 0.0)
        else:
            continue

        # Anchor grid centres in input-pixel coordinates
        gy, gx = np.meshgrid(np.arange(H), np.arange(W), indexing='ij')
        cx = (gx.ravel() + 0.5) * stride    # [H*W]
        cy = (gy.ravel() + 0.5) * stride

        # xyxy in input pixels
        x1 = np.clip(cx - ltrb[:, 0] * stride, 0, MODEL_W)
        y1 = np.clip(cy - ltrb[:, 1] * stride, 0, MODEL_H)
        x2 = np.clip(cx + ltrb[:, 2] * stride, 0, MODEL_W)
        y2 = np.clip(cy + ltrb[:, 3] * stride, 0, MODEL_H)

        # Class probabilities
        cls_flat = cls_t.reshape(-1, num_classes)
        cls_prob = 1.0 / (1.0 + np.exp(-cls_flat))   # sigmoid [H*W, nc]
        best_cls = cls_prob.argmax(axis=-1)
        best_conf = cls_prob.max(axis=-1)

        mask = best_conf >= conf_thresh
        if mask.sum() == 0:
            continue

        all_boxes.append(np.stack([x1[mask], y1[mask], x2[mask], y2[mask]], axis=-1))
        all_scores.append(best_conf[mask])
        all_classes.append(best_cls[mask])

    if not all_boxes:
        return (np.empty((0, 4), np.float32),
                np.empty((0,), np.float32),
                np.empty((0,), np.int32))

    boxes   = np.vstack(all_boxes)
    scores  = np.concatenate(all_scores)
    classes = np.concatenate(all_classes).astype(np.int32)

    # Optional minimum size filter in normalized coordinates (Vasco-like).
    if min_box_size > 0.0:
        w = (boxes[:, 2] - boxes[:, 0]) / float(MODEL_W)
        h = (boxes[:, 3] - boxes[:, 1]) / float(MODEL_H)
        size_mask = (w >= min_box_size) & (h >= min_box_size)
        boxes = boxes[size_mask]
        scores = scores[size_mask]
        classes = classes[size_mask]
        if boxes.shape[0] == 0:
            return (np.empty((0, 4), np.float32),
                    np.empty((0,), np.float32),
                    np.empty((0,), np.int32))

    # Per-class NMS
    keep = []
    for cid in np.unique(classes):
        idx = np.where(classes == cid)[0]
        k = _nms(boxes[idx], scores[idx], iou_thresh)
        keep.extend(idx[k].tolist())

    if not keep:
        return (np.empty((0, 4), np.float32),
                np.empty((0,), np.float32),
                np.empty((0,), np.int32))

    keep = np.array(keep, dtype=int)
    boxes = boxes[keep]
    scores = scores[keep]
    classes = classes[keep]

    # Optional global NMS pass to suppress cross-class duplicates (Vasco-like).
    if global_nms_iou > 0.0 and boxes.shape[0] > 0:
        keep2 = _nms(boxes, scores, global_nms_iou)
        if keep2.size == 0:
            return (np.empty((0, 4), np.float32),
                    np.empty((0,), np.float32),
                    np.empty((0,), np.int32))
        boxes = boxes[keep2]
        scores = scores[keep2]
        classes = classes[keep2]

    return boxes, scores, classes


# ── Pre-processing ─────────────────────────────────────────────────────────────
def preprocess(frame_bgr: np.ndarray) -> np.ndarray:
    """Resize BGR frame to MODEL_H×MODEL_W, convert to RGB uint8."""
    img = cv2.resize(frame_bgr, (MODEL_W, MODEL_H), interpolation=cv2.INTER_LINEAR)
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    return img.astype(np.uint8)


# ── Drawing ────────────────────────────────────────────────────────────────────
def draw_detections(frame: np.ndarray, boxes, scores, classes,
                    class_names: list, frame_w: int, frame_h: int,
                    conf_decimals: int = 2) -> np.ndarray:
    """
    Draw bounding boxes on `frame` (already at frame_w × frame_h).
    Boxes are in model-input pixel space (640×640) → scale to frame dimensions.
    """
    sx = frame_w / MODEL_W
    sy = frame_h / MODEL_H
    for box, score, cid in zip(boxes, scores, classes):
        x1 = int(box[0] * sx)
        y1 = int(box[1] * sy)
        x2 = int(box[2] * sx)
        y2 = int(box[3] * sy)
        color = _PALETTE[int(cid) % len(_PALETTE)]
        cv2.rectangle(frame, (x1, y1), (x2, y2), color, 2)
        score_txt = f"{float(score):.{conf_decimals}f}"
        label = f"{class_names[int(cid)] if int(cid) < len(class_names) else cid} {score_txt}"
        (tw, th), _ = cv2.getTextSize(label, cv2.FONT_HERSHEY_SIMPLEX, 0.38, 1)
        bg_y1 = max(0, y1 - th - 4)
        cv2.rectangle(frame, (x1, bg_y1), (x1 + tw + 2, y1), color, -1)
        cv2.putText(frame, label, (x1 + 1, y1 - 3),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.38, (0, 0, 0), 1)
    return frame


def draw_overlay(frame: np.ndarray, fps: float, frame_idx: int,
                 t_pre: float, t_hailo: float, t_post: float,
                 num_dets: int, model_name: str) -> np.ndarray:
    h, w = frame.shape[:2]
    overlay = frame.copy()
    cv2.rectangle(overlay, (0, 0), (230, 100), (0, 0, 0), -1)
    cv2.addWeighted(overlay, 0.45, frame, 0.55, 0, frame)
    lines = [
        (f"FPS:    {fps:.1f}",              (0, 255, 0)),
        (f"Pre:    {t_pre:.1f} ms",         (200, 200, 200)),
        (f"Hailo:  {t_hailo:.1f} ms",       (200, 200, 200)),
        (f"Post:   {t_post:.1f} ms",        (200, 200, 200)),
        (f"Detec.: {num_dets}",             (255, 255, 255)),
        (f"Frame:  {frame_idx}",            (180, 180, 180)),
    ]
    for i, (text, color) in enumerate(lines):
        cv2.putText(frame, text, (5, 16 + i * 14),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.35, color, 1)
    cv2.putText(frame, f"SEAME | {model_name} | Sprint13",
                (w - 230, h - 6), cv2.FONT_HERSHEY_SIMPLEX, 0.30, (180, 180, 180), 1)
    return frame


# ── Async video writer ─────────────────────────────────────────────────────────
class AsyncVideoWriter:
    def __init__(self, path: str, fps: float, width: int, height: int):
        fourcc = cv2.VideoWriter_fourcc(*"mp4v")
        self._writer = cv2.VideoWriter(path, fourcc, fps, (width, height))
        if not self._writer.isOpened():
            raise RuntimeError(f"Cannot open video writer: {path}")
        self._queue  = queue.Queue(maxsize=120)
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

    def write(self, frame: np.ndarray):
        try:
            self._queue.put_nowait(frame)
        except queue.Full:
            pass  # drop frame rather than block

    def release(self):
        self._stop.set()
        self._thread.join(timeout=15)
        self._writer.release()


class SyncVideoWriter:
    """Blocking writer used for strict stage-by-stage timing (includes encode/write)."""

    def __init__(self, path: str, fps: float, width: int, height: int):
        fourcc = cv2.VideoWriter_fourcc(*"mp4v")
        self._writer = cv2.VideoWriter(path, fourcc, fps, (width, height))
        if not self._writer.isOpened():
            raise RuntimeError(f"Cannot open video writer: {path}")

    def write(self, frame: np.ndarray):
        self._writer.write(frame)

    def release(self):
        self._writer.release()


# ── Stats helper ───────────────────────────────────────────────────────────────
def _pct(values: list, p: float) -> float:
    if not values:
        return 0.0
    s = sorted(values)
    return s[min(len(s) - 1, int(len(s) * p / 100))]


# ── Main inference loop ────────────────────────────────────────────────────────
def run_inference(args):
    if HEF is None:
        raise RuntimeError("hailo_platform is required to run HEF inference on this script")

    class_names  = SEG_CLASSES if args.model_type == "seg" else DETECT_CLASSES
    num_classes  = len(class_names)
    model_label  = args.model_name or Path(args.hef).stem

    print(f"\n{'='*60}")
    print(f"  Sprint 13 Video Inference")
    print(f"  HEF        : {args.hef}")
    print(f"  Source     : {args.source}")
    print(f"  Model type : {args.model_type}  ({num_classes} classes)")
    print(f"  Conf/IOU   : {args.conf} / {args.iou}")
    print(f"  Output     : {args.output}")
    print(f"{'='*60}\n")

    # ── Open video source ─────────────────────────────────────────────────────
    cap = cv2.VideoCapture(args.source)
    if not cap.isOpened():
        raise RuntimeError(f"Cannot open source: {args.source}")

    src_fps    = cap.get(cv2.CAP_PROP_FPS) or 30.0
    src_w      = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    src_h      = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    total_frs  = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))

    print(f"Source: {src_w}×{src_h} @ {src_fps:.1f}fps  ({total_frs} frames)")

    # ── Prepare output paths ──────────────────────────────────────────────────
    out_video = args.output
    out_stats = str(Path(out_video).with_suffix("")) + "_stats.json"
    os.makedirs(str(Path(out_video).parent), exist_ok=True)

    if args.sync_writer:
        writer = SyncVideoWriter(out_video, src_fps, src_w, src_h)
        print("Writer mode  : sync (strict stage timing)")
    else:
        writer = AsyncVideoWriter(out_video, src_fps, src_w, src_h)
        print("Writer mode  : async (higher throughput)")

    # ── Load HEF and set up Hailo pipeline ───────────────────────────────────
    hef = HEF(args.hef)
    print(f"HEF loaded: {args.hef}")

    with VDevice() as target:
        cfg_params = ConfigureParams.create_from_hef(
            hef, interface=HailoStreamInterface.PCIe)
        net_groups = target.configure(hef, cfg_params)
        net_group  = net_groups[0]

        in_params  = InputVStreamParams.make(net_group, format_type=FormatType.UINT8)
        out_params = OutputVStreamParams.make(net_group, format_type=FormatType.FLOAT32)

        with net_group.activate():
            with InferVStreams(net_group, in_params, out_params) as pipeline:
                # Discover input stream name
                avail_inputs = list(
                    getattr(pipeline, "_input_name_to_network_name", {}).keys()
                )
                input_name = avail_inputs[0] if avail_inputs else None
                if input_name is None:
                    # Fallback: use HEF input info
                    input_name = hef.get_input_vstream_infos()[0].name
                print(f"Input stream : {input_name}")

                # Print all output stream names for debugging
                out_infos = hef.get_output_vstream_infos()
                print("Output streams:")
                for info in out_infos:
                    print(f"  {info.name}  shape={info.shape}")
                print()

                # ── Inference loop ────────────────────────────────────────────
                frame_idx   = 0
                times_decode = []
                times_total = []
                times_hailo = []
                times_render = []
                times_stage_total = []
                all_scores  = []
                fps_window  = []
                frame_stats = []

                print(f"{'Frame':<7} {'Pre':>6} {'Hailo':>8} {'Post':>7} {'Total':>8} {'Dets':>5}")
                print("─" * 52)

                t_start = time.time()
                try:
                    while True:
                        if args.max_frames and frame_idx >= args.max_frames:
                            break
                        t0 = time.time()
                        ret, frame_bgr = cap.read()
                        t_decode = (time.time() - t0) * 1000.0
                        if not ret:
                            break  # end of video
                        times_decode.append(t_decode)

                        # Pre-process
                        t0    = time.time()
                        img   = preprocess(frame_bgr)
                        t_pre = (time.time() - t0) * 1000.0

                        # Hailo inference
                        t0         = time.time()
                        input_data = {input_name: img[np.newaxis]}
                        raw_output = pipeline.infer(input_data)
                        t_hailo    = (time.time() - t0) * 1000.0

                        # Host-side decode + NMS
                        t0 = time.time()
                        boxes, scores, classes = decode_byom(
                            raw_output, num_classes,
                            args.conf, args.iou,
                            args.min_box_size,
                            args.global_nms_iou,
                        )
                        t_post = (time.time() - t0) * 1000.0

                        t_total  = t_pre + t_hailo + t_post
                        num_dets = len(boxes)
                        times_total.append(t_total)
                        times_hailo.append(t_hailo)
                        if num_dets > 0:
                            all_scores.extend([float(x) for x in np.asarray(scores).tolist()])

                        fps_window.append(1000.0 / (t_total + 1e-9))
                        if len(fps_window) > 30:
                            fps_window.pop(0)
                        fps_live = sum(fps_window) / len(fps_window)

                        if frame_idx % 30 == 0:
                            print(f"{frame_idx:<7} "
                                  f"{t_pre:>5.1f}ms "
                                  f"{t_hailo:>7.1f}ms "
                                  f"{t_post:>6.1f}ms "
                                  f"{t_total:>7.1f}ms "
                                  f"{num_dets:>5}")

                        # Annotate and write frame
                        t0 = time.time()
                        frame_out = draw_detections(
                            frame_bgr.copy(), boxes, scores, classes,
                            class_names, src_w, src_h,
                            conf_decimals=args.label_decimals,
                        )
                        frame_out = draw_overlay(
                            frame_out, fps_live, frame_idx,
                            t_pre, t_hailo, t_post, num_dets, model_label,
                        )
                        writer.write(frame_out)
                        t_render = (time.time() - t0) * 1000.0
                        times_render.append(t_render)

                        t_stage_total = t_decode + t_pre + t_hailo + t_post + t_render
                        times_stage_total.append(t_stage_total)

                        frame_stats.append({
                            "frame_idx":     frame_idx,
                            "decode_ms":     round(t_decode, 3),
                            "pre_ms":        round(t_pre, 3),
                            "hailo_ms":      round(t_hailo, 3),
                            "post_ms":       round(t_post, 3),
                            "render_ms":     round(t_render, 3),
                            "total_ms":      round(t_total, 3),
                            "stage_total_ms": round(t_stage_total, 3),
                            "num_detections": int(num_dets),
                            "mean_conf":     round(float(np.mean(scores)), 4) if num_dets > 0 else None,
                            "p50_conf":      round(float(np.median(scores)), 4) if num_dets > 0 else None,
                            "p95_conf":      round(float(np.percentile(scores, 95)), 4) if num_dets > 0 else None,
                            "pipeline_fps":  round(fps_live, 2),
                        })

                        frame_idx += 1

                except KeyboardInterrupt:
                    print("\nInterrompido pelo utilizador.")

    cap.release()
    print("\nA aguardar escrita do vídeo...")
    writer.release()

    # ── Summary stats ─────────────────────────────────────────────────────────
    elapsed  = time.time() - t_start
    real_fps = frame_idx / elapsed if elapsed > 0 else 0.0
    avg_tot  = statistics.mean(times_total)  if times_total  else 0.0
    avg_dec  = statistics.mean(times_decode) if times_decode else 0.0
    avg_hal  = statistics.mean(times_hailo) if times_hailo else 0.0
    avg_ren  = statistics.mean(times_render) if times_render else 0.0
    avg_stage_total = statistics.mean(times_stage_total) if times_stage_total else 0.0
    p50_tot  = _pct(times_total, 50)
    p95_tot  = _pct(times_total, 95)
    p99_tot  = _pct(times_total, 99)
    jitter   = statistics.pstdev(times_total) if len(times_total) > 1 else 0.0
    mean_conf = statistics.mean(all_scores) if all_scores else None
    min_conf = min(all_scores) if all_scores else None
    max_conf = max(all_scores) if all_scores else None
    p50_conf = statistics.median(all_scores) if all_scores else None
    p95_conf = np.percentile(np.array(all_scores, dtype=np.float32), 95).item() if all_scores else None

    summary = {
        "model":         model_label,
        "model_type":    args.model_type,
        "hef":           args.hef,
        "source":        args.source,
        "num_frames":    frame_idx,
        "elapsed_s":     round(elapsed, 2),
        "real_fps":      round(real_fps, 2),
        "pipeline_fps":  round(1000.0 / avg_tot, 2) if avg_tot > 0 else 0.0,
        "pipeline_fps_stage_total": round(1000.0 / avg_stage_total, 2) if avg_stage_total > 0 else 0.0,
        "avg_decode_ms": round(avg_dec, 2),
        "avg_hailo_ms":  round(avg_hal, 2),
        "avg_render_ms": round(avg_ren, 2),
        "avg_total_ms":  round(avg_tot, 2),
        "avg_stage_total_ms": round(avg_stage_total, 2),
        "p50_ms":        round(p50_tot, 2),
        "p95_ms":        round(p95_tot, 2),
        "p99_ms":        round(p99_tot, 2),
        "jitter_ms":     round(jitter, 3),
        "min_conf":      round(float(min_conf), 4) if min_conf is not None else None,
        "mean_conf":     round(float(mean_conf), 4) if mean_conf is not None else None,
        "max_conf":      round(float(max_conf), 4) if max_conf is not None else None,
        "p50_conf":      round(float(p50_conf), 4) if p50_conf is not None else None,
        "p95_conf":      round(float(p95_conf), 4) if p95_conf is not None else None,
        "conf_thresh":   args.conf,
        "iou_thresh":    args.iou,
        "min_box_size":  args.min_box_size,
        "global_nms_iou": args.global_nms_iou,
        "per_frame":     frame_stats,
    }

    with open(out_stats, "w") as f:
        json.dump(summary, f, indent=2)

    print("\n" + "─" * 60)
    print(f"  Frames processados : {frame_idx}  ({elapsed:.1f}s)")
    print(f"  FPS real           : {real_fps:.1f}")
    print(f"  FPS pipeline puro  : {1000.0/avg_tot:.1f}" if avg_tot > 0 else "")
    print(f"  Hailo médio        : {avg_hal:.1f} ms")
    print(f"  Latência total P50 : {p50_tot:.1f} ms")
    print(f"  Latência total P95 : {p95_tot:.1f} ms")
    print(f"  Latência total P99 : {p99_tot:.1f} ms")
    print(f"  Jitter (std)       : {jitter:.2f} ms")
    print(f"\n  Vídeo anotado      : {out_video}")
    print(f"  Stats JSON         : {out_stats}")
    print("─" * 60)


# ── Entry point ────────────────────────────────────────────────────────────────
def main():
    parser = argparse.ArgumentParser(
        description="Sprint 13 offline video inference — Hailo-8 BYOM HEFs (host NMS)",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument("--hef",        required=True,  help="Path to .hef file")
    parser.add_argument("--source",     required=True,  help="Input video path (or 0 for camera)")
    parser.add_argument("--output",     required=True,  help="Output annotated video path (.mp4)")
    parser.add_argument("--model-type", default="detect", choices=["detect", "seg"],
                        help="'detect' (17cls) or 'seg' (19cls)")
    parser.add_argument("--model-name", default=None,
                        help="Display label for overlay (defaults to HEF stem)")
    parser.add_argument("--conf",       type=float, default=0.45,
                        help="Confidence threshold")
    parser.add_argument("--iou",        type=float, default=0.55,
                        help="NMS IOU threshold")
    parser.add_argument("--min-box-size", type=float, default=0.0,
                        help="Minimum bbox size in normalized units (0 disables). Example: 0.04")
    parser.add_argument("--global-nms-iou", type=float, default=0.0,
                        help="Global NMS IOU after per-class NMS (0 disables). Example: 0.25")
    parser.add_argument("--sync-writer", action="store_true",
                        help="Use blocking writer to include encode/write cost in per-frame stage timing")
    parser.add_argument("--label-decimals", type=int, default=3,
                        help="Decimal places for confidence text in bbox labels")
    parser.add_argument("--max-frames", type=int, default=0,
                        help="Stop after N frames (0 = full video)")
    args = parser.parse_args()

    # Allow integer camera index as source
    try:
        args.source = int(args.source)
    except ValueError:
        pass  # keep as string path

    run_inference(args)


if __name__ == "__main__":
    main()
