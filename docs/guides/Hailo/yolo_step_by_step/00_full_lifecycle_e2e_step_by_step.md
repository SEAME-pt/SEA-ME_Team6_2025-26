# 00 - Full Lifecycle E2E Step by Step (From Zero to Current State)

This is the main phase-by-phase guide. It is designed for a teammate arriving today who needs to replicate everything done so far, with technical context and practical commands.

## Index

1. [Technical objective and criteria](#phase-0---objective-criteria-and-expected-result)
2. [Setup/infra and path organization](#phase-1---setup-and-environment-organization)
3. [Model acquisition](#phase-2---base-model-acquisition)
4. [ONNX export](#phase-3---export-to-onnx)
5. [Calibration](#phase-4---hailo-quantization-calibration)
6. [HEF compilation](#phase-5---onnx--hef-compilation)
7. [Validation with parse-hef](#phase-6---technical-hef-validation-on-the-agl)
8. [Real E2E](#phase-7---real-e2e-decode--preprocess--infer--postprocess--render)
9. [Benchmark by scenario](#phase-8---benchmark-by-scenario-and-metric)
10. [Metrics](#phase-8---benchmark-by-scenario-and-metric)
11. [Segmentation evaluation](#phase-8---benchmark-by-scenario-and-metric)
12. [Round4 offline validation](#phase-9---round4-offline-validation-script-correctness)
13. [Real troubleshooting](#phase-10---real-troubleshooting-what-happened)
14. [Team handover/reproducibility](#phase-11---results-integration-and-team-handover)

## Phase 0 - Objective, criteria, and expected result

### What to do
1. Define the technical objective: compare 3 YOLO candidates on Hailo-8 with a single protocol.
2. Define the real pipeline scope: `decode -> preprocess -> infer -> postprocess -> render`.
3. Define comparison criteria:
   - latencia (`mean`, `p50`, `p95`, `p99`),
   - real FPS and pipeline FPS,
   - jitter and frame drops,
   - CPU usage in postprocess,
   - temporal stability by scenario (straight, curve, shadow),
   - segmentation metrics (mIoU/Dice or mAP-seg) when ground truth exists.

### Why
- Infer-only benchmarking does not represent the real ADAS system cost.
- The correct decision depends on E2E performance and stability, not only raw FPS.

### Quick definition of GT

GT means ground truth.
In detection and segmentation, it is the annotated reference used to compare what the model predicted with what actually exists in the image or video.

Examples of GT:
- annotated boxes per object,
- pixel-level annotated masks,
- annotated lanes or lines,
- classes per scenario.

Without annotated GT, it is not possible to calculate mIoU, Dice, or mAP-seg in a valid way.

### Real project state so far

At this moment:
- no annotated dataset has been used to run the benchmarks completed so far;
- there is still no consolidated and automated protocol to calculate mIoU/Dice/mAP-seg;
- what has been done so far is running the models after calibration and measuring the parameters already exposed by the inference scripts;
- the real-track dataset in Roboflow exists as a future option, but it has not yet been integrated into this flow.

---

## Phase 1 - Setup and environment organization

### What to do
1. Confirmar paths principais:
   - repo: `/home/seame/Documents/SEA-ME_Team6_2025-26`
   - area de trabalho Lenovo: `/home/seame/Documents/AI/Yolo_benchmark`
   - target AGL: `root@10.21.220.191`
2. Confirmar scripts ativos:
   - repo: `src/hailo/scripts/*.py`
   - Lenovo: `/home/seame/Documents/AI/Yolo_benchmark/scripts/*.py`
3. Confirm where results are stored:
   - AGL: `/data/results/bench_runs`
   - Lenovo: `/home/seame/Documents/AI/Yolo_benchmark/results/bench_runs`

### Why
- Without consistent naming and paths, the team loses traceability and cannot reproduce results.

---

## Phase 2 - Base model acquisition

### What was done
1. Starting models in Ultralytics (`.pt`) format for YOLO families.
2. Candidate selection:
   - `yolov8s` (detection baseline),
   - `yolov8n-seg` (segmentation baseline),
   - `yolo26n-seg` (modern experimental candidate).

### How to repeat
- Validate the availability of the base weights in the local model area.
- Always document the exact checkpoint name used.

### Why
- The comparison is only fair if the source checkpoints are clearly identified and frozen per round.

---

## Phase 3 - Export to ONNX

### What to do
1. Export each model to ONNX with the target resolution.
2. Save ONNX files with explicit names to avoid overwriting.
3. Validate the ONNX in Netron (input/output nodes and shapes).

### Example export
```bash
python -c "from ultralytics import YOLO; YOLO('yolo26n-seg.pt').export(format='onnx', imgsz=640, opset=11)"
python -c "from ultralytics import YOLO; YOLO('yolov8n-seg.pt').export(format='onnx', imgsz=640, opset=11)"
```

### Why
- ONNX export errors propagate into compilation and later appear as allocator failures or incoherent outputs.

---

## Phase 4 - Calibration for Hailo quantization

### What to do
1. Prepare `calibration_images` representative of the real track.
2. Mix scenarios:
   - straight,
   - curve,
   - shadow,
   - crosswalk/signage.
3. Start with 64 images for quick debugging; increase for final validation.

### Why
- Poor calibration degrades detection and mask confidence after quantization.

---

## Phase 5 - ONNX to HEF compilation

### Validated baseline
1. Compilar baseline com recipes oficiais:
   - `hailomz compile yolov8s`
   - `hailomz compile yolov8n_seg`
2. Guardar logs completos por tentativa.

### Custom yolo26n-seg path
1. The `yolo26n-seg` ONNX was compiled via the `yolov8n_seg` recipe.
2. There were recurring allocator failures in larger variants.
3. The operational fallback that worked was device-free NMS (`no_nms`) with host-side NMS.

### Why
- The custom compilation hit allocator blocks (`Agent infeasible`), and the path without device NMS was what unlocked a functional HEF.

---

## Phase 6 - Technical HEF validation on the AGL

### What to do
1. Copy HEFs to `/data` on the AGL.
2. Validate each HEF with `parse-hef`.
3. Extract the real streams to feed the scripts.

### Comandos
```bash
ssh root@10.21.220.191 "hailortcli parse-hef /data/yolov8s.hef | sed -n '1,30p'"
ssh root@10.21.220.191 "hailortcli parse-hef /data/yolov8n_seg_h8.hef | sed -n '1,35p'"
ssh root@10.21.220.191 "hailortcli parse-hef /data/yolo26n_seg_320_h8_no_nms.hef | sed -n '1,35p'"
```

### Important current result
- The `yolo26n_seg_320_h8_no_nms.hef` HEF exposes internal `yolov8n_seg/*` streams.
- This is not a runtime error; it is a consequence of the recipe used during compilation.

---

## Phase 7 - Real E2E: decode -> preprocess -> infer -> postprocess -> render

## 7.1 Decode
- Read the video/camera and transform the encoded stream into raw frames.
- Example: CSI camera via Picamera2, or offline video for replay.

## 7.2 Preprocess
- Adjust the frame for model input:
  - resize,
  - crop/scalercrop,
  - BGR<->RGB,
  - layout/tipo (`UINT8 NHWC`).

## 7.3 Infer
- Execute the HEF on Hailo via `InferVStreams`.
- The input stream must match `parse-hef`.

## 7.4 Postprocess
- Interpret raw outputs.
- Apply thresholds.
- Apply NMS.
- Reconstruct masks and boxes.

In the current `yolo26n-seg` case:
- NMS is on the host (CPU), not on the device.
- This makes CPU and jitter measurement mandatory for the final decision.

## 7.5 Render
- Overlay boxes, masks, labels, FPS, and latencies.
- Record evidence video (`.mp4`) and stats (`.csv`).

### Why this phase is critical
- This is where real problems of temporal stability, CPU cost, and visual quality appear.

---

## Phase 8 - Benchmark by scenario and metric

### What to measure per run
1. Latency: `mean`, `p50`, `p95`, `p99`.
2. Throughput: real FPS and pipeline FPS.
3. Stability: jitter (standard deviation), frame drops.
4. Resources: CPU/RAM and temperature.
5. Detection and segmentation quality by scenario.

### Minimum scenarios
- straight,
- curve,
- shadow,
- (optional) crosswalk/signage.

### Segmentation (when labels exist)
- mIoU,
- Dice/F1 de mascara,
- mAP-seg (se houver protocolo adequado).

Note:
- without annotated ground truth, there is no reliable mIoU/Dice.
- in that case, report "visual evaluation + stability metrics" as provisional.

### Current state of the scripts/protocols we have

The scripts currently present in `/data/scripts/yolo_realtime` are operational inference and benchmark scripts:
- `inference_camera_scalercrop_yolo26n_seg.py`
- `inference_camera_scalercrop_yolov8n_seg.py`
- `inference_camera_scalercrop_yolov8s.py`
- `run_bench_scenario_with_pidstat.sh`

What they do today:
- run inference,
- measure latency/FPS,
- save logs/videos,
- help validate operational stability.

What they still do not do in a consolidated way:
- read annotated GT,
- automatically calculate mIoU/Dice/mAP-seg,
- perform formal pixel/class evaluation on an annotated dataset.

Practical conclusion:
- for now, the real protocol is E2E + stability + visual result comparison;
- GT-based segmentation evaluation should be added when the annotated dataset and evaluation script are ready.

---

## Phase 9 - Round4 offline: script validation with existing videos

### What to do
1. Reuse the existing round2 and round3 videos.
2. Run the scripts offline to confirm the pipeline is correct.
3. Copy videos, logs, and stats to Lenovo and review them visually.
4. Fix the scripts if ghost boxes, wrong streams, or incoherent metrics appear.

### Why this step comes before GT and mIoU/Dice
- Because the first risk is the pipeline itself being wrong.
- Before measuring formal quality, the script must be correct.
- Round4 exists to freeze the operational baseline.

### Practical plan
1. Prepare `round4` per model.
2. Run offline with the existing videos.
3. Save results in:
   - `/data/results/bench_runs/yolov8s/round4/`
   - `/data/results/bench_runs/yolov8n_seg/round4/`
   - `/data/results/bench_runs/yolo26n_seg/round4/`
4. Copiar para o Lenovo em:
   - `/home/seame/Documents/AI/Yolo_benchmark/results/bench_runs/<modelo>/round4/`
5. Review videos, logs, and stats.

### GO/NO-GO decision for round4
- GO: the script runs, records video, generates logs/stats, and the overlays look coherent.
- NO-GO: wrong streams appear, many ghost boxes appear, or the output does not match the expected model.

### Expected result
- a clean operational baseline before thinking about GT/annotated datasets and formal metrics.

---

## Phase 9 - Round4 offline validation (script correctness)

### What to do
1. Reuse the existing round2 and round3 videos.
2. Execute the scripts in offline mode to validate the pipeline without depending on the car on the track.
3. Copy videos, logs, and stats to Lenovo.
4. Review the outputs and fix scripts if needed.

### Round4 folder structure per model

No AGL:
```bash
/data/results/bench_runs/
├── yolov8s/round4/
├── yolov8n_seg/round4/
└── yolo26n_seg/round4/
```

No Lenovo:
```bash
/home/seame/Documents/AI/Yolo_benchmark/results/bench_runs/
├── yolov8s/round4/
├── yolov8n_seg/round4/
└── yolo26n_seg/round4/
```

### Suggested commands

Important:
- `inference_camera_scalercrop_yolov8s.py` in `/data/scripts/yolo_realtime` is camera-live (CSI), not offline replay.
- True offline mode in the current flow uses `--images-dir` (image folder), supported by the segmentation scripts.

```bash
ssh root@10.21.220.191 "mkdir -p /data/results/bench_runs/yolov8s/round4 /data/results/bench_runs/yolov8n_seg/round4 /data/results/bench_runs/yolo26n_seg/round4"

# Offline round4 (from extracted round2/round3 frames)
ssh root@10.21.220.191 "python3 /data/scripts/yolo_realtime/inference_camera_scalercrop_yolov8n_seg.py 60 --images-dir /data/offline/frames_round2/yolov8n_seg --loop --save --output /data/results/bench_runs/yolov8n_seg/round4/demo_yolov8n_seg_round4_offline.mp4"
ssh root@10.21.220.191 "python3 /data/scripts/inference_camera_scalercrop_yolo26n_seg_FIXED.py 60 --images-dir /data/offline/frames_round2/yolo26n_seg --loop --save --output /data/results/bench_runs/yolo26n_seg/round4/demo_yolo26n_seg_round4_offline.mp4"

# Live camera run (not offline) for yolov8s in the current script version
ssh root@10.21.220.191 "python3 /data/scripts/yolo_realtime/inference_camera_scalercrop_yolov8s.py 60 --save --output /data/results/bench_runs/yolov8s/round4/demo_yolov8s_round4_live.mp4"
```

### How to decide GO/NO-GO in round4
- GO: the script runs, records video, generates logs/stats, and the overlay is coherent.
- NO-GO: wrong streams appear, ghost boxes appear, or the output does not match the model.

### Why this round comes before formal GT
- because first we must ensure the pipeline is correct;
- only then does it make sense to compare against annotated GT.

## Phase 10 - Real troubleshooting (what happened)

### Problema A
- `ModuleNotFoundError: hailo_demo_common`
-- Fix: copy `hailo_demo_common.py` to `/data/scripts`.

### Problema B
- `--duration` not recognized in the FIXED script
- Cause: positional argument.
- Fix: usar `python ... 30 --save ...`.

### Problema C
- many ghost boxes / `INPUT_STREAM not found`
- Cause: incorrect streams in the script for the current HEF.
- Fix: align the script with the real `parse-hef` output.

---

## Phase 11 - Results integration and team handover

### What to do
1. Store videos/logs/stats on the AGL by `round`.
2. Copy to Lenovo for team review.
3. Update the artifact registry and states.
4. Publish the conclusion with:
   - baseline vs experimental,
   - riscos remanescentes,
   - proxima acao.

### Recent materials already integrated
- `/home/seame/Documents/AI/Yolo_benchmark/*.md`
- `/home/seame/Documents/AI/Yolo_benchmark/scripts/*`

---

## Phase 12 - Current state (snapshot)

- `yolov8s`: baseline operacional.
- `yolov8n-seg`: baseline operacional.
- `yolo26n-seg`: operacional em modo `experimental-host-nms`.
- benchmark round3 do fix do yolo26n-seg executado com sucesso.
- recommended next step: close the comparable benchmark of the 3 models under the same protocol/scenarios.

---

## Checklist final para replicacao completa

1. Setup e paths validados.
2. ONNX e calibraçao rastreaveis.
3. HEFs validados com `parse-hef` no AGL.
4. Scripts aligned with the real streams.
5. Real E2E executed with logs/videos/stats.
6. Medicoes por cenario e estabilidade temporal.
7. Documentacao final atualizada com evidencias.
