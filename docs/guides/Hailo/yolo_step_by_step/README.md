# YOLO Step by Step

Goal: provide any teammate with a complete, reproducible path from the start to the current state, covering every phase of the process:
- model acquisition,
- ONNX export,
- calibration,
- HEF compilation,
- deployment on the AGL,
- real E2E (`decode -> preprocess -> infer -> postprocess -> render`),
- benchmarking and scenario-based evaluation.

## Recommended reading order

1. `00_full_lifecycle_e2e_step_by_step.md` (main phase-by-phase guide)
2. `03_artifact_model_registry.md` (source and state of each artifact)
3. `05_known_issues_and_fixes.md` (real troubleshooting)
4. `04_results_round3_integration.md` (integration of Lenovo results)
5. `01_quickstart_from_zero.md` and `02_replication_runbook.md` (fast execution)

## Current scope

- `yolov8s` (detection baseline)
- `yolov8n-seg` (segmentation baseline)
- `yolo26n-seg` (current experimental host-NMS state)

## Consolidated sources

- Current documentation: `docs/guides/Hailo/{00_overview,10_setup,20_conversion,30_benchmarks,40_e2e,50_issues,60_planning,90_history}`
- Legacy: `docs/guides/Hailo/backup/legacy_2026-04-14`
- Recent materials on Lenovo: `/home/seame/Documents/AI/Yolo_benchmark`
- Versioned scripts in the repo: `src/hailo/scripts`

## Critical naming note

In the `yolo26n_seg_320_h8_no_nms.hef` artifact, the internal streams appear as `yolov8n_seg/*`.
This is expected in the current state because the `yolo26n-seg` ONNX was compiled using the `yolov8n_seg` recipe.
Operational rule: always follow the final artifact's `hailortcli parse-hef` output.
