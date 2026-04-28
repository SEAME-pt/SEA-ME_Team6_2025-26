#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="${1:-$HOME/Documents/AI/hailo/shared_with_docker}"
DRY_RUN="${DRY_RUN:-1}"
TIMESTAMP="$(date +%Y%m%d_%H%M%S)"
BACKUP_DIR="$ROOT_DIR/ops/backup/$TIMESTAMP"

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

must_exist() {
  local path="$1"
  [[ -d "$path" ]] || { echo "ERROR: directory not found: $path"; exit 1; }
}

move_if_exists() {
  local src="$1"
  local dst="$2"
  if [[ -e "$src" ]]; then
    run "mkdir -p \"$(dirname "$dst")\""
    run "mv \"$src\" \"$dst\""
  fi
}

copy_if_missing() {
  local src="$1"
  local dst="$2"
  if [[ -f "$src" && ! -f "$dst" ]]; then
    run "mkdir -p \"$(dirname "$dst")\""
    run "cp -n \"$src\" \"$dst\""
  fi
}

must_exist "$ROOT_DIR"

log "Root: $ROOT_DIR"
log "Dry-run: $DRY_RUN"

run "mkdir -p \"$ROOT_DIR/models\" \"$ROOT_DIR/logs\" \"$ROOT_DIR/ops/reports\" \"$ROOT_DIR/ops/tmp\" \"$ROOT_DIR/ops/trash\" \"$BACKUP_DIR\""

log "1) Consolidating model artifacts into models/"
for artifact in yolov8n.pt yolov8n.onnx yolov8n.har yolov8n.hef yolov8n_simplified.onnx yolov8n_simplified.yaml; do
  if [[ -f "$ROOT_DIR/$artifact" ]]; then
    move_if_exists "$ROOT_DIR/$artifact" "$ROOT_DIR/models/$artifact"
  fi
done

log "2) Moving logs to logs/"
for logf in acceleras.log allocator.log hailo_examples.log hailort.log pyhailort.log hailo_sdk.client.log compile_baseline.log compile_yolov8n.log; do
  if [[ -f "$ROOT_DIR/$logf" ]]; then
    move_if_exists "$ROOT_DIR/$logf" "$ROOT_DIR/logs/$logf"
  fi
done

log "3) Cleaning duplicate COCO archives"
if [[ -f "$ROOT_DIR/COCO/annotations_trainval2017.zip.1" ]]; then
  move_if_exists "$ROOT_DIR/COCO/annotations_trainval2017.zip.1" "$BACKUP_DIR/annotations_trainval2017.zip.1"
fi

if [[ -f "$ROOT_DIR/COCO/annotations_trainval2017.zip" && -f "$ROOT_DIR/COCO/annotations_trainval2017.zip.1" ]]; then
  run "cmp -s \"$ROOT_DIR/COCO/annotations_trainval2017.zip\" \"$ROOT_DIR/COCO/annotations_trainval2017.zip.1\" || true"
fi

log "4) Normalize calibration images (keep only first 256 if larger)"
if [[ -d "$ROOT_DIR/calibration_images" ]]; then
  count="$(find "$ROOT_DIR/calibration_images" -type f \( -iname '*.jpg' -o -iname '*.jpeg' -o -iname '*.png' \) | wc -l | tr -d ' ')"
  if [[ "$count" -gt 256 ]]; then
    run "mkdir -p \"$BACKUP_DIR/calibration_overflow\""
    find "$ROOT_DIR/calibration_images" -type f \( -iname '*.jpg' -o -iname '*.jpeg' -o -iname '*.png' \) | sort | tail -n +257 | while read -r img; do
      base="$(basename "$img")"
      move_if_exists "$img" "$BACKUP_DIR/calibration_overflow/$base"
    done
  fi
fi

log "5) Ensure scripts and outputs exist"
run "mkdir -p \"$ROOT_DIR/scripts\" \"$ROOT_DIR/outputs\""

log "6) Optional: copy shared HEF/HAR from container root if present in mounted workspace mirror"
copy_if_missing "$ROOT_DIR/yolov8n.hef" "$ROOT_DIR/models/yolov8n.hef"
copy_if_missing "$ROOT_DIR/yolov8n.har" "$ROOT_DIR/models/yolov8n.har"

log "7) Summary"
run "find \"$ROOT_DIR\" -maxdepth 2 -mindepth 1 -print | sed 's|$ROOT_DIR|.|' | sort"

log "Done. Backup/trash location: $BACKUP_DIR"
if [[ "$DRY_RUN" == "1" ]]; then
  log "No changes were applied. Re-run with: DRY_RUN=0 $0 \"$ROOT_DIR\""
fi
