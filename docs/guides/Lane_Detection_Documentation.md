# SEAME — Lane Detection: Documentation

## Overview

This document summarises the full lane detection implementation for the SEAME autonomous vehicle project. It covers two approaches — **Ultra Fast Lane Detection (UFLD)** and **SegFormer** — including the pipeline, key decisions, problems encountered, and solutions applied.

The goal is real-time detection of the two lane markings delimiting the car's current lane, for use in a Lane Departure Warning (LDW) system, running on a Hailo-8 accelerator with a Raspberry Pi 5.

---

## Part 1 — Ultra Fast Lane Detection (UFLD)

### What is UFLD?

UFLD treats lane detection as a classification problem. The image is divided into 18 horizontal row anchors (y = 144 to 280, step 8). For each anchor, the model predicts the column index (0–100) where each lane crosses that line. If a lane is not visible at a given anchor, it outputs index 100.

Architecture: ResNet-18 backbone + fully-connected classification head.

### Dataset Generation

**Script:** `generate_dataset.py`

CARLA's semantic segmentation camera assigns class ID 24 (RoadLine) to all lane marking pixels. For each frame, the script extracts lane coordinates from this mask and saves them in UFLD label format.

**Label format:**
```json
{
  "lanes": [[320, 318, ...], [480, 483, ...]],
  "h_samples": [144, 152, ..., 280],
  "raw_file": "images/000001.png"
}
```

**Key changes made during development:**

- Changed `LANE_MARK_ID` from 6 to 24 — the correct RoadLine ID for CARLA 0.9.14+
- Switched from 4 lanes to **2 lanes** — simplified the problem and eliminated label ambiguity
- Fixed the lane extraction logic from a flawed clustering algorithm to a simple and robust approach:
  - Left lane = rightmost pixel left of image center (closest to car)
  - Right lane = leftmost pixel right of image center (closest to car)
- Added quality filters: discard frames with fewer than 200 lane pixels or vehicle speed below 1 km/h (stopped at intersections)
- Added automatic vehicle respawn on collision

**Maps used:** Town03, Town04, Town01, Town02

**Dataset sizes:**
- Initial: 5000 frames (Town03)
- Combined: 10000 frames (Town03 + Town04)

### Training

**Script:** `train.py configs/carla.py`

Key config:
```python
num_lanes     = 2
griding_num   = 100
row_anchor    = list(range(144, 288, 8))
batch_size    = 4   # reduced due to CUDA OOM with CARLA running
epoch         = 50
learning_rate = 4e-4
backbone      = '18'
```

**Problems and solutions:**

| Problem | Cause | Solution |
|---|---|---|
| CUDA out of memory | CARLA + training running simultaneously | Close CARLA before training; reduce batch_size to 4 |
| Training stopped immediately | `epoch` treated as total, not additional | Set `epoch = 80` when resuming from ep049 |
| Zigzag lane detection | Incorrect labels from flawed clustering | Rewrote `extract_lane_annotations` with simple center-split logic |
| Curves not detected | Town03 has mostly straight roads | Added Town04 (highway with curves) to dataset |

### Data Augmentation

Added to `data/carla.py` (CarlaLaneDataset):
- **Brightness/Contrast** (50%) — simulates different lighting conditions
- **Shadow** (40%) — simulates shadows from trees and buildings
- **Horizontal Flip** (50%) — doubles effective dataset diversity

### Inference

**Script:** `carla_inference.py`

- Connects to CARLA, spawns a vehicle with autopilot
- Captures RGB frames, runs UFLD inference
- Draws left lane (green) and right lane (blue)
- Manual control: W/S throttle, A/D steer, P toggle autopilot, Q quit
- Lane Departure Warning: calculates deviation from lane center using the 3 lowest anchors; triggers warning if deviation exceeds 60px

### Results

- Straight road detection: good
- Curve detection: improved after adding Town04 data
- Remaining issue: occasional zigzag from dashed center line pixels influencing predictions

---

## Part 2 — SegFormer

### Why SegFormer?

UFLD struggles with curves because it predicts at fixed row anchors. SegFormer is a Transformer-based semantic segmentation model that classifies every pixel — it naturally follows lane shape including curves.

SegFormer comes pre-trained on Cityscapes (real street scenes) and is fine-tuned on CARLA data.

```
SegFormer (Cityscapes pre-training)
    ↓ fine-tune on CARLA lane masks
SegFormer (CARLA lane detection)
    ↓ inference: RGB → pixel mask → polynomial fitting → coordinates
```

### Dataset Preparation

**Script:** `prepare_segformer_dataset.py`

Converts CARLA segmentation images into binary pixel masks for SegFormer training. Key evolution of this script:

**v1 — All lane pixels:**
Generated masks with all ID 24 pixels. Problem: model learned to detect all visible lanes, not just the two adjacent to the car.

**v2 — Closest to center:**
Used `np.max(left_xs)` and `np.min(right_xs)` to select the pixel closest to image center on each side. Problem: on multi-lane roads this still captured wrong lines.

**v3 — Clustering + CARLA Waypoint API:**
- Groups pixels into clusters by horizontal proximity (gap > 15px = new cluster)
- Selects the 2 clusters whose centers are closest to the image center
- When `lane_left_x` and `lane_right_x` are available from the CARLA Waypoint API, uses a ±80px search window around those positions instead — eliminates lines from other lanes entirely
- Discards frames where both lanes are not detected (`or` filter instead of `and`)
- Discards frames where lane separation < 100px (dashed center line only)

**Lane strip width:** 6px around each detected pixel — handles dashed lines naturally.

### CARLA Waypoint API Integration

**Added to `generate_dataset.py`:**

```python
waypoint   = world.get_map().get_waypoint(vehicle.get_location())
lane_width = waypoint.lane_width  # meters

# Convert to pixels using camera intrinsics
focal        = (img_width / 2) / tan(fov/2)
half_lane_px = focal * (lane_width / 2) / distance
left_x       = center_x - half_lane_px
right_x      = center_x + half_lane_px
```

These values (`lane_left_x`, `lane_right_x`) are saved in each label entry and used by `prepare_segformer_dataset.py` to generate precise masks even on multi-lane roads.

### Training

**Script:** `train_segformer.py`

Key settings:
```python
MODEL_NAME  = 'nvidia/segformer-b0-finetuned-cityscapes-512-1024'
NUM_CLASSES = 2   # 0 = background, 1 = lane
IMG_SIZE    = 512
BATCH_SIZE  = 4
EPOCHS      = 20
LR          = 6e-5
```

**Weighted loss** — lane pixels are ~1-2% of the image; without weighting the model predicts all background:
```python
class_weights = torch.tensor([1.0, 20.0])
criterion     = nn.CrossEntropyLoss(weight=class_weights)
```

**Best result:** `train_loss: 0.0148 | val_loss: 0.0196` after 20 epochs.

### Post-processing: Mask → Lane Coordinates

**Script:** `carla_segformer_inference_v3.py`

SegFormer outputs a pixel mask. To use it for LDW, coordinates are extracted via polynomial fitting:

1. Apply ROI — ignore top 40% of image
2. For each row anchor: find lane pixel closest to center on each side
3. Filter outliers — remove points more than 80px from median
4. Fit 2nd degree polynomial: `x = a·y² + b·y + c`
5. Sample polynomial at each row anchor

### Inference

**Script:** `carla_segformer_inference_v3.py`

Same controls as UFLD inference. Additional visual: semi-transparent yellow overlay showing the raw pixel mask.

**FPS achieved:** ~39-42 FPS in CARLA.

### Problems Encountered

| Problem | Cause | Solution |
|---|---|---|
| Detecting all lanes on highway | Mask contained all ID 24 pixels | Clustering + Waypoint API to select only adjacent lanes |
| Lanes pointing at trees/buildings | Polynomial fitting using noisy pixels | ROI + outlier filtering in `mask_to_lanes` |
| Training on single-line frames | Roads with only dashed center line | Added `or` filter: discard if either lane missing |
| Frames at intersections | Car stopped, no lanes visible | Speed filter + lane pixel count filter |
| HFValidationError on model load | Relative path `./segformer_lane` | Use absolute path `/home/seame/Ultra-Fast-Lane-Detection/segformer_lane` |

---

## Part 3 — Comparison

| | UFLD | SegFormer |
|---|---|---|
| Output | Coordinates directly | Pixel mask → post-processing needed |
| Curve handling | Limited | Better |
| Speed | Very fast | ~40 FPS |
| Model size | ~5MB | ~15MB |
| Hailo-8 deployment | Straightforward | Requires ONNX export + compilation |
| Training complexity | Simple | Moderate |

---

## Part 4 — Deployment Pipeline (Planned)

```
PyTorch model (.pth / HuggingFace)
    ↓ torch.onnx.export()
model.onnx
    ↓ Hailo Dataflow Compiler
model.hef
    ↓ Hailo-8 runtime on Raspberry Pi 5
Real camera → lane detection → LDW
```

ONNX is a universal intermediate format — it bridges PyTorch (training) and Hailo-8 (deployment). Without it, the Hailo compiler cannot process the model.

---

## Key Scripts Reference

| Script | Purpose |
|---|---|
| `generate_dataset.py` | Record CARLA frames with lane labels + waypoint info |
| `merge_datasets.py` | Combine datasets from multiple maps |
| `carla_dataset.py` | UFLD dataloader with data augmentation |
| `carla_config.py` | UFLD training config |
| `carla_inference.py` | UFLD real-time inference + LDW in CARLA |
| `prepare_segformer_dataset.py` | Convert segmentation images to SegFormer masks |
| `train_segformer.py` | SegFormer fine-tuning |
| `carla_segformer_inference_v3.py` | SegFormer real-time inference in CARLA |
