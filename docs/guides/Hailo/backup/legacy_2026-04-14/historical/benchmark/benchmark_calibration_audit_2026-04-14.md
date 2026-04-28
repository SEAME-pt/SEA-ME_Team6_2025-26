# Benchmark & Calibration Audit — 2026-04-14

## 1) Scope
This note consolidates factual evidence from:
- `docs/guides/Hailo/Comands/lenovo_history.md`
- `docs/guides/Hailo/Comands/docker_history.md`

Goal: clarify calibration provenance and what benchmark claims are defensible.

## 2) Branch facts (checked in git)
- `feature/Hailo/models_benchmarks` is based on the `feature/Hailo/model_conversion` line.
- `feature/Hailo/model_conversion` is not synchronized with `development`.
- Divergence observed in git analysis: `origin/development...origin/feature/Hailo/model_conversion = 103 4`.

## 3) Calibration provenance timeline (history-backed)
### 3.1 TFRecord path
- Lenovo history command `1090`: download of official COCO calibration TFRecord from Hailo S3.
- Lenovo history commands `1097`, `1100`, `1115`, `1144`, `1146`: download COCO `val2017` + annotations and generate `coco_calib2017.tfrecord` locally.

### 3.2 Folder-based image path
- Lenovo history command `1198`: local `calibration_images` copied into `shared_with_docker`.
- Lenovo history commands `1246`, `1247`, `1248`: `calibration_images` recreated from COCO subset (`head -n 256`) and counted.

### 3.3 Compilation evidence with real images
- Docker history command `309`: compile of `yolo26n-seg_320.onnx` using `--calib-path /local/workspace/shared_with_docker/calibration_images` and custom no-NMS `.alls` script.
- Docker history lines `327`, `337`, `338`: log states real RGB calibration and `Using dataset with 64 entries for calibration`.

## 4) What is proven vs not proven
### Proven
- Calibration existed and was used in compile flows.
- Calibration sources were mixed over time (official TFRecord, generated TFRecord, copied folder, COCO subset folder).
- `yolo26n_seg_320_h8_no_nms.hef` was produced and transferred to target.

### Not proven (from these histories)
- No explicit `hailortcli benchmark` command is present in the provided history excerpts (only `hailortcli scan` appears).
- No evidence of a full E2E ADAS validation loop (video-driven metrics per class for passadeira/seta/STOP) in these logs alone.

## 5) Assessment of the “YOLO26s precompiled” claim
- Ultralytics links provide pretrained `.pt` weights, not Hailo `.hef` binaries.
- A claim of "precompiled YOLO26s" is only valid if the exact `.hef` artifact, compile recipe, calibration set, and target runtime context are traceable.
- Current evidence does not provide that traceability for a decision-grade comparison against `yolo26n-seg` Hailo flow.

## 6) Execution decision
- Proceed with implementation now using partial labels as **technical validation only**.
- Gate final model choice on full labels for passadeira/seta/STOP and repeatable E2E metrics.
- Keep infer-only and E2E reports strictly separated.
