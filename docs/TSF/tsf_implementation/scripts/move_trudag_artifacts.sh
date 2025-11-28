#!/usr/bin/env bash
set -euo pipefail

# move_trudag_artifacts.sh
# Safely move .trudag_items, .dotstop.dot, and .trudag-venv into the TSF implementation directory.
# This script is conservative: it will create backups and will not overwrite existing destination
# without saving a backup first. It does not run any trudag commands. Run this script and inspect
# its output; no publish or lint will be triggered.

ROOT="$(git rev-parse --show-toplevel 2>/dev/null || true)"
if [ -z "$ROOT" ]; then
  ROOT="$(cd "$(dirname "$0")/../../../../.." && pwd)"
fi
TSF_DIR=${TSF_DIR:-docs/TSF/tsf_implementation}
TSF_ROOT="$ROOT/$TSF_DIR"
mkdir -p "$TSF_ROOT"

DATE=$(date +%Y%m%d_%H%M%S)
LOG="$ROOT/.move_trudag_artifacts_${DATE}.log"
exec > >(tee -a "$LOG") 2>&1

echo "Move trudag artifacts — starting at $(date)"
echo "Repo root: $ROOT"
echo "TSF root: $TSF_ROOT"

targets=(".trudag_items" ".dotstop.dot" ".trudag-venv")

for t in "${targets[@]}"; do
  src="$ROOT/$t"
  dest="$TSF_ROOT/$t"
  if [ ! -e "$src" ]; then
    echo "Source not found: $src — skipping"
    continue
  fi

  if [ -e "$dest" ]; then
    # backup existing destination before touching it
    dest_bak="$ROOT/${t#./}_dest_backup_$DATE"
    echo "Destination already exists: $dest"
    echo "Creating backup of destination -> $dest_bak"
    if [ -d "$dest" ]; then
      tar -czf "$dest_bak.tar.gz" -C "$(dirname "$dest")" "$(basename "$dest")"
      echo "Backup created: $dest_bak.tar.gz"
    else
      cp -a "$dest" "$dest_bak" && echo "Backup created: $dest_bak"
    fi
  fi

  # create parent dir and move
  mkdir -p "$(dirname "$dest")"
  echo "Moving $src -> $dest"
  mv "$src" "$dest"
  echo "Moved $src to $dest"
done

echo "Move completed at $(date). Log: $LOG"

echo "NOTE: No trudag commands were executed. To validate, run:"
echo "  TSF_DIR=$TSF_DIR DRY_RUN=1 NO_PUBLISH=1 bash $TSF_ROOT/scripts/run_trudag_publish.sh"
