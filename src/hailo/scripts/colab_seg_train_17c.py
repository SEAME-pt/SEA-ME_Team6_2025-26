"""
colab_seg_train_17c.py
=======================
Train yolov8n-seg + yolo26n-seg on a Roboflow segmentation export.
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
DATASET_ROOT = str(Path(TRAIN_IMG_SRC).parents[1])
SOURCE_DATA_YAML = Path(DATASET_ROOT) / "data.yaml"
print("train images:", TRAIN_IMG_SRC)
print("train labels:", TRAIN_LBL_SRC)
print("dataset root:", DATASET_ROOT)

src_valid_images = Path(DATASET_ROOT) / "valid" / "images"
src_valid_labels = Path(DATASET_ROOT) / "valid" / "labels"
src_test_images = Path(DATASET_ROOT) / "test" / "images"
src_test_labels = Path(DATASET_ROOT) / "test" / "labels"

def _count_files(p: Path, exts=None):
    if not p.exists():
        return 0
    if exts is None:
        return sum(1 for f in p.iterdir() if f.is_file())
    exts = {e.lower() for e in exts}
    return sum(1 for f in p.iterdir() if f.is_file() and f.suffix.lower() in exts)

IMAGE_EXTS = {".jpg", ".jpeg", ".png", ".bmp", ".webp"}
has_source_valid = _count_files(src_valid_images, IMAGE_EXTS) > 0 and _count_files(src_valid_labels, {".txt"}) > 0
has_source_test = _count_files(src_test_images, IMAGE_EXTS) > 0 and _count_files(src_test_labels, {".txt"}) > 0

print("source valid available:", has_source_valid)
print("source test available:", has_source_test)

# CELL 3: prepare train/valid/test
random.seed(SEED)
for split in ("train", "valid", "test"):
    split_root = Path(SPLIT_DIR) / split
    if split_root.exists():
        shutil.rmtree(split_root)

def list_pairs(images_dir: Path, labels_dir: Path):
    imgs = []
    for ext in IMAGE_EXTS:
        imgs.extend(images_dir.glob(f"*{ext}"))
        imgs.extend(images_dir.glob(f"*{ext.upper()}"))
    imgs = sorted(set(imgs))
    pairs = []
    for img in imgs:
        lbl = labels_dir / f"{img.stem}.txt"
        if lbl.exists():
            pairs.append((img, lbl))
    return pairs

def write_pairs(split_name: str, pairs):
    img_out = Path(SPLIT_DIR) / split_name / "images"
    lbl_out = Path(SPLIT_DIR) / split_name / "labels"
    img_out.mkdir(parents=True, exist_ok=True)
    lbl_out.mkdir(parents=True, exist_ok=True)
    for img_path, lbl_path in pairs:
        shutil.copy2(img_path, img_out / img_path.name)
        shutil.copy2(lbl_path, lbl_out / lbl_path.name)

train_pairs = list_pairs(Path(TRAIN_IMG_SRC), Path(TRAIN_LBL_SRC))
if not train_pairs:
    raise RuntimeError("No valid train image/label pairs found")

if has_source_valid and has_source_test:
    valid_pairs = list_pairs(src_valid_images, src_valid_labels)
    test_pairs = list_pairs(src_test_images, src_test_labels)
    if not valid_pairs or not test_pairs:
        raise RuntimeError("Source valid/test were detected but pairs are empty after label matching")
    write_pairs("train", train_pairs)
    write_pairs("valid", valid_pairs)
    write_pairs("test", test_pairs)
    print(f"using source splits: train={len(train_pairs)} valid={len(valid_pairs)} test={len(test_pairs)}")
else:
    # Fallback only when export has train-only data.
    random.shuffle(train_pairs)
    n = len(train_pairs)
    n_train = int(0.8 * n)
    n_valid = int(0.1 * n)
    train_out = train_pairs[:n_train]
    valid_out = train_pairs[n_train:n_train + n_valid]
    test_out = train_pairs[n_train + n_valid:]
    write_pairs("train", train_out)
    write_pairs("valid", valid_out)
    write_pairs("test", test_out)
    print("WARNING: source export has no valid/test; created deterministic local split from train")
    print(f"local split: train={len(train_out)} valid={len(valid_out)} test={len(test_out)}")

# CELL 4: write data.yaml
if SOURCE_DATA_YAML.exists():
    src_cfg = yaml.safe_load(SOURCE_DATA_YAML.read_text())
    SRC_NAMES = src_cfg.get("names", [])
    SRC_NC = int(src_cfg.get("nc", len(SRC_NAMES)))
    if isinstance(SRC_NAMES, dict):
        SRC_NAMES = [SRC_NAMES[k] for k in sorted(SRC_NAMES.keys(), key=lambda x: int(x))]
    if SRC_NC != len(SRC_NAMES):
        raise ValueError(f"Source data.yaml mismatch: nc={SRC_NC}, len(names)={len(SRC_NAMES)}")
else:
    raise FileNotFoundError(f"Missing source data.yaml at {SOURCE_DATA_YAML}")

DATA_YAML = f"{SPLIT_DIR}/data.yaml"
with open(DATA_YAML, "w") as f:
    yaml.dump(
        {
            "path": SPLIT_DIR,
            "train": "train/images",
            "val": "valid/images",
            "test": "test/images",
            "nc": SRC_NC,
            "names": SRC_NAMES,
        },
        f,
        default_flow_style=False,
        allow_unicode=True,
    )

with open(DATA_YAML) as f:
    cfg = yaml.safe_load(f)
assert cfg["nc"] == SRC_NC
print("data.yaml ok")

# CELL 5: train yolov8n-seg
from ultralytics import YOLO

run_suffix = f"{SRC_NC}c"
y8_run_name = f"yolov8n_seg_{run_suffix}"
y26_run_name = f"yolo26n_seg_{run_suffix}"

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
    name=y8_run_name,
    exist_ok=True,
    patience=20,
    amp=True,
    close_mosaic=10,
    pretrained=True,
    optimizer="auto",
)
best_8n = f"{OUTPUT_DIR}/{y8_run_name}/weights/best.pt"
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
    name=y26_run_name,
    exist_ok=True,
    patience=20,
    amp=True,
    close_mosaic=10,
    pretrained=True,
    optimizer="auto",
)
best_26n = f"{OUTPUT_DIR}/{y26_run_name}/weights/best.pt"
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
