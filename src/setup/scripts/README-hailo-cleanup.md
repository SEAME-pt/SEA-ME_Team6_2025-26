# Hailo Cleanup Scripts (Lenovo + Docker)

These scripts help consolidate repeated files/folders for the Hailo workspace.

## Scripts

- `hailo-audit-duplicates.sh`  
  Audits `shared_with_docker` and generates a report.

- `hailo-reorganize.sh`  
  Reorganizes `shared_with_docker` (models/logs/calibration duplicates) with backup and dry-run.

- `hailo-reorganize-host.sh`  
  Reorganizes host root `~/Documents/AI/hailo` (outside `shared_with_docker`) and consolidates old folders.

- `hailo-compile-and-deploy.sh`  
  Runs baseline/custom `hailomz compile`, persists `.hef` in mounted shared folders, and can `scp` to Raspberry Pi.

## Safe usage flow

1. Run audit.
2. Run dry-run reorganize.
3. Run apply mode (`DRY_RUN=0`).

## Commands

```bash
cd "/Users/joaojesussilva/Documents/untitled folder/SEA-ME_Team6_2025-26"

chmod +x src/setup/scripts/hailo-audit-duplicates.sh \
  src/setup/scripts/hailo-reorganize.sh \
  src/setup/scripts/hailo-reorganize-host.sh

./src/setup/scripts/hailo-audit-duplicates.sh "$HOME/Documents/AI/hailo/shared_with_docker"

DRY_RUN=1 ./src/setup/scripts/hailo-reorganize.sh "$HOME/Documents/AI/hailo/shared_with_docker"
DRY_RUN=0 ./src/setup/scripts/hailo-reorganize.sh "$HOME/Documents/AI/hailo/shared_with_docker"

DRY_RUN=1 ./src/setup/scripts/hailo-reorganize-host.sh "$HOME/Documents/AI/hailo"
DRY_RUN=0 ./src/setup/scripts/hailo-reorganize-host.sh "$HOME/Documents/AI/hailo"

# Run inside Hailo container
./src/setup/scripts/hailo-compile-and-deploy.sh baseline

# Custom compile + optional deploy
ONNX_PATH=/local/workspace/shared/models/yolov8n.onnx \
PI_TARGET=root@10.21.220.191:/root/models/ \
./src/setup/scripts/hailo-compile-and-deploy.sh custom
```

## Notes

- All destructive changes are avoided; moved content goes to timestamped backup folders under `ops/backup*`.
- Docker container sees the same `shared_with_docker` content via mount, so host cleanup is reflected in container.
