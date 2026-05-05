#!/usr/bin/env python3
"""Shared I/O helpers for Hailo demo scripts."""

from __future__ import annotations

import argparse
import queue
import threading
import time
from pathlib import Path
from typing import Iterable, Optional, Sequence, Tuple

import cv2

_IMAGE_EXTENSIONS = {".jpg", ".jpeg", ".png", ".bmp", ".webp", ".tif", ".tiff"}


class CameraFrameSource:
    def __init__(self, width: int, height: int, fps: int, sensor_w: int, sensor_h: int, roi_ratio: float):
        from picamera2.picamera2 import Picamera2

        self._width = width
        self._height = height
        self._picam2 = Picamera2()
        config = self._picam2.create_preview_configuration(
            main={"format": "RGB888", "size": (width, height)},
            controls={"FrameRate": fps},
        )
        self._picam2.configure(config)

        crop_top = int(sensor_h * roi_ratio)
        crop_rect = (0, crop_top, sensor_w, sensor_h - crop_top)
        self._picam2.set_controls({"ScalerCrop": crop_rect})

        self._latest = None
        self._frame_count = 0
        self._lock = threading.Lock()
        self._stop = threading.Event()
        self._ready = threading.Event()
        self._picam2.start()
        threading.Thread(target=self._loop, daemon=True).start()
        print("A aguardar câmara CSI com ScalerCrop...")
        if not self._ready.wait(timeout=10):
            raise RuntimeError("Timeout a inicializar a câmara")
        print(f"✅ Câmara iniciada — {width}×{height} @ {fps}fps")
        print(f"   ScalerCrop: sensor crop top={crop_top}px ({roi_ratio * 100:.0f}% cortado)")
        print("   Frame entregue representa a zona da estrada")

    @property
    def is_live(self) -> bool:
        return True

    def _loop(self):
        import cv2 as _cv2

        while not self._stop.is_set():
            frame = self._picam2.capture_array()
            frame_bgr = _cv2.cvtColor(frame, _cv2.COLOR_RGB2BGR)
            with self._lock:
                self._latest = frame_bgr
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


class ImageFolderSource:
    def __init__(self, folder: str | Path, loop: bool = False):
        self._folder = Path(folder).expanduser().resolve()
        if not self._folder.exists():
            raise FileNotFoundError(f"Folder não encontrado: {self._folder}")
        if not self._folder.is_dir():
            raise NotADirectoryError(f"Caminho não é pasta: {self._folder}")
        self._paths = [
            path
            for path in sorted(self._folder.rglob("*"))
            if path.is_file() and path.suffix.lower() in _IMAGE_EXTENSIONS
        ]
        if not self._paths:
            raise FileNotFoundError(f"Não foram encontradas imagens em: {self._folder}")
        self._loop = loop
        self._index = 0

    @property
    def is_live(self) -> bool:
        return False

    @property
    def total_frames(self) -> int:
        return len(self._paths)

    @property
    def folder(self) -> Path:
        return self._folder

    def read(self):
        if self._index >= len(self._paths):
            if not self._loop:
                return False, None, -1
            self._index = 0

        path = self._paths[self._index]
        self._index += 1
        frame = cv2.imread(str(path), cv2.IMREAD_COLOR)
        if frame is None:
            return self.read()
        return True, frame, self._index - 1

    def release(self):
        return None


class AsyncVideoWriter:
    def __init__(self, path: str, fps: int, width: int, height: int):
        self._queue = queue.Queue(maxsize=60)
        self._writer = cv2.VideoWriter(
            path, cv2.VideoWriter_fourcc(*"mp4v"), fps, (width, height)
        )
        if not self._writer.isOpened():
            raise RuntimeError(f"Não foi possível criar: {path}")
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
        self._thread.join(timeout=10)
        self._writer.release()


def build_arg_parser(description: str) -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument("duration", nargs="?", type=int, default=60, help="Duração em segundos no modo câmara")
    parser.add_argument("--save", action="store_true", help="Gravar vídeo de saída")
    parser.add_argument("--images-dir", type=str, default=None, help="Modo offline: pasta Roboflow com imagens")
    parser.add_argument("--loop", action="store_true", help="Repetir as imagens quando usar --images-dir")
    parser.add_argument("--output", type=str, default=None, help="Caminho do vídeo de saída")
    return parser


def make_frame_source(width: int, height: int, fps: int, sensor_w: int, sensor_h: int, roi_ratio: float, images_dir: Optional[str], loop: bool):
    if images_dir:
        return ImageFolderSource(images_dir, loop=loop)
    return CameraFrameSource(width, height, fps, sensor_w, sensor_h, roi_ratio)


def resize_for_output(frame, width: int, height: int):
    if frame.shape[1] == width and frame.shape[0] == height:
        return frame
    return cv2.resize(frame, (width, height))


def format_mode_label(is_live: bool, images_dir: Optional[str]) -> str:
    if is_live:
        return "Real-time camera"
    return f"Offline folder: {images_dir}"
