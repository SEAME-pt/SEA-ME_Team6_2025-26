# 03 - Artifact and Model Registry

This table is the source of truth for:
- where each model came from,
- how it was compiled,
- which streams the runtime expects,
- and what the operational state is.

## Table of contents

1. [Current models and artifacts](#a-models-and-current-artifacts)
2. [Validated internal streams](#b-internal-streams-validated-with-parse-hef)
3. [Why yolo26 uses yolov8n_seg prefix](#c-why-yolo26-uses-the-yolov8n_seg-prefix)
4. [Training status](#d-training-status)
5. [Minimum evidence per artifact](#e-minimum-evidence-per-artifact)

## A) Current models and artifacts

| Model ID | Type | Source | Trained in project | Compile recipe | HEF on AGL | Current phase | State |
|---|---|---|---|---|---|---|---|
| `yolov8s` | detection | Ultralytics pretrained (`yolov8s`) | no (infer-only benchmark) | `hailomz compile yolov8s` | `/data/yolov8s.hef` | E2E baseline | baseline |
| `yolov8n-seg` | segmentation | Ultralytics pretrained (`yolov8n-seg.pt -> onnx`) | no (infer-only benchmark) | `hailomz compile yolov8n_seg` | `/data/yolov8n_seg_h8.hef` | E2E baseline | baseline |
| `yolo26n-seg` | segmentation | Ultralytics pretrained (`yolo26n-seg.pt -> onnx`) | no (infer-only benchmark) | `hailomz compile yolov8n_seg --ckpt yolo26n-seg_320.onnx --model-script ...no_nms.alls` | `/data/yolo26n_seg_320_h8_no_nms.hef` | E2E with host NMS | experimental-host-nms |

## B) Internal streams validated with parse-hef

### 1) `yolov8s.hef`

- Network group: `yolov8s`
- Input: `yolov8s/input_layer1` `NHWC(640x640x3)`
- Output: `yolov8s/yolov8_nms_postprocess` (NMS on device)

### 2) `yolov8n_seg_h8.hef`

- Network group: `yolov8n_seg`
- Input: `yolov8n_seg/input_layer1` `NHWC(640x640x3)`
- Outputs principais: `conv73/74/75`, `conv60/61/62`, `conv44/45/46`, `conv48`

### 3) `yolo26n_seg_320_h8_no_nms.hef`

- Network group: `yolov8n_seg`
- Input: `yolov8n_seg/input_layer1` `NHWC(320x320x3)`
- Output: `yolov8n_seg/conv109` `FCR(80x80x32)`
- Output: `yolov8n_seg/format_conversion16` `FCR(1x116x2100)`

## C) Why yolo26 uses the `yolov8n_seg` prefix

Short answer:
- the file is named `yolo26n_seg_320_h8_no_nms.hef`,
- but it was compiled with the `yolov8n_seg` recipe and therefore inherited the internal names `yolov8n_seg/*`.

Operational rule:
- the Hailo runtime uses the HEF's internal names,
- therefore the scripts must follow `parse-hef`, not only the file name.

## D) Training status

Documented current status:
- main focus on inference/benchmark and Hailo compilation,
- with pretrained base weights and ONNX export,
- training or retraining with the track dataset is a later step and is not yet closed in this round.

## E) Minimum evidence per artifact

For each candidate HEF, keep:
1. the compile command used,
2. the full compile log,
3. the `hailortcli parse-hef` output on the AGL,
4. the associated inference script,
5. the E2E video + log + stats.
