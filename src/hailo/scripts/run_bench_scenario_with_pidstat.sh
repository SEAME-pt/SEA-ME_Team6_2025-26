#!/usr/bin/env bash
set -euo pipefail

SC="${1:-}"
DURATION="${2:-60}"

if [[ -z "${SC}" ]]; then
  echo "Uso: $0 <cenario> [duracao_s]"
  echo "Exemplo: $0 recta 60"
  echo "Cenários: recta curva sombra cruzamento"
  exit 1
fi

if [[ ! "${SC}" =~ ^(recta|curva|sombra|cruzamento)$ ]]; then
  echo "Cenário inválido: ${SC}"
  echo "Usa um de: recta curva sombra cruzamento"
  exit 1
fi

BASE_OUT="/data/results/bench_runs"
SCRIPT_DIR="/data/scripts/yolo_realtime"

mkdir -p "${BASE_OUT}/yolov8s" "${BASE_OUT}/yolov8n_seg" "${BASE_OUT}/yolo26n_seg"

run_one_model() {
  local model_name="$1"
  local script_path="$2"
  local video_out="$3"
  local run_log="$4"
  local cpu_log="$5"

  echo "[RUN] ${model_name} | cenário=${SC} | duração=${DURATION}s"

  local pidstat_pid=""
  if command -v pidstat >/dev/null 2>&1; then
    pidstat -h -u -r -d 1 >"${cpu_log}" 2>&1 &
    pidstat_pid=$!
    echo "[CPU] pidstat ligado -> ${cpu_log}"
  else
    echo "[WARN] pidstat não encontrado; CPU log não será gerado para ${model_name}" | tee "${cpu_log}"
  fi

  cleanup_pidstat() {
    if [[ -n "${pidstat_pid}" ]] && kill -0 "${pidstat_pid}" >/dev/null 2>&1; then
      kill "${pidstat_pid}" >/dev/null 2>&1 || true
      wait "${pidstat_pid}" >/dev/null 2>&1 || true
    fi
  }

  trap cleanup_pidstat EXIT

  python3 "${script_path}" "${DURATION}" --save --output "${video_out}" | tee "${run_log}"

  cleanup_pidstat
  trap - EXIT

  echo "[OK] ${model_name} concluído"
  echo "      vídeo: ${video_out}"
  echo "      run log: ${run_log}"
  echo "      cpu log: ${cpu_log}"
}

echo "=== Cenário: ${SC} ==="
echo "Posiciona o carro no cenário e prime ENTER para começar..."
read -r

run_one_model \
  "YOLOv8s" \
  "${SCRIPT_DIR}/inference_camera_scalercrop_yolov8s.py" \
  "${BASE_OUT}/yolov8s/demo_yolov8s_${SC}.mp4" \
  "${BASE_OUT}/yolov8s/yolov8s_${SC}.log" \
  "${BASE_OUT}/yolov8s/pidstat_yolov8s_${SC}.log"

echo "Reposiciona para o MESMO cenário e prime ENTER..."
read -r

run_one_model \
  "YOLOv8n-seg" \
  "${SCRIPT_DIR}/inference_camera_scalercrop_yolov8n_seg.py" \
  "${BASE_OUT}/yolov8n_seg/demo_yolov8n_seg_${SC}.mp4" \
  "${BASE_OUT}/yolov8n_seg/yolov8n_seg_${SC}.log" \
  "${BASE_OUT}/yolov8n_seg/pidstat_yolov8n_seg_${SC}.log"

echo "Reposiciona para o MESMO cenário e prime ENTER..."
read -r

run_one_model \
  "YOLO26n-seg" \
  "${SCRIPT_DIR}/inference_camera_scalercrop_yolo26n_seg.py" \
  "${BASE_OUT}/yolo26n_seg/demo_yolo26n_seg_${SC}.mp4" \
  "${BASE_OUT}/yolo26n_seg/yolo26n_seg_${SC}.log" \
  "${BASE_OUT}/yolo26n_seg/pidstat_yolo26n_seg_${SC}.log"

echo "=== Cenário ${SC} concluído para os 3 modelos ==="
echo "Resultados em: ${BASE_OUT}"
