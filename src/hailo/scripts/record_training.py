#!/usr/bin/env python3
"""Record clean camera video for YOLO training data collection.

Uses CameraBroker (same as INFERENCE_DUAL) — no picamera2/av issues.
Output video has no overlays — raw camera feed only.
"""

import sys
import os
import time
import datetime
import cv2

# CameraBroker path (matches INFERENCE_DUAL.py)
sys.path.insert(0, "/opt/seame/adas")
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                "../../lane-detection/inference"))

from camera_broker import CameraBroker

DURATION_S = 120    # 2 minutes
FPS        = 30
WIDTH      = 820
HEIGHT     = 616


def main():
    ts      = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
    outpath = f"training_{ts}.mp4"

    broker = CameraBroker(width=WIDTH, height=HEIGHT, fps=FPS, camera=1)  # 1=frontal, 0=traseira
    q      = broker.register("record")
    broker.start()
    time.sleep(2)   # warm-up

    fourcc = cv2.VideoWriter_fourcc(*"mp4v")
    writer = cv2.VideoWriter(outpath, fourcc, FPS, (WIDTH, HEIGHT))
    if not writer.isOpened():
        print(f"ERROR: could not open VideoWriter → {outpath}", file=sys.stderr)
        broker.stop()
        sys.exit(1)

    print(f"Recording → {outpath}")
    print(f"Duration:   {DURATION_S}s  |  {FPS}fps  |  {WIDTH}×{HEIGHT}")
    print("Ctrl+C to stop early.\n")

    t_start     = time.monotonic()
    frame_count = 0

    try:
        while True:
            elapsed = time.monotonic() - t_start
            if elapsed >= DURATION_S:
                break

            frame = q.get()                             # RGB888 numpy array
            bgr   = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
            writer.write(bgr)
            frame_count += 1

            if frame_count % FPS == 0:
                remaining = int(DURATION_S - elapsed)
                print(f"\r  {remaining:3d}s remaining  ({frame_count} frames written)",
                      end="", flush=True)

    except KeyboardInterrupt:
        print("\nStopped early.")

    finally:
        writer.release()
        broker.stop()

    elapsed_total = time.monotonic() - t_start
    print(f"\n\nDone. {frame_count} frames  ({elapsed_total:.1f}s) → {outpath}")


if __name__ == "__main__":
    main()
