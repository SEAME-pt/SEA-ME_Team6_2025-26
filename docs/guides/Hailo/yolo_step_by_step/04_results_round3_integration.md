# 04 - Integration of Round3 Results and Lenovo Materials

This document integrates what was recently produced on Lenovo in `/home/seame/Documents/AI/Yolo_benchmark`.

## Table of contents

1. [Current Lenovo materials](#1-current-lenovo-materials)
2. [How these materials relate to the repo](#2-how-these-materials-relate-to-the-repo)
3. [Round3 already validated](#3-round3-already-validated)
4. [Team handover decision](#4-team-handover-decision)
5. [Remaining gap](#5-remaining-gap)

## 1) Current Lenovo materials

Directory:
- `/home/seame/Documents/AI/Yolo_benchmark`

Documents:
- `ANALYSIS_2026-04-23.md`
- `INSTRUCOES_ROUND3.md`
- `README_NOVO.md`
- `RESUMO_EXECUTIVO.md`
- `START_HERE.md`
- `STATUS_FINAL.md`
- `TESTE_FIXED_RESULTADOS.md`

Scripts:
- `scripts/inference_camera_scalercrop_yolo26n_seg_FIXED.py`
- `scripts/run_benchmark_round3_offline.sh`
- `scripts/setup_test_rounds.sh`
- `scripts/prepare_offline_tests.sh`
- `scripts/check_videos.py`

## 2) How these materials relate to the repo

- The versioned official scripts stay in `src/hailo/scripts`.
- The fast experiment scripts live in the Lenovo workspace (`AI/Yolo_benchmark/scripts`).
- This `yolo_step_by_step` folder is the onboarding and reproducibility layer.

Suggested rule:
- when an experimental script stabilizes, promote it to `src/hailo/scripts`.

## 3) Round3 already validated

Validated yolo26n-seg fix results:
- `/data/results/bench_runs/yolo26n_seg/round3/test_FIXED.mp4`
- `/data/results/bench_runs/yolo26n_seg/round3/test_FIXED_final.mp4`
- `/data/results/bench_runs/yolo26n_seg/round3/test_FIXED_final_stats.csv`

Local copy on Lenovo:
- `/home/seame/Documents/AI/Yolo_benchmark/results/bench_runs/yolo26n_seg/round3/*`

## 4) Team handover decision

For new teammates:
1. Start with `01_quickstart_from_zero.md`.
2. Run `02_replication_runbook.md` without changing commands.
3. Confirm artifacts in `03_artifact_model_registry.md`.
4. Use `05_known_issues_and_fixes.md` for troubleshooting.

## 5) Remaining gap

- We still need to unify the fixed and baseline scripts into one versioned package in the repo.
- We still need to close the final comparable benchmark for the 3 models using the same protocol and round naming.
- We still need to decide which scripts from `/home/seame/Documents/AI/Yolo_benchmark/scripts` should be promoted into `src/hailo/scripts`.
