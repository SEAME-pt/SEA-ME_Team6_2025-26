#!/usr/bin/env bash
set -euo pipefail

# Build stronger calibration set from Sprint13 videos and recompile detect HEFs.
# Output files are saved as:
#   <phase_e>/<model>_sprint13<HEF_VARIANT_SUFFIX>.hef

LOCAL_REPO="/home/seame/Documents/SEA-ME_Team6_2025-26"
LOCAL_PY="$LOCAL_REPO/.venv/bin/python"
DOCKER_IMAGE="${DOCKER_IMAGE:-hailo8_ai_sw_suite_2025-10:1}"

RESULTS_DIR="/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs"
PHASE_E_DIR="$RESULTS_DIR/phase_e_export"
HAILO_SHARED="/home/seame/Documents/AI/hailo/shared_with_docker"
VIDEOS_DIR="/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset"

CALIB_NAME="${CALIB_NAME:-calibration_images_sprint13_detect_1024}"
CALIB_DIR="$HAILO_SHARED/$CALIB_NAME"
CALIB_COUNT="${CALIB_COUNT:-1024}"
HEF_VARIANT_SUFFIX="${HEF_VARIANT_SUFFIX:-_recalib1024}"

ONNX_8S="$PHASE_E_DIR/yolov8s_detect_sprint13.onnx"
ONNX_26N="$PHASE_E_DIR/yolo26n_detect_sprint13.onnx"

HEF_8S_OUT="$PHASE_E_DIR/yolov8s_detect_sprint13${HEF_VARIANT_SUFFIX}.hef"
HEF_26N_OUT="$PHASE_E_DIR/yolo26n_detect_sprint13${HEF_VARIANT_SUFFIX}.hef"

ALLS_8S="$LOCAL_REPO/src/hailo/scripts/yolov8s_detect_no_nms.alls"

if [[ ! -x "$LOCAL_PY" ]]; then
  echo "Missing local Python: $LOCAL_PY" >&2
  exit 1
fi
if [[ ! -f "$ONNX_8S" || ! -f "$ONNX_26N" ]]; then
  echo "Missing ONNX detect artifacts under: $PHASE_E_DIR" >&2
  exit 1
fi
if [[ ! -f "$ALLS_8S" ]]; then
  echo "Missing model script: $ALLS_8S" >&2
  exit 1
fi

mkdir -p "$CALIB_DIR"

if [[ "$CALIB_COUNT" -gt 64 ]]; then
  echo "Note: current hailomz flow on this setup reports using 64 calibration entries during optimize."
  echo "      Keeping CALIB_COUNT=$CALIB_COUNT for dataset quality, but compile may still sample 64."
fi

echo "[1/3] Build calibration dataset: $CALIB_DIR (target=$CALIB_COUNT)"
"$LOCAL_PY" - <<PY
import cv2
from pathlib import Path

videos = [
    Path('$VIDEOS_DIR/teste1.mp4'),
    Path('$VIDEOS_DIR/teste2.mp4'),
]
out_dir = Path('$CALIB_DIR')
target = int('$CALIB_COUNT')
out_dir.mkdir(parents=True, exist_ok=True)

# Remove previous generated frames for deterministic refresh.
for p in out_dir.glob('calib_*.jpg'):
    p.unlink()

caps = []
for v in videos:
    if v.exists():
        caps.append(cv2.VideoCapture(str(v)))

if not caps:
    raise SystemExit('No input videos found for calibration set generation')

idx = 0
saved = 0
while saved < target:
    cap = caps[idx % len(caps)]
    ok, frame = cap.read()
    if not ok:
        cap.set(cv2.CAP_PROP_POS_FRAMES, 0)
        ok, frame = cap.read()
        if not ok:
            idx += 1
            continue
    frame = cv2.resize(frame, (640, 640), interpolation=cv2.INTER_LINEAR)
    cv2.imwrite(str(out_dir / f'calib_{saved:05d}.jpg'), frame)
    saved += 1
    idx += 1

for cap in caps:
    cap.release()

print(f'Generated calibration images: {saved}')
PY

failures=0

echo "[2/3] Compile yolov8s_detect -> $HEF_8S_OUT"
if docker run --rm -u 0:0 \
  -v "$HAILO_SHARED:/local/workspace/shared_with_docker" \
  -v "$PHASE_E_DIR:/phase_e" \
  -v "$ALLS_8S:/work/yolov8s_detect_no_nms.alls" \
  "$DOCKER_IMAGE" \
  bash -lc "set -euo pipefail; cd /local/workspace; \
    hailomz compile yolov8s --ckpt /phase_e/yolov8s_detect_sprint13.onnx --hw-arch hailo8 --calib-path shared_with_docker/$CALIB_NAME --model-script /work/yolov8s_detect_no_nms.alls; \
    cp yolov8s.hef /phase_e/$(basename \"$HEF_8S_OUT\")"; then
  echo "OK: yolov8s detect compiled"
else
  echo "WARN: yolov8s detect compile failed (continuing)" >&2
  failures=$((failures + 1))
fi

echo "[3/3] Compile yolo26n_detect -> $HEF_26N_OUT"
if docker run --rm -u 0:0 \
  -v "$HAILO_SHARED:/local/workspace/shared_with_docker" \
  -v "$PHASE_E_DIR:/phase_e" \
  "$DOCKER_IMAGE" \
  bash -lc "set -euo pipefail; cd /local/workspace; \
    hailomz compile yolov8s --ckpt /phase_e/yolo26n_detect_sprint13.onnx --hw-arch hailo8 --calib-path shared_with_docker/$CALIB_NAME; \
    cp yolov8s.hef /phase_e/$(basename \"$HEF_26N_OUT\")"; then
  echo "OK: yolo26n detect compiled"
else
  echo "WARN: yolo26n detect compile failed (continuing)" >&2
  failures=$((failures + 1))
fi

echo "Done. Generated HEFs:"
for p in "$HEF_8S_OUT" "$HEF_26N_OUT"; do
  if [[ -f "$p" ]]; then
    ls -lh "$p"
  else
    echo "MISSING: $p"
  fi
done
echo "Compile failures: $failures"
echo
echo "Next: run comparison with"
echo "  VIDEO_NAME=teste1 MAX_FRAMES=30 HEF_VARIANT_SUFFIX=$HEF_VARIANT_SUFFIX bash $LOCAL_REPO/src/hailo/scripts/Vasquinho/run_compare_detect_models_onnx_vs_hef.sh"