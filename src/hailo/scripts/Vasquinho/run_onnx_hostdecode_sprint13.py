#!/usr/bin/env python3
"""
Run Sprint13 ONNX exported from .pt using the same host-side decode path as the HEF runtime.

Purpose:
- isolate compile/quantization effects by comparing ONNX pre-HEF vs HEF
- keep the same decode_byom, NMS, drawing, and confidence aggregation logic
"""

import argparse
import importlib.util
import json
import os
import statistics
import time
from pathlib import Path

import cv2
import numpy as np
import onnxruntime as ort


def _load_sprint13_module():
    here = Path(__file__).resolve()
    target = (here.parents[1] / "inference_video_sprint13.py").resolve()
    spec = importlib.util.spec_from_file_location("inference_video_sprint13", str(target))
    if spec is None or spec.loader is None:
        raise RuntimeError(f"Cannot load Sprint13 runtime module: {target}")
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def _pct(values, p):
    if not values:
        return 0.0
    s = sorted(values)
    return s[min(len(s) - 1, int(len(s) * p / 100))]


def _decode_export_output(mod, raw_output, num_classes, conf_thresh, iou_thresh,
                          min_box_size=0.0, global_nms_iou=0.0):
    """Decode Ultralytics-exported ONNX detect head: [1, 4+nc, N] or [1, N, 4+nc]."""
    if isinstance(raw_output, dict):
        if "output0" in raw_output:
            pred = np.asarray(raw_output["output0"], dtype=np.float32)
        else:
            # Fallback: choose first non-proto-like tensor.
            cand = None
            for v in raw_output.values():
                a = np.asarray(v)
                if a.ndim == 3:
                    cand = a
                    break
            if cand is None:
                return None
            pred = np.asarray(cand, dtype=np.float32)
    else:
        pred = np.asarray(raw_output, dtype=np.float32)

    if pred.ndim == 3:
        # Some ONNX exports already include NMS and return [1, N, 6]
        # where columns are typically [x1, y1, x2, y2, score, class_id].
        # Guard against [1, C, 8400] style raw heads used by non-NMS exports.
        if 6 <= pred.shape[-1] <= 128:
            det = pred[0]
            if det.size == 0:
                return (np.empty((0, 4), np.float32),
                        np.empty((0,), np.float32),
                        np.empty((0,), np.int32))

            boxes = det[:, :4].astype(np.float32)
            scores = det[:, 4].astype(np.float32)
            classes = det[:, 5].astype(np.int32)

            keep = scores >= conf_thresh
            boxes = boxes[keep]
            scores = scores[keep]
            classes = classes[keep]
            if boxes.shape[0] == 0:
                return (np.empty((0, 4), np.float32),
                        np.empty((0,), np.float32),
                        np.empty((0,), np.int32))

            boxes[:, [0, 2]] = np.clip(boxes[:, [0, 2]], 0, mod.MODEL_W)
            boxes[:, [1, 3]] = np.clip(boxes[:, [1, 3]], 0, mod.MODEL_H)

            if min_box_size > 0.0:
                bw = (boxes[:, 2] - boxes[:, 0]) / float(mod.MODEL_W)
                bh = (boxes[:, 3] - boxes[:, 1]) / float(mod.MODEL_H)
                size_mask = (bw >= min_box_size) & (bh >= min_box_size)
                boxes = boxes[size_mask]
                scores = scores[size_mask]
                classes = classes[size_mask]
                if boxes.shape[0] == 0:
                    return (np.empty((0, 4), np.float32),
                            np.empty((0,), np.float32),
                            np.empty((0,), np.int32))

            # Apply per-class NMS for consistency with HEF path.
            keep_idx = []
            for cid in np.unique(classes):
                idx = np.where(classes == cid)[0]
                k = mod._nms(boxes[idx], scores[idx], iou_thresh)
                keep_idx.extend(idx[k].tolist())

            if not keep_idx:
                return (np.empty((0, 4), np.float32),
                        np.empty((0,), np.float32),
                        np.empty((0,), np.int32))

            keep_idx = np.asarray(keep_idx, dtype=int)
            boxes = boxes[keep_idx]
            scores = scores[keep_idx]
            classes = classes[keep_idx]

            if global_nms_iou > 0.0 and boxes.shape[0] > 0:
                keep2 = mod._nms(boxes, scores, global_nms_iou)
                boxes = boxes[keep2]
                scores = scores[keep2]
                classes = classes[keep2]

            return boxes.astype(np.float32), scores.astype(np.float32), classes.astype(np.int32)

        pred = pred[0]

    if pred.ndim != 2:
        return None

    if pred.shape[0] >= 4 + num_classes:
        pred = pred.transpose(1, 0)
    elif pred.shape[1] < 4 + num_classes:
        return None

    if pred.shape[1] < 5:
        return None

    boxes_xywh = pred[:, :4]
    cls_scores = pred[:, 4:4 + num_classes]
    if cls_scores.size == 0:
        return None

    # Exported ONNX often already contains class probabilities; fall back to sigmoid if needed.
    if cls_scores.max() > 1.0 or cls_scores.min() < 0.0:
        cls_scores = 1.0 / (1.0 + np.exp(-cls_scores))

    classes = cls_scores.argmax(axis=1).astype(np.int32)
    scores = cls_scores.max(axis=1)
    keep = scores >= conf_thresh
    if not np.any(keep):
        return (np.empty((0, 4), np.float32),
                np.empty((0,), np.float32),
                np.empty((0,), np.int32))

    boxes_xywh = boxes_xywh[keep]
    scores = scores[keep]
    classes = classes[keep]

    x, y, w, h = boxes_xywh[:, 0], boxes_xywh[:, 1], boxes_xywh[:, 2], boxes_xywh[:, 3]
    boxes = np.stack([x - w / 2.0, y - h / 2.0, x + w / 2.0, y + h / 2.0], axis=1)
    boxes[:, [0, 2]] = np.clip(boxes[:, [0, 2]], 0, mod.MODEL_W)
    boxes[:, [1, 3]] = np.clip(boxes[:, [1, 3]], 0, mod.MODEL_H)

    if min_box_size > 0.0:
        bw = (boxes[:, 2] - boxes[:, 0]) / float(mod.MODEL_W)
        bh = (boxes[:, 3] - boxes[:, 1]) / float(mod.MODEL_H)
        size_mask = (bw >= min_box_size) & (bh >= min_box_size)
        boxes = boxes[size_mask]
        scores = scores[size_mask]
        classes = classes[size_mask]
        if boxes.shape[0] == 0:
            return (np.empty((0, 4), np.float32),
                    np.empty((0,), np.float32),
                    np.empty((0,), np.int32))

    keep_idx = []
    for cid in np.unique(classes):
        idx = np.where(classes == cid)[0]
        k = mod._nms(boxes[idx], scores[idx], iou_thresh)
        keep_idx.extend(idx[k].tolist())

    if not keep_idx:
        return (np.empty((0, 4), np.float32),
                np.empty((0,), np.float32),
                np.empty((0,), np.int32))

    keep_idx = np.asarray(keep_idx, dtype=int)
    boxes = boxes[keep_idx]
    scores = scores[keep_idx]
    classes = classes[keep_idx]

    if global_nms_iou > 0.0 and boxes.shape[0] > 0:
        keep2 = mod._nms(boxes, scores, global_nms_iou)
        boxes = boxes[keep2]
        scores = scores[keep2]
        classes = classes[keep2]

    return boxes.astype(np.float32), scores.astype(np.float32), classes.astype(np.int32)


def run(args):
    mod = _load_sprint13_module()
    class_names = mod.SEG_CLASSES if args.model_type == "seg" else mod.DETECT_CLASSES
    num_classes = len(class_names)
    model_label = args.model_name or Path(args.onnx).stem

    cap = cv2.VideoCapture(args.source)
    if not cap.isOpened():
        raise RuntimeError(f"Cannot open source: {args.source}")

    src_fps = cap.get(cv2.CAP_PROP_FPS) or 30.0
    src_w = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    src_h = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    total_frs = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))

    out_video = args.output
    out_stats = str(Path(out_video).with_suffix("")) + "_stats.json"
    os.makedirs(str(Path(out_video).parent), exist_ok=True)

    writer = mod.SyncVideoWriter(out_video, src_fps, src_w, src_h) if args.sync_writer else mod.AsyncVideoWriter(out_video, src_fps, src_w, src_h)

    sess = ort.InferenceSession(args.onnx, providers=["CPUExecutionProvider"])
    input_meta = sess.get_inputs()[0]
    input_name = input_meta.name

    times_decode = []
    times_total = []
    times_onnx = []
    times_render = []
    times_stage_total = []
    fps_window = []
    frame_stats = []
    all_scores = []
    frame_idx = 0
    t_start = time.time()

    print(f"\n{'='*60}")
    print("  Sprint 13 ONNX Host Decode Inference")
    print(f"  ONNX       : {args.onnx}")
    print(f"  Source     : {args.source}")
    print(f"  Model type : {args.model_type} ({num_classes} classes)")
    print(f"  Conf/IOU   : {args.conf} / {args.iou}")
    print(f"  Output     : {args.output}")
    print(f"  Frames     : {total_frs}")
    print(f"{'='*60}\n")

    try:
        while True:
            if args.max_frames and frame_idx >= args.max_frames:
                break
            t0 = time.time()
            ret, frame_bgr = cap.read()
            t_decode = (time.time() - t0) * 1000.0
            if not ret:
                break
            times_decode.append(t_decode)

            t0 = time.time()
            img = mod.preprocess(frame_bgr)
            inp = img.astype(np.float32).transpose(2, 0, 1)[None] / 255.0
            t_pre = (time.time() - t0) * 1000.0

            t0 = time.time()
            raw_list = sess.run(None, {input_name: inp})
            t_onnx = (time.time() - t0) * 1000.0

            raw_output = {meta.name: arr for meta, arr in zip(sess.get_outputs(), raw_list)}

            t0 = time.time()
            decoded = _decode_export_output(
                mod,
                raw_output,
                num_classes,
                args.conf,
                args.iou,
                args.min_box_size,
                args.global_nms_iou,
            )
            if decoded is None:
                boxes, scores, classes = mod.decode_byom(
                    raw_output,
                    num_classes,
                    args.conf,
                    args.iou,
                    args.min_box_size,
                    args.global_nms_iou,
                )
            else:
                boxes, scores, classes = decoded
            t_post = (time.time() - t0) * 1000.0

            t_total = t_pre + t_onnx + t_post
            num_dets = len(boxes)
            times_total.append(t_total)
            times_onnx.append(t_onnx)
            if num_dets > 0:
                all_scores.extend([float(x) for x in np.asarray(scores).tolist()])

            fps_window.append(1000.0 / (t_total + 1e-9))
            if len(fps_window) > 30:
                fps_window.pop(0)
            fps_live = sum(fps_window) / len(fps_window)

            t0 = time.time()
            frame_out = mod.draw_detections(
                frame_bgr.copy(), boxes, scores, classes,
                class_names, src_w, src_h,
                conf_decimals=args.label_decimals,
            )
            frame_out = mod.draw_overlay(
                frame_out, fps_live, frame_idx,
                t_pre, t_onnx, t_post, num_dets, model_label,
            )
            writer.write(frame_out)
            t_render = (time.time() - t0) * 1000.0
            times_render.append(t_render)

            t_stage_total = t_decode + t_pre + t_onnx + t_post + t_render
            times_stage_total.append(t_stage_total)

            frame_stats.append({
                "frame_idx": frame_idx,
                "decode_ms": round(t_decode, 3),
                "pre_ms": round(t_pre, 3),
                "onnx_ms": round(t_onnx, 3),
                "post_ms": round(t_post, 3),
                "render_ms": round(t_render, 3),
                "total_ms": round(t_total, 3),
                "stage_total_ms": round(t_stage_total, 3),
                "num_detections": int(num_dets),
                "mean_conf": round(float(np.mean(scores)), 4) if num_dets > 0 else None,
                "p50_conf": round(float(np.median(scores)), 4) if num_dets > 0 else None,
                "p95_conf": round(float(np.percentile(scores, 95)), 4) if num_dets > 0 else None,
                "pipeline_fps": round(fps_live, 2),
            })

            frame_idx += 1

    finally:
        cap.release()
        writer.release()

    elapsed = time.time() - t_start
    real_fps = frame_idx / elapsed if elapsed > 0 else 0.0
    avg_tot = statistics.mean(times_total) if times_total else 0.0
    avg_dec = statistics.mean(times_decode) if times_decode else 0.0
    avg_onnx = statistics.mean(times_onnx) if times_onnx else 0.0
    avg_ren = statistics.mean(times_render) if times_render else 0.0
    avg_stage_total = statistics.mean(times_stage_total) if times_stage_total else 0.0
    p50_tot = _pct(times_total, 50)
    p95_tot = _pct(times_total, 95)
    p99_tot = _pct(times_total, 99)
    jitter = statistics.pstdev(times_total) if len(times_total) > 1 else 0.0
    mean_conf = statistics.mean(all_scores) if all_scores else None
    min_conf = min(all_scores) if all_scores else None
    max_conf = max(all_scores) if all_scores else None
    p50_conf = statistics.median(all_scores) if all_scores else None
    p95_conf = np.percentile(np.array(all_scores, dtype=np.float32), 95).item() if all_scores else None

    summary = {
        "model": model_label,
        "model_type": args.model_type,
        "backend": "onnx_hostdecode",
        "onnx": args.onnx,
        "source": args.source,
        "num_frames": frame_idx,
        "elapsed_s": round(elapsed, 2),
        "real_fps": round(real_fps, 2),
        "pipeline_fps": round(1000.0 / avg_tot, 2) if avg_tot > 0 else 0.0,
        "pipeline_fps_stage_total": round(1000.0 / avg_stage_total, 2) if avg_stage_total > 0 else 0.0,
        "avg_decode_ms": round(avg_dec, 2),
        "avg_onnx_ms": round(avg_onnx, 2),
        "avg_render_ms": round(avg_ren, 2),
        "avg_total_ms": round(avg_tot, 2),
        "avg_stage_total_ms": round(avg_stage_total, 2),
        "p50_ms": round(p50_tot, 2),
        "p95_ms": round(p95_tot, 2),
        "p99_ms": round(p99_tot, 2),
        "jitter_ms": round(jitter, 3),
        "min_conf": round(float(min_conf), 4) if min_conf is not None else None,
        "mean_conf": round(float(mean_conf), 4) if mean_conf is not None else None,
        "max_conf": round(float(max_conf), 4) if max_conf is not None else None,
        "p50_conf": round(float(p50_conf), 4) if p50_conf is not None else None,
        "p95_conf": round(float(p95_conf), 4) if p95_conf is not None else None,
        "conf_thresh": args.conf,
        "iou_thresh": args.iou,
        "min_box_size": args.min_box_size,
        "global_nms_iou": args.global_nms_iou,
        "per_frame": frame_stats,
    }

    with open(out_stats, "w", encoding="utf-8") as f:
        json.dump(summary, f, indent=2)

    print(f"Saved video: {out_video}")
    print(f"Saved stats: {out_stats}")


def main():
    parser = argparse.ArgumentParser(
        description="Sprint13 ONNX inference with the same host-side decode used by HEF runtime",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument("--onnx", required=True, help="Path to ONNX file exported before HEF compile")
    parser.add_argument("--source", required=True, help="Input video path")
    parser.add_argument("--output", required=True, help="Output annotated video path (.mp4)")
    parser.add_argument("--model-type", default="detect", choices=["detect", "seg"])
    parser.add_argument("--model-name", default=None)
    parser.add_argument("--conf", type=float, default=0.45)
    parser.add_argument("--iou", type=float, default=0.55)
    parser.add_argument("--min-box-size", type=float, default=0.0)
    parser.add_argument("--global-nms-iou", type=float, default=0.0)
    parser.add_argument("--sync-writer", action="store_true")
    parser.add_argument("--label-decimals", type=int, default=3)
    parser.add_argument("--max-frames", type=int, default=0,
                        help="Stop after N frames (0 = full video)")
    args = parser.parse_args()
    run(args)


if __name__ == "__main__":
    main()