# Sprint 13 Context (YOLO8/YOLO26 Detect+Seg)

Last update: 2026-05-07 (Phase F complete)

## Index

- [1. Objective](#1-objective)
- [2. Dataset Locations (Lenovo)](#2-dataset-locations-lenovo)
- [3. Model/Artifact Paths and Hailo Paths](#3-modelartifact-paths-and-hailo-paths)
- [4. Official Ultralytics References (Core)](#4-official-ultralytics-references-core)
- [5. Official Website Discovery Scope](#5-official-website-discovery-scope)
- [6. Commands Baseline (Train/Predict/Export)](#6-commands-baseline-trainpredictexport)
- [7. Hailo Conversion and Runtime Notes](#7-hailo-conversion-and-runtime-notes)
- [8. Known Constraints to Keep in Sprint 13](#8-known-constraints-to-keep-in-sprint-13)
- [9. Current Recommendation Snapshot](#9-current-recommendation-snapshot)
- [10. Class Inventory (Project-Specific)](#10-class-inventory-project-specific)
- [11. Hailo Model Zoo Location and Contents](#11-hailo-model-zoo-location-and-contents)
- [12. Strategy Update (2026-05-05)](#12-strategy-update-2026-05-05)
- [13. Clarifications Added (2026-05-06)](#13-clarifications-added-2026-05-06)
- [14. ONNX -> HEF Detailed Phase Map (2026-05-06)](#14-onnx---hef-detailed-phase-map-2026-05-06)
- [15. End-to-End Architecture Clarification (2026-05-06)](#15-end-to-end-architecture-clarification-2026-05-06)

## 1. Objective
Build a fair and traceable comparison of 4 models with their correct datasets:

- YOLOv8s (detect)
- YOLOv8-seg (instance segmentation)
- YOLO26 (detect)
- YOLO26-seg (instance segmentation)
- Compare YOLOv8s (new training) vs Vasco `best.pt` using the same detect split and runtime conditions.
- Note: keep YOLO26 as INT8 (avoid hybrid output for this comparison).

Pipeline to compare end-to-end:
`decode -> preprocess -> infer -> postprocess -> render`

## 1.1 Current Progress Snapshot (Live)

- Phase A: complete (dataset QA and freeze).
- Phase B: complete (4-model training finished).
- Phase C: complete (prediction videos generated).
- Phase D: complete (validation metrics generated).
- Phase E: complete (ONNX + HEF complete for 4 models).
- Phase F: complete (full AGL runtime benchmark completed).

## 1.2 Phase B Results Snapshot (Final Epoch)

- YOLOv8s detect: mAP50=0.9457, mAP50-95=0.6525, precision=0.8808, recall=0.9131
- YOLO26n detect: mAP50=0.9192, mAP50-95=0.6480, precision=0.8850, recall=0.8652
- YOLOv8n-seg: mAP50-M=0.6365, mAP50-95-M=0.3641, precision=0.7200, recall=0.6687
- YOLO26n-seg: mAP50-M=0.6492, mAP50-95-M=0.3956, precision=0.6895, recall=0.6349

Weights location:
- `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/*/weights/best.pt`

## 1.3 Phase C Outputs (Generated)

Output folder:
- `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_c_predictions`

Generated videos (8):
- `yolov8s_detect_sprint13_teste1.mp4`
- `yolov8s_detect_sprint13_teste2.mp4`
- `yolo26n_detect_sprint13_teste1.mp4`
- `yolo26n_detect_sprint13_teste2.mp4`
- `yolov8n_seg_sprint13_teste1.mp4`
- `yolov8n_seg_sprint13_teste2.mp4`
- `yolo26n_seg_sprint13_teste1.mp4`
- `yolo26n_seg_sprint13_teste2.mp4`

Also generated:
- 8 per-run JSON stats files (`*_stats.json`) in the same folder.

## 1.4 Phase D Outputs (Generated)

Output folder:
- `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_d_validation`

Generated validation metrics files (4):
- `yolov8s_detect_val_metrics.json`
- `yolo26n_detect_val_metrics.json`
- `yolov8n_seg_val_metrics.json`
- `yolo26n_seg_val_metrics.json`

Validation metrics snapshot:
- YOLOv8s detect: mAP50=0.9511, mAP50-95=0.6673, precision=0.8926, recall=0.9068
- YOLO26n detect: mAP50=0.9227, mAP50-95=0.6485, precision=0.8253, recall=0.8952
- YOLOv8n-seg (mask): mAP50=0.6811, mAP50-95=0.4058, precision=0.7349, recall=0.6545
- YOLO26n-seg (mask): mAP50=0.6531, mAP50-95=0.4127, precision=0.6050, recall=0.6207

## 1.5 Phase E Outputs (Partial)

Output folder:
- `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_e_export`

Generated ONNX files (4/4):
- `yolov8s_detect_sprint13.onnx`
- `yolo26n_detect_sprint13.onnx`
- `yolov8n_seg_sprint13.onnx`
- `yolo26n_seg_sprint13.onnx`

HEF compile status:
- 0/4 HEF generated.
- command flow updated to Model Zoo profile + `--ckpt` and validated as executable.
- current failures are model/backend specific:
  - detect compile path:
    - `yolov8s_detect_sprint13` -> `AllocatorScriptParserException`.
    - `yolo26n_detect_sprint13` -> `NMSConfigPostprocessException`.
  - segmentation compile path: allocator mapping failure (`BackendAllocatorException`, `Agent infeasible`).
- log reference: `/tmp/phase_e_export_retry2.log`.

### 1.6 Compile Mode Glossary (`full-int8`, `device_nms`, `host_nms`, `no_nms`)

- `full-int8`: model graph fully quantized and compiled to HEF.
- `device_nms` (or `nms-on-device`): NMS is included in `.alls` via `nms_postprocess(...)` and runs on the device flow.
- `host_nms`: NMS is performed in runtime code (host CPU) after inference outputs.
- `no_nms` (compile setting): `.alls` without `nms_postprocess(...)`; this forces runtime to use `host_nms`.

Important:
- `full-int8` is not mutually exclusive with NMS location.
- You can have `full-int8 + device_nms` or `full-int8 + host_nms`.

Sprint 13 current state:
- no HEF produced yet, so there is no validated mode for final benchmark at this moment.
- `no_nms` is currently the most pragmatic fallback path to unblock initial HEF generation if `device_nms` keeps failing.

Can detect be fixed without `no_nms`?
- Yes, potentially, by matching `nms_postprocess` config and end-node mapping to the exported detect graph.
- But this has not been proven yet in Sprint 13; current evidence still shows detect compile failures with `device_nms` flow.
┌──────────────────────────────────────────────────────────────┐
│           yolov8s.alls (Model Zoo Recipe)                    │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  nms_postprocess(                                           │
│    "yolov8s/conv41",    ← presume 84 canais                │
│    "yolov8s/conv42",    ← presume 84 canais                │
│    "yolov8s/conv52",    ← presume 84 canais                │
│    "yolov8s/conv63",    ← presume 84 canais                │
│  )                                                          │
│                                                              │
│  ❌ PROBLEMA: conv41/42/52/63 output é agora (1, 21, ...)  │
│     não (1, 84, ...) como a recipe espera!                 │
│                                                              │
│  Hailo Compiler tenta fazer:                               │
│  ┌─────────────────────────────────────────────────────┐  │
│  │ "Espera: estou à espera de 84 canais                │  │
│  │  mas recebi 21. Isto não bate com a config NMS."    │  │
│  │  → AllocatorScriptParserException                   │  │
│  └─────────────────────────────────────────────────────┘  │
│                                                              │
└──────────────────────────────────────────────────────────────┘
### 1.7 Hardware Architecture Notation: What Does "h8" Mean?

**`h8` = Hailo-8** — the specific chip for which the HEF was compiled.

**Important distinction:**
- HEF (Hailo Executable Format) is **hardware-specific**.
- A HEF compiled for `Hailo-8` cannot run on `Hailo-8L` (lite) or `Hailo-15`.
- The notation `_h8` in filenames is just a naming convention to identify the target architecture.
- The Hailo toolchain exposes this control via `--hw-arch hailo8` flag in `hailomz compile`.

**Examples in Sprint 13 artifacts:**
- `yolov8n_h8.hef` → compiled for Hailo-8
- `yolo26n_seg_320_h8_no_nms.hef` → compiled for Hailo-8 with `no_nms` mode
- `yolov8s_baseline_h8.hef` → baseline model, Hailo-8 target

### 1.8 Root Cause Analysis: Why COCO Models Compiled but SEAME-Trained Models Failed

#### The Root Problem: Model Zoo Recipe Assumes Exact COCO-80 Topology

The `hailomz compile` command uses a **recipe from the Model Zoo** (e.g., `yolov8s`) that contains a `.alls` file (Allocator Script) with **hardcoded NMS configuration** for specific graph nodes:

```python
# From yolov8s.alls (Model Zoo recipe)
nms_postprocess(
    "yolov8s/conv41",    # Presume 84 channels
    "yolov8s/conv42",    # Presume 84 channels
    "yolov8s/conv52",    # Presume 84 channels
    "yolov8s/conv63",    # Presume 84 channels  
)
```

#### Why COCO Models (Stock) Compiled Successfully

**YOLOv8s with COCO-80 classes:**

```
Output shape: (batch=1, num_detections=8400, channels=84)
  = 4 bbox coords + 1 confidence + 80 classes
  = 4 + 1 + 80 = 85 channels (some variance in frame format)
```

**Topology match:**
- Output layer: `(1, 84, 8400)`
- Conv nodes `conv41, conv42, conv52, conv63` present and **exactly as recipe expects**
- NMS mapping succeeds: `compile_baseline.log` line 79-84 shows `output_from_conv41_to_yolov8_nms_postprocess: Pass`
- Result: ✅ **Successful HEF generation** (proof: `yolov8s_baseline_h8.hef` exists, 9.2 MB)

#### Why SEAME-Trained Models (16 Classes) Failed to Compile

**YOLOv8s trained on SEAME dataset (16 custom classes):**

```
Output shape: (batch=1, num_detections=8400, channels=21)
  = 4 bbox coords + 1 confidence + 16 classes  
  = 4 + 1 + 16 = 21 channels (instead of 84)
```

**Topology mismatch cascades:**
- Last layer is now `activation` (sigmoid for multi-label), NOT `conv` as recipe expects
- Conv nodes `conv41/42/52/63` output `(1, 21, ...)` instead of `(1, 84, ...)`
- NMS config hardcoded for 84-channel format → **mismatch**

**Resulting errors from `/tmp/phase_e_export_retry2.log`:**

| Model | Error | Reason |
|---|---|---|
| `yolov8s_detect_sprint13` | `AllocatorScriptParserException: expected conv but found activation layer` | Last layer changed topology |
| `yolo26n_detect_sprint13` | `NMSConfigPostprocessException: layer yolov8s/conv41 doesn't have one output layer` | YOLO26 has native NMS in ONNX; no `conv41` exists |
| `yolov8n_seg_sprint13` | `BackendAllocatorException` / `Agent infeasible` | Channel mismatch in concatenation layers |
| `yolo26n_seg_sprint13` | `BackendAllocatorException` / `Agent infeasible` | Unsupported multi-output topology |

#### Case Study: Vasquinho's Experience (Same Root Cause)

**What happened to Vasquinho:**
- Trained `YOLOv8s` on custom SEAME dataset (16 classes)
- Exported to ONNX → `(1, 21, 8400)` shape
- Used standard Model Zoo recipe (COCO-80 NMS config)
- ✅ **HEF compiled** (unlike Sprint 13 — why?)
- ❌ **HEF produced zero detections**

**Why he got compilation but corrupted output:**
- His Hailo version may have had looser validation on NMS struct mismatch
- The NMS config was applied incorrectly to wrong channel indices
- Inference ran, but NMS produced garbage output

**Sprint 13 went further** — errors appeared at compile time (stricter version rules), not at runtime.

**Lesson learned:** The Model Zoo recipe is **
not** flexible to custom class counts. It assumes COCO-80 exactly.

### 1.9 The Three Solution Approaches

#### Approach 1️⃣: `no_nms` (Our Solution — Already Proven) ✅

**Concept:** Remove `nms_postprocess()` from `.alls`, force NMS to run on host CPU at runtime.

**Evidence it works:**
- Artifact proof: `yolo26n_seg_320_h8_no_nms.hef` (8.2 MB, compiled 2026-04-01 20:51)
- This is an actual HEF that compiled successfully
- No NMS in device graph → no mismatch possible

**Characteristics:**
- **Complexity:** LOW  
- **Setup required:** None (we have infrastructure)
- **Time to deploy all 4:** 2-3 hours  
- **NMS location:** Host CPU (runtime code, ~5-10ms per frame)
- **Process:** `pt → onnx → hailomz compile --model-script <no_nms.alls>` → `hef`
- **Files to create:** `yolov8s_detect_no_nms.alls`, `yolov8n_seg_no_nms.alls`, etc. (copy and remove `nms_postprocess` line)

**Pros:**
- ✅ Already works (proven artifact exists)  
- ✅ Unblocks Phase F immediately
- ✅ No additional dependencies

**Cons:**
- ⚠️ NMS on CPU = ~5-10ms latency per frame on RPi5
- ⚠️ Not optimal for real-time (30 FPS video → ~333ms per frame budget)

#### Approach 2️⃣: `device_nms` Fix (Our Advanced Solution) ⚙️

**Concept:** Rewrite the `.alls` NMS config to match the **actual exported graph topology** (16 classes, 21 channels).

**What needs to change:**
- Identify the actual `conv` nodes in the trained SEAME graph (not COCO-80 nodes)
- Rewrite `nms_postprocess()` to map to **new node names**  
- Adjust channel counts: 21 instead of 84

**Characteristics:**
- **Complexity:** MEDIUM (requires graph debugging)
- **Setup required:** Manual `.alls` rewriting per model
- **Time per model:** 4-8 hours (trial-and-error)
- **NMS location:** Device (Hailo-8 hardware)
- **Process:** `pt → onnx → analyze graph → rewrite .alls → hailomz compile → hef`

**Pros:**
- ✅ Optimal latency: NMS runs inside Hailo → <1ms
- ✅ Frees CPU for other tasks

**Cons:**
- ❌ **Not proven** with SEAME models yet  
- ❌ Requires manual `.alls` authoring (error-prone)
- ❌ High risk of compilation failure
- ❌ Very time-consuming

**Status:** Theoretical; no evidence it will work with Sprint 13 models.

#### Approach 3️⃣: Team5 ONNX Cut + BYOM (Complex Fallback) 🔧

**Concept:** Pre-process the ONNX before compilation (cut ops, reduce complexity), then use Team5's Python API instead of `hailomz compile`.

**Process:**
1. `pt → onnx (opset=17)`
2. Cut ONNX with `cut_onnx_nano.sh` or `cut_onnx_small.sh` (reduce multi-scale concat complexity)
3. `translate.sh` (ONNX → HAR)
4. `quantize.sh` (HAR → quantized HAR, **requires CUDA 12.3**)
5. `compile_hef.sh` with `resources_param(strategy=greedy, max_utilization=0.6/0.9)`

**Characteristics:**
- **Complexity:** HIGH (pipeline has 5 steps)  
- **Setup required:** CUDA 12.3, GPU quantization, Team5 scripts
- **Time per model:** 6-10 hours  
- **NMS location:** Host CPU (no `--model-script`)
- **Process:** Fully custom pipeline (not `hailomz`)

**Pros:**
- ✅ **Proven by Team5** for YOLO26 detect/seg
- ✅ Handles weird topologies (YOLO26 native NMS in ONNX)
- ✅ `resources_param` tuning can solve allocator failures

**Cons:**
- ❌ **Not tested in our environment yet**  
- ❌ Requires CUDA GPU (we have Docker, check setup)
- ❌ 6-10 hours per model is a lot of elapsed time
- ❌ Still produces host-side NMS (like `no_nms`)
- ❌ Complex debugging if it fails

**When to use:** Only if `no_nms` fails for YOLO26 models specifically.

### 1.10 Recommended Strategy: Dual-Track Approach

**Why Dual-Track?** Because we need HEF **now** (Phase F is blocked), but we also want optimized performance later.

```
┌─────────────────────────────────────────────────────────────────┐
│              SPRINT 13 EXECUTION PLAN                           │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  TRACK 1: `no_nms` (IMMEDIATE) ⭐ Priority 1                   │
│  ────────────────────────────────                              │
│  Timeline: 2-3 hours                                           │
│  Goal: Unblock Phase F benchmark                              │
│                                                                 │
│  Steps:                                                        │
│  1. Create `.alls` files without `nms_postprocess` for 4 models│
│     - yolov8s_detect_no_nms.alls                              │
│     - yolov8n_seg_no_nms.alls                                 │
│     - (yolo26n variants with no_nms)                          │
│  2. Run `hailomz compile` with `--model-script` for each      │
│  3. Expected outcome: 4/4 HEF generated                       │
│  4. Measure latency on AGL (Phase F)                          │
│                                                                 │
│  ─────────────────────────────────────────────────────────────│
│                                                                 │
│  TRACK 2: `device_nms` Fix (PARALLEL if time) ⚙️ Priority 2   │
│  ───────────────────────────────────────────────             │
│  Timeline: 4-8 hours (parallel, not blocking)                 │
│  Goal: Optimize NMS latency                                  │
│                                                                 │
│  Steps:                                                        │
│  1. Analyze exported ONNX graph for 16-class topology         │
│  2. Identify actual conv nodes (conv41 replacements)          │
│  3. Rewrite `.alls` with new NMS config                       │
│  4. Test compile on 1 model first (yolov8s_detect)           │
│  5. If successful: benchmark comparison vs no_nms            │
│                                                                 │
│  Success metric: NMS latency improves by >5ms                │
│                                                                 │
│  ─────────────────────────────────────────────────────────────│
│                                                                 │
│  TRACK 3: Team5 ONNX Cut (FALLBACK) 🔧 Priority 3             │
│  ───────────────────────────────────────────────────         │
│  Timeline: Only if track 1 fails                              │
│  Goal: Solve edge cases (YOLO26 detect)                      │
│                                                                 │
│  Trigger conditions:                                          │
│  - IF yolo26n_detect_no_nms.hef fails to compile             │
│  - OR yolo26n_seg_no_nms.hef fails to compile                │
│                                                                 │
│  Then:                                                         │
│  1. Setup CUDA 12.3 environment                              │
│  2. Use Team5 cut_onnx_nano.sh for the failing model          │
│  3. Run full conversion pipeline                              │
│  4. Benchmark comparison                                      │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

**Why this order?**
1. **Track 1 is fastest** → unblocks Phase F immediately
2. **Track 2 is optional optimization** → if it works, great; if not, Track 1 is sufficient
3. **Track 3 is insurance** → only activate if Track 1 fails for specific models

**Success metrics:**
- Track 1 ✅: 4/4 HEF compiled, Phase F runs
- Track 2 ✅: NMS latency < 1ms (device) vs ~5-10ms (host)
- Track 3 ✅: YOLO26 models produce detections

## 2. Dataset Locations (Lenovo)

- YOLOv8 detect dataset:
  - `~/Documents/AI/Yolo_benchmark/roboflow_dataset/yolo8`
- YOLOv8 segmentation dataset:
  - `~/Documents/AI/Yolo_benchmark/roboflow_dataset/yolo8seg`
- YOLO26 detect dataset:
  - `~/Documents/AI/Yolo_benchmark/roboflow_dataset/yolo26`
- YOLO26 segmentation dataset:
  - `~/Documents/AI/Yolo_benchmark/roboflow_dataset/yolo26seg`
- Sprint 13 YAML configs:
  - `~/Documents/AI/Yolo_benchmark/sprint13_configs`

### 2.1 Dataset Statistics (frozen, Phase A)

| Dataset | Split | Images | Notes |
|---|---|---|---|
| yolo8 (detect) | train | 4143 | Roboflow data augmentation applied (4735 total) |
| yolo8 (detect) | valid | 394 | |
| yolo8 (detect) | test | 198 | |
| yolo26 (detect) | train | 4143 | Same export as yolo8 detect |
| yolo26 (detect) | valid | 394 | |
| yolo26 (detect) | test | 198 | |
| yolo8seg | train | 489 | |
| yolo8seg | valid | 47 | |
| yolo8seg | test | 23 | |
| yolo26seg | train | 489 | Same export as yolo8seg |
| yolo26seg | valid | 47 | |
| yolo26seg | test | 23 | |

> **Data augmentation note (detect):** The detection datasets were exported from Roboflow with online data augmentation enabled. The raw annotated set was augmented to reach **4735 total images** (train=4143, valid=394, test=198). Seg datasets are not augmented (small set, 559 total).

- Offline test videos:
  - `~/Documents/AI/Yolo_benchmark/Vasco/final_dataset/teste1.mp4`
  - `~/Documents/AI/Yolo_benchmark/Vasco/final_dataset/teste2.mp4`

## 3. Model/Artifact Paths and Hailo Paths

- Hailo shared folder (host):
  - `~/Documents/AI/hailo/shared_with_docker`
- Typical shared subfolders:
  - `~/Documents/AI/hailo/shared_with_docker/models`
  - `~/Documents/AI/hailo/shared_with_docker/hef`
  - `~/Documents/AI/hailo/shared_with_docker/scripts`
  - `~/Documents/AI/hailo/shared_with_docker/calibration_images`
- Hailo root shown by team:
  - `~/Documents/AI/hailo`

Known existing artifacts already present in shared folder include `.pt`, `.onnx`, `.har`, `.hef` for YOLOv8n/v8n-seg and YOLO26n-seg variants.

## 4. Official Ultralytics References (Core)

### 4.1 Main model/task references

- Model configs repo:
  - `https://github.com/ultralytics/ultralytics/tree/main/ultralytics/cfg/models`
- Models index:
  - `https://docs.ultralytics.com/models/`
- YOLOv8:
  - `https://docs.ultralytics.com/models/yolov8/`
- YOLO26:
  - `https://docs.ultralytics.com/models/yolo26/`
- Detect task:
  - `https://docs.ultralytics.com/tasks/detect/`
- Segment task:
  - `https://docs.ultralytics.com/tasks/segment/`
- Modes:
  - `https://docs.ultralytics.com/modes/`
  - `https://docs.ultralytics.com/modes/train/`
  - `https://docs.ultralytics.com/modes/predict/`
  - `https://docs.ultralytics.com/modes/val/`
  - `https://docs.ultralytics.com/modes/export/`
- Train on platform:
  - `https://www.ultralytics.com/train`
- Ultralytics platform model pages:
  - `https://platform.ultralytics.com/ultralytics/yolov8`
  - `https://platform.ultralytics.com/ultralytics/yolo26`

### 4.2 Segmentation and SAM references

- SAM model docs:
  - `https://docs.ultralytics.com/models/sam/`
- SAM auto-annotation (for seg dataset bootstrap):
  - `https://docs.ultralytics.com/models/sam/#auto-annotation-a-quick-path-to-segmentation-datasets`

## 5. Official Website Discovery Scope

You asked for all YOLO-related official pages/subpages from `ultralytics.com`.

Practical note:
- The official sitemap contains thousands of entries (products, events, glossary, blog).
- For engineering execution, this project should prioritize the pages in Section 4 plus selected high-signal links below.

### 5.1 High-signal additional official pages

- Main product:
  - `https://www.ultralytics.com/`
- YOLO landing:
  - `https://www.ultralytics.com/yolo`
- Annotate:
  - `https://www.ultralytics.com/annotate`
- Deploy:
  - `https://www.ultralytics.com/deploy`
- Solutions (automotive):
  - `https://www.ultralytics.com/solutions/ai-in-automotive`
- Sitemap root (for exhaustive crawl if needed):
  - `https://www.ultralytics.com/sitemap.xml`

## 6. Commands Baseline (Train/Predict/Export)

### 6.1 Detection models

```bash
yolo detect train model=yolov8s.pt data=/path/to/data_detect.yaml imgsz=640 epochs=100
yolo detect train model=yolo26n.pt data=/path/to/data_detect.yaml imgsz=640 epochs=100

yolo predict model=/path/to/best.pt source=/path/to/teste1.mp4 imgsz=640 conf=0.25 save=True

yolo detect val model=/path/to/best.pt data=/path/to/data_detect.yaml imgsz=640

yolo export model=/path/to/best.pt format=onnx imgsz=640 opset=11 simplify=True
```

### 6.2 Segmentation models

```bash
yolo segment train model=yolov8n-seg.pt data=/path/to/data_seg.yaml imgsz=640 epochs=100
yolo segment train model=yolo26n-seg.pt data=/path/to/data_seg.yaml imgsz=640 epochs=100

yolo predict model=/path/to/best.pt source=/path/to/teste2.mp4 imgsz=640 conf=0.25 save=True

yolo segment val model=/path/to/best.pt data=/path/to/data_seg.yaml imgsz=640

yolo export model=/path/to/best.pt format=onnx imgsz=640 opset=11 simplify=True
```

## 7. Hailo Conversion and Runtime Notes

- For Hailo deployment, the effective runtime artifact is `.hef`.
- Typical lineage: `.pt -> .onnx -> .har -> .hef`.
- Video generation in your AGL scripts is done at runtime using Python + HEF inference + render, then saved as `.mp4`.
- Existing scripts in `src/hailo/scripts` already cover:
  - detection runtime (`inference_camera_scalercrop_yolov8s.py`, `inference_camera_scalercrop_yolov8n.py`)
  - segmentation runtime (`inference_camera_scalercrop_yolov8n_seg.py`, `inference_camera_scalercrop_yolo26n_seg.py`)

## 8. Known Constraints to Keep in Sprint 13

- Do not compare detect and seg with the same quality metric without separating tasks.
- For seg models, ensure dataset labels are polygon/seg format and not detect-only labels.
- For fair benchmark, keep fixed:
  - same video inputs,
  - same duration,
  - same confidence thresholds,
  - same output metrics and collection scripts.
- If model uses host-side NMS/post-process, E2E interpretation must include host CPU impact.

## 9. Current Recommendation Snapshot

- Use your new four datasets as intended (one per model/task).
- Keep YOLOv8s vs Vasco best.pt comparison as a dedicated detect baseline track.
- Keep Sprint 13 decision gate based on E2E metrics, not infer-only.

## 10. Class Inventory (Project-Specific)

### 10.1 Detection datasets (17 classes)

- `50_maxspeed`
- `80_maxspeed`
- `Crosswalk`
- `Gate`
- `Pedestrians_crossing`
- `Stop_sign`
- `Traffic_priority`
- `both_arrow`
- `car`
- `cars not allowed`
- `left_cross`
- `obstacle`
- `right_cross`
- `traffic_lights_green`
- `traffic_lights_off`
- `traffic_lights_red`
- `traffic_lights_yellow`

### 10.2 Instance segmentation datasets (19 classes)

- `50_maxspeed_sign`
- `80_maxspeed_sign`
- `arrow_left`
- `arrow_left_front`
- `arrow_right`
- `car`
- `crosswalk`
- `crosswalk_sign`
- `curve`
- `gate`
- `obstacle`
- `shadow`
- `stop_sign`
- `traffic_danger_sign`
- `traffic_light_green`
- `traffic_light_off`
- `traffic_light_red`
- `traffic_light_yellow`
- `traffic_priority_yield_sign`

## 11. Hailo Model Zoo Location and Contents

Current location used by the team:

- `~/Documents/AI/hailo`

Observed top-level contents:

- `calibration_images/`
- `docker_custom/`
- `hailo-dfc/`
- `Pre-Requisitos/`
- `shared_with_docker/`

Operational note:

- `shared_with_docker/` is the practical working area for model artifacts and conversion pipeline assets (`models/`, `hef/`, logs, scripts, calibration inputs).

## 12. Strategy Update (2026-05-05)

### 12.1 What is BYOM?

BYOM means `Bring Your Own Model`.

In this sprint context, BYOM means using your exported ONNX and the SDK pipeline directly:

1. `translate_onnx_model(...)`
2. `optimize(...)` with representative calibration data
3. `compile()` to generate HEF

This avoids hard dependency on fixed Model Zoo COCO recipes (`conv41/conv42/...`) and allows explicit control of `end_node_names` from custom trained graphs.

### 12.2 Ranking of the 3 Conversion Options (practical)

This ranking depends on which criterion is primary:

| Criterion | Best option | Why |
|---|---|---|
| Fastest unblock today | `hailomz + no_nms` | Fastest path to generate HEF, lowest compile friction |
| Most correct for custom 16-class graphs | `BYOM (ClientRunner + explicit end_node_names)` | Decouples from COCO node assumptions and reduces mismatch risk |
| Best potential runtime latency (if tuned correctly) | `hailomz + corrected device_nms (.alls + JSON)` | NMS on device can reduce host load and latency |

Practical interpretation:
- `no_nms` is best for speed-to-first-HEF.
- `BYOM` is best for correctness on custom topology.
- `device_nms corrected` can be best in final performance, but has highest tuning risk.

### 12.3 Team5 vs Our Approach (decision rule)

Our approach is not worse. It already proved value (`YOLO26n-seg split hybrid` and previous `no_nms` artifact).

Use this rule:

1. Try our proven path first (`BYOM` or `no_nms` depending on model).
2. Use Team5 techniques only as surgical fallback when compiler blocks on allocator issues.
3. Do not replace whole pipeline if fallback is only needed for specific models.

Team5 value is mainly fallback for hard cases (`BackendAllocatorException`, `Agent infeasible`), especially on YOLO26-seg.

### 12.4 Final Matrix by Model (Sprint 13)

| Model | Primary path | Fallback 1 | Fallback 2 | Success criteria |
|---|---|---|---|---|
| `yolov8s_detect_sprint13` | BYOM (Vasco style, explicit `end_node_names`) | `hailomz + no_nms` | `hailomz + corrected device_nms` | HEF generated + valid detections + p95 within gate |
| `yolo26n_detect_sprint13` | BYOM (YOLO26 custom graph) | Team5-style ONNX cut + BYOM | `hailomz + no_nms` only if graph compatible | HEF generated + no NMS config crash + valid detections |
| `yolov8n_seg_sprint13` | BYOM with host-side postprocess | ONNX simplify/cut + BYOM | `hailomz + no_nms` | HEF generated + no allocator failure + stable masks |
| `yolo26n_seg_sprint13` | Team5-style cut (`cut_onnx_*`) + BYOM/resources tuning | `hailomz + no_nms` | deeper graph reduction | HEF generated + no `Agent infeasible` + E2E quality acceptable |

### 12.5 Implementation Plan (updated)

Execution order (fastest unblock with controlled risk):

1. Compile in parallel: `yolov8s_detect` (BYOM) and `yolo26n_seg` (Team5/cut path).
2. Then compile `yolo26n_detect` (BYOM, no COCO recipe).
3. Then compile `yolov8n_seg` (BYOM, fallback cut if needed).
4. After HEFs exist, run real inference validation (not compile-only).

Pass/fail gates per model:

- Compile gate: HEF exists, parse succeeds, no critical compiler exception.
- Functional gate: detections/masks are semantically valid on test videos.
- Performance gate: E2E p95 and CPU stay within sprint thresholds.

Decision rule for production use:

- `infer-only` metrics can rank candidates technically.
- Final ADAS decision must be `E2E vs E2E`, including host-NMS overhead when applicable.

## 13. Clarifications Added (2026-05-06)

### 13.1 Can we run `hailomz + no_nms` first and BYOM later?

Yes. That is safe and recommended when the goal is fast unblock plus correctness track.

Why this does not "break" BYOM:

- `hailomz + no_nms` works from ONNX + `.alls` script and outputs HEF.
- BYOM works from ONNX + SDK pipeline (`translate -> optimize -> compile`) and outputs HAR/HEF.
- They can share the same ONNX input but generate separate artifacts.
- One does not overwrite the other if outputs are named and stored in separate files/folders.

Practical safety rule:

1. Freeze ONNX input files (read-only naming by model/version).
2. Store `no_nms` outputs under dedicated names.
3. Store BYOM outputs under dedicated names.
4. Never reuse generic names like `best.hef` across methods.

Example naming:
- `yolov8s_detect_sprint13_no_nms.hef`
- `yolov8s_detect_sprint13_byom.hef`

### 13.2 Is BYOM the same as ONNX cut?

No.

- BYOM is a conversion approach (bring your own ONNX into SDK).
- ONNX cut is an optional graph surgery step that can be used before BYOM.

So ONNX cut can be part of BYOM pipeline, but BYOM does not require cut by definition.

### 13.3 Is BYOM the same as fixing `.alls` outputs?

No, they are different control layers.

`BYOM` (Vasco style):
- Controls parse/translation endpoints via `end_node_names` from real graph nodes.
- Does not depend on Model Zoo COCO recipe node names.
- Uses SDK runner API directly.

`Fix .alls outputs` (device NMS corrected):
- Keeps `hailomz` + Model Zoo style flow.
- Requires editing `.alls` and NMS JSON to match custom graph layers/classes.
- Still tied to correct mapping between recipe semantics and actual graph.

Key difference:
- BYOM bypasses recipe coupling.
- `.alls` fix repairs recipe coupling.

### 13.4 Did Vasco use COCO `.alls` recipe?

In the observed scripts, no.

- Vasco used `ClientRunner` parse/optimize/compile with explicit end nodes.
- That is BYOM-style conversion.
- This is the main difference from `hailomz yolov8s` recipe path.

### 13.5 How to implement corrected `.alls`/NMS (device-side) in practice

Detailed steps:

1. Inspect exported ONNX and list real detection head output layers.
2. Identify regression/classification branches per stride (`8/16/32`).
3. Create custom NMS JSON:
  - set `classes` to project class count (for detect model),
  - map `reg_layer`/`cls_layer` to real layer names,
  - keep image dims and thresholds explicit.
4. Create custom `.alls`:
  - normalization,
  - output activations if required,
  - `nms_postprocess(<custom_json>, meta_arch=..., engine=...)`.
5. Compile with `hailomz` using this custom model script.
6. Validate:
  - compile success,
  - detection sanity (not empty/corrupted),
  - class mapping and confidence distributions.

Why this is hard:
- Layer naming can differ from stock recipes.
- Wrong decoder mapping may compile but produce bad detections.
- Iterative debugging is common.

### 13.6 BYOM (Vasco) vs `hailomz` path: what changes exactly?

Main differences:

1. Entry command/interface:
  - BYOM: SDK API (`ClientRunner`).
  - Recipe path: `hailomz compile <model>`.
2. Graph binding:
  - BYOM: explicit `end_node_names` from real ONNX.
  - Recipe path: implicit expected nodes from Model Zoo profile.
3. NMS coupling:
  - BYOM: can stay decoupled from COCO recipe assumptions.
  - Recipe path: often coupled to recipe NMS config unless customized.
4. Failure mode profile:
  - BYOM: fewer recipe mismatch errors, still possible allocator issues.
  - Recipe path: more likely to fail on topology mismatch if model is not stock.

### 13.7 Team5 pipeline explained (what, how, why)

Team5 flow (conceptual):

1. Cut ONNX (`cut_onnx_*`):
  - What: edit graph outputs/branches to reduce compile complexity.
  - Why: reduce allocator pressure and remove problematic graph regions.
2. Translate to HAR:
  - What: import ONNX to Hailo internal representation.
  - Why: required intermediate for optimization/compile.
3. Quantize/optimize:
  - What: calibrate INT8 scales with representative images.
  - Why: map model to hardware-friendly numeric ranges.
4. Compile with tuning:
  - What: compile HAR to HEF with resource strategy settings.
  - Why: improve mapping success on constrained graphs.

Why/when to use Team5 techniques:
- Use as fallback when normal paths fail with allocator/solver errors.
- Especially useful for YOLO26-seg edge cases.
- Not mandatory for every model; apply surgically where needed.

## 14. ONNX -> HEF Detailed Phase Map (2026-05-06)

### 14.1 All major phases between ONNX and HEF

The conversion path from ONNX to HEF contains several distinct phases. It is not only `binding/parsing` and `NMS configuration`.

1. Graph import / parse
- Read ONNX graph and create internal representation.
- In BYOM, this is where `translate_onnx_model(...)` and `end_node_names` matter.

2. Graph binding / endpoint selection
- Decide where the graph starts and ends for Hailo compilation.
- This is where explicit output layers or end nodes are selected.

3. Model-script integration
- Apply `.alls` instructions such as normalization, output activations, and optional `nms_postprocess(...)`.
- This is compile-time metadata/configuration, not runtime app preprocessing.

4. Postprocess/NMS configuration binding
- If device-side NMS is used, the compiler must map JSON decoder config to real graph layers and channel structure.
- This is where COCO-80 assumptions can fail for custom 16-class models.

5. Quantization / optimization
- Use calibration data to determine numeric ranges and quantize graph to INT8 where possible.
- This affects numeric precision, not semantic class count.

6. Allocator / mapping / partitioning
- The compiler tries to place graph operations into Hailo hardware resources and contexts.
- This is where `BackendAllocatorException`, `Agent infeasible`, or long mapping timeouts appear.

7. Compile / HEF generation
- Final hardware executable is emitted as `.hef`.

8. Runtime postprocess (outside compilation)
- If `no_nms` or host-side path is used, CPU code performs decode/NMS after inference.
- This is not part of compile-time, but is part of final E2E runtime.

### 14.2 Are allocator and tuning phases too?

Yes.

- `allocator` / `mapping` is a compile-time phase where hardware resources are assigned.
- `tuning` means changing compile parameters (for example `resources_param`, max utilization, cut graph strategy) to help allocator succeed.

They are compile-time phases, not runtime stages.

### 14.3 Do these belong to preprocessing or postprocessing?

Usually no, not in the application E2E sense.

Important distinction:

- `decode -> preprocess -> infer -> postprocess -> render` describes runtime application flow.
- `parse -> bind -> optimize -> allocate -> compile` describes model conversion flow.

So:
- Binding/parsing is compile-time, before any runtime pipeline exists.
- Device-NMS configuration is compile-time configuration for what will later behave like postprocessing during inference.
- Host-NMS is runtime postprocessing.

### 14.4 Where do these fit in `decode -> preprocess -> infer -> postprocess -> render`?

They do not sit inside that runtime chain directly; they happen before deployment.

Mapping:

- Compile-time:
  - parse / binding / `.alls` / NMS config / quantize / allocator / compile
- Runtime:
  - `decode -> preprocess -> infer -> postprocess -> render`

Special note about NMS:
- `device_nms`: configured at compile-time, executed effectively inside `infer`.
- `host_nms`: not compiled into device flow; executed inside runtime `postprocess`.

### 14.5 How each solution changes the system and why errors happen

Not all solutions "change channels" directly.

What each solution mainly changes:

1. `hailomz + corrected device_nms`
- Changes compile-time expectations for decoder layers, class count, and NMS wiring.
- It usually does not change the model channels themselves.
- It changes the compiler's interpretation of those channels.

2. `hailomz + no_nms`
- Removes device-side NMS from model script.
- Does not change graph channels.
- Avoids NMS mapping errors by not asking compiler to wire NMS at all.

3. `BYOM`
- Changes how graph endpoints are selected and translated.
- Does not inherently change channels.
- Avoids dependence on wrong recipe assumptions.

4. `ONNX cut`
- Can change graph structure and sometimes exposed outputs/branches.
- This can indirectly change what the compiler has to map.

5. Quantization/optimization
- Changes numeric precision/ranges.
- Does not change class count or semantic meaning of channels.

Why current errors happen:

- `AllocatorScriptParserException` / `NMSConfigPostprocessException`:
  - Mostly due to wrong layer expectations, wrong topology assumptions, or wrong decoder mapping.
- `BackendAllocatorException` / `Agent infeasible`:
  - Mostly due to graph complexity/resource placement issues.
  - Can be influenced by graph shape/branches/contexts, not only by class channels.

### 14.6 Team5 pipeline detail (requested text)

Cut ONNX
- What: simplify/adjust output branches.
- Why: reduce complexity that breaks allocator.

Translate to HAR
- What: convert ONNX into Hailo internal IR.
- Why: mandatory stage before optimize/compile.

Quantize/Optimize
- What: calibration and INT8 quantization.
- Why: map numeric ranges to hardware constraints.

Compile with tuning (`resources_param`, etc.)
- What: compile with resource allocation/use parameters.
- Why: increase mapping success probability on difficult graphs, especially YOLO26-seg.

### 14.7 Time estimates

Estimated elapsed time (practical, not idealized):

| Task | Estimated time |
|---|---|
| `hailomz + no_nms` on one model | 30-90 min |
| BYOM on YOLOv8 detect if graph is already known | 1-3 h |
| Correct `.alls` / NMS JSON for YOLOv8 detect | 2-6 h |
| BYOM on seg model | 2-4 h |
| Team5 cut + translate + optimize + tuned compile on hard YOLO26-seg | 4-10 h |

For corrected `.alls` specifically:

1. Inspect ONNX / identify real head layers: `30-90 min`
2. Create custom JSON + custom `.alls`: `30-90 min`
3. Compile + first debug cycle: `1-3 h`
4. Functional validation of detections/classes: `30-120 min`

Practical total:
- Best case: `~2 h`
- Realistic case: `4-6 h`
- Hard case with multiple retries: `6-8 h`

## 15. End-to-End Architecture Clarification (2026-05-06)

### 15.1 What is an ONNX graph?

An ONNX model is a directed computational graph:
- Nodes = operations (`Conv`, `Concat`, `Sigmoid`, `Reshape`, etc.)
- Edges/tensors = data flowing between operations
- Inputs/outputs = graph entry and exit points

Simplified representation:

```text
Input image (1,3,640,640)
    |
  Backbone
    |
   Neck (multi-scale features)
  /   |   \
Head8 Head16 Head32
  \   |   /
   Concat / decode
      |
  Output tensor(s)
```

For YOLO detect, output is typically `(1, C, N)` where:
- `C = 4 bbox + objectness + num_classes`
- `N = number of candidate positions`

### 15.2 Full process map: before compile-time, compile-time, runtime, after runtime

There are more phases than only compile-time and runtime.

Phase A - Pre-compile preparation:
1. Train/fine-tune `.pt`
2. Export `.pt -> .onnx`
3. Optional ONNX surgery (`cut/simplify`) if graph is too hard

Phase B - Compile-time (`onnx -> hef`):
1. Parse/import ONNX
2. Binding/endpoint selection (`start_node_names`, `end_node_names`)
3. Model script integration (`.alls`: normalization, activations, optional NMS)
4. NMS config binding (when `device_nms` is used)
5. Quantize/optimize (calibration + INT8 ranges)
6. Allocator/mapping/partitioning (fit graph into hardware resources)
7. Compile and emit HEF

Phase C - Deployment/runtime:
1. Load HEF in target runtime
2. `decode -> preprocess -> infer -> postprocess -> render`
3. If no device NMS, host CPU does NMS in runtime postprocess

Phase D - Post-runtime validation:
1. Functional checks (detections/masks sane)
2. E2E performance checks (p50/p95/p99, CPU/jitter)
3. Quality checks by class/scenario

### 15.3 Are allocator/mapping and tuning separate phases?

Yes:
- Allocator/mapping is a compile-time phase.
- Tuning is a compile strategy applied around allocator/mapping and graph complexity.

Examples of tuning:
- resource constraints (`resources_param` style)
- graph reduction (`cut ONNX`)
- changing endpoint strategy

### 15.4 Compile-time vs runtime (and preprocess/postprocess confusion)

Key distinction:
- Compile-time flow is model conversion workflow.
- Runtime flow is application inference workflow.

So:
- `binding/parsing` is not runtime preprocess.
- `NMS config binding` is compile-time config.
- `host NMS` is runtime postprocess.

### 15.5 Channels: what changes and what usually does not

Most solution switches do not directly change class channels.

What usually changes channels:
- training class count
- architecture/head design
- optional ONNX graph surgery that rewires outputs

What usually does not change channels directly:
- selecting BYOM vs recipe path
- enabling/disabling device NMS in `.alls`
- quantization itself

Why errors happen anyway:
- recipe expects wrong layers/channels
- NMS decoder mapping mismatched to real graph
- allocator cannot place graph in hardware resources

### 15.6 Where each solution enters in the process map

`hailomz + no_nms`:
- Compile-time: phase B3 (script without device NMS), B6, B7
- Runtime: host NMS shifts into phase C postprocess

`BYOM (ClientRunner)`:
- Compile-time: phase B1/B2/B5/B6/B7 with explicit graph endpoints
- Runtime: depends on runtime app postprocess strategy

`device_nms corrected (.alls + JSON)`:
- Compile-time: phase B3/B4 (largest effect), then B6/B7
- Runtime: less host postprocess burden

`Team5 cut + translate + quantize + tuned compile`:
- Pre-compile: phase A3 (cut ONNX)
- Compile-time: B1/B2/B5/B6/B7 with stronger tuning pressure

### 15.7 Team5 steps mapped exactly

1. Cut ONNX
- What: simplify/adjust output branches
- Why: reduce allocator-breaking graph complexity
- Process map: Phase A3

2. Translate to HAR
- What: ONNX -> Hailo internal IR
- Why: mandatory before optimize/compile
- Process map: Phase B1/B2

3. Quantize/Optimize
- What: calibration + INT8 quantization
- Why: map numeric ranges to hardware
- Process map: Phase B5

4. Compile with tuning
- What: compile using mapping/resource tuning parameters
- Why: improve mapping success on hard graphs (especially YOLO26-seg)
- Process map: Phase B6/B7

### 15.8 Risk ranking by solution

Postprocess/inference pain (most likely to hurt runtime):
1. `no_nms` (highest host CPU/jitter risk)
2. Team5 hard fallback pipelines (host-side heavy, higher integration complexity)
3. BYOM (moderate; depends on runtime postprocess implementation)
4. corrected `device_nms` (lowest runtime burden if done correctly)

Whole-process pain (compile + runtime):
1. corrected `device_nms` (hard to get right; high compile debugging effort)
2. Team5 hard flow (many moving parts)
3. BYOM (middle complexity)
4. `no_nms` (fastest compile path but runtime tradeoff)

### 15.9 Risk ranking by model (Sprint 13)

Most difficult end-to-end:
1. `yolo26n_seg_sprint13`
2. `yolov8n_seg_sprint13`
3. `yolo26n_detect_sprint13`
4. `yolov8s_detect_sprint13`

Reason summary:
- segmentation + YOLO26 + allocator pressure is worst combination
- detect YOLOv8 custom is easier with BYOM endpoint control

### 15.10 Recommended strategy validity

This strategy is valid and coherent:

1. `no_nms` first for rapid HEF unblock.
2. BYOM in parallel for correctness/stability.
3. corrected `device_nms` as final optimization track.

Estimated elapsed time (4 models, practical):
- Fastest technical unblock to first usable HEFs: `3-8 h`
- Parallel BYOM stabilization: `4-10 h`
- Device-NMS optimization track: `4-12 h`
- Full cycle including validation: `1-2 working days`

### 15.11 Notes on "all phases"

For Sprint 13 documentation, the compile-time list is complete enough operationally.
Lower-level internal compiler substeps exist, but they are not usually controllable directly by project scripts.

## 16. Final Sprint 13 Runtime Snapshot (2026-05-07)

### 16.1 What was executed

- Target: `root@10.21.220.191` (AGL)
- Command: `bash /data/yolo_benchmark/scripts/run_video_benchmark_sprint13.sh`
- Inputs: 4 HEFs in `/data/yolo_benchmark/models/hef/sprint13` and 2 videos in `/data/yolo_benchmark/videos`
- Outputs: `/data/yolo_benchmark/results/sprint13/*.mp4` and `*_stats.json`

### 16.2 Runtime confirmation (AGL + Hailo)

Evidence from logs:
- `Source: 640x480 @ 25.0fps` and `HEF loaded: /data/yolo_benchmark/models/hef/sprint13/...`
- output vstreams listed by HEF (`conv41`, `conv42`, ...)
- per-run summary includes `Hailo medio`, confirming accelerator-side inference timing was captured

### 16.3 Consolidated numbers (mean across teste1 + teste2)

| Model | FPS real | Hailo avg | p50 | p95 | p99 |
|---|---:|---:|---:|---:|---:|
| `yolov8s_detect_sprint13.hef` | 28.3 | 14.4 ms | 31.4 ms | 36.2 ms | 40.5 ms |
| `yolo26n_detect_sprint13.hef` | 46.0 | 10.5 ms | 17.8 ms | 22.2 ms | 25.3 ms |
| `yolov8n_seg_sprint13.hef` | 32.5 | 11.5 ms | 27.1 ms | 32.0 ms | 35.0 ms |
| `yolo26n_seg_sprint13.hef` | 39.2 | 14.5 ms | 21.9 ms | 26.1 ms | 28.6 ms |

### 16.3.1 Per-video numbers (no averaging)

| Model | Video | FPS real | Hailo avg | p50 | p95 | p99 |
|---|---|---:|---:|---:|---:|---:|
| `yolov8s_detect_sprint13.hef` | `teste1` | 28.34 | 14.39 ms | 31.35 ms | 36.62 ms | 40.80 ms |
| `yolov8s_detect_sprint13.hef` | `teste2` | 28.31 | 14.33 ms | 31.30 ms | 35.81 ms | 40.19 ms |
| `yolo26n_detect_sprint13.hef` | `teste1` | 45.82 | 10.53 ms | 17.57 ms | 22.17 ms | 25.69 ms |
| `yolo26n_detect_sprint13.hef` | `teste2` | 46.20 | 10.53 ms | 17.86 ms | 22.14 ms | 24.93 ms |
| `yolov8n_seg_sprint13.hef` | `teste1` | 32.40 | 11.49 ms | 27.29 ms | 30.99 ms | 34.92 ms |
| `yolov8n_seg_sprint13.hef` | `teste2` | 32.51 | 11.40 ms | 26.84 ms | 32.88 ms | 35.06 ms |
| `yolo26n_seg_sprint13.hef` | `teste1` | 39.16 | 14.55 ms | 21.88 ms | 25.98 ms | 28.20 ms |
| `yolo26n_seg_sprint13.hef` | `teste2` | 39.09 | 14.53 ms | 21.94 ms | 26.23 ms | 28.86 ms |

### 16.3.2 Phase C `.pt` detailed metrics added

To complement old Phase C counts-only JSON files, a detailed run was executed and saved to:
- `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_c_predictions/phase_c_detailed_pt_metrics.json`

Average (2 videos):

| Model `.pt` | FPS real | Inference avg | Mean conf | p50 conf | p95 conf |
|---|---:|---:|---:|---:|---:|
| `yolov8s_detect` | 183.26 | 3.361 ms | 0.686 | 0.736 | 0.896 |
| `yolo26n_detect` | 183.35 | 3.961 ms | 0.685 | 0.746 | 0.950 |
| `yolov8n_seg` | 197.12 | 2.970 ms | 0.677 | 0.710 | 0.979 |
| `yolo26n_seg` | 152.73 | 4.810 ms | 0.651 | 0.650 | 0.987 |

### 16.4 Important interpretation note

Segmentation HEFs in this sprint were benchmarked as box-only postprocess in runtime.
Reason: BYOM compile endpoints included `cv2/cv3` (+ proto), but no `cv4` mask-coeff branch in the runtime decode path.

### 16.5 E2E scope clarification

- Phase F `avg_total_ms` is not full E2E; it captures preprocess + infer + postprocess only.
- Phase F `real_fps` is the practical full-loop proxy because it includes frame IO and render/write overhead.
- For strict stage accounting (`decode->preprocess->infer->postprocess->render`) the runtime JSON should include explicit `decode_ms` and `render_ms` per frame in the next iteration.

### 16.6 Confidence `0.50` investigation

Observed mismatch: HEF overlays often around `0.50`, while `.pt` confidence spread is wider.

Likely contributors:
1. Score displayed with 2-decimal rounding in overlay.
2. Runtime threshold at `0.45` keeps many near-threshold values.
3. INT8 quantization narrows confidence distribution relative to FP `.pt`.

Latest sweep confirmation (`yolov8s_detect_sprint13.hef`, `teste1/teste2`):
- `conf=0.25..0.45`: `p50_conf` remains around `0.5000`.
- `conf=0.55`: `p50_conf` shifts up (`~0.58` to `~0.65`) and detections reduce.
- Interpretation: threshold alone does not remove the clustering pattern; postprocess and quantization/calibration remain primary suspects.

A/B comparison path added:
- Baseline mode (current Sprint13 behavior) vs Vasco-like mode on the same HEF.
- Output folder:
  - `/data/yolo_benchmark/results/fase_f_comparacao_com_vasco/`
- Runner script:
  - `src/hailo/scripts/Vasquinho/run_phase_f_comparacao_com_vasco.sh`
4. Potential input-scale inconsistency: BYOM calibration used `0..1` normalization while runtime sends `UINT8 0..255`.

Mitigation already applied:
- `inference_video_sprint13.py` now writes `mean_conf/p50_conf/p95_conf` at frame level and global level to support objective validation.

## 17. Delta Context (2026-05-08)

### 17.1 ONNX source used by Vasco wrapper

Wrapper used: `src/hailo/scripts/Vasquinho/run_vasco_scripts_all_models.sh`

Source directory:
- `/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_e_export`

File selection rule used by wrapper:
- default: `${MODEL_NAME}_sprint13.onnx`
- special-case for `yolo26n_seg`: `${MODEL_NAME}_sprint13_fixed.onnx` when present

### 17.2 HEF outputs generated in this delta

- `yolov8s_detect_sprint13_vasco.hef`
- `yolo26n_detect_sprint13_vasco.hef`
- `yolov8n_seg_sprint13_vasco.hef`
- `yolo26n_seg_sprint13_vasco_noproto.hef` (final usable variant)

Note:
- `yolo26n_seg` full-proto compile remained blocked by allocator error (`matmul1` multi-output).
- `_noproto` variant compiles and runs; runtime still reports seg outputs for compare path.

### 17.3 Lenovo video outputs (final compare run)

Folder:
- `/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl_rerun/compare_onnx_vs_hef_hostdecode/`

Main MP4 outputs:
- `teste1_yolov8s_detect_onnx_hostdecode.mp4`
- `teste1_yolov8s_detect_hef_hostdecode.mp4`
- `teste1_yolo26n_detect_onnx_hostdecode.mp4`
- `teste1_yolo26n_detect_hef_hostdecode.mp4`
- `teste1_yolov8n_seg_onnx_hostdecode.mp4`
- `teste1_yolov8n_seg_hef_hostdecode.mp4`
- `teste1_yolo26n_seg_onnx_hostdecode.mp4`
- `teste1_yolo26n_seg_hef_hostdecode.mp4`

### 17.4 Script hygiene applied

Removed obsolete/superseded files from `src/hailo/scripts/Vasquinho/`:
- `run_vasco_docker_pipeline.sh`
- `run_vasco_full_pipeline.sh`
- `run_vasco_original_on_our_models.sh`
- `run_recalib_then_compare_detect.sh`
- `run_compare_detect_models_onnx_vs_hef.sh`
- temp artifacts: `best.har`, `best.onnx`, `yolov8s.hef`, `run_best.py`
- caches: `__pycache__/`

### 17.5 `teste2` consistency artifacts and ONNX snapshot

Consistency subfolder created for `teste2` compare outputs:
- `/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl_rerun/compare_onnx_vs_hef_hostdecode/consistencia_teste2_2026-05-08/`

Consistency subfolder created for `teste1` compare outputs:
- `/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl_rerun/compare_onnx_vs_hef_hostdecode/consistencia_teste1_2026-05-08/`

ONNX files explicitly used in hostdecode compare:
- `yolov8s_detect_sprint13.onnx`
- `yolo26n_detect_sprint13.onnx`
- `yolov8n_seg_sprint13.onnx`
- `yolo26n_seg_sprint13.onnx`

ONNX snapshot subfolder created:
- `/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_e_export/onnx_usados_compare_hostdecode_2026-05-08/`

### 17.6 Why some compare videos are very short

- Short compare clips were generated with `--max-frames 30` for fast parity checks.
- At 25 FPS this produces ~`1.2s` videos.
- This does not indicate data loss; it is a controlled short-run mode.

Additional note (2026-05-08 late):
- `teste2` detect HEF full-length artifacts were also staged into the consistency folder:
  - `teste2_yolov8s_detect_hef_full.mp4`
  - `teste2_yolo26n_detect_hef_full.mp4`
