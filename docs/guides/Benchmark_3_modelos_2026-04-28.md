# 3-Model Benchmark And Full Process Log (2026-04-28)

## Index
1. [Scope and Goal](#1-scope-and-goal)
2. [Models Compared](#2-models-compared)
3. [End-to-End Process (Step by Step)](#3-end-to-end-process-step-by-step)
4. [Commands Executed](#4-commands-executed)
5. [Main Benchmark Results (Vasco Dataset)](#5-main-benchmark-results-vasco-dataset)
6. [Additional Remote AGL Benchmark](#6-additional-remote-agl-benchmark)
7. [Key Technical Findings](#7-key-technical-findings)
8. [Decision Taken](#8-decision-taken)
9. [Recommended Next Plan](#9-recommended-next-plan)
10. [Evidence Paths](#10-evidence-paths)
11. [Next Steps For More Realistic Tests](#11-next-steps-for-more-realistic-tests)
12. [Segmentation Dataset Pre-Training Technical Checklist](#12-segmentation-dataset-pre-training-technical-checklist)

## 1. Scope and Goal
This document records the full workflow that was executed to compare three models and decide the next pipeline step.

Primary goal:
- Compare `yolo26n-seg` and `yolov8n-seg` (trained in the 17-class segmentation flow) against Vasco's `yolov8s` baseline on Vasco data.

## 2. Models Compared
- `yolo26n_seg_17c` (segmentation)
- `yolov8n_seg_17c` (segmentation)
- `vasco_yolov8s` (detection)

## 3. End-to-End Process (Step by Step)
1. The Colab notebook pipeline (`colab_seg_train_17c.ipynb`) was aligned to a strict execution order (process gate before ONNX export).
2. `yolov8n-seg` and `yolo26n-seg` were trained and exported as `.pt` (and optionally `.onnx`) artifacts.
3. Artifacts were bundled and transferred to local Lenovo workflow directories.
4. Initial visual comparison was run on Vasco `valid/images` using `yolo predict` for all three models.
5. Validation was executed using Vasco `data.yaml`.
6. Both segmentation models failed validation with detect/segment mismatch warnings and dataloader `IndexError`.
7. Vasco `yolov8s` completed validation successfully and produced strong metrics.
8. A low-confidence rerun (`conf=0.05`) was executed for both segmentation models to test recall behavior.
9. Low-confidence run still showed mostly empty predictions with rare isolated detections.
10. Video inference was attempted with a placeholder path (`.../videos/input.mp4`) and failed due to file-not-found.
11. Actual available video paths were checked and documented for correct direct `.mp4` inference.
12. Latest comparison folders and output artifacts were verified in local filesystem.
13. Final operational decision: keep Vasco baseline as winner for this dataset and avoid repeating known failing seg validation on mixed labels.

## 4. Commands Executed
Representative commands used in this phase:

```bash
# Predict on Vasco validation images (seg models and Vasco baseline)
yolo predict model=".../yolo26n_seg_17c/weights/best.pt" source=".../Vasco/final_dataset/valid/images" conf=0.25 imgsz=640 save=True ...
yolo predict model=".../yolov8n_seg_17c/weights/best.pt" source=".../Vasco/final_dataset/valid/images" conf=0.25 imgsz=640 save=True ...
yolo predict model=".../vasco.../weights/best.pt" source=".../Vasco/final_dataset/valid/images" conf=0.25 imgsz=640 save=True ...

# Validation on Vasco data.yaml
yolo detect val model="..." data=".../Vasco/final_dataset/data.yaml" imgsz=640

# Low-confidence re-run
yolo predict model=".../yolo26n_seg_17c/weights/best.pt" source=".../valid/images" conf=0.05 imgsz=640 save=True ...
yolo predict model=".../yolov8n_seg_17c/weights/best.pt" source=".../valid/images" conf=0.05 imgsz=640 save=True ...
```

## 5. Main Benchmark Results (Vasco Dataset)
| Model | Task | Predict on `valid/images` | Validation on Vasco `data.yaml` | Status |
|---|---|---|---|---|
| `yolo26n_seg_17c` | Seg | Mostly no detections, rare isolated hits | Failed (detect/segment warning + `IndexError`) | Not approved for this dataset |
| `yolov8n_seg_17c` | Seg | Mostly no detections, rare isolated hits (even at `conf=0.05`) | Failed (detect/segment warning + `IndexError`) | Not approved for this dataset |
| `vasco_yolov8s` | Detect | Consistent detections on same split | Completed successfully (`mAP50=0.95`, `mAP50-95=0.687` from run logs) | Current winner |

## 6. Additional Remote AGL Benchmark
Source:
- `~/Documents/AI/Yolo_benchmark/results/bench_runs/remote_agl_5models_video_20260425_021402/*_stats.csv`

| Model | Frames | Frames with detections | Total detections | Avg detections/frame |
|---|---:|---:|---:|---:|
| `yolo26n_seg` | 59 | 59 (100.0%) | 958 | 16.24 |
| `yolov8n_seg` | 59 | 25 (42.4%) | 25 | 0.42 |
| `yolov8s_vasco` | 59 | 0 (0.0%) | 0 | 0.00 |

Important note:
- This remote AGL scenario is not directly equivalent to the local Vasco dataset validation scenario. It should be treated as supplemental behavior evidence, not as the main decision criterion.

## 7. Key Technical Findings
- Segmentation validation breaks on Vasco `data.yaml` because the dataset/task appears mixed (detect + segment mismatch).
- The two seg models were evaluated out-of-domain relative to their training distribution.
- Low-confidence inference (`conf=0.05`) did not materially fix recall on the main Vasco validation image set.
- A previous video run failed because the video path used did not exist.
- For image-folder sources, `yolo predict` saves annotated images, not `.mp4`.

## 8. Decision Taken
For the current pipeline gate:
- Select `vasco_yolov8s` as the operational winner for this comparison stage.
- Mark comparison stage complete and continue export/conversion flow only under the gated process.

## 9. Recommended Next Plan
1. Run a "Vasco-style" baseline training attempt for `yolo26n-seg` first (same style as Vasco workflow, but with seg-compatible labels).
2. Ensure dataset is 100% segmentation-consistent before `val`.
3. Run short target-domain fine-tune (10-30 epochs), then compare again.
4. If needed, test larger `imgsz` (768 then 960) for small objects.
5. Rebalance rare classes and reduce overly aggressive augmentations.

## 10. Evidence Paths
- `~/Documents/AI/Yolo_benchmark/compare/predict_lowconf_20260428_013424/yolo26n_seg_17c`
- `~/Documents/AI/Yolo_benchmark/compare/predict_lowconf_20260428_013424/yolov8n_seg_17c`
- `~/Documents/AI/Yolo_benchmark/compare/val_cmp_20260428_012925/vasco_yolov8s`
- `~/Documents/AI/Yolo_benchmark/results/bench_runs/remote_agl_5models_video_20260425_021402`

## 11. Next Steps For More Realistic Tests

### 11.1 Why "Vasco-style first" can still fail
Yes, this is a valid concern.

If we only copy Vasco's train/predict flow but keep a mixed or detect-oriented dataset definition, segmentation models can fail again for the same reason:
- task mismatch (seg model vs detect labels),
- mixed annotation format (boxes and polygons not aligned),
- unstable validation behavior (`IndexError` / empty effective labels).

So, the first hard requirement is:
- build and validate a segmentation-consistent dataset before running seg-model validation.

### 11.2 Practical realistic test plan across models
1. Freeze a shared evaluation set.
	- Keep one fixed video list and one fixed image validation split for all models.
2. Build two explicit datasets.
	- `dataset_detect_vasco.yaml` for detect models.
	- `dataset_seg_vasco.yaml` for seg models (100% polygon-valid labels only).
3. Run label QA before training.
	- Count valid/invalid files.
	- Print class distribution and missing classes.
4. Train quick baselines (smoke run).
	- `yolo26n-seg` (10 epochs).
	- `yolov8n-seg` (10 epochs).
	- Keep existing `vasco_yolov8s` as detect reference.
5. Run identical inference protocol for all models.
	- Same input videos.
	- Same FPS/output settings.
	- Same confidence sweep (`0.25`, then `0.10`, then `0.05`).
6. Run task-correct validation.
	- Seg models validated only on `dataset_seg_vasco.yaml`.
	- Detect model validated only on `dataset_detect_vasco.yaml`.
7. Produce one comparison table.
	- mAP metrics (task-correct).
	- Frame detection rate.
	- Avg detections per frame.
	- False-positive notes from visual spot-check.
8. Promote best candidates to longer run.
	- 20-30 epochs only for models that pass smoke run.
9. Increase realism.
	- Add night/rain/occlusion clips.
	- Add camera angle variation.
	- Re-evaluate with same fixed protocol.

### 11.3 Model-specific next actions
- `yolo26n_seg_17c`
  - Retrain on `dataset_seg_vasco.yaml` (10 epochs smoke).
  - If stable, extend to 20-30 epochs and test `imgsz=768`.
- `yolov8n_seg_17c`
  - Same flow as `yolo26n_seg_17c` for fair comparison.
- `vasco_yolov8s`
  - Keep as operational baseline on detect dataset.
  - Use as reference for deployment readiness while seg models iterate.

### 11.4 Expected timeline for first realistic answer
- Dataset cleanup + QA: 1-2 hours.
- Smoke training (both seg models): 2-4 hours (GPU dependent).
- Predict/val/report update: 1 hour.

Estimated total: ~4-7 hours for a first realistic, comparable result.

## 12. Segmentation Dataset Pre-Training Technical Checklist

Use this checklist before any `yolo26n-seg` / `yolov8n-seg` training run.

### 12.1 Define dataset paths
```bash
SEG_ROOT="/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset_seg"
TRAIN_IMG="$SEG_ROOT/train/images"
TRAIN_LBL="$SEG_ROOT/train/labels"
VAL_IMG="$SEG_ROOT/valid/images"
VAL_LBL="$SEG_ROOT/valid/labels"
```

### 12.2 Confirm required folder structure exists
```bash
for d in "$TRAIN_IMG" "$TRAIN_LBL" "$VAL_IMG" "$VAL_LBL"; do
  [[ -d "$d" ]] && echo "OK: $d" || echo "MISSING: $d"
done
```

### 12.3 Check image/label cardinality and missing pairs
```bash
python3 - << 'PY'
from pathlib import Path

splits = {
	'train': ('/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset_seg/train/images',
			  '/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset_seg/train/labels'),
	'valid': ('/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset_seg/valid/images',
			  '/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset_seg/valid/labels'),
}
img_ext = {'.jpg', '.jpeg', '.png', '.bmp', '.tif', '.tiff'}

for split, (img_dir, lbl_dir) in splits.items():
	img_dir = Path(img_dir)
	lbl_dir = Path(lbl_dir)
	imgs = [p for p in img_dir.rglob('*') if p.suffix.lower() in img_ext]
	missing_lbl = [p for p in imgs if not (lbl_dir / f'{p.stem}.txt').exists()]
	lbls = list(lbl_dir.glob('*.txt'))
	missing_img = [p for p in lbls if not any((img_dir / f'{p.stem}{ext}').exists() for ext in img_ext)]
	print(f'[{split}] images={len(imgs)} labels={len(lbls)} missing_labels={len(missing_lbl)} missing_images={len(missing_img)}')
	if missing_lbl[:5]:
		print('  sample missing labels:', [str(x) for x in missing_lbl[:5]])
	if missing_img[:5]:
		print('  sample missing images:', [str(x) for x in missing_img[:5]])
PY
```

### 12.4 Validate segmentation label syntax (polygon format)
Rules checked:
- each line has at least 7 tokens,
- class id is integer in range `[0, nc-1]`,
- coordinate count is even and at least 6,
- all coords are normalized in `[0.0, 1.0]`.

```bash
python3 - << 'PY'
from pathlib import Path

nc = 17
label_dirs = [
	Path('/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset_seg/train/labels'),
	Path('/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset_seg/valid/labels'),
]

bad = []
empty = 0
total = 0

for d in label_dirs:
	for p in d.glob('*.txt'):
		total += 1
		lines = [ln.strip() for ln in p.read_text().splitlines() if ln.strip()]
		if not lines:
			empty += 1
			bad.append((str(p), 'empty'))
			continue
		for i, ln in enumerate(lines, 1):
			parts = ln.split()
			if len(parts) < 7:
				bad.append((str(p), f'line {i}: <7 tokens'))
				continue
			try:
				cls = int(float(parts[0]))
				coords = [float(x) for x in parts[1:]]
			except Exception:
				bad.append((str(p), f'line {i}: parse error'))
				continue
			if not (0 <= cls < nc):
				bad.append((str(p), f'line {i}: class out of range'))
			if len(coords) < 6 or len(coords) % 2 != 0:
				bad.append((str(p), f'line {i}: invalid coord count'))
			if any((c < 0.0 or c > 1.0) for c in coords):
				bad.append((str(p), f'line {i}: coord out of [0,1]'))

print(f'total_label_files={total}')
print(f'empty_label_files={empty}')
print(f'invalid_entries={len(bad)}')
for row in bad[:30]:
	print('BAD:', row[0], '|', row[1])
if bad:
	raise SystemExit('Seg label validation failed.')
print('OK: segmentation labels look valid.')
PY
```

### 12.5 Check class coverage and imbalance
```bash
python3 - << 'PY'
from pathlib import Path
from collections import Counter

names = [
	'50_maxspeed','80_maxspeed','Crosswalk','Gate','Pedestrians_crossing','Stop_sign',
	'Traffic_priority','both_arrow','car','cars not allowed','left_cross','obstacle',
	'right_cross','traffic_lights_green','traffic_lights_off','traffic_lights_red','traffic_lights_yellow'
]

counter = Counter()
for d in [
	Path('/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset_seg/train/labels'),
	Path('/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset_seg/valid/labels'),
]:
	for p in d.glob('*.txt'):
		for ln in [x.strip() for x in p.read_text().splitlines() if x.strip()]:
			cls = int(float(ln.split()[0]))
			counter[cls] += 1

print('Class distribution:')
for i in range(len(names)):
	print(f'{i:02d} {names[i]}: {counter.get(i, 0)}')

missing = [names[i] for i in range(len(names)) if counter.get(i, 0) == 0]
print('Missing classes:', missing if missing else 'None')
PY
```

### 12.6 Create a segmentation-only `data.yaml`
```bash
cat > /home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset_seg/data_seg.yaml << 'YAML'
path: /home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset_seg
train: train/images
val: valid/images
nc: 17
names:
  - 50_maxspeed
  - 80_maxspeed
  - Crosswalk
  - Gate
  - Pedestrians_crossing
  - Stop_sign
  - Traffic_priority
  - both_arrow
  - car
  - cars not allowed
  - left_cross
  - obstacle
  - right_cross
  - traffic_lights_green
  - traffic_lights_off
  - traffic_lights_red
  - traffic_lights_yellow
YAML
```

### 12.7 Clear stale caches
```bash
rm -f \
  /home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset_seg/train/labels.cache \
  /home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset_seg/valid/labels.cache
```

### 12.8 Dry-run validation before training
This catches loader/task issues early.

```bash
yolo segment val \
  model=yolov8n-seg.pt \
  data=/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset_seg/data_seg.yaml \
  imgsz=640 batch=8 device=0
```

If this step fails, fix dataset consistency first. Do not start training yet.
