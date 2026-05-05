#!/usr/bin/env bash
set -euo pipefail

HOST_ROOT="${1:-$HOME/Documents/AI/hailo}"
DRY_RUN="${DRY_RUN:-1}"
TS="$(date +%Y%m%d_%H%M%S)"
OPS_DIR="$HOST_ROOT/ops"
BACKUP_DIR="$OPS_DIR/backup_host/$TS"

log() {
  printf '[%s] %s\n' "$(date +%H:%M:%S)" "$*"
}

run() {
  if [[ "$DRY_RUN" == "1" ]]; then
    printf 'DRY-RUN: %s\n' "$*"
  else
    eval "$@"
  fi
}

ensure_dir() {
  local path="$1"
  run "mkdir -p \"$path\""
}

move_path() {
  local src="$1"
  local dst="$2"
  if [[ -e "$src" ]]; then
    ensure_dir "$(dirname "$dst")"
    run "mv \"$src\" \"$dst\""
  fi
}

copy_if_missing() {
  local src="$1"
  local dst="$2"
  if [[ -e "$src" && ! -e "$dst" ]]; then
    ensure_dir "$(dirname "$dst")"
    run "cp -R \"$src\" \"$dst\""
  fi
}

if [[ ! -d "$HOST_ROOT" ]]; then
  echo "ERROR: host root not found: $HOST_ROOT"
  exit 1
fi

log "Host root: $HOST_ROOT"
log "DRY_RUN: $DRY_RUN"

ensure_dir "$BACKUP_DIR"
ensure_dir "$HOST_ROOT/shared_with_docker"
ensure_dir "$HOST_ROOT/packages"
ensure_dir "$HOST_ROOT/imagesets"
ensure_dir "$HOST_ROOT/sdk"
ensure_dir "$HOST_ROOT/legacy"
ensure_dir "$HOST_ROOT/tools"
ensure_dir "$HOST_ROOT/docs_local"
ensure_dir "$HOST_ROOT/ops/reports"

log "1) Consolidate duplicate calibration directories"
if [[ -d "$HOST_ROOT/calibration_images" ]]; then
  if [[ -d "$HOST_ROOT/shared_with_docker/calibration_images" ]]; then
    move_path "$HOST_ROOT/calibration_images" "$BACKUP_DIR/calibration_images_host_duplicate"
  else
    move_path "$HOST_ROOT/calibration_images" "$HOST_ROOT/shared_with_docker/calibration_images"
  fi
fi

log "2) Consolidate package sources"
move_path "$HOST_ROOT/Pre-Requisitos" "$HOST_ROOT/legacy/Pre-Requisitos"
if [[ -d "$HOST_ROOT/legacy/Pre-Requisitos/packages" ]]; then
  ensure_dir "$HOST_ROOT/packages"
  for pkg in "$HOST_ROOT/legacy/Pre-Requisitos/packages"/*; do
    [[ -e "$pkg" ]] || continue
    base="$(basename "$pkg")"
    if [[ -e "$HOST_ROOT/packages/$base" ]]; then
      move_path "$pkg" "$BACKUP_DIR/packages_duplicate/$base"
    else
      move_path "$pkg" "$HOST_ROOT/packages/$base"
    fi
  done
fi

log "3) Normalize hailo-dfc and sdk placement"
if [[ -d "$HOST_ROOT/hailo-dfc" ]]; then
  move_path "$HOST_ROOT/hailo-dfc" "$HOST_ROOT/sdk/hailo-dfc"
fi

if [[ -d "$HOST_ROOT/sdk/hailo-dfc/hailo8_sdk/shared_with_docker" ]]; then
  if [[ -d "$HOST_ROOT/shared_with_docker/doc" ]]; then
    move_path "$HOST_ROOT/sdk/hailo-dfc/hailo8_sdk/shared_with_docker" "$BACKUP_DIR/hailo8_sdk_shared_with_docker_duplicate"
  else
    move_path "$HOST_ROOT/sdk/hailo-dfc/hailo8_sdk/shared_with_docker" "$HOST_ROOT/shared_with_docker"
  fi
fi

log "4) Normalize hailo_models folder"
if [[ -d "$HOST_ROOT/hailo_models" ]]; then
  move_path "$HOST_ROOT/hailo_models" "$HOST_ROOT/tools/hailo_models"
fi

log "5) Ensure canonical shared subfolders"
ensure_dir "$HOST_ROOT/shared_with_docker/COCO/val2017"
ensure_dir "$HOST_ROOT/shared_with_docker/calibration_images"
ensure_dir "$HOST_ROOT/shared_with_docker/outputs"
ensure_dir "$HOST_ROOT/shared_with_docker/models"
ensure_dir "$HOST_ROOT/shared_with_docker/logs"
ensure_dir "$HOST_ROOT/shared_with_docker/scripts"
ensure_dir "$HOST_ROOT/shared_with_docker/ops/reports"

log "6) Move top-level files into canonical shared folders"
for model in yolov8n.pt yolov8n.onnx yolov8n.har yolov8n.hef yolov8n_simplified.onnx yolov8n_simplified.yaml; do
  if [[ -f "$HOST_ROOT/shared_with_docker/$model" ]]; then
    move_path "$HOST_ROOT/shared_with_docker/$model" "$HOST_ROOT/shared_with_docker/models/$model"
  fi
done

for logfile in acceleras.log allocator.log hailo_examples.log hailort.log pyhailort.log hailo_sdk.client.log compile_baseline.log compile_yolov8n.log compile_custom.log; do
  if [[ -f "$HOST_ROOT/shared_with_docker/$logfile" ]]; then
    move_path "$HOST_ROOT/shared_with_docker/$logfile" "$HOST_ROOT/shared_with_docker/logs/$logfile"
  fi
done

log "7) Handle duplicate COCO zip suffix files"
if [[ -f "$HOST_ROOT/shared_with_docker/COCO/annotations_trainval2017.zip.1" ]]; then
  move_path "$HOST_ROOT/shared_with_docker/COCO/annotations_trainval2017.zip.1" "$BACKUP_DIR/annotations_trainval2017.zip.1"
fi

log "8) Preserve large outputs but relocate optional old folder"
if [[ -d "$HOST_ROOT/docker_custom" ]]; then
  move_path "$HOST_ROOT/docker_custom" "$HOST_ROOT/legacy/docker_custom"
fi

log "9) Generate post-run tree report"
REPORT="$HOST_ROOT/ops/reports/hailo_host_layout_${TS}.txt"
run "find \"$HOST_ROOT\" -maxdepth 3 -mindepth 1 -print | sed 's|$HOST_ROOT|.|' | sort > \"$REPORT\""

log "Done. Backup: $BACKUP_DIR"
log "Layout report: $REPORT"
if [[ "$DRY_RUN" == "1" ]]; then
  log "No changes applied. Apply with: DRY_RUN=0 $0 \"$HOST_ROOT\""
fi
