#!/usr/bin/env python3
"""
Offline runner with Vasco original visual style (green boxes/text) and Vasco-like postprocess.

Purpose:
- Reproduce Vasco-like rendering behavior on Sprint13 HEFs.
- Keep postprocess knobs explicit for A/B checks.
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

from hailo_platform import (
    HEF,
    VDevice,
    HailoStreamInterface,
    InferVStreams,
    ConfigureParams,
    InputVStreamParams,
    OutputVStreamParams,
    FormatType,
)

DETECT_CLASSES = [
    "50_maxspeed", "80_maxspeed", "Crosswalk", "Gate",
    "Pedestrians_crossing", "Stop_sign", "Traffic_priority",
    "both_arrow", "car", "cars not allowed", "left_cross",
    "obstacle", "right_cross", "traffic_lights_green",
    "traffic_lights_off", "traffic_lights_red", "traffic_lights_yellow",
]

SEG_CLASSES = [
    "50_maxspeed_sign", "80_maxspeed_sign", "arrow_left", "arrow_left_front",
    "arrow_right", "car", "crosswalk", "crosswalk_sign", "curve", "gate",
    "obstacle", "shadow", "stop_sign", "traffic_danger_sign",
    "traffic_light_green", "traffic_light_off", "traffic_light_red",
    "traffic_light_yellow", "traffic_priority_yield_sign",
]

BOX_COLOR = (0, 255, 0)
TEXT_COLOR = (0, 255, 0)

MODEL_W = 640
MODEL_H = 640
STRIDES = [8, 16, 32]
REG_MAX = 16
BOX_CHANNELS = 4 * REG_MAX


def _softmax(x: np.ndarray) -> np.ndarray:
    e = np.exp(x - x.max(axis=-1, keepdims=True))
    return e / (e.sum(axis=-1, keepdims=True) + 1e-9)


def _to_hwc(arr: np.ndarray) -> np.ndarray:
    if arr.ndim != 3:
        return arr
    c, h, w = arr.shape
    if h == w and c < h:
        return arr.transpose(1, 2, 0)
    return arr


def _nms(boxes: np.ndarray, scores: np.ndarray, iou_threshold: float) -> np.ndarray:
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
        inter = np.maximum(0.0, xx2 - xx1) * np.maximum(0.0, yy2 - yy1)
        iou = inter / (areas[i] + areas[rest] - inter + 1e-9)
        order = rest[iou <= iou_threshold]

    return np.array(keep, dtype=int)


def decode_vasco_style(output_dict: dict, num_classes: int, conf_thresh: float,
                       iou_thresh: float, min_box_size: float, global_nms_iou: float):
    tensors = []
    for _, arr in output_dict.items():
        a = np.asarray(arr, dtype=np.float32)
        if a.ndim == 4:
            a = a[0]
        a = _to_hwc(a)
        if a.ndim == 3:
            tensors.append(a)

    expected_areas = {s * s for s in [MODEL_H // st for st in STRIDES]}
    groups = {}
    for a in tensors:
        h, w, _ = a.shape
        if h != w:
            continue
        area = h * w
        if area not in expected_areas:
            continue
        groups.setdefault(area, []).append(a)

    boxes_all, scores_all, classes_all = [], [], []

    for stride in STRIDES:
        grid = MODEL_H // stride
        area = grid * grid
        group = groups.get(area, [])
        if not group:
            continue

        box_t = cls_t = None
        for t in group:
            ch = int(t.shape[-1])
            if ch in (BOX_CHANNELS, 4):
                box_t = t
            elif ch == num_classes:
                cls_t = t

        if box_t is None or cls_t is None:
            continue

        h, w = box_t.shape[:2]
        box_ch = int(box_t.shape[-1])
        if box_ch == BOX_CHANNELS:
            reg = box_t.reshape(-1, 4, REG_MAX)
            reg = _softmax(reg)
            reg = (reg * np.arange(REG_MAX, dtype=np.float32)).sum(axis=-1)
        elif box_ch == 4:
            reg = np.maximum(box_t.reshape(-1, 4), 0.0)
        else:
            continue

        gy, gx = np.meshgrid(np.arange(h), np.arange(w), indexing="ij")
        cx = (gx.ravel() + 0.5) * stride
        cy = (gy.ravel() + 0.5) * stride

        x1 = np.clip((cx - reg[:, 0] * stride) / MODEL_W, 0.0, 1.0)
        y1 = np.clip((cy - reg[:, 1] * stride) / MODEL_H, 0.0, 1.0)
        x2 = np.clip((cx + reg[:, 2] * stride) / MODEL_W, 0.0, 1.0)
        y2 = np.clip((cy + reg[:, 3] * stride) / MODEL_H, 0.0, 1.0)

        cls_prob = 1.0 / (1.0 + np.exp(-cls_t.reshape(-1, num_classes)))
        best_cls = cls_prob.argmax(axis=1)
        best_conf = cls_prob.max(axis=1)

        mask = best_conf >= conf_thresh
        if not np.any(mask):
            continue

        boxes_all.append(np.stack([x1[mask], y1[mask], x2[mask], y2[mask]], axis=1))
        scores_all.append(best_conf[mask])
        classes_all.append(best_cls[mask])

    if not boxes_all:
        return np.empty((0, 4), np.float32), np.empty((0,), np.float32), np.empty((0,), np.int32)

    boxes = np.vstack(boxes_all)
    scores = np.concatenate(scores_all)
    classes = np.concatenate(classes_all).astype(np.int32)

    if min_box_size > 0:
        bw = boxes[:, 2] - boxes[:, 0]
        bh = boxes[:, 3] - boxes[:, 1]
        size_mask = (bw >= min_box_size) & (bh >= min_box_size)
        boxes = boxes[size_mask]
        scores = scores[size_mask]
        classes = classes[size_mask]
        if boxes.shape[0] == 0:
            return np.empty((0, 4), np.float32), np.empty((0,), np.float32), np.empty((0,), np.int32)

    keep_final = []
    for cid in np.unique(classes):
        idx = np.where(classes == cid)[0]
        keep = _nms(boxes[idx], scores[idx], iou_thresh)
        keep_final.extend(idx[keep].tolist())

    if not keep_final:
        return np.empty((0, 4), np.float32), np.empty((0,), np.float32), np.empty((0,), np.int32)

    keep_final = np.array(keep_final, dtype=int)
    boxes = boxes[keep_final]
    scores = scores[keep_final]
    classes = classes[keep_final]

    if global_nms_iou > 0 and boxes.shape[0] > 0:
        keep2 = _nms(boxes, scores, global_nms_iou)
        boxes = boxes[keep2]
        scores = scores[keep2]
        classes = classes[keep2]

    return boxes, scores, classes


def preprocess(frame):
    img = cv2.resize(frame, (MODEL_W, MODEL_H), interpolation=cv2.INTER_LINEAR)
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    return img.astype(np.uint8)


def draw_detections(frame, boxes_norm, scores, classes, class_names, decimals):
    h, w = frame.shape[:2]
    for box, score, cls_id in zip(boxes_norm, scores, classes):
        x1 = int(box[0] * w)
        y1 = int(box[1] * h)
        x2 = int(box[2] * w)
        y2 = int(box[3] * h)

        cv2.rectangle(frame, (x1, y1), (x2, y2), BOX_COLOR, 2)
        label_name = class_names[int(cls_id)] if int(cls_id) < len(class_names) else str(int(cls_id))
        label = f"{label_name}:{float(score):.{decimals}f}"

        font = cv2.FONT_HERSHEY_SIMPLEX
        font_scale = 0.55
        thickness = 2
        (tw, th), baseline = cv2.getTextSize(label, font, font_scale, thickness)
        ty = max(th + 4, y1 - 4)
        cv2.rectangle(frame, (x1, ty - th - baseline - 2), (x1 + tw + 4, ty + 2), (0, 0, 0), -1)
        cv2.putText(frame, label, (x1 + 2, ty - baseline), font, font_scale, TEXT_COLOR, thickness)

    return frame


class AsyncVideoWriter:
    def __init__(self, path, fps, width, height):
        self._queue = queue.Queue(maxsize=60)
        self._writer = cv2.VideoWriter(path, cv2.VideoWriter_fourcc(*"mp4v"), fps, (width, height))
        if not self._writer.isOpened():
            raise RuntimeError(f"Cannot create output video: {path}")
        self._stop = threading.Event()
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
        self._thread.join(timeout=15)
        self._writer.release()


def run(args):
    class_names = SEG_CLASSES if args.model_type == "seg" else DETECT_CLASSES
    num_classes = len(class_names)

    cap = cv2.VideoCapture(args.source)
    if not cap.isOpened():
        raise RuntimeError(f"Cannot open source: {args.source}")

    src_fps = cap.get(cv2.CAP_PROP_FPS) or 30.0
    src_w = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    src_h = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))

    os.makedirs(str(Path(args.output).parent), exist_ok=True)
    out_stats = str(Path(args.output).with_suffix("")) + "_stats.json"

    writer = AsyncVideoWriter(args.output, src_fps, src_w, src_h)

    hef = HEF(args.hef)

    frame_idx = 0
    times_total = []
    all_scores = []

    with VDevice() as target:
        cfg = ConfigureParams.create_from_hef(hef, interface=HailoStreamInterface.PCIe)
        network_group = target.configure(hef, cfg)[0]
        in_params = InputVStreamParams.make(network_group, format_type=FormatType.UINT8)
        out_params = OutputVStreamParams.make(network_group, format_type=FormatType.FLOAT32)

        with network_group.activate():
            with InferVStreams(network_group, in_params, out_params) as pipeline:
                input_name = hef.get_input_vstream_infos()[0].name

                t_start = time.time()
                while True:
                    ret, frame = cap.read()
                    if not ret:
                        break

                    t0 = time.time()
                    img = preprocess(frame)
                    raw = pipeline.infer({input_name: img[np.newaxis]})
                    boxes, scores, classes = decode_vasco_style(
                        raw,
                        num_classes=num_classes,
                        conf_thresh=args.conf,
                        iou_thresh=args.iou,
                        min_box_size=args.min_box_size,
                        global_nms_iou=args.global_nms_iou,
                    )
                    total_ms = (time.time() - t0) * 1000.0
                    times_total.append(total_ms)
                    if scores.size > 0:
                        all_scores.extend([float(x) for x in scores.tolist()])

                    out = draw_detections(frame.copy(), boxes, scores, classes, class_names, args.label_decimals)
                    writer.write(out)

                    frame_idx += 1

    cap.release()
    writer.release()

    elapsed = max(1e-9, time.time() - t_start)
    real_fps = frame_idx / elapsed
    mean_conf = statistics.mean(all_scores) if all_scores else None
    p50_conf = statistics.median(all_scores) if all_scores else None
    p95_conf = float(np.percentile(np.array(all_scores, dtype=np.float32), 95)) if all_scores else None

    summary = {
        "model": args.model_name,
        "model_type": args.model_type,
        "hef": args.hef,
        "source": args.source,
        "num_frames": frame_idx,
        "elapsed_s": round(elapsed, 2),
        "real_fps": round(real_fps, 2),
        "avg_total_ms": round(statistics.mean(times_total), 2) if times_total else 0.0,
        "mean_conf": round(mean_conf, 4) if mean_conf is not None else None,
        "p50_conf": round(p50_conf, 4) if p50_conf is not None else None,
        "p95_conf": round(p95_conf, 4) if p95_conf is not None else None,
        "conf_thresh": args.conf,
        "iou_thresh": args.iou,
        "min_box_size": args.min_box_size,
        "global_nms_iou": args.global_nms_iou,
    }

    with open(out_stats, "w", encoding="utf-8") as f:
        json.dump(summary, f, indent=2)

    print(f"Saved video: {args.output}")
    print(f"Saved stats: {out_stats}")


def main():
    parser = argparse.ArgumentParser(description="Vasco original-style offline runner")
    parser.add_argument("--hef", required=True)
    parser.add_argument("--source", required=True)
    parser.add_argument("--output", required=True)
    parser.add_argument("--model-type", default="detect", choices=["detect", "seg"])
    parser.add_argument("--model-name", default="vasco_original_style")
    parser.add_argument("--conf", type=float, default=0.30)
    parser.add_argument("--iou", type=float, default=0.40)
    parser.add_argument("--min-box-size", type=float, default=0.04)
    parser.add_argument("--global-nms-iou", type=float, default=0.25)
    parser.add_argument("--label-decimals", type=int, default=2)
    args = parser.parse_args()
    run(args)


if __name__ == "__main__":
    main()
