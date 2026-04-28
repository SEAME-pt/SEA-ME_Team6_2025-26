"""
colab_seg_train_17c.py
=======================
Train yolov8n-seg + yolo26n-seg on the 17-class dataset.
Run in Google Colab (GPU runtime: T4/L4/A100).

Quick usage (Colab):
1) Upload dataset zip to Drive as: MyDrive/seame_dataset_17c.zip
2) Runtime -> Change runtime type -> GPU
3) Paste each CELL block into separate notebook cells and run in order
4) Download the two best.pt files and place locally at:
   ~/Documents/AI/Yolo_benchmark/trained/yolov8n_seg_17c/weights/best.pt
   ~/Documents/AI/Yolo_benchmark/trained/yolo26n_seg_17c/weights/best.pt
"""

# CELL 1: install + mount drive
# !pip install ultralytics -q
# from google.colab import drive
# drive.mount('/drive')

import glob
import os
import random
import shutil
import yaml
from pathlib import Path

DRIVE_ROOT = "/drive/MyDrive"
DATASET_ZIP = f"{DRIVE_ROOT}/seame_dataset_17c.zip"
DATASET_DIR = "/content/dataset_17c_raw"
SPLIT_DIR = "/content/dataset_17c"
OUTPUT_DIR = f"{DRIVE_ROOT}/seame_seg_17c"

EPOCHS = 50
BATCH = 16
IMGSZ = 640
SEED = 42

os.makedirs(OUTPUT_DIR, exist_ok=True)
print(f"Output dir: {OUTPUT_DIR}")

# CELL 2: extract dataset
import zipfile

if not os.path.exists(DATASET_DIR):
    with zipfile.ZipFile(DATASET_ZIP, "r") as z:
        z.extractall(DATASET_DIR)

candidates = sorted(glob.glob(f"{DATASET_DIR}/**/train/images", recursive=True))
if not candidates:
    raise FileNotFoundError("train/images not found in extracted zip")
TRAIN_IMG_SRC = candidates[0]
TRAIN_LBL_SRC = TRAIN_IMG_SRC.replace("/images", "/labels")
print("train images:", TRAIN_IMG_SRC)
print("train labels:", TRAIN_LBL_SRC)

# CELL 3: create train/valid split
random.seed(SEED)
all_imgs = sorted(Path(TRAIN_IMG_SRC).glob("*.jpg")) + sorted(Path(TRAIN_IMG_SRC).glob("*.jpeg")) + sorted(Path(TRAIN_IMG_SRC).glob("*.png"))
all_imgs = sorted(set(all_imgs))
random.shuffle(all_imgs)

split_idx = int(0.8 * len(all_imgs))
train_imgs = all_imgs[:split_idx]
valid_imgs = all_imgs[split_idx:]
print(f"split: {len(train_imgs)} train / {len(valid_imgs)} valid")

for split_name, img_list in [("train", train_imgs), ("valid", valid_imgs)]:
    img_out = Path(SPLIT_DIR) / split_name / "images"
    lbl_out = Path(SPLIT_DIR) / split_name / "labels"
    img_out.mkdir(parents=True, exist_ok=True)
    lbl_out.mkdir(parents=True, exist_ok=True)
    for img_path in img_list:
        lbl_path = Path(TRAIN_LBL_SRC) / f"{img_path.stem}.txt"
        shutil.copy2(img_path, img_out / img_path.name)
        if lbl_path.exists():
            shutil.copy2(lbl_path, lbl_out / lbl_path.name)

# CELL 4: write data.yaml
CLASSES_17 = [
    "50_maxspeed", "80_maxspeed", "Crosswalk", "Gate", "Pedestrians_crossing",
    "Stop_sign", "Traffic_priority", "both_arrow", "car", "cars not allowed",
    "left_cross", "obstacle", "right_cross", "traffic_lights_green",
    "traffic_lights_off", "traffic_lights_red", "traffic_lights_yellow",
]

DATA_YAML = f"{SPLIT_DIR}/data.yaml"
with open(DATA_YAML, "w") as f:
    yaml.dump(
        {
            "path": SPLIT_DIR,
            "train": "train/images",
            "val": "valid/images",
            "nc": 17,
            "names": CLASSES_17,
        },
        f,
        default_flow_style=False,
        allow_unicode=True,
    )

with open(DATA_YAML) as f:
    cfg = yaml.safe_load(f)
assert cfg["nc"] == 17
print("data.yaml ok")

# CELL 5: train yolov8n-seg
from ultralytics import YOLO

m8 = YOLO("yolov8n-seg.pt")
m8.train(
    data=DATA_YAML,
    epochs=EPOCHS,
    batch=BATCH,
    imgsz=IMGSZ,
    device=0,
    workers=4,
    seed=SEED,
    deterministic=True,
    project=OUTPUT_DIR,
    name="yolov8n_seg_17c",
    exist_ok=True,
    patience=20,
    amp=True,
    close_mosaic=10,
    pretrained=True,
    optimizer="auto",
)
best_8n = f"{OUTPUT_DIR}/yolov8n_seg_17c/weights/best.pt"
assert os.path.exists(best_8n)
print("best_8n:", best_8n)

# CELL 6: train yolo26n-seg
m26 = YOLO("yolo26n-seg.pt")
m26.train(
    data=DATA_YAML,
    epochs=EPOCHS,
    batch=BATCH,
    imgsz=IMGSZ,
    device=0,
    workers=4,
    seed=SEED,
    deterministic=True,
    project=OUTPUT_DIR,
    name="yolo26n_seg_17c",
    exist_ok=True,
    patience=20,
    amp=True,
    close_mosaic=10,
    pretrained=True,
    optimizer="auto",
)
best_26n = f"{OUTPUT_DIR}/yolo26n_seg_17c/weights/best.pt"
assert os.path.exists(best_26n)
print("best_26n:", best_26n)

# CELL 7: validate
val8 = m8.val(data=DATA_YAML, split="val", imgsz=IMGSZ, device=0, batch=BATCH)
val26 = m26.val(data=DATA_YAML, split="val", imgsz=IMGSZ, device=0, batch=BATCH)
print(f"yolov8n-seg: mAP50={val8.seg.map50:.3f} mAP50-95={val8.seg.map:.3f}")
print(f"yolo26n-seg: mAP50={val26.seg.map50:.3f} mAP50-95={val26.seg.map:.3f}")

# CELL 8: final paths
print("\nSave these two files on Lenovo:")
print(best_8n)
print(best_26n)
