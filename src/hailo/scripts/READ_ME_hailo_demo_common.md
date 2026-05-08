# Hailo Demo Common - Runtime Notes

`hailo_demo_common.py` is the shared runtime utility module used by the camera/offline inference scripts in this folder.

## Why this module exists

- Avoid duplicated code across `inference_camera_scalercrop_*.py` scripts.
- Keep CLI behavior consistent for all model-specific scripts.
- Provide a single I/O implementation for camera mode and offline image-folder mode.

## What it provides

- `CameraFrameSource`: frame acquisition from Picamera2 with ScalerCrop.
- `ImageFolderSource`: offline frame source using `--images-dir`.
- `AsyncVideoWriter`: non-blocking video writing.
- `build_arg_parser(...)`: standard CLI args (`duration`, `--save`, `--images-dir`, `--loop`, `--output`).
- `make_frame_source(...)`: selects camera vs folder source based on arguments.
- `resize_for_output(...)`: keeps output video dimensions consistent.

## Sprint 13 status

Current scripts to keep (active workflow):

- `inference_camera_scalercrop_yolov8s.py`
- `inference_camera_scalercrop_yolov8n.py`
- `inference_camera_scalercrop_yolov8n_seg.py`
- `inference_camera_scalercrop_yolo26n_seg.py`

Deletion rule:

- Only remove a script after confirming it has no references in repo scripts/docs and no active benchmark dependency.

