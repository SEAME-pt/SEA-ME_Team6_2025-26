# Planning Sprint 13 - YOLO8/YOLO26 Full Pipeline

Last update: 2026-05-07 (Phase F complete)

## Index

- [1. Goal](#1-goal)
- [2. Direct Answers to Key Questions](#2-direct-answers-to-key-questions)
- [3. Scope and Metrics](#3-scope-and-metrics)
- [4. Execution Plan (Phased)](#4-execution-plan-phased)
- [4.1 Understanding Hardware Notation: What \"h8\" Means](#41-understanding-hardware-notation-what-h8-means)
- [4.2 Root Cause Analysis: Why COCO Models Compiled but SEAME-Trained Failed](#42-root-cause-analysis-why-coco-models-compiled-but-seame-trained-failed)
- [4.3 Three Solution Approaches: Comparison & Tradeoffs](#43-three-solution-approaches-comparison--tradeoffs)
- [4.4 Recommended Execution Strategy: Dual-Track](#44-recommended-execution-strategy-dual-track)
- [5. YOLOv8s vs Vasco Comparison Track](#5-yolov8s-vs-vasco-comparison-track)
- [6. Colab Notebook and Script Status](#6-colab-notebook-and-script-status)
- [7. Runtime Scripts Keep/Review Decision](#7-runtime-scripts-keepreview-decision)
- [8. Commands and Locations to Preserve in Docs](#8-commands-and-locations-to-preserve-in-docs)
- [9. Risks and Mitigations](#9-risks-and-mitigations)
- [10. Sprint 13 Deliverables](#10-sprint-13-deliverables)
- [11. Updated Decision Matrix (2026-05-05)](#11-updated-decision-matrix-2026-05-05)
- [12. Updated Implementation Plan (execution)](#12-updated-implementation-plan-execution)
- [13. Additional Clarifications (2026-05-06)](#13-additional-clarifications-2026-05-06)
- [14. ONNX to HEF: Detailed Compile-Time vs Runtime Map](#14-onnx-to-hef-detailed-compile-time-vs-runtime-map)
- [15. Consolidated Process and Updated Plan (2026-05-06)](#15-consolidated-process-and-updated-plan-2026-05-06)
- [16. Confidence Score Correction Plan (2026-05-07)](#16-confidence-score-correction-plan-2026-05-07)

## 1. Goal
Build and deliver a fair, auditable and traceable comparison of 4 models with their correct datasets:

- YOLOv8s (detect)
- YOLOv8-seg (instance segmentation)
- YOLO26 (detect)
- YOLO26-seg (instance segmentation)
Across complete runtime pipeline:
`decode -> preprocess -> infer -> postprocess -> render`
- Compare YOLOv8s (new training) vs Vasco `best.pt` using the same detect split and runtime conditions.
- Note: keep YOLO26 as INT8 (avoid hybrid output for this comparison).

## 1.1 Current Execution Status (Live)

- Phase A: complete (datasets frozen and documented).
- Phase B: complete (all 4 models trained, best weights available).
- Phase C: complete (8 annotated MP4 videos + 8 JSON stats generated).
- Phase D: complete (4 validation JSON metrics generated).
- Phase E: complete (4 ONNX exported, 4 HEF compiled via BYOM).
- Phase F: complete (AGL benchmark executed for all 4 models x 2 videos).

Latest validated outputs:
- `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_c_predictions/*.mp4` (8 files)
- `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_c_predictions/*_stats.json` (8 files)
- `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_d_validation/*_val_metrics.json` (4 files)
- `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_e_export/*.onnx` (4 files)
- `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_e_export/*.hef` (4 files)
- AGL runtime outputs: `/data/yolo_benchmark/results/sprint13/*.mp4` + `*_stats.json` (16 files)


## 2. Direct Answers to Key Questions

### 2.1 Train/predict commands

Yes: the core commands are `train`, `predict`, `val`, and `export`.

- `train`: train/fine-tune model
- `predict`: run inference and create visual outputs
- `val`: objective quality metrics on labeled set
- `export`: convert from `.pt` to deployable formats

### 2.2 Videos are made from which artifact?

- During development comparison (PC/Colab): from `.pt` or exported `.onnx` with Ultralytics runtime.
- In Hailo target runtime (AGL): from `.hef` inference outputs rendered by your runtime script.

### 2.3 Re-download all models or reuse current files?

Recommendation:
- Reuse existing artifacts when checksums/version are known and paths are clean.
- Re-download official base weights (`yolov8s.pt`, `yolo26n.pt`, `yolov8n-seg.pt`, `yolo26n-seg.pt`) if provenance is unclear.

Decision rule:
- If artifact source/version is uncertain, refresh.
- If artifact is traceable and validated in logs, keep it.

### 2.4 Is YOLOv8 better than YOLO26?

No universal winner.

- YOLO26 official docs emphasize edge/deployment simplification and CPU speed improvements.
- YOLOv8 is mature and very stable in existing integrations.

For your project, winner must be chosen per gate:
1. Quality on your datasets/classes.
2. E2E latency and jitter on your pipeline.
3. Host CPU cost for postprocess/NMS.
4. Robustness in shadows/curves/night-like conditions.

## 3. Scope and Metrics

## 3.1 Mandatory E2E metrics

- Per-stage latency (ms): decode/preprocess/infer/postprocess/render
- End-to-end latency p50/p95/p99
- Jitter (stddev and p99-p50)
- Host CPU avg/max
- Optional if available: temperature/power

## 3.2 Quality metrics

Detection track:
- mAP50, mAP50-95
- class-wise FN rate for critical classes

Segmentation track:
- mAP-seg (box and mask)
- Dice or mIoU on selected validation subset
- class-wise FN rate (critical semantic classes)

## 3.3 Robustness scenarios

At minimum tag samples as:
- normal
- shadow
- curve
- low-light
- partial occlusion

## 4. Execution Plan (Phased)

### Phase A - Dataset QA and freeze (Day 1)

1. Validate all 4 datasets format and class mapping.
2. Freeze split for fair comparison.
3. Create YAMLs:
   - `data_yolo8_detect.yaml`
   - `data_yolo26_detect.yaml`
   - `data_yolo8seg.yaml`
   - `data_yolo26seg.yaml`

Exit gate:
- zero missing image/label pairs
- no invalid seg polygons
- class IDs aligned and documented

Current snapshot (2026-05-04, after re-export):
- Detect datasets: `nc=17`, with train/valid/test populated.
  - train=4143, valid=394, test=198 (4735 total; Roboflow data augmentation applied to training set)
- Segmentation datasets: `nc=19`, with train/valid/test populated from official Roboflow export (no local fallback split needed).
  - train=489, valid=47, test=23 (559 total; no augmentation)

### Phase B - Colab training (Day 1-2)

Train 4 models with same policy (where applicable):

```bash
# Detect
yolo detect train model=yolov8s.pt data=data_yolo8_detect.yaml imgsz=640 epochs=100
yolo detect train model=yolo26n.pt data=data_yolo26_detect.yaml imgsz=640 epochs=100

# Segment
yolo segment train model=yolov8n-seg.pt data=data_yolo8seg.yaml imgsz=640 epochs=100
yolo segment train model=yolo26n-seg.pt data=data_yolo26seg.yaml imgsz=640 epochs=100
```

Artifacts to keep:
- best.pt for each run
- run args and logs

Exit gate:
- all 4 runs complete and reproducible

Status update (2026-05-04):
- complete; total wall-clock training time: ~158 minutes.
- best weights available at:
  - `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/yolov8s_detect_sprint13/weights/best.pt`
  - `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/yolo26n_detect_sprint13/weights/best.pt`
  - `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/yolov8n_seg_sprint13/weights/best.pt`
  - `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/yolo26n_seg_sprint13/weights/best.pt`

### Phase C - Predict and visual triage (Day 2)

Run on `teste1.mp4` and `teste2.mp4`:

```bash
yolo predict model=/path/to/best.pt source=/path/to/teste1.mp4 imgsz=640 conf=0.25 save=True
yolo predict model=/path/to/best.pt source=/path/to/teste2.mp4 imgsz=640 conf=0.25 save=True
```

Confidence sweep:
- `0.25`, `0.10`, `0.05`

Exit gate:
- videos generated for all models
- obvious failure modes tagged

Status update (2026-05-04):
- complete; all 8 expected videos generated and validated on disk.
- output directory: `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_c_predictions`
- generation script used: `src/hailo/scripts/predict_phase_c_generate_videos.py`

### Phase D - Validation and quality table (Day 2)

```bash
# Detect models
yolo detect val model=/path/to/best.pt data=/path/to/data_detect.yaml imgsz=640

# Seg models
yolo segment val model=/path/to/best.pt data=/path/to/data_seg.yaml imgsz=640
```

Exit gate:
- one unified quality table with detect and seg sections

Status update (2026-05-04):
- complete; 4 validation runs finished.
- output directory: `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_d_validation`

Validation snapshot:
- YOLOv8s detect: mAP50=0.9511, mAP50-95=0.6673, P=0.8926, R=0.9068
- YOLO26n detect: mAP50=0.9227, mAP50-95=0.6485, P=0.8253, R=0.8952
- YOLOv8n-seg (mask): mAP50=0.6811, mAP50-95=0.4058, P=0.7349, R=0.6545
- YOLO26n-seg (mask): mAP50=0.6531, mAP50-95=0.4127, P=0.6050, R=0.6207

### Phase E - Export and Hailo conversion (Day 3)

```bash
yolo export model=/path/to/best.pt format=onnx imgsz=640 opset=11 simplify=True
```

Then ONNX -> HEF via your Hailo toolchain and validated compile scripts.

Exit gate:
- one HEF per candidate model for runtime testing

Status update (2026-05-04):
- partial completion: ONNX export succeeded for all 4 models.
- output directory: `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_e_export`
- generated ONNX files:
  - `yolov8s_detect_sprint13.onnx`
  - `yolo26n_detect_sprint13.onnx`
  - `yolov8n_seg_sprint13.onnx`
  - `yolo26n_seg_sprint13.onnx`
- HEF compile status: failed for all 4 models.

Current blocker:
- Command flow was updated to Model Zoo profile + `--ckpt <onnx>` and executes correctly, but compilation still fails at model-specific backend stages.
- Detect models also failed in this sprint run:
  - `yolov8s_detect_sprint13` -> `AllocatorScriptParserException` (last layers expected conv, found activation).
  - `yolo26n_detect_sprint13` -> `NMSConfigPostprocessException` (layer `yolov8s/conv41` output mismatch).
- Segmentation models reach optimization/compile but fail in allocator mapping (`BackendAllocatorException`, `Agent infeasible`).
- Phase F remains blocked until HEF artifacts are produced.

Mode notes for Phase E decisions:
- `full-int8` means the graph is fully quantized and compiled to HEF. This can be combined with either device-side NMS or host-side NMS depending on the model script.
- `device_nms` (same as `nms-on-device`) means `nms_postprocess(...)` is inside the Hailo model script (`.alls`).
- `host_nms` means NMS/postprocess is executed by Python/C++ on host CPU after raw outputs.
- `no_nms` in compile context means a model script without `nms_postprocess(...)`; runtime then must use host-side NMS.
- Current sprint status: no variant has produced HEF yet (`0/4`), so `full-int8` is currently blocked by compile errors, not by benchmark policy.

Can detect be solved without `no_nms`?
- Possible in theory: align `nms_postprocess` config and output nodes to the exact exported detect graph.
- Not yet demonstrated in Sprint 13 artifacts/logs.
- Operationally, `no_nms` is the fastest unblock path for first HEF generation, then `device_nms` can be retried as a quality/perf optimization track.

## 4.1 Understanding Hardware Notation: What \"h8\" Means

`h8` = **Hailo-8** — the specific chip for which the HEF was compiled.

- HEF (Hailo Executable Format) is **hardware-specific**: a HEF for Hailo-8 cannot run on Hailo-8L or Hailo-15.
- The `_h8` suffix in filenames is just a naming convention; the actual target is controlled via `--hw-arch hailo8` in `hailomz compile`.
- **Examples from Sprint 13:**
  - `yolov8n_h8.hef` → compiled for Hailo-8
  - `yolo26n_seg_320_h8_no_nms.hef` → compiled for Hailo-8 with `no_nms` mode (and this **actually compiled successfully**)
  - `yolov8s_baseline_h8.hef` → baseline, Hailo-8 target

## 4.2 Root Cause Analysis: Why COCO Models Compiled but SEAME-Trained Failed

### The Core Problem: Model Zoo Recipe Assumes COCO-80 Exactly

The `hailomz compile` command uses a recipe from Model Zoo (e.g., `yolov8s`) containing a `.alls` file with **hardcoded NMS mapping** for specific graph nodes:

```python
# yolov8s.alls (Model Zoo recipe - assumes COCO-80)
nms_postprocess(
    "yolov8s/conv41",   # expects 84 channels: 4 bbox + 1 conf + 80 classes
    "yolov8s/conv42",   # expects 84 channels
    "yolov8s/conv52",   # expects 84 channels
    "yolov8s/conv63",   # expects 84 channels
)
```

### Why COCO-80 Model (Stock YOLOv8s) Compiled ✅

**Output topology:**
- Shape: `(batch=1, num_detections=8400, channels=84)`
- Formula: 4 bbox + 1 confidence + 80 classes = 85 channels (rounded to 84 format)

**Graph structure:**
- Conv nodes `conv41, conv42, conv52, conv63` present exactly as recipe expects
- NMS config matches perfectly

**Result:**
- ✅ Successful compilation: `yolov8s_baseline_h8.hef` exists (9.2 MB)
- Evidence in `compile_baseline.log` line 79-84: `output_from_conv41_to_yolov8_nms_postprocess: Pass`

### Why SEAME-Trained Models (16 Classes) Failed ❌

**Output topology for SEAME-16:**
- Shape: `(batch=1, num_detections=8400, channels=21)`
- Formula: 4 bbox + 1 confidence + 16 classes = 21 channels
- **Last layer:** Changed from `conv` → `activation` (sigmoid for multi-label output)

**Topology mismatch cascade:**
- Recipe expects 84 channels → receives 21
- Recipe expects last layer = conv → receives activation
- Conv nodes `conv41/42/52/63` have different meaning in SEAME graph
- YOLO26 has **native NMS built into ONNX** → no `conv41` node exists in it

**Errors from `/tmp/phase_e_export_retry2.log`:**

| Model | Error | Root Cause |
|---|---|---|
| `yolov8s_detect_sprint13` | `AllocatorScriptParserException: expected conv but found activation layer` | Last layer topology changed; recipe incompatible |
| `yolo26n_detect_sprint13` | `NMSConfigPostprocessException: layer yolov8s/conv41 doesn't have one output layer` | YOLO26 architecture completely different; recipe assumes YOLOv8 structure |
| `yolov8n_seg_sprint13` | `BackendAllocatorException` / `Agent infeasible` | 21-channel mismatch in multi-scale concatenation layers |
| `yolo26n_seg_sprint13` | `BackendAllocatorException` / `Agent infeasible` | YOLO26 multi-output topology unsupported by allocator |

### Case Study: Vasquinho's Observation (Same Root Cause)

**What Vasquinho experienced:**
- Trained YOLOv8s on SEAME dataset (16 classes)
- Exported to ONNX → output shape `(1, 21, 8400)`
- Used standard Model Zoo tutorial (COCO-80 recipe)
- ✅ **HEF compiled** (unlike Sprint 13, which failed at compile time)
- ❌ **HEF produced zero detections** (outputs were corrupted)

**Why his succeeded at compile but failed at inference:**
- Older Hailo version validation was looser
- NMS config applied to wrong channel indices → corrupted output
- Sprint 13 runs stricter validation → errors caught at compile time

**Lesson:** Model Zoo recipe is **not flexible** to custom class counts. It assumes COCO-80 exactly.

### Understanding Output Channels: Why 16 Classes = 21 Channels

**YOLOv8 output formula:**
```
output_channels = bbox_coordinates + objectness + num_classes
                = 4 + 1 + num_classes
```

**COCO-80 model:**
```
[bbox_x, bbox_y, bbox_w, bbox_h, conf, c1, c2, ... c80]
 └─ 4 ─┘ └──── 1 ────┘ └────── 80 ──────┘
 └────────────────── 85 total ────────────┘
 → Output shape: (1, 85, 8400) or (1, 8400, 85)
```

**SEAME-16 model:**
```
[bbox_x, bbox_y, bbox_w, bbox_h, conf, c1, c2, ... c16]
 └─ 4 ─┘ └──── 1 ────┘ └──── 16 ───┘
 └────────────── 21 total ──────────┘
 → Output shape: (1, 21, 8400) or (1, 8400, 21)
```

This change **cascades through the entire Hailo compilation graph**:
- Each detection head conv outputs 21 channels (not 85)
- NMS expects 85 → gets 21 → **Exception**

## 4.3 Three Solution Approaches: Comparison & Tradeoffs

### Approach 1️⃣: `no_nms` (Our Solution — Already Proven) ✅

**Concept:** Remove `nms_postprocess()` from `.alls`; force NMS to host CPU at runtime.

**Proof it works:**
- Artifact exists: `yolo26n_seg_320_h8_no_nms.hef` (8.2 MB, compiled 2026-04-01 20:51 successfully)
- This is **real evidence**, not theoretical

**Characteristics:**
- **Complexity:** LOW
- **Setup needed:** None (infrastructure exists)
- **Time to deploy all 4 models:** 2-3 hours
- **NMS location:** Host CPU (~5-10ms per frame)
- **Process:** `pt → onnx → hailomz compile --model-script <no_nms.alls> → hef`

**Files to create:**
- `yolov8s_detect_no_nms.alls` (copy `yolov8s.alls`, remove `nms_postprocess` line)
- `yolov8n_seg_no_nms.alls`
- `yolo26n_detect_no_nms.alls`
- `yolo26n_seg_no_nms.alls` (may reuse existing since it worked before)

**Pros:** ✅ Proven ✅ Fast ✅ Low risk
**Cons:** ⚠️ NMS on CPU adds latency; not optimal for 30 FPS real-time

---

### Approach 2️⃣: `device_nms` Fix (Our Advanced Solution) ⚙️

**Concept:** Rewrite `.alls` NMS config to **match the actual exported graph** (16 classes, 21 channels).

**What changes:**
- Identify actual conv nodes in trained SEAME graph (not COCO nodes)
- Rewrite `nms_postprocess()` with **new node names**
- Adjust channel counts: 21 instead of 85

**Characteristics:**
- **Complexity:** MEDIUM (manual debugging)
- **Setup needed:** Graph analysis tools
- **Time per model:** 4-8 hours (trial-and-error)
- **NMS location:** Device (Hailo-8 hardware, ~1ms)
- **Process:** `pt → onnx → analyze graph → rewrite .alls → compile → hef`

**Pros:** ✅ Optimal latency (NMS inside device)
**Cons:** ❌ **Not proven** with SEAME models ❌ Error-prone ❌ Very time-consuming

**Status:** Theoretical; represents upside potential if time permits.

---

### Approach 3️⃣: Team5 ONNX Cut + BYOM (Complex Fallback) 🔧

**Concept:** Pre-process ONNX (cut layer complexity), use Team5's Python API + `resources_param` tuning.

**Process (5 steps):**
1. `pt → onnx (opset=17)`
2. Cut ONNX with `cut_onnx_nano.sh` or `cut_onnx_small.sh`
3. `translate.sh` (ONNX → HAR)
4. `quantize.sh` (requires **CUDA 12.3** on GPU)
5. `compile_hef.sh` with `resources_param(strategy=greedy, max_utilization=0.6/0.9)`

**Characteristics:**
- **Complexity:** HIGH (5-step pipeline)
- **Setup needed:** CUDA 12.3, GPU, Team5 scripts
- **Time per model:** 6-10 hours
- **NMS location:** Host CPU (like `no_nms`)
- **Status:** Proven by Team5 (not by us yet)

**Pros:** ✅ Works for weird topologies (YOLO26 native NMS)
**Cons:** ❌ Not tested in our env ❌ Long elapsed time ❌ Still host-side NMS

**When to use:** Only if `no_nms` fails specifically for YOLO26 models.

### Comparison Table

| Factor | `no_nms` (Track 1) | `device_nms` (Track 2) | Team5 cut (Track 3) |
|---|---|---|---|
| **Status** | ✅ Proven | ❌ Theoretical | ⚠️ Proven elsewhere |
| **Time to first HEF** | 2-3h | 4-8h | 6-10h |
| **Risk** | LOW | MEDIUM | MEDIUM-HIGH |
| **Setup complexity** | NONE | LOW | HIGH (CUDA) |
| **NMS latency** | ~5-10ms (CPU) | ~1ms (device) | ~5-10ms (CPU) |
| **Unblocks Phase F** | ✅ YES (fast) | ⏸️ LATER | ⏸️ LATER |

---

## 4.4 Recommended Execution Strategy: Dual-Track

**Why dual-track?** We need HEF **immediately** to unblock Phase F benchmark, but we also want optimized performance.

```
╔═══════════════════════════════════════════════════════════════════╗
║           SPRINT 13 DUAL-TRACK EXECUTION PLAN                    ║
╠═══════════════════════════════════════════════════════════════════╣
║                                                                   ║
║  TRACK 1: `no_nms` (IMMEDIATE) ⭐ PRIORITY 1                    ║
║  ───────────────────────────────────────────────────────────    ║
║  Timeline: 2-3 hours                                            ║
║  Goal: Unblock Phase F AGL runtime benchmark                   ║
║                                                                  ║
║  Steps:                                                         ║
║  1. Create .alls files without nms_postprocess (all 4 models)  ║
║  2. Run hailomz compile --model-script for each                ║
║  3. Expected outcome: 4/4 HEF generated ✅                     ║
║  4. Proceed immediately to Phase F benchmark                   ║
║                                                                  ║
║  Success criteria:                                              ║
║  - All 4 HEF files: 0404, sizes reasonable                    ║
║  - No compile exceptions                                       ║
║  - Phase F runs without HEF missing errors                     ║
║                                                                  ║
║  ─────────────────────────────────────────────────────────────║
║                                                                  ║
║  TRACK 2: `device_nms` Fix (IN PARALLEL if time) ⚙️ PRIORITY 2║
║  ───────────────────────────────────────────────────────────   ║
║  Timeline: 4-8 hours (non-blocking; Track 1 continues)         ║
║  Goal: Optimize NMS latency (future sprint integration)        ║
║                                                                  ║
║  Steps:                                                         ║
║  1. Analyze exported ONNX graph for 16-class topology          ║
║  2. Identify actual conv nodes (not COCO nodes)                ║
║  3. Rewrite .alls NMS config with new node mappings            ║
║  4. Test compile on yolov8s_detect first (lowest risk)         ║
║  5. If successful: benchmark comparison vs no_nms             ║
║                                                                  ║
║  Success criteria:                                              ║
║  - At least 1 model compiles with device_nms ✅               ║
║  - NMS latency < 1ms (vs ~5-10ms host)                        ║
║  - Same detection accuracy as no_nms variant                   ║
║                                                                  ║
║  ─────────────────────────────────────────────────────────────║
║                                                                  ║
║  TRACK 3: Team5 Fallback (CONDITIONAL) 🔧 PRIORITY 3           ║
║  ───────────────────────────────────────────────────────────   ║
║  Trigger: IF Track 1 fails for specific models                ║
║  Goal: Solve edge cases (YOLO26 particularly)                  ║
║                                                                  ║
║  Specific conditions (activate only if):                       ║
║  - yolo26n_detect_no_nms.hef fails to compile OR               ║
║  - yolo26n_seg_no_nms.hef fails to compile                    ║
║                                                                  ║
║  Then:                                                          ║
║  1. Assess time cost vs Phase F deadline                       ║
║  2. Setup CUDA 12.3 (check Docker capability)                 ║
║  3. Run Team5 cut + compile pipeline on failing model          ║
║  4. Compare HEF quality vs no_nms                              ║
║                                                                  ║
║  Success criteria:                                              ║
║  - Alternative HEF produced for YOLO26 ✅                      ║
║  - Inference runs without errors                               ║
║                                                                  ║
╚═══════════════════════════════════════════════════════════════════╝
```

**Rationale:**
1. ✅ Track 1 is **fastest** (2-3h) → unblocks critical Phase F immediately
2. ⚙️ Track 2 is **optional optimization** if time allows (non-blocking)
3. 🔧 Track 3 is **insurance** only if Track 1 fails on specific models

**Next action:** Implement Track 1 starting immediately.

### Phase F - AGL runtime benchmark (Day 3-4)

Use your existing remote flow scripts and run all candidates with equal conditions.
Collect:
- `_stats.csv`
- `.log`
- `.mp4`

Compute final table:
- stage latencies
- E2E p50/p95/p99
- jitter
- CPU avg/max
- quality references

Exit gate:
- final ranked recommendation with production vs experimental label

## 5. YOLOv8s vs Vasco Comparison Track

Parallel dedicated track:

1. Validate both on same detect dataset split.
2. Predict on same 2 videos with same configs.
3. If moving to Hailo, compile both to HEF and run same benchmark script.

Output:
- direct delta table `your_yolov8s` vs `vasco_best`.

## 6. Colab Notebook and Script Status

### 6.1 `src/hailo/scripts/colab_seg_train_17c.ipynb`

Keep it. It remains useful as the operational Colab flow.
Current status (updated):
- Notebook now includes a dedicated Sprint 13 section with four training cells (`YOLOv8s detect`, `YOLO26n detect`, `YOLOv8n-seg`, `YOLO26n-seg`).
- Dataset YAML paths are explicit and can be adjusted in one config cell before running training.

### 6.2 `src/hailo/scripts/colab_seg_train_17c.py`

Do not delete now.
Reason:
- It is still a reproducible script version of notebook logic.
- Existing benchmark script references it as guidance.

If later removed, first update all references and keep an archived replacement in docs.

## 7. Runtime Scripts Keep/Review Decision

Keep (active in current benchmark flows):
- `src/hailo/scripts/hailo_demo_common.py`
- `src/hailo/scripts/inference_camera_scalercrop_yolo26n_seg.py`
- `src/hailo/scripts/inference_camera_scalercrop_yolov8n_seg.py`
- `src/hailo/scripts/inference_camera_scalercrop_yolov8n.py`
- `src/hailo/scripts/inference_camera_scalercrop_yolov8s.py`

Review later for deletion only if both conditions hold:
1. no script references (`rg` clean)
2. no benchmark workflow dependency

## 8. Commands and Locations to Preserve in Docs

- Hailo model root:
  - `~/Documents/AI/hailo`
- Shared with docker:
  - `~/Documents/AI/hailo/shared_with_docker`
- Typical compile invocation:

```bash
docker run --rm -u 0:0 -v ~/Documents/AI/hailo/shared_with_docker:/shared hailo8_ai_sw_suite_2025-10:1 bash -lc "... hailomz compile ..."
```

- Remote parse check:

```bash
ssh root@10.21.220.191 "hailortcli parse-hef /data/models/hef/<file>.hef | sed -n '1,120p'"
```

## 9. Risks and Mitigations

- Risk: mixing detect and seg labels in same validation path.
  - Mitigation: strict separate YAMLs and validation commands.
- Risk: infer-only ranking used as final ADAS decision.
  - Mitigation: enforce E2E gate with p95/p99 and CPU/jitter.
- Risk: host-NMS/postprocess hidden cost in segmentation models.
  - Mitigation: instrument postprocess stage explicitly.
- Risk: path drift between Lenovo and AGL.
  - Mitigation: keep canonical paths in docs and scripts.

## 10. Sprint 13 Deliverables

1. Four-model training/eval table (detect + seg separated).
2. YOLOv8s vs Vasco direct comparison table.
3. Two test videos per model (`teste1`, `teste2`) with overlays.
4. Hailo E2E benchmark table with p50/p95/p99, jitter, CPU.
5. Final recommendation split into:
   - production-safe now
   - experimental next

## 11. Updated Decision Matrix (2026-05-05)

### 11.1 BYOM definition for this sprint

`BYOM` = `Bring Your Own Model`.

In practice:

1. Parse custom ONNX with explicit `end_node_names` (`translate_onnx_model`).
2. Quantize/optimize with representative calibration data (`optimize`).
3. Compile HEF (`compile`).

Why this matters now:
- It avoids COCO-specific Model Zoo assumptions (`conv41/conv42/...`) for custom 16-class models.
- It gives direct control over graph endpoints.

### 11.2 Best option depends on objective

| Objective | Best option | Tradeoff |
|---|---|---|
| Fastest path to unblock HEF today | `hailomz + no_nms` | Host-side NMS cost (CPU/jitter risk) |
| Most correct path for custom topology | `BYOM (ClientRunner + explicit end nodes)` | Requires careful endpoint mapping |
| Best potential runtime latency in final state | `hailomz + corrected device_nms` | Highest tuning/debug effort |

Practical reading:
- Use `no_nms` for speed.
- Use `BYOM` for correctness on custom models.
- Use corrected `device_nms` as optimization track after stability.

### 11.3 Why not switch fully to Team5

Team5 is not a mandatory replacement. It is a high-value fallback toolkit.

Decision rule:

1. Start with our proven path first (already validated in this project context).
2. Use Team5 techniques only when specific models fail with allocator constraints.
3. Keep fallback surgical (model-specific), not full pipeline replacement by default.

So the correct framing is:
- Our approach is not worse.
- Team5 helps in edge cases where normal conversion blocks.

### 11.4 Final matrix for the 4 Sprint 13 models

| Model | Primary path | Fallback 1 | Fallback 2 | Pass criteria |
|---|---|---|---|---|
| `yolov8s_detect_sprint13` | BYOM (Vasco-style parse with explicit head nodes) | `hailomz + no_nms` | corrected `device_nms` recipe | HEF + valid boxes/classes + p95 gate |
| `yolo26n_detect_sprint13` | BYOM (custom YOLO26 graph) | Team5-style ONNX cut + BYOM | `no_nms` only if compatible | HEF + no NMS recipe mismatch + valid detections |
| `yolov8n_seg_sprint13` | BYOM + host postprocess | simplify/cut ONNX + BYOM | `hailomz + no_nms` | HEF + stable masks + no allocator crash |
| `yolo26n_seg_sprint13` | Team5-style cut (`cut_onnx_*`) + BYOM/resources tuning | `hailomz + no_nms` | stronger graph reduction and retest | HEF + no `Agent infeasible` + E2E acceptable |

## 12. Updated Implementation Plan (execution)

### 12.1 Execution order

Run this order to unblock fastest while controlling risk:

1. In parallel:
  - `yolov8s_detect_sprint13` with BYOM.
  - `yolo26n_seg_sprint13` with Team5/cut path.
2. Then: `yolo26n_detect_sprint13` with BYOM.
3. Then: `yolov8n_seg_sprint13` with BYOM; apply cut fallback if needed.

### 12.2 Per-model workflow template

1. Export/check ONNX (fixed input, explicit shapes).
2. Parse (BYOM) with explicit end nodes or cut-ONNX route.
3. Optimize with fixed calibration subset.
4. Compile HEF.
5. Run functional sanity on real video samples.
6. Record infer-only and E2E metrics separately.

### 12.3 Pass/fail gates

Compile gate (mandatory):
- HEF generated.
- No critical compile exception (`AllocatorScriptParserException`, `NMSConfigPostprocessException`, `BackendAllocatorException`).

Functional gate (mandatory):
- Detections/masks are valid (no empty/corrupted output in normal scenes).
- Correct class mapping for SEAME labels.

Performance gate (mandatory for final decision):
- E2E p95 within sprint threshold.
- CPU and jitter acceptable for target runtime.

### 12.4 Reporting format

Report two separate tables to avoid wrong conclusions:

1. `Infer-only` (HEF/hardware runtime only).
2. `E2E` (`decode -> preprocess -> infer -> postprocess -> render`).

Final recommendation can only be based on table 2, while table 1 is technical support evidence.

## 13. Additional Clarifications (2026-05-06)

### 13.1 Can we do `no_nms` first and BYOM after?

Yes, and this is safe if artifacts are isolated.

Important:
- Running `hailomz + no_nms` does not inherently corrupt BYOM flow.
- BYOM and `no_nms` can consume the same ONNX but produce separate outputs.
- Risk only appears if files are overwritten due to reused generic names.

Operational rule:

1. Keep ONNX immutable once exported/frozen.
2. Use explicit output names by method.
3. Keep per-method folders (`no_nms/`, `byom/`, `team5/`).

### 13.2 Is BYOM the same as fixing `.alls`?

No.

- BYOM controls parsing/translation endpoints from actual ONNX graph.
- `.alls` fix modifies Model Zoo script and NMS config to fit custom graph.

So they address similar symptoms (mismatch) through different mechanisms.

### 13.3 Did Vasco use BYOM or COCO recipe?

Based on available scripts, Vasco used BYOM-style SDK pipeline:

`translate_onnx_model -> optimize -> compile`

with explicit `end_node_names`.

He did not rely on the stock `hailomz yolov8s` COCO `.alls` recipe in that flow.

### 13.4 How to implement corrected `.alls` output path (device NMS)

Detailed implementation steps:

1. Parse ONNX and identify real head layers for each stride.
2. Build a custom NMS JSON with correct class count and decoder mapping.
3. Build custom `.alls` using that JSON in `nms_postprocess(...)`.
4. Compile with `hailomz` and this model script.
5. Validate compile + detection correctness + class mapping.

Why this is fragile:
- Wrong decoder layer mapping can compile but degrade outputs.
- Requires iterative graph-level debugging.

### 13.5 BYOM vs `hailomz` recipe path (exact difference)

| Aspect | BYOM (Vasco style) | `hailomz` recipe path |
|---|---|---|
| Interface | SDK API (`ClientRunner`) | CLI (`hailomz compile`) |
| Graph endpoint control | Explicit (`end_node_names`) | Recipe/profile-driven |
| Dependency on COCO node naming | Low | Higher unless customized |
| Typical failure class | allocator/tuning | recipe mismatch + allocator |

Conclusion:
- BYOM is often better for custom-trained topology.
- `hailomz` recipe is faster for stock-compatible models.

### 13.6 Team5 pipeline details (what/how/why)

Team5-style flow:

1. ONNX cut:
  - What: reduce/reshape problematic graph branches.
  - Why: reduce allocator complexity.
2. Translate ONNX to HAR:
  - What: convert into Hailo internal graph IR.
  - Why: required for optimization and compile.
3. Quantize/optimize HAR:
  - What: calibration-based INT8 optimization.
  - Why: hardware deployment efficiency.
4. Compile HEF with resource tuning:
  - What: compile using tuned resource parameters.
  - Why: improve mapping success for hard graphs.

When to apply Team5 in Sprint 13:
- Not as default for all models.
- Use as targeted fallback for allocator-heavy YOLO26 cases.

## 14. ONNX to HEF: Detailed Compile-Time vs Runtime Map

### 14.1 Full conversion path

Between ONNX and HEF there are multiple phases:

1. Parse/import ONNX graph.
2. Select graph endpoints (`start_node_names`, `end_node_names`, or recipe defaults).
3. Apply model script (`.alls`) instructions.
4. Bind NMS/postprocess config if device-side NMS is enabled.
5. Quantize/optimize with calibration data.
6. Allocate/map graph into Hailo hardware resources.
7. Compile final HEF.
8. Later at runtime, execute host-side postprocess if device NMS is absent.

### 14.2 Allocator and tuning are phases too

Yes.

- Allocator/mapping is the compile stage that tries to fit the graph into hardware resources.
- Tuning means changing compile parameters or graph shape to help allocator succeed.

These are compile-time stages, not runtime app stages.

### 14.3 Are binding and NMS part of preprocess/postprocess?

Not in the runtime E2E sense.

Runtime pipeline:
- `decode -> preprocess -> infer -> postprocess -> render`

Compile-time pipeline:
- `parse -> bind -> script config -> quantize -> allocate -> compile`

Important NMS distinction:
- Device NMS is configured during compile-time, but executes effectively inside inference.
- Host NMS executes in runtime postprocess.

### 14.4 Solution impact on graph/channels/errors

Most solutions do not directly change channel counts.

What they change:

- Corrected `device_nms`: changes compiler expectations and decoder mapping for real layers/classes.
- `no_nms`: removes NMS wiring from compile path; channels stay the same.
- BYOM: changes graph endpoint binding and translation path; channels stay the same.
- ONNX cut: changes graph structure and sometimes exposed outputs.
- Quantization: changes numeric precision, not class count.

Why errors happen:

- Recipe mismatch errors happen when compiler expects the wrong layers/channels/topology.
- Allocator errors happen when graph complexity cannot be mapped well into hardware resources.

### 14.5 Requested Team5 breakdown

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

### 14.6 Time estimates

| Task | Estimated elapsed time |
|---|---|
| `hailomz + no_nms` for one model | 30-90 min |
| BYOM on YOLOv8 detect | 1-3 h |
| Correct `.alls`/NMS JSON for YOLOv8 detect | 2-6 h |
| BYOM on seg model | 2-4 h |
| Team5-style hard-case pipeline | 4-10 h |

Corrected `.alls` practical breakdown:

1. Inspect ONNX and identify head layers: `30-90 min`
2. Build custom JSON + `.alls`: `30-90 min`
3. Compile and first debug cycle: `1-3 h`
4. Validate output/class correctness: `30-120 min`

Realistic total:
- Best case: `~2 h`
- Normal case: `4-6 h`
- Hard case: `6-8 h`

## 15. Consolidated Process and Updated Plan (2026-05-06)

### 15.1 Full process map (operational)

Pre-compile:
1. Train/fine-tune
2. Export to ONNX
3. Optional ONNX cut/simplify

Compile-time:
1. Parse/import ONNX
2. Endpoint binding (`start/end node names`)
3. `.alls` integration
4. NMS config binding (if device NMS)
5. Quantize/optimize
6. Allocator/mapping/partitioning
7. Compile to HEF

Runtime:
1. HEF load
2. `decode -> preprocess -> infer -> postprocess -> render`
3. Host NMS applies only if not using device-side NMS

Validation:
1. Functional output sanity
2. Infer-only metrics
3. E2E metrics and quality

### 15.2 Where allocator/mapping and tuning belong

- Allocator/mapping is compile-time step 6.
- Tuning is an intervention layer around compile steps 2-7 (especially 5-7).

Tuning examples:
- adjust graph shape via ONNX cut
- adjust endpoint strategy
- apply compile resource settings

### 15.3 Which solution affects which steps

| Solution | Main impacted steps |
|---|---|
| `hailomz + no_nms` | compile steps 3/4 (disables device NMS path), runtime step 2 postprocess increases |
| BYOM | compile steps 1/2 strongly (explicit endpoint control), then 5/6/7 |
| corrected `device_nms` | compile steps 3/4 strongly, then 6/7 |
| Team5 cut+tuning | pre-compile step 3 + compile steps 5/6/7 |

### 15.4 Updated practical implementation plan

Goal: unblock quickly while preserving a path to final optimized architecture.

Track A (speed):
1. Run `no_nms` flow first for all 4 models where possible.
2. Produce fastest HEF set for immediate benchmark unblock.

Track B (correctness/stability):
1. Run BYOM in parallel on top-priority models.
2. Replace temporary HEFs when BYOM output is functionally superior.

Track C (optimization):
1. Run corrected device NMS only after stable outputs exist.
2. Use as final optimization, not as first unblock path.

Track D (hard fallback):
1. Apply Team5 cut+tuning only on models that still fail.

### 15.5 Estimated execution time for this combined strategy

| Work item | Estimated elapsed time |
|---|---|
| `no_nms` unblock pass (all models, parallelized where possible) | 3-8 h |
| BYOM stabilization pass | 4-10 h |
| corrected device NMS optimization pass | 4-12 h |
| Team5 hard-case fallback (if required) | 4-10 h additional |

## 16. Confidence Score Correction Plan (2026-05-07)

Objective:
- Recover confidence distribution consistency between `.pt` reference and HEF runtime without regressing FPS/p95.

Success criteria:
- HEF confidence histogram no longer collapsed near threshold.
- `mean_conf` and `p50_conf` move closer to `.pt` trend for same video/model.
- No regression beyond `+10%` in `avg_stage_total_ms`.

### 16.1 Baseline and instrumentation lock

1. Run 8 offline benchmark runs (`4 models x 2 videos`) with strict stage timing:
  - `--sync-writer`
  - `--label-decimals 3`
2. Store baseline table fields:
  - `real_fps`, `pipeline_fps_stage_total`, `mean_conf`, `p50_conf`, `p95_conf`.
3. Save all `*_stats.json` and freeze as `confidence_baseline_v1`.

### 16.2 Root-cause isolation matrix

Perform controlled A/B runs (one factor changed at a time):

1. Preprocess parity check:
  - Resize-only vs letterbox path equivalent to training/inference reference.
2. Input scale parity check:
  - Confirm calibration preprocessing equals runtime preprocessing (`0..1` vs `0..255`, RGB order).
3. Threshold sensitivity:
  - Sweep `--conf` in `{0.25, 0.35, 0.45, 0.55}`.
4. Quantization sensitivity:
  - Rebuild with improved calibration subset diversity (lighting/shadow/curves).

Expected output:
- A ranked list of impact magnitude per factor on confidence shift.

### 16.3 Corrections to apply

1. Immediate runtime correction:
  - Keep `--label-decimals 3` for visibility.
2. Structural correction (primary):
  - Recompile HEFs with strict preprocessing parity between calibration and runtime.
3. Optional postprocess correction:
  - Apply class-wise confidence calibration (temperature scaling or Platt) on host output.
  - Keep calibration parameters versioned per model.

### 16.4 Validation gate and rollout

1. Re-run 8 strict runs after each correction candidate.
2. Compare against baseline and `.pt` reference.
3. Promote only candidates that satisfy all:
  - confidence distribution improved,
  - no class-critical degradation,
  - latency within gate (`avg_stage_total_ms` regression <= `10%`).

Deliverables:
- `confidence_ablation_table.csv`
- `confidence_fix_recommendation.md`
- Updated benchmark matrix with before/after confidence metrics.

Implementation status update (2026-05-07):
- `src/hailo/scripts/inference_video_sprint13.py` now supports optional Vasco-like postprocess knobs:
  - `--min-box-size`
  - `--global-nms-iou`
- Defaults keep the current Sprint 13 behavior (disabled), so existing benchmark comparability is preserved.
- Dedicated comparison outputs should be stored in:
  - `/data/yolo_benchmark/results/fase_f_comparacao_com_vasco/`

Planning expectation:
- Minimum to first usable result set: same day
- Full stabilized + optimized pass: 1-2 working days (without severe retries)

### 15.6 Final yes/no guidance

Yes, it makes sense to implement in this exact order:

1. `no_nms` for fast HEF unblock
2. BYOM in parallel for correctness/stability
3. corrected `device_nms` as optimization final track

Reason:
- This minimizes schedule risk while preserving technical quality path.

## 16. Final Runtime Results (Phase F, 2026-05-07)

Execution command used on Lenovo:

```bash
ssh root@10.21.220.191 "bash /data/yolo_benchmark/scripts/run_video_benchmark_sprint13.sh"
```

Execution scope:
- hardware: AGL target with Hailo accelerator
- runs: 4 HEFs x 2 videos (`teste1.mp4`, `teste2.mp4`) = 8 runs

Average result per model (2 videos):

| Model | FPS real | FPS pipeline | Hailo avg | p95 | p99 |
|---|---:|---:|---:|---:|---:|
| `yolov8s_detect_sprint13.hef` | 28.3 | 31.7 | 14.4 ms | 36.2 ms | 40.5 ms |
| `yolo26n_detect_sprint13.hef` | 46.0 | 55.4 | 10.5 ms | 22.2 ms | 25.3 ms |
| `yolov8n_seg_sprint13.hef` | 32.5 | 36.6 | 11.5 ms | 32.0 ms | 35.0 ms |
| `yolo26n_seg_sprint13.hef` | 39.2 | 45.2 | 14.5 ms | 26.1 ms | 28.6 ms |

Per-video detail (no averaging):

| Model | Video | FPS real | FPS pipeline | Hailo avg | p95 | p99 |
|---|---|---:|---:|---:|---:|---:|
| `yolov8s_detect_sprint13.hef` | `teste1` | 28.34 | 31.54 | 14.39 ms | 36.62 ms | 40.80 ms |
| `yolov8s_detect_sprint13.hef` | `teste2` | 28.31 | 31.76 | 14.33 ms | 35.81 ms | 40.19 ms |
| `yolo26n_detect_sprint13.hef` | `teste1` | 45.82 | 55.69 | 10.53 ms | 22.17 ms | 25.69 ms |
| `yolo26n_detect_sprint13.hef` | `teste2` | 46.20 | 55.15 | 10.53 ms | 22.14 ms | 24.93 ms |
| `yolov8n_seg_sprint13.hef` | `teste1` | 32.40 | 36.50 | 11.49 ms | 30.99 ms | 34.92 ms |
| `yolov8n_seg_sprint13.hef` | `teste2` | 32.51 | 36.56 | 11.40 ms | 32.88 ms | 35.06 ms |
| `yolo26n_seg_sprint13.hef` | `teste1` | 39.16 | 45.37 | 14.55 ms | 25.98 ms | 28.20 ms |
| `yolo26n_seg_sprint13.hef` | `teste2` | 39.09 | 45.11 | 14.53 ms | 26.23 ms | 28.86 ms |

Phase C detailed `.pt` metrics (re-run to fill missing values):
- source: `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_c_predictions/phase_c_detailed_pt_metrics.json`

| Model `.pt` | FPS real avg | Inference avg | Mean conf | p50 conf | p95 conf |
|---|---:|---:|---:|---:|---:|
| `yolov8s_detect` | 183.26 | 3.361 ms | 0.686 | 0.736 | 0.896 |
| `yolo26n_detect` | 183.35 | 3.961 ms | 0.685 | 0.746 | 0.950 |
| `yolov8n_seg` | 197.12 | 2.970 ms | 0.677 | 0.710 | 0.979 |
| `yolo26n_seg` | 152.73 | 4.810 ms | 0.651 | 0.650 | 0.987 |

Pipeline scope clarification:
- `avg_total_ms` in Phase F = preprocess + infer + postprocess.
- `real_fps` in Phase F is the practical E2E proxy (frame read + pipeline + draw/write).
- Next iteration should add explicit `decode_ms` and `render_ms` for strict stage-by-stage E2E accounting.

Confidence `0.50` investigation note:
- Rounded overlay (`{score:.2f}`) + threshold at `0.45` explains part of the visual concentration.
- INT8 quantization compresses score spread vs `.pt` FP inference.
- Potential scale mismatch exists: BYOM calibration used `0..1` input, runtime feeds `UINT8 0..255`; this can bias scores toward threshold region.
- Runtime script now stores `mean_conf/p50_conf/p95_conf` per frame and global in the output JSON for objective validation.

Key conclusions:
- Best throughput winner: `yolo26n_detect_sprint13.hef`.
- Best segmentation runtime winner: `yolo26n_seg_sprint13.hef`.
- `yolov8s_detect_sprint13.hef` remains a stable baseline but with highest latency.

## 17. Delta Update (2026-05-08) - Vasco Recompile and Compare

What was executed:
- Recompile path using Vasco order (`parse.py -> optimize.py -> compile.py`) for the 4 Sprint13 ONNX files.
- Final compare path on `teste1` with hostdecode parity (`ONNX vs HEF`) using 30-frame runs.

Important compile outcomes:
- `yolov8s_detect_sprint13_vasco.hef` generated.
- `yolo26n_detect_sprint13_vasco.hef` generated.
- `yolov8n_seg_sprint13_vasco.hef` generated.
- `yolo26n_seg` failed with full proto branch due to `matmul1` multi-output allocation constraint.
- Workaround applied and validated: `yolo26n_seg_sprint13_vasco_noproto.hef` (9 end-nodes, proto removed from HEF graph).

Strategic conclusion on sequence (Option 3):
- Original plan requested `A -> B -> C`.
- Execution started with `B` (Vasco-style recompile) before completing `A` mixed-precision first.
- Reason: Vasco path had highest probability of immediate unblock because it matched known working flow and produced actionable compiler diagnostics faster.
- `A` remains valid as fallback/optimization track, but `B` resolved detect collapse first.

Metric coverage status after this delta:
- Added/available in runtime stats: `real_fps`, `avg_total_ms`, `p95_ms`, `p99_ms`, `jitter_ms`, stage means (`avg_decode_ms`, `avg_infer_ms`, `avg_render_ms`).
- Still missing for full benchmark scope: explicit host-NMS cost, explicit overlay-only cost, CPU/thermal telemetry, scenario-tagged robustness, class-wise FN runtime analysis, mAP-seg/Dice per scenario in runtime pass.

## 18. Delta Update (2026-05-08, late) - Consistency `teste2` and sequence clarification

Sequence clarification (as executed in practice):
- Operational order observed in this closure cycle was closer to `C -> B -> A` (diagnostic/quick-compare first, then benchmark table/consolidation, then planned move to mixed-precision track).
- `A` (mixed-precision) remains pending execution as the next optimization pass.

What was organized:
- `teste2` consistency artifacts were grouped in:
  - `/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl_rerun/compare_onnx_vs_hef_hostdecode/consistencia_teste2_2026-05-08/`
- ONNX inputs used in compare were grouped in:
  - `/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_e_export/onnx_usados_compare_hostdecode_2026-05-08/`

Immediate next step (approved path):
1. Execute Option A (`mixed-precision .alls`) for detect models.
2. Re-run compare (`teste1` and `teste2`) with same hostdecode protocol.
3. Publish a new table (`A` vs current Vasco baseline) with latency/confidence deltas.
