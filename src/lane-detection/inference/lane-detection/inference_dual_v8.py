#!/usr/bin/env python3
"""
SEAME — Dual Inference Pipeline v8
  • UFLDv2 (cut v2)   — Lane Detection   — Hailo-8 PCIe
  • best_model        — Object Detection — Hailo-8 PCIe

Único VDevice, dois network groups, inferência sequencial por frame.
Optimizações acumuladas:
  v7: undistort no frame pequeno (800×320) — poupa ~6ms
  v8: YPre corre em thread CPU durante LHailo (NPU idle) — poupa ~4ms
"""

import numpy as np
import cv2
import threading
import queue
import time
import sys
import os
import argparse
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from hailo_platform import (HEF, VDevice, HailoStreamInterface,
                             InferVStreams, ConfigureParams,
                             InputVStreamParams, OutputVStreamParams,
                             FormatType)

# ── Módulos lane-detection ────────────────────────────────────────────────────
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                "lane-detection"))
from config import (
    HEF_PATH as LANE_HEF_PATH,
    WEIGHTS_PATH,
    FULL_W, FULL_H, CAM_FPS,
    TRAIN_WIDTH, TRAIN_HEIGHT, CROP_RATIO,
    INPUT_STREAM  as LANE_INPUT,
    OUTPUT_LAYER  as LANE_OUTPUT,
    EXIST_THRESHOLD, TEMPORAL_HISTORY, TEMPORAL_MIN_HITS,
    CAMERA_OFFSET_CM,
)
from postprocess import (
    load_weights, postprocess, decode_lanes,
    calc_lateral_deviation, TemporalLaneSmoother, smooth_lanes,
)
from visualization import draw_lanes, draw_drivable_area, draw_overlay as draw_overlay_lane


# ══════════════════════════════════════════════════════════════════════════════
# Configuração YOLO (best_model)
# ══════════════════════════════════════════════════════════════════════════════
YOLO_HEF_PATH = "/data/yolov8s.hef"
OUTPUT_PATH   = "/data/demo_dual_v8.mp4"

MODEL_W = 640
MODEL_H = 640

YOLO_INPUT   = "best_model/input_layer1"
CONF_THRESH  = 0.30
IOU_THRESH   = 0.40
MIN_BOX_SIZE = 0.04

YOLO_STREAMS = [
    ("best_model/conv41", "best_model/conv42", 8),
    ("best_model/conv52", "best_model/conv53", 16),
    ("best_model/conv62", "best_model/conv63", 32),
]
REG_MAX = 16

BOX_COLOR  = (0, 255, 255)
TEXT_COLOR = (0, 255, 255)

CALIB_DIR_FRONT = Path("/data/seame-configs/camera")
CALIB_DIR_REAR  = Path("/data/seame-configs/camera_rear")
INTRINSIC_DIR   = Path("/data/seame-configs/camera")

# ── Constantes de preprocess (necessárias antes da classe Calibration) ────────
_resize_h   = int(TRAIN_HEIGHT / CROP_RATIO)
_top_native = round(FULL_H * ((_resize_h - TRAIN_HEIGHT) / _resize_h))


# ══════════════════════════════════════════════════════════════════════════════
# Calibração — mapas computados directamente para 800×320
# ══════════════════════════════════════════════════════════════════════════════
class Calibration:
    def __init__(self, is_rear=False):
        self.enabled      = False
        self._small_map1  = None
        self._small_map2  = None
        self.H_img2world  = None

        calib_dir = CALIB_DIR_REAR if is_rear else CALIB_DIR_FRONT
        cam_label = "REAR" if is_rear else "FRONT"

        files = {
            "camera_matrix.npy":        INTRINSIC_DIR / "camera_matrix.npy",
            "dist_coeffs.npy":          INTRINSIC_DIR / "dist_coeffs.npy",
            "homography_img2world.npy":  calib_dir    / "homography_img2world.npy",
        }
        missing = [n for n, p in files.items() if not p.exists()]
        if missing:
            print(f"[CALIB] [{cam_label}] Ficheiros em falta: {', '.join(missing)}")
            print(f"[CALIB] A correr SEM calibração")
            return

        camera_matrix    = np.load(files["camera_matrix.npy"])
        dist_coeffs      = np.load(files["dist_coeffs.npy"])
        self.H_img2world = np.load(files["homography_img2world.npy"])

        # Camera matrix optimizada para o frame completo
        new_matrix, _ = cv2.getOptimalNewCameraMatrix(
            camera_matrix, dist_coeffs, (FULL_W, FULL_H), 0, (FULL_W, FULL_H))

        # Ajustar K para o ROI da lane: crop y=_top_native + resize para TRAIN_WIDTH×TRAIN_HEIGHT
        scale_x = TRAIN_WIDTH  / FULL_W
        scale_y = TRAIN_HEIGHT / (FULL_H - _top_native)

        K_small = new_matrix.copy()
        K_small[0, 0] *= scale_x                                       # fx
        K_small[1, 1] *= scale_y                                       # fy
        K_small[0, 2] *= scale_x                                       # cx
        K_small[1, 2]  = (new_matrix[1, 2] - _top_native) * scale_y   # cy

        self._small_map1, self._small_map2 = cv2.initUndistortRectifyMap(
            camera_matrix, dist_coeffs, None, K_small,
            (TRAIN_WIDTH, TRAIN_HEIGHT), cv2.CV_16SC2)

        self.enabled = True
        print(f"[CALIB] [{cam_label}] OK — small maps {TRAIN_WIDTH}×{TRAIN_HEIGHT} "
              f"+ extrinsic ({calib_dir.name}/)")

    def undistort_small(self, img_bgr):
        """Undistort num frame já cropado e resized para TRAIN_WIDTH×TRAIN_HEIGHT (BGR)."""
        if not self.enabled:
            return img_bgr
        return cv2.remap(img_bgr, self._small_map1, self._small_map2, cv2.INTER_LINEAR)

    def pixels_to_world(self, points_px):
        if not self.enabled or self.H_img2world is None:
            return None
        pts = np.array(points_px, dtype=np.float32).reshape(-1, 1, 2)
        return cv2.perspectiveTransform(pts, self.H_img2world).reshape(-1, 2)


def calc_lateral_deviation_cm(lanes, calib):
    def bottom_points(pts, n=5):
        if len(pts) < 3:
            return None
        return sorted(pts, key=lambda p: p[1], reverse=True)[:n]

    left_pts  = bottom_points(lanes[1])
    right_pts = bottom_points(lanes[2])
    has_left  = left_pts  is not None
    has_right = right_pts is not None

    if not has_left and not has_right:
        return None, "none"

    left_x = right_x = None
    if has_left:
        left_world = calib.pixels_to_world(left_pts)
        left_x = float(np.mean(left_world[:, 0]))
    if has_right:
        right_world = calib.pixels_to_world(right_pts)
        right_x = float(np.mean(right_world[:, 0]))

    if has_left and has_right:
        return -((left_x + right_x) / 2.0) - CAMERA_OFFSET_CM, "both"
    if has_left:
        return -(left_x + 15.0) - CAMERA_OFFSET_CM, "left"
    return -(right_x - 15.0) - CAMERA_OFFSET_CM, "right"


# ══════════════════════════════════════════════════════════════════════════════
# Socket ADAS Manager
# ══════════════════════════════════════════════════════════════════════════════
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                'ADAS-Manager-test-v6'))
from socket_sender import start_socket_thread, send_perception as publish_deviation


# ══════════════════════════════════════════════════════════════════════════════
# Writer assíncrono
# ══════════════════════════════════════════════════════════════════════════════
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


# ══════════════════════════════════════════════════════════════════════════════
# Pré-processamento
# ══════════════════════════════════════════════════════════════════════════════
def preprocess_lane(frame):
    """Crop nativo + resize → BGR 800×320. Undistort aplicado depois."""
    img = frame[_top_native:, :]
    return cv2.resize(img, (TRAIN_WIDTH, TRAIN_HEIGHT), interpolation=cv2.INTER_AREA)

def preprocess_yolo(frame):
    img = cv2.resize(frame, (MODEL_W, MODEL_H))
    return np.ascontiguousarray(cv2.cvtColor(img, cv2.COLOR_BGR2RGB).astype(np.uint8))


# ══════════════════════════════════════════════════════════════════════════════
# Post-processing YOLO (best_model — DFL manual)
# ══════════════════════════════════════════════════════════════════════════════
def nms_boxes(boxes, scores, iou_threshold=0.45):
    if len(boxes) == 0:
        return []
    x1, y1, x2, y2 = boxes[:, 0], boxes[:, 1], boxes[:, 2], boxes[:, 3]
    areas = np.maximum(0.0, x2 - x1) * np.maximum(0.0, y2 - y1)
    order = scores.argsort()[::-1]
    keep  = []
    while order.size > 0:
        i = order[0]
        keep.append(i)
        xx1   = np.maximum(x1[i], x1[order[1:]])
        yy1   = np.maximum(y1[i], y1[order[1:]])
        xx2   = np.minimum(x2[i], x2[order[1:]])
        yy2   = np.minimum(y2[i], y2[order[1:]])
        inter = np.maximum(0.0, xx2 - xx1) * np.maximum(0.0, yy2 - yy1)
        iou   = inter / (areas[i] + areas[order[1:]] - inter + 1e-9)
        order = order[np.where(iou <= iou_threshold)[0] + 1]
    return keep


def decode_yolov8_outputs(raw_outputs, conf_thresh=CONF_THRESH, iou_thresh=IOU_THRESH):
    boxes_all, scores_all, cls_all = [], [], []

    for cv2_key, cv3_key, stride in YOLO_STREAMS:
        cv2_out    = raw_outputs[cv2_key][0]
        cv3_out    = raw_outputs[cv3_key][0]
        cls_scores = 1.0 / (1.0 + np.exp(-cv3_out))
        max_scores = cls_scores.max(axis=2)
        cls_ids    = cls_scores.argmax(axis=2)
        mask = max_scores >= conf_thresh
        if not mask.any():
            continue
        ys, xs = np.where(mask)
        reg = cv2_out[ys, xs, :].reshape(-1, 4, REG_MAX)
        reg = np.exp(reg - reg.max(axis=2, keepdims=True))
        reg = reg / reg.sum(axis=2, keepdims=True)
        reg = (reg * np.arange(REG_MAX)).sum(axis=2)
        cx  = (xs + 0.5) * stride
        cy  = (ys + 0.5) * stride
        x1  = np.clip((cx - reg[:, 0] * stride) / MODEL_W, 0, 1)
        y1  = np.clip((cy - reg[:, 1] * stride) / MODEL_H, 0, 1)
        x2  = np.clip((cx + reg[:, 2] * stride) / MODEL_W, 0, 1)
        y2  = np.clip((cy + reg[:, 3] * stride) / MODEL_H, 0, 1)
        boxes_all.append(np.stack([x1, y1, x2, y2], axis=1))
        scores_all.append(max_scores[ys, xs])
        cls_all.append(cls_ids[ys, xs])

    if not boxes_all:
        return np.empty((0, 4)), np.empty((0,)), np.empty((0,), dtype=int)

    boxes   = np.vstack(boxes_all)
    scores  = np.concatenate(scores_all)
    classes = np.concatenate(cls_all)

    w = boxes[:, 2] - boxes[:, 0]
    h = boxes[:, 3] - boxes[:, 1]
    size_mask = (w >= MIN_BOX_SIZE) & (h >= MIN_BOX_SIZE)
    boxes, scores, classes = boxes[size_mask], scores[size_mask], classes[size_mask]

    if len(boxes) == 0:
        return np.empty((0, 4)), np.empty((0,)), np.empty((0,), dtype=int)

    keep_final = []
    for cls_id in np.unique(classes):
        idx  = np.where(classes == cls_id)[0]
        keep = nms_boxes(boxes[idx], scores[idx], iou_thresh)
        keep_final.extend(idx[keep])

    if not keep_final:
        return np.empty((0, 4)), np.empty((0,)), np.empty((0,), dtype=int)

    keep_final        = np.array(keep_final)
    boxes, scores, classes = boxes[keep_final], scores[keep_final], classes[keep_final]
    keep2 = nms_boxes(boxes, scores, iou_threshold=0.25)
    if not keep2:
        return np.empty((0, 4)), np.empty((0,)), np.empty((0,), dtype=int)

    return boxes[keep2], scores[keep2], classes[keep2]


def scale_boxes(boxes):
    if boxes.size == 0:
        return boxes.astype(int)
    b = boxes.copy()
    b[:, 0] *= FULL_W;  b[:, 2] *= FULL_W
    b[:, 1] *= FULL_H;  b[:, 3] *= FULL_H
    return b.astype(int)


# ══════════════════════════════════════════════════════════════════════════════
# Visualização YOLO
# ══════════════════════════════════════════════════════════════════════════════
def draw_detections(frame, boxes, scores, classes):
    font = cv2.FONT_HERSHEY_SIMPLEX
    for box, score, cls in zip(boxes, scores, classes):
        x1, y1, x2, y2 = box
        cv2.rectangle(frame, (x1, y1), (x2, y2), BOX_COLOR, 2)
        label = f"{cls}:{score:.2f}"
        (tw, th), baseline = cv2.getTextSize(label, font, 0.8, 2)
        ty = max(th + 4, y1 - 4)
        cv2.rectangle(frame, (x1, ty - th - baseline - 2), (x1 + tw + 4, ty + 2), (0, 0, 0), -1)
        cv2.putText(frame, label, (x1 + 2, ty - baseline), font, 0.8, TEXT_COLOR, 2)
    return frame


def draw_overlay_dual(frame, fps, frame_idx,
                      t_lane_pre, t_lane_hailo, t_lane_post, t_coords,
                      t_yolo_pre, t_yolo_hailo, t_yolo_post,
                      num_lanes, num_dets, deviation, status, calib_enabled):
    h, w = frame.shape[:2]
    overlay = frame.copy()
    cv2.rectangle(overlay, (0, 0), (235, 215), (0, 0, 0), -1)
    cv2.addWeighted(overlay, 0.5, frame, 0.5, 0, frame)

    dev_str   = (f"{deviation:+.1f}cm" if calib_enabled and deviation is not None
                 else f"{deviation:+.3f}" if deviation is not None else "N/A")
    dev_color = (0, 255, 0) if status == "both" else (0, 165, 255)

    lines = [
        (f"FPS:           {fps:.1f}",                   (0, 255, 0)),
        ("── Lane ──────────────────",                  (180, 180, 180)),
        (f"  Pre:         {t_lane_pre:.1f}ms",          (200, 200, 200)),
        (f"  Hailo:       {t_lane_hailo:.1f}ms",        (200, 200, 200)),
        (f"  Post:        {t_lane_post:.1f}ms",         (200, 200, 200)),
        (f"  Coords:      {t_coords:.1f}ms",            (200, 200, 200)),
        (f"  Lanes:       {num_lanes}",                 (255, 255, 255)),
        (f"  Dev:         {dev_str} [{status}]",        dev_color),
        ("── YOLO ──────────────────",                  (180, 180, 180)),
        (f"  Pre:         {t_yolo_pre:.1f}ms",          (200, 200, 200)),
        (f"  Hailo:       {t_yolo_hailo:.1f}ms",        (200, 200, 200)),
        (f"  Post:        {t_yolo_post:.1f}ms",         (200, 200, 200)),
        (f"  Detec.:      {num_dets}",                  (255, 255, 255)),
        (f"Frame: {frame_idx}",                         (255, 255, 255)),
    ]
    for i, (text, color) in enumerate(lines):
        cv2.putText(frame, text, (5, 16 + i * 14),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.33, color, 1)

    cv2.putText(frame, "SEAME | UFLDv2 + best_model (dual, small undistort + overlap)",
                (w - 310, h - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (200, 200, 200), 1)
    return frame


# ══════════════════════════════════════════════════════════════════════════════
# Pipeline principal
# ══════════════════════════════════════════════════════════════════════════════
def run(frame_queue, duration_seconds=60, save_video=False, is_rear=False):
    start_socket_thread()
    W1, b1, W2, b2, keep_rows = load_weights(WEIGHTS_PATH)
    calib = Calibration(is_rear=is_rear)

    print(f"\nFrame câmara:      {FULL_W}×{FULL_H} (via CameraBroker)")
    print(f"Lane modelo:       {TRAIN_WIDTH}×{TRAIN_HEIGHT} (crop_ratio={CROP_RATIO})")
    print(f"YOLO modelo:       {MODEL_W}×{MODEL_H} (best_model, 17 classes)")
    print(f"Undistort:         small ({TRAIN_WIDTH}×{TRAIN_HEIGHT}) — sem undistort no frame completo")
    print(f"Overlap:           YPre corre em CPU durante LHailo")
    print(f"Socket:            DGRAM /tmp/adas_lane.sock")

    async_writer = None
    if save_video:
        async_writer = AsyncVideoWriter(OUTPUT_PATH, CAM_FPS, FULL_W, FULL_H)
        print(f"A gravar: {OUTPUT_PATH}")

    hef_lane = HEF(LANE_HEF_PATH)
    hef_yolo = HEF(YOLO_HEF_PATH)

    with VDevice() as target, ThreadPoolExecutor(max_workers=1) as executor:
        cfg_lane = ConfigureParams.create_from_hef(hef_lane, interface=HailoStreamInterface.PCIe)
        cfg_yolo = ConfigureParams.create_from_hef(hef_yolo, interface=HailoStreamInterface.PCIe)

        lane_ng = target.configure(hef_lane, cfg_lane)[0]
        yolo_ng = target.configure(hef_yolo, cfg_yolo)[0]

        lane_inp  = InputVStreamParams.make(lane_ng, format_type=FormatType.UINT8)
        lane_outp = OutputVStreamParams.make(lane_ng, format_type=FormatType.UINT8)
        yolo_inp  = InputVStreamParams.make(yolo_ng, format_type=FormatType.UINT8)
        yolo_outp = OutputVStreamParams.make(yolo_ng, format_type=FormatType.FLOAT32)

        print(f"\nA correr durante {duration_seconds}s — Ctrl+C para parar\n")
        print(f"{'Frame':<7} {'LPre':>6} {'LHailo':>7} {'LPost':>7} {'Coords':>7} "
              f"{'YPre':>6} {'YHailo':>7} {'YPost':>7} "
              f"{'Total':>7} {'Lanes':>6} {'Dets':>5} {'Dev':>9} {'Status'}")
        print("─" * 108)

        frame_idx         = 0
        t_start           = time.time()
        times_all         = []
        fps_acc           = []
        temporal_smoother = TemporalLaneSmoother()
        prev_status       = None
        single_lane_mode  = False

        try:
            while (time.time() - t_start) < duration_seconds:
                try:
                    frame = frame_queue.get(timeout=0.1)
                except queue.Empty:
                    continue

                t_frame_start = time.time()

                # ── LPre: crop + resize + undistort_small + cvtColor ───────
                t0       = time.time()
                img_lane = preprocess_lane(frame)                          # BGR 800×320
                img_lane = calib.undistort_small(img_lane)                 # remap ~1ms
                img_lane = np.ascontiguousarray(
                    cv2.cvtColor(img_lane, cv2.COLOR_BGR2RGB))
                t_lane_pre = (time.time() - t0) * 1000

                # ── YPre em paralelo com LHailo ───────────────────────────
                # Submete YPre para CPU antes de bloquear no NPU
                t0_ypre  = time.time()
                fut_ypre = executor.submit(preprocess_yolo, frame)

                t0 = time.time()
                with lane_ng.activate():
                    with InferVStreams(lane_ng, lane_inp, lane_outp) as pipe:
                        lane_out = pipe.infer({LANE_INPUT: img_lane[np.newaxis]})
                t_lane_hailo = (time.time() - t0) * 1000

                # YPre já pronto (4ms << 25ms LHailo)
                img_yolo   = fut_ypre.result()
                t_yolo_pre = (time.time() - t0_ypre) * 1000

                # ── Inferência YOLO ────────────────────────────────────────
                t0 = time.time()
                with yolo_ng.activate():
                    with InferVStreams(yolo_ng, yolo_inp, yolo_outp) as pipe:
                        yolo_out = pipe.infer({YOLO_INPUT: img_yolo[np.newaxis]})
                t_yolo_hailo = (time.time() - t0) * 1000

                # ── Post-processing Lane ───────────────────────────────────
                t0     = time.time()
                conv37 = lane_out[LANE_OUTPUT][0]
                loc_row_lanes12, exist_row_lanes12 = postprocess(
                    conv37, W1, b1, W2, b2, keep_rows)
                lanes = decode_lanes(loc_row_lanes12, exist_row_lanes12,
                                     single_lane_mode=single_lane_mode)
                t_lane_post = (time.time() - t0) * 1000

                t0                = time.time()
                lanes             = temporal_smoother.update(lanes)
                lanes             = smooth_lanes(lanes)
                if calib.enabled:
                    deviation, status = calc_lateral_deviation_cm(lanes, calib)
                else:
                    deviation, status = calc_lateral_deviation(lanes)
                t_coords          = (time.time() - t0) * 1000

                publish_deviation(deviation, status)

                if prev_status == "both" and status in ("left", "right"):
                    single_lane_mode = True
                elif status == "both":
                    single_lane_mode = False
                prev_status = status

                # ── Post-processing YOLO ───────────────────────────────────
                t0                     = time.time()
                boxes, scores, classes = decode_yolov8_outputs(yolo_out)
                boxes_full             = scale_boxes(boxes)
                t_yolo_post            = (time.time() - t0) * 1000

                # ── Métricas (wall-clock) ──────────────────────────────────
                t_total   = (time.time() - t_frame_start) * 1000
                num_lanes = sum(1 for l in lanes if l)
                num_dets  = len(boxes_full)
                times_all.append(t_total)

                fps_acc.append(1000.0 / (t_total + 1e-9))
                if len(fps_acc) > 30:
                    fps_acc.pop(0)
                fps = sum(fps_acc) / len(fps_acc)

                dev_str = (f"{deviation:+.1f}cm" if calib.enabled and deviation is not None
                           else f"{deviation:+.3f}" if deviation is not None
                           else "   N/A")
                print(f"{frame_idx:<7} "
                      f"{t_lane_pre:>5.1f}ms "
                      f"{t_lane_hailo:>6.1f}ms "
                      f"{t_lane_post:>6.1f}ms "
                      f"{t_coords:>6.1f}ms "
                      f"{t_yolo_pre:>5.1f}ms "
                      f"{t_yolo_hailo:>6.1f}ms "
                      f"{t_yolo_post:>6.1f}ms "
                      f"{t_total:>6.1f}ms "
                      f"{num_lanes:>6} "
                      f"{num_dets:>5} "
                      f"{dev_str:>9} "
                      f"{status}")

                if save_video and async_writer is not None:
                    frame_out = draw_drivable_area(frame.copy(), lanes)
                    frame_out = draw_lanes(frame_out, lanes)
                    frame_out = draw_detections(frame_out, boxes_full, scores, classes)
                    frame_out = draw_overlay_dual(
                        frame_out, fps, frame_idx,
                        t_lane_pre, t_lane_hailo, t_lane_post, t_coords,
                        t_yolo_pre, t_yolo_hailo, t_yolo_post,
                        num_lanes, num_dets, deviation, status, calib.enabled)
                    async_writer.write(frame_out)

                frame_idx += 1

        except KeyboardInterrupt:
            print("\nInterrompido.")

    if async_writer is not None:
        print("\nA aguardar gravação...")
        async_writer.release()
        print(f"Vídeo guardado: {OUTPUT_PATH}")

    elapsed  = time.time() - t_start
    real_fps = frame_idx / elapsed if elapsed > 0 else 0
    avg      = sum(times_all) / len(times_all) if times_all else 0
    sorted_t = sorted(times_all) if times_all else [0]
    p50      = sorted_t[int(len(sorted_t) * 0.50)]
    p95      = sorted_t[int(len(sorted_t) * 0.95)]

    print("─" * 108)
    print(f"\nResumo ({frame_idx} frames em {elapsed:.1f}s):")
    print(f"   FPS real:          {real_fps:.1f}")
    print(f"   FPS pipeline puro: {1000/avg:.1f}" if avg > 0 else "")
    print(f"   Latência média:    {avg:.1f}ms")
    print(f"   P50:               {p50:.1f}ms")
    print(f"   P95:               {p95:.1f}ms")
    if save_video:
        print(f"   Vídeo guardado:    {OUTPUT_PATH}")


# ══════════════════════════════════════════════════════════════════════════════
# Entry point
# ══════════════════════════════════════════════════════════════════════════════
if __name__ == "__main__":
    sys.path.insert(0, "/opt/seame/adas")
    from camera_broker import CameraBroker

    p = argparse.ArgumentParser(description="SEAME Dual Inference — UFLDv2 + best_model (small undistort)")
    p.add_argument("duration", type=int, nargs="?", default=60)
    p.add_argument("--save",   action="store_true")
    p.add_argument("--camera", type=int, default=1,
                   help="1=frente (default), 0=trás")
    p.add_argument("--flip",   type=int, default=None,
                   help="0=vertical, 1=horizontal, -1=180°")
    args = p.parse_args()

    cam_label = "TRASEIRA (flip=-1)" if args.camera == 0 else "FRONTAL"
    print(f"SEAME | Dual Inference v8: UFLDv2 (lane) + best_model (objects)")
    print(f"Câmara: {cam_label} (index={args.camera}) | {FULL_W}×{FULL_H} @ {CAM_FPS}fps")
    print(f"Undistort: small maps {TRAIN_WIDTH}×{TRAIN_HEIGHT} + YPre/LHailo overlap\n")

    broker = CameraBroker(width=FULL_W, height=FULL_H, fps=CAM_FPS,
                          camera=args.camera, flip=args.flip)
    q = broker.register("dual")
    broker.start()

    run(q, duration_seconds=args.duration, save_video=args.save,
        is_rear=(args.camera == 0))

    broker.stop()
