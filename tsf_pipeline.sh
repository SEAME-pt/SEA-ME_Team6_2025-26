#!/bin/bash
# TSF Pipeline Automation Script
# Usage: bash tsf_pipeline.sh

set -e

# 1. Backup
backup_dir="backups"
timestamp=$(date +%Y%m%d_%H%M%S)
backup_file="$backup_dir/items_backup_$timestamp.tar.gz"
mkdir -p "$backup_dir"
tar -czf "$backup_file" docs/TSF/tsf_implementation/items docs/TSF/tsf_implementation/.trudag_items docs/TSF/requirements/tsf-requirements-table.md docs/sprints

echo "[TSF] Backup created at $backup_file"

# 2. Parse and Sync (Python script must handle all logic)
cd docs/TSF/tsf_implementation/scripts
python3 generate_from_requirements.py --overwrite

# 3. Copy items to .trudag_items (if script exists)
if [ -f sync_evidences.py ]; then
    python3 sync_evidences.py
else
    echo "[TSF] sync_evidences.py not found, skipping .trudag_items sync."
fi

cd ../..

# 4. TruDAG validation
cd tsf_implementation
trudag manage lint || { echo '[TSF] Lint failed!'; exit 1; }
trudag score || { echo '[TSF] Score failed!'; exit 1; }
trudag publish || { echo '[TSF] Publish failed!'; exit 1; }

echo "[TSF] Pipeline completed successfully."
