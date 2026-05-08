import numpy as np
import cv2
import threading
import queue
import time
import sys
import os
import socket
import struct

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from hailo_platform import (HEF, VDevice, HailoStreamInterface,
                             InferVStreams, ConfigureParams,
                             InputVStreamParams, OutputVStreamParams,
                             FormatType)

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                 "../inference/lane-detection"))
from config import (
    HEF_PATH, WEIGHTS_PATH, OUTPUT_PATH,
    FULL_W, FULL_H, CAM_FPS,
    TRAIN_WIDTH, TRAIN_HEIGHT, CROP_RATIO,
    INPUT_STREAM, OUTPUT_LAYER,
    EXIST_THRESHOLD, TEMPORAL_HISTORY, TEMPORAL_MIN_HITS,
    CAMERA_OFFSET_CM,
)
from postprocess import (
    load_weights, postprocess, decode_lanes,
    calc_lateral_deviation, TemporalLaneSmoother, smooth_lanes,
)
from visualization import draw_lanes, draw_drivable_area, draw_overlay
from pathlib import Path


# ── Calibração da câmara ──────────────────────────────────────────────────────
CALIB_DIR_FRONT = Path("/data/seame-configs/camera")
CALIB_DIR_REAR  = Path("/data/seame-configs/camera_rear")
INTRINSIC_DIR   = Path("/data/seame-configs/camera")  # mesma lente — partilhado


class Calibration:
    def __init__(self, is_rear=False):
        self.enabled = False
        self._undist_map1 = None
        self._undist_map2 = None
        self.H_img2world = None

        calib_dir = CALIB_DIR_REAR if is_rear else CALIB_DIR_FRONT
        cam_label = "REAR" if is_rear else "FRONT"

        files = {
            "camera_matrix.npy": INTRINSIC_DIR / "camera_matrix.npy",
            "dist_coeffs.npy": INTRINSIC_DIR / "dist_coeffs.npy",
            "homography_img2world.npy": calib_dir / "homography_img2world.npy",
        }
        missing = [n for n, p in files.items() if not p.exists()]
        if missing:
            print(f"[CALIB] [{cam_label}] Ficheiros em falta: {', '.join(missing)}")
            print(f"[CALIB] A correr SEM calibração")
            return

        camera_matrix = np.load(files["camera_matrix.npy"])
        dist_coeffs = np.load(files["dist_coeffs.npy"])
        self.H_img2world = np.load(files["homography_img2world.npy"])

        new_matrix, _ = cv2.getOptimalNewCameraMatrix(
            camera_matrix, dist_coeffs,
            (FULL_W, FULL_H), 0, (FULL_W, FULL_H)
        )
        self._undist_map1, self._undist_map2 = cv2.initUndistortRectifyMap(
            camera_matrix, dist_coeffs, None, new_matrix,
            (FULL_W, FULL_H), cv2.CV_16SC2
        )
        self.enabled = True
        print(f"[CALIB] [{cam_label}] OK — intrinsic (shared) + extrinsic ({calib_dir.name}/) loaded")

    def undistort(self, frame):
        if not self.enabled:
            return frame
        return cv2.remap(frame, self._undist_map1, self._undist_map2,
                         cv2.INTER_LINEAR)

    def pixels_to_world(self, points_px):
        if not self.enabled or self.H_img2world is None:
            return None
        pts = np.array(points_px, dtype=np.float32).reshape(-1, 1, 2)
        return cv2.perspectiveTransform(pts, self.H_img2world).reshape(-1, 2)


def calc_lateral_deviation_cm(lanes, calib):
    from postprocess import calc_lateral_deviation as _fallback

    def bottom_points(pts, n=5):
        if len(pts) < 3:
            return None
        return sorted(pts, key=lambda p: p[1], reverse=True)[:n]

    left_pts = bottom_points(lanes[1])
    right_pts = bottom_points(lanes[2])

    has_left = left_pts is not None
    has_right = right_pts is not None

    if not has_left and not has_right:
        return None, "none"

    if has_left:
        left_world = calib.pixels_to_world(left_pts)
        left_x = float(np.mean(left_world[:, 0]))
    if has_right:
        right_world = calib.pixels_to_world(right_pts)
        right_x = float(np.mean(right_world[:, 0]))

    if has_left and has_right:
        lane_center = (left_x + right_x) / 2.0
        deviation = -lane_center - CAMERA_OFFSET_CM
        return deviation, "both"

    if has_left:
        deviation = -(left_x + 15.0) - CAMERA_OFFSET_CM
        return deviation, "left"

    deviation = -(right_x - 15.0) - CAMERA_OFFSET_CM
    return deviation, "right"


# ── Socket (DGRAM — matches SocketReceiver in ADAS Manager C++) ───────────────
import sys as _sys, os as _os
_sys.path.insert(0, _os.path.join(_os.path.dirname(_os.path.abspath(__file__)), '../ADAS-Manager-test'))
from socket_sender import start_socket_thread, send_perception as publish_deviation


# ── Writer assíncrono ──────────────────────────────────────────────────────────
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


# ── Pré-processamento ──────────────────────────────────────────────────────────
_resize_h   = int(TRAIN_HEIGHT / CROP_RATIO)                          # 400
_top_native = round(FULL_H * ((_resize_h - TRAIN_HEIGHT) / _resize_h))  # ≈123

def preprocess(frame):
    img = frame[_top_native:, :]
    img = cv2.resize(img, (TRAIN_WIDTH, TRAIN_HEIGHT), interpolation=cv2.INTER_AREA)
    return cv2.cvtColor(img, cv2.COLOR_BGR2RGB)


# ── Pipeline principal ─────────────────────────────────────────────────────────
def run(frame_queue, duration_seconds=60, save_video=False, is_rear=False):
    start_socket_thread()

    W1, b1, W2, b2, keep_rows = load_weights(WEIGHTS_PATH)
    calib = Calibration(is_rear=is_rear)

    print(f"\nFrame câmara:      {FULL_W}×{FULL_H} (via CameraBroker)")
    print(f"Modelo recebe:     {TRAIN_WIDTH}×{TRAIN_HEIGHT} (crop_ratio={CROP_RATIO})")
    print(f"HEF cortado:       /pool/Conv → conv37 (10,25,8) UINT8")
    print(f"Post-processing:   CPU (dequant + FC1 + ReLU + FC2)")
    print(f"EXIST_THRESHOLD:   {EXIST_THRESHOLD}")
    print(f"Temporal smoother: history={TEMPORAL_HISTORY} min_hits={TEMPORAL_MIN_HITS}")
    print(f"Socket:            DGRAM /tmp/adas_lane.sock")

    async_writer = None
    if save_video:
        async_writer = AsyncVideoWriter(OUTPUT_PATH, CAM_FPS, FULL_W, FULL_H)
        print(f"A gravar: {OUTPUT_PATH}")

    hef = HEF(HEF_PATH)
    with VDevice() as target:
        configure_params = ConfigureParams.create_from_hef(
            hef, interface=HailoStreamInterface.PCIe)
        network_groups = target.configure(hef, configure_params)
        network_group  = network_groups[0]

        input_params  = InputVStreamParams.make(network_group, format_type=FormatType.UINT8)
        output_params = OutputVStreamParams.make(network_group, format_type=FormatType.UINT8)

        with network_group.activate():
            with InferVStreams(network_group, input_params, output_params) as pipeline:
                print(f"\nA correr durante {duration_seconds}s — Ctrl+C para parar\n")
                print(f"{'Frame':<7} {'Pre':>6} {'Hailo':>7} {'Post':>7} "
                      f"{'Coords':>8} {'Total':>7} {'Lanes':>6} {'Dev':>8} {'Status'}")
                print("─" * 78)

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

                        t0       = time.time()
                        frame    = calib.undistort(frame)
                        t_undist = (time.time() - t0) * 1000

                        t0    = time.time()
                        img   = preprocess(frame)
                        t_pre = (time.time() - t0) * 1000

                        if frame_idx % 30 == 0:
                            print(f"[TIMING] undist={t_undist:.1f}ms  pre={t_pre:.1f}ms")

                        img        = np.ascontiguousarray(img)
                        t0         = time.time()
                        input_data = {INPUT_STREAM: img[np.newaxis]}
                        output     = pipeline.infer(input_data)
                        t_hailo    = (time.time() - t0) * 1000

                        t0     = time.time()
                        conv37 = output[OUTPUT_LAYER][0]
                        loc_row_lanes12, exist_row_lanes12 = \
                            postprocess(conv37, W1, b1, W2, b2, keep_rows)
                        lanes  = decode_lanes(loc_row_lanes12, exist_row_lanes12,
                                              single_lane_mode=single_lane_mode)
                        t_post = (time.time() - t0) * 1000

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

                        t_total   = t_pre + t_hailo + t_post + t_coords
                        num_lanes = sum(1 for l in lanes if l)
                        times_all.append(t_total)

                        fps_acc.append(1000.0 / (t_total + 1e-9))
                        if len(fps_acc) > 30:
                            fps_acc.pop(0)
                        fps = sum(fps_acc) / len(fps_acc)

                        dev_str = (f"{deviation:+.1f}cm" if calib.enabled and deviation is not None
                                   else f"{deviation:+.3f}" if deviation is not None
                                   else "   N/A")
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
                            frame_out = draw_drivable_area(frame.copy(), lanes)
                            frame_out = draw_lanes(frame_out, lanes)
                            frame_out = draw_overlay(
                                frame_out, fps, frame_idx,
                                t_pre, t_hailo, t_post, t_coords,
                                deviation, status)
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
    sorted_t = sorted(times_all)
    p50      = sorted_t[int(len(sorted_t) * 0.50)] if sorted_t else 0
    p95      = sorted_t[int(len(sorted_t) * 0.95)] if sorted_t else 0

    print("─" * 78)
    print(f"\nResumo ({frame_idx} frames em {elapsed:.1f}s):")
    print(f"   FPS real:          {real_fps:.1f}")
    print(f"   FPS pipeline puro: {1000/avg:.1f}" if avg > 0 else "   FPS pipeline puro: N/A")
    print(f"   Latência média:    {avg:.1f}ms")
    print(f"   P50:               {p50:.1f}ms")
    print(f"   P95:               {p95:.1f}ms")


if __name__ == "__main__":
    import argparse
    sys.path.insert(0, "/opt/seame/adas")
    from camera_broker import CameraBroker

    p = argparse.ArgumentParser(description="LKA Inference — UFLD + Socket")
    p.add_argument("duration", type=int, nargs="?", default=60,
                   help="Duração em segundos (default: 60)")
    p.add_argument("--save",   action="store_true",
                   help="Gravar vídeo anotado")
    p.add_argument("--camera", type=int, default=1,
                   help="Índice da câmara: 1=frente (default), 0=trás")
    p.add_argument("--flip",   type=int, default=None,
                   help="Flip: 0=vertical, 1=horizontal, -1=180° (câmara traseira)")
    args = p.parse_args()

    cam_label = "TRASEIRA (flip=-1)" if args.camera == 0 else "FRONTAL"
    print(f"SEAME Lane Detection | TuSimple cut v2 | {TRAIN_WIDTH}×{TRAIN_HEIGHT}")
    print(f"Câmara: {cam_label} (index={args.camera})")
    print(f"HEF cortado em /pool/Conv | Post-processing CPU\n")

    broker = CameraBroker(width=FULL_W, height=FULL_H, fps=CAM_FPS,
                          camera=args.camera, flip=args.flip)
    q = broker.register("lane_detection")
    broker.start()

    run(q, duration_seconds=args.duration, save_video=args.save,
        is_rear=(args.camera == 0))

    broker.stop()
