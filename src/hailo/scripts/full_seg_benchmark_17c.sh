#!/usr/bin/env bash
set -euo pipefail

# Full benchmark pipeline (17 classes):
# - YOLOv8s (Vasco best.pt) -> ONNX -> HEF
# - YOLOv8n-seg (fine-tuned) -> ONNX -> HEF
# - YOLO26n-seg (fine-tuned) -> ONNX -> HEF (no-NMS model script)
# - AGL inference (3 models) + videos + stats + summary table

AGL_HOST="root@10.21.220.191"
AGL_ALLOWED_PREFIX="/data/yolo_benchmark"

safe_agl_path() {
  local p="$1"
  if [[ "$p" != "${AGL_ALLOWED_PREFIX}"* ]]; then
    echo "[FATAL] AGL path outside allowed prefix: $p" >&2
    exit 99
  fi
}

YOLO_BASE="${HOME}/Documents/AI/Yolo_benchmark"
HAILO_SHARED="${HOME}/Documents/AI/hailo/shared_with_docker"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Model selection
VASCO_RUN="${VASCO_RUN:-train2}" # train|train2|train3
VASCO_PT="${YOLO_BASE}/Vasco/final_dataset/runs/detect/${VASCO_RUN}/weights/best.pt"
SEG8N_PT="${YOLO_BASE}/trained/yolov8n_seg_17c/weights/best.pt"
SEG26N_PT="${YOLO_BASE}/trained/yolo26n_seg_17c/weights/best.pt"

# Data and working dirs
DATASET_TRAIN_IMGS="${YOLO_BASE}/roboflow_dataset/yolo8/train/images"
CALIB_DIR="${HAILO_SHARED}/calibration_images_17c"
MODELS_DIR="${HAILO_SHARED}/models"
LOGS_DIR="${HAILO_SHARED}/logs"
MS_DIR="${HAILO_SHARED}/model_scripts"
ALLS_SRC="${SCRIPT_DIR}/yolov8n_seg_no_nms.alls"

# Artifacts
ONNX_8S="yolov8s_vasco_17c_640.onnx"
ONNX_8N="yolov8n_seg_17c_640.onnx"
ONNX_26N="yolo26n_seg_17c_320.onnx"
HEF_8S="yolov8s_vasco_17c_h8.hef"
HEF_8N="yolov8n_seg_17c_h8.hef"
HEF_26N="yolo26n_seg_17c_320_h8_no_nms.hef"

DOCKER_IMAGE="hailo_custom_suite:latest"
RUN_ID="bench17c_$(date +%Y%m%d_%H%M%S)"
INFER_SECS="${INFER_SECS:-30}"
CALIB_IMAGES="${CALIB_IMAGES:-128}"
INFER_IMAGES="${INFER_IMAGES:-100}"

# AGL paths (restricted)
AGL_HEF_DIR="${AGL_ALLOWED_PREFIX}/models/hef"
AGL_FRAMES_DIR="${AGL_ALLOWED_PREFIX}/frames_17c"
AGL_SCRIPTS_DIR="${AGL_ALLOWED_PREFIX}/scripts/yolo_realtime"
AGL_RESULTS_DIR="${AGL_ALLOWED_PREFIX}/results"

LOCAL_RESULTS_DIR="${YOLO_BASE}/results/${RUN_ID}"
mkdir -p "${LOCAL_RESULTS_DIR}" "${CALIB_DIR}" "${MODELS_DIR}" "${LOGS_DIR}" "${MS_DIR}"

# Flags
SKIP_ONNX=0
SKIP_COMPILE_8S=0
SKIP_COMPILE_8N=0
SKIP_COMPILE_26N=0
SKIP_UPLOAD=0
SKIP_INFER=0
SKIP_TABLE=0

for arg in "$@"; do
  case "$arg" in
    --skip-onnx) SKIP_ONNX=1 ;;
    --skip-compile-8s) SKIP_COMPILE_8S=1 ;;
    --skip-compile-8n) SKIP_COMPILE_8N=1 ;;
    --skip-compile-26n) SKIP_COMPILE_26N=1 ;;
    --skip-upload) SKIP_UPLOAD=1 ;;
    --skip-infer) SKIP_INFER=1 ;;
    --skip-table) SKIP_TABLE=1 ;;
    *) echo "[WARN] Unknown flag: $arg" ;;
  esac
done

log() { echo -e "\n[INFO] $*"; }
ok() { echo "[OK] $*"; }
fail() { echo "[ERR] $*" >&2; exit 1; }

# Phase 0: preflight
log "Phase 0 - preflight"
[[ -f "${VASCO_PT}" ]] || fail "Missing Vasco best.pt: ${VASCO_PT} (set VASCO_RUN=train|train2|train3)"
[[ -d "${DATASET_TRAIN_IMGS}" ]] || fail "Missing dataset images: ${DATASET_TRAIN_IMGS}"
[[ -f "${ALLS_SRC}" ]] || fail "Missing model script: ${ALLS_SRC}"

if [[ ${SKIP_ONNX} -eq 0 || ${SKIP_COMPILE_8N} -eq 0 || ${SKIP_COMPILE_26N} -eq 0 ]]; then
  [[ -f "${SEG8N_PT}" ]] || fail "Missing seg weight: ${SEG8N_PT}. Run colab_seg_train_17c.ipynb (or matching training flow) first."
  [[ -f "${SEG26N_PT}" ]] || fail "Missing seg weight: ${SEG26N_PT}. Run colab_seg_train_17c.ipynb (or matching training flow) first."
fi

python3 -c "from ultralytics import YOLO" >/dev/null 2>&1 || fail "ultralytics missing"
command -v docker >/dev/null 2>&1 || fail "docker missing"
docker image inspect "${DOCKER_IMAGE}" >/dev/null 2>&1 || fail "Docker image missing: ${DOCKER_IMAGE}"
ssh -o BatchMode=yes -o ConnectTimeout=5 "${AGL_HOST}" "echo ok" >/dev/null 2>&1 || fail "SSH failed: ${AGL_HOST}"
ok "Preflight passed"

# Phase 1: calibration set
log "Phase 1 - calibration set (${CALIB_IMAGES} imgs)"
existing_calib=$(find "${CALIB_DIR}" -maxdepth 1 \( -name '*.jpg' -o -name '*.jpeg' -o -name '*.png' \) | wc -l)
if [[ ${existing_calib} -lt ${CALIB_IMAGES} ]]; then
  rm -f "${CALIB_DIR}"/*.jpg "${CALIB_DIR}"/*.jpeg "${CALIB_DIR}"/*.png 2>/dev/null || true
  find "${DATASET_TRAIN_IMGS}" -maxdepth 1 \( -name '*.jpg' -o -name '*.jpeg' -o -name '*.png' \) \
    | shuf | head -n "${CALIB_IMAGES}" | xargs -I{} cp "{}" "${CALIB_DIR}/"
fi
final_calib=$(find "${CALIB_DIR}" -maxdepth 1 \( -name '*.jpg' -o -name '*.jpeg' -o -name '*.png' \) | wc -l)
[[ ${final_calib} -gt 0 ]] || fail "Calibration folder is empty"
ok "Calibration images ready: ${final_calib}"

# Phase 2: ONNX export
if [[ ${SKIP_ONNX} -eq 0 ]]; then
  log "Phase 2 - ONNX export"
  python3 - << PY
from ultralytics import YOLO
from pathlib import Path
import glob, os, shutil

def export_to(src_pt, out_onnx, imgsz):
    model = YOLO(src_pt)
    model.export(format='onnx', imgsz=imgsz, simplify=True, dynamic=False, opset=13)
    d = Path(src_pt).parent
    files = sorted(glob.glob(str(d / '*.onnx')), key=os.path.getmtime, reverse=True)
    if not files:
        raise RuntimeError(f'No ONNX exported near {src_pt}')
    shutil.move(files[0], out_onnx)
    print('saved', out_onnx)

export_to("${VASCO_PT}", "${MODELS_DIR}/${ONNX_8S}", 640)
export_to("${SEG8N_PT}", "${MODELS_DIR}/${ONNX_8N}", 640)
export_to("${SEG26N_PT}", "${MODELS_DIR}/${ONNX_26N}", 320)
PY
  ok "ONNX export finished"
else
  [[ -f "${MODELS_DIR}/${ONNX_8S}" ]] || fail "Missing ONNX: ${MODELS_DIR}/${ONNX_8S}"
  [[ -f "${MODELS_DIR}/${ONNX_8N}" ]] || fail "Missing ONNX: ${MODELS_DIR}/${ONNX_8N}"
  [[ -f "${MODELS_DIR}/${ONNX_26N}" ]] || fail "Missing ONNX: ${MODELS_DIR}/${ONNX_26N}"
  ok "Skipping ONNX export"
fi

# Phase 3a: compile yolov8s
if [[ ${SKIP_COMPILE_8S} -eq 0 ]]; then
  log "Phase 3a - compile yolov8s"
  docker run --rm -v "${HAILO_SHARED}:/local/workspace/shared_with_docker" "${DOCKER_IMAGE}" bash -lc "
    set -e
    cd /local/workspace
    hailomz compile yolov8s \
      --ckpt shared_with_docker/models/${ONNX_8S} \
      --hw-arch hailo8 \
      --calib-path shared_with_docker/calibration_images_17c \
      2>&1 | tee shared_with_docker/logs/compile_yolov8s_vasco_17c.log
    cp yolov8s.hef shared_with_docker/models/${HEF_8S}
  "
fi
[[ -f "${MODELS_DIR}/${HEF_8S}" ]] || fail "Missing HEF: ${MODELS_DIR}/${HEF_8S}"
ok "HEF ready: ${HEF_8S}"

# Phase 3b: compile yolov8n-seg
if [[ ${SKIP_COMPILE_8N} -eq 0 ]]; then
  log "Phase 3b - compile yolov8n-seg"
  docker run --rm -v "${HAILO_SHARED}:/local/workspace/shared_with_docker" "${DOCKER_IMAGE}" bash -lc "
    set -e
    cd /local/workspace
    hailomz compile yolov8n_seg \
      --ckpt shared_with_docker/models/${ONNX_8N} \
      --hw-arch hailo8 \
      --calib-path shared_with_docker/calibration_images_17c \
      --start-node-names images \
      --end-node-names output0 output1 \
      2>&1 | tee shared_with_docker/logs/compile_yolov8n_seg_17c.log
    cp yolov8n_seg.hef shared_with_docker/models/${HEF_8N}
  "
fi
[[ -f "${MODELS_DIR}/${HEF_8N}" ]] || fail "Missing HEF: ${MODELS_DIR}/${HEF_8N}"
ok "HEF ready: ${HEF_8N}"

# Phase 3c: compile yolo26n-seg no-NMS
if [[ ${SKIP_COMPILE_26N} -eq 0 ]]; then
  log "Phase 3c - compile yolo26n-seg no-NMS"
  cp "${ALLS_SRC}" "${MS_DIR}/yolov8n_seg_no_nms.alls"
  docker run --rm -v "${HAILO_SHARED}:/local/workspace/shared_with_docker" "${DOCKER_IMAGE}" bash -lc "
    set -e
    cd /local/workspace
    hailomz compile yolov8n_seg \
      --ckpt shared_with_docker/models/${ONNX_26N} \
      --hw-arch hailo8 \
      --calib-path shared_with_docker/calibration_images_17c \
      --start-node-names images \
      --end-node-names output0 output1 \
      --model-script shared_with_docker/model_scripts/yolov8n_seg_no_nms.alls \
      2>&1 | tee shared_with_docker/logs/compile_yolo26n_seg_17c.log
    cp yolov8n_seg.hef shared_with_docker/models/${HEF_26N}
  "
fi
[[ -f "${MODELS_DIR}/${HEF_26N}" ]] || fail "Missing HEF: ${MODELS_DIR}/${HEF_26N}"
ok "HEF ready: ${HEF_26N}"

# Phase 4: upload to AGL
safe_agl_path "${AGL_HEF_DIR}"
safe_agl_path "${AGL_FRAMES_DIR}"
safe_agl_path "${AGL_RESULTS_DIR}"
if [[ ${SKIP_UPLOAD} -eq 0 ]]; then
  log "Phase 4 - upload to AGL"
  ssh "${AGL_HOST}" "mkdir -p '${AGL_HEF_DIR}' '${AGL_FRAMES_DIR}' '${AGL_RESULTS_DIR}'"

  tmp_frames="/tmp/frames17c_${RUN_ID}"
  mkdir -p "${tmp_frames}"
  find "${DATASET_TRAIN_IMGS}" -maxdepth 1 \( -name '*.jpg' -o -name '*.jpeg' -o -name '*.png' \) \
    | shuf | head -n "${INFER_IMAGES}" | xargs -I{} cp "{}" "${tmp_frames}/"
  scp -q "${tmp_frames}"/* "${AGL_HOST}:${AGL_FRAMES_DIR}/"
  rm -rf "${tmp_frames}"

  scp -q "${MODELS_DIR}/${HEF_8S}" "${AGL_HOST}:${AGL_HEF_DIR}/"
  scp -q "${MODELS_DIR}/${HEF_8N}" "${AGL_HOST}:${AGL_HEF_DIR}/"
  scp -q "${MODELS_DIR}/${HEF_26N}" "${AGL_HOST}:${AGL_HEF_DIR}/"
  ok "Upload done"
fi

# Phase 5: inference on AGL
if [[ ${SKIP_INFER} -eq 0 ]]; then
  log "Phase 5 - AGL inference"
  remote_run_dir="${AGL_RESULTS_DIR}/${RUN_ID}"
  safe_agl_path "${remote_run_dir}"
  ssh "${AGL_HOST}" "mkdir -p '${remote_run_dir}'"

  run_model() {
    local label="$1"
    local script="$2"
    local hef="$3"
    local out_mp4="${remote_run_dir}/${label}.mp4"
    safe_agl_path "${out_mp4}"
    ssh "${AGL_HOST}" "cd '${AGL_SCRIPTS_DIR}' && HEF_PATH='${hef}' MODEL_LABEL='${label}' python3 '${script}' ${INFER_SECS} --images-dir '${AGL_FRAMES_DIR}' --loop --save --output '${out_mp4}' > '${remote_run_dir}/${label}.log' 2>&1"
  }

  run_model "yolov8s_vasco_17c" "inference_camera_scalercrop_yolov8s.py" "${AGL_HEF_DIR}/${HEF_8S}"
  run_model "yolov8n_seg_17c" "inference_camera_scalercrop_yolov8n_seg.py" "${AGL_HEF_DIR}/${HEF_8N}"
  run_model "yolo26n_seg_17c" "inference_camera_scalercrop_yolo26n_seg.py" "${AGL_HEF_DIR}/${HEF_26N}"

  ok "Inference done"

  log "Phase 6 - fetch results"
  scp -q -r "${AGL_HOST}:${remote_run_dir}" "${LOCAL_RESULTS_DIR}/"
  ok "Fetched: ${LOCAL_RESULTS_DIR}/${RUN_ID}"
fi

# Phase 7: summary table
if [[ ${SKIP_TABLE} -eq 0 ]]; then
  log "Phase 7 - summary table"
  RESULT_DIR="${LOCAL_RESULTS_DIR}/${RUN_ID}"
  [[ -d "${RESULT_DIR}" ]] || RESULT_DIR="$(ls -dt ${YOLO_BASE}/results/*/bench17c_* 2>/dev/null | head -n 1 || true)"
  [[ -n "${RESULT_DIR}" ]] || fail "No result directory found"

  python3 - << PY
import csv,glob,os,json,statistics
result_dir="${RESULT_DIR}"
csvs=sorted(glob.glob(os.path.join(result_dir,'*_stats.csv')))
if not csvs:
    print('[WARN] no *_stats.csv found in',result_dir)
    raise SystemExit(0)
rows=[]
for p in csvs:
    name=os.path.basename(p).replace('_stats.csv','')
    fps=[]; tms=[]; nd=[]
    with open(p,newline='') as f:
        r=csv.DictReader(f)
        for row in r:
            try: fps.append(float(row.get('pipeline_fps',0)))
            except: pass
            try: tms.append(float(row.get('total_ms',0)))
            except: pass
            try: nd.append(float(row.get('num_detections',0)))
            except: pass
    rows.append({
      'model':name,
      'frames':len(fps),
      'fps_mean':round(sum(fps)/len(fps),3) if fps else 'N/A',
      'lat_ms_mean':round(sum(tms)/len(tms),3) if tms else 'N/A',
      'lat_ms_p95':round(statistics.quantiles(tms,n=100)[94],3) if len(tms)>=20 else 'N/A',
      'total_detections':int(sum(nd)) if nd else 0,
    })

print('\n=== BENCHMARK 17C SUMMARY ===')
for r in rows:
    print(f"{r['model']}: frames={r['frames']} fps_mean={r['fps_mean']} lat_mean_ms={r['lat_ms_mean']} lat_p95_ms={r['lat_ms_p95']} det_total={r['total_detections']}")
out=os.path.join(result_dir,'summary_table.json')
with open(out,'w') as f: json.dump(rows,f,indent=2)
print('summary_json=',out)
PY
fi

echo
echo "DONE: ${RUN_ID}"
echo "Local results: ${LOCAL_RESULTS_DIR}"
echo "AGL results: ${AGL_RESULTS_DIR}/${RUN_ID}"
