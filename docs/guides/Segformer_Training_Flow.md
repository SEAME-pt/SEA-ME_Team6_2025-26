# SEAME — SegFormer Lane Detection Workflow

This document explains the full pipeline from dataset generation to real-time inference in CARLA, for the SegFormer-based lane detection system.

---

## Step 1 — Dataset Generation

**Script:** `generate_dataset.py`

CARLA is used as a simulator to generate synthetic training data. The script spawns a vehicle with autopilot and attaches two cameras: an RGB camera (which sees the world as a human would) and a semantic segmentation camera (which assigns a class ID to every pixel). Both cameras run in synchronous mode — ensuring that each RGB frame has an exactly matching segmentation frame.

For each frame, the script automatically filters out situations that are useless for training: frames with few visible lane markings (intersections, junctions) and frames where the car is stopped at traffic lights. Accepted frames are saved with lane coordinates in UFLD format, and also with the expected lane positions in pixels (`lane_left_x`, `lane_right_x`), calculated using the CARLA Waypoint API. This API tells the script exactly where the current lane boundaries are in the 3D map, which is then converted to pixel coordinates using the camera geometry (focal length and field of view).

**Output:**
```
dataset_v3/
    images/         → RGB frames
    segmentation/   → semantic segmentation frames (class ID per pixel)
    labels/
        train_labels.json  → lane coordinates + lane_left_x/right_x per frame
```

---

## Step 2 — Merge Datasets *(if recorded separately per map)*

**Script:** `merge_datasets.py`

Since each map has different characteristics (Town04 has more curves, Town01 and Town02 have simpler roads), the dataset is recorded separately per map and then combined. The script renumbers all frames with globally unique IDs and merges all labels into a single `train_labels.json` file.

**Output:**
```
dataset_combined/
    images/
    segmentation/
    labels/train_labels.json
```

---

## Step 3 — Prepare Dataset for SegFormer

**Script:** `prepare_segformer_dataset.py`

SegFormer does not use coordinates like UFLD — it needs **binary pixel masks**: images where lane pixels are white (255) and everything else is black (0). This script converts the CARLA semantic segmentation images into exactly those masks.

The most critical part is that the script does not paint all lane pixels. It uses the `lane_left_x` and `lane_right_x` values saved in Step 1 to define a ±80px search window around the expected position of each lane. Only pixels within that window are included in the mask, ignoring lines from other lanes on multi-lane roads. If that information is not available, it falls back to a clustering algorithm that selects the 2 groups of pixels closest to the image center.

The script also applies several quality filters:
- Discards frames where only one lane is visible
- Discards frames where both lanes are too close together (dashed center line only, no lateral line)
- Discards frames with fewer than 5 valid row detections per lane

Finally, it splits the dataset into 90% training and 10% validation.

**Output:**
```
dataset_segformer_v2/
    images/       → RGB frames (copied)
    masks/        → binary masks (255 = adjacent lane, 0 = background)
    train.txt     → image/mask pairs for training
    val.txt       → image/mask pairs for validation
```

---

## Step 4 — Fine-tune SegFormer

**Script:** `train_segformer.py`

SegFormer-B0 comes pre-trained in two phases: first on ImageNet (learns generic features like edges, textures, and shapes) and then on Cityscapes (learns to segment real street scenes including roads, vehicles, pedestrians, and lane markings). Fine-tuning adapts that knowledge to CARLA synthetic images and to the specific task of detecting the 2 adjacent lane boundaries.

Training uses a **weighted loss function** — lane pixels represent only 1–2% of the image. Without weighting, the model would learn to always predict background and still achieve low loss without detecting any lanes. Applying a 20× weight to lane pixels forces the model to prioritise their correct detection.

The model with the lowest `val_loss` across all epochs is saved automatically. The `val_loss` measures error on images the model has never seen during training — it is the key indicator of how well the model generalises.

**Key settings:**
```python
MODEL_NAME  = 'nvidia/segformer-b0-finetuned-cityscapes-512-1024'
NUM_CLASSES = 2       # 0 = background, 1 = lane
IMG_SIZE    = 512     # SegFormer input resolution
BATCH_SIZE  = 4
EPOCHS      = 20
LR          = 6e-5    # low — fine-tuning, not training from scratch
class_weights = [1.0, 20.0]  # upweight lane pixels
```

**Output:**
```
segformer_lane/   → saved model weights (best val_loss checkpoint)
```

---

## Step 5 — Real-time Inference in CARLA

**Script:** `carla_segformer_inference_v3.py`

The script connects to CARLA, spawns a vehicle, and captures live RGB frames. For each frame, the following pipeline runs:

```
RGB frame (800×288)
    ↓ resize to 512×512
SegFormer inference
    ↓ binary mask (512×512)
    ↓ resize back to 800×288
ROI — ignore top 40% of image (sky, buildings)
    ↓
For each row anchor (y = 144 to 280):
    → find lane pixel closest to center on left side
    → find lane pixel closest to center on right side
    ↓
Outlier filtering — remove points >80px from median
    ↓
Polynomial fitting — fit 2nd degree curve through points
    ↓
Sample polynomial at 18 row anchors → final lane coordinates
    ↓
Draw lanes + mask overlay on frame → display via pygame
```

**Controls:**

| Key | Action |
|---|---|
| W / S | Throttle / Brake |
| A / D | Steer left / right |
| P | Toggle autopilot |
| Q | Quit |

---

## Full Pipeline Summary

```
CARLA simulator
    ↓ generate_dataset.py
dataset_v3/  (images + segmentation + labels with waypoint info)
    ↓ merge_datasets.py  (if multiple maps)
dataset_combined/
    ↓ prepare_segformer_dataset.py
dataset_segformer_v2/  (binary masks — 2 adjacent lanes only)
    ↓ train_segformer.py
segformer_lane/  (fine-tuned model)
    ↓ carla_segformer_inference_v3.py
Real-time lane detection in CARLA
```

---

## Planned: Deployment on Hailo-8

The final deployment step — not yet implemented — requires converting the PyTorch model to a format the Hailo-8 can execute:

```
segformer_lane/ (PyTorch)
    ↓ torch.onnx.export()
segformer.onnx  (universal intermediate format)
    ↓ Hailo Dataflow Compiler
segformer.hef   (Hailo executable format)
    ↓ Hailo-8 runtime on Raspberry Pi 5
Real camera → lane detection → LDW system
```

ONNX (Open Neural Network Exchange) is a universal model format that bridges PyTorch (training framework) and Hailo-8 (deployment hardware). The Hailo compiler only accepts `.hef` files, so ONNX export is a mandatory intermediate step for any model regardless of framework or architecture.
