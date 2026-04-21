# Hailo YOLOv8 Compilation Tutorial (End-to-End)

> Consolidated version for the SEA:ME Team 6 project (Mar/2026).  
> This guide reflects what was **validated in practice** on Lenovo + Docker + Hailo AI Software Suite.

## Index

- [1) Goal](#1-goal)
- [2) Key Concepts (No Ambiguity)](#2-key-concepts-no-ambiguity)
  - [Validated Facts](#validated-facts)
- [3) Requirements](#3-requirements)
  - [Host (Lenovo)](#host-lenovo)
  - [Hailo Artifacts](#hailo-artifacts)
  - [Recommended Working Structure](#recommended-working-structure)
- [Tutorial (Step-by-Step)](#tutorial-step-by-step)
- [4) Clean Installation (Official AI Software Suite)](#4-clean-installation-official-ai-software-suite)
- [5) Start Container with Mounted Volume](#5-start-container-with-mounted-volume)
- [6) Prepare Calibration Dataset](#6-prepare-calibration-dataset)
- [7) Baseline Compile (Recommended First Test)](#7-baseline-compile-recommended-first-test)
  - [Expected Result](#expected-result)
  - [Important Note](#important-note)
- [8) Custom Compile with Your ONNX](#8-custom-compile-with-your-onnx)
  - [Difference Between Baseline and Custom](#difference-between-baseline-and-custom)
- [9) Common Errors and Quick Fixes](#9-common-errors-and-quick-fixes)
- [10) Deploy to Raspberry Pi (Real Hailo)](#10-deploy-to-raspberry-pi-real-hailo)
- [11) Recommended Model Order (Sprint)](#11-recommended-model-order-sprint)
- [12) Automation (Repeatable)](#12-automation-repeatable)
- [13) Future Integration in the ADAS Stack](#13-future-integration-in-the-adas-stack)
- [14) Final Checklist](#14-final-checklist)
- [15) Important Team Notes](#15-important-team-notes)
- [16) Appendix: Legacy Paths and Rationale](#16-appendix-legacy-paths-and-rationale)
  - [A) Why Manual Installation Was Not Kept as Main Flow](#a-why-manual-installation-was-not-kept-as-main-flow)
  - [B) Docker Permission Error (`docker.sock`)](#b-docker-permission-error-dockersock)
  - [C) Historical Contradiction Resolved](#c-historical-contradiction-resolved)
  - [D) What to Keep vs What to Archive](#d-what-to-keep-vs-what-to-archive)
  - [E) Why the Initial colega Tutorial Could Fail (and Why This Took Longer)](#e-why-the-initial-colega-tutorial-could-fail-and-why-this-took-longer)
  - [F) Why João 2.0 Also Failed Before Final Success](#f-why-joão-20-also-failed-before-final-success)
- [17) Legacy Reference: colega Initial Tutorial](#17-legacy-reference-colega-initial-tutorial)

## 1) Goal

Compile YOLOv8 models for Hailo and generate `HEF` for deployment on Raspberry Pi + Hailo.

Target pipeline:

`PT/ONNX -> (Model Zoo + DFC) -> HAR/HEF -> Raspberry Pi (HailoRT) -> Inference`

---

## 2) Key Concepts (No Ambiguity)

- `/dev/hailo0`: runtime hardware/driver device (it is not a compiler).
- `DFC (Dataflow Compiler)`: compiles and optimizes models for Hailo.
- `HAR`: intermediate artifact.
- `HEF`: executable artifact for Hailo inference.

### What is a `.hef`

`HEF` means `Hailo Executable Format`.

It is the deployable model file used by:
- `Hailo-8 AI Accelerator`
- `Raspberry Pi AI Kit`

Practical rule:
- You compile/export to `.hef` during model preparation.
- You execute `.hef` on target runtime with `HailoRT`.

### What is Hailo PCIe

`Hailo PCIe` is the physical connection path between CPU and Hailo accelerator hardware.

Architecture:

`CPU -> PCIe -> Hailo-8 -> AI inference`

If no physical Hailo device is attached on the host, messages such as `No Hailo PCIe device was found` are expected.

### Docker Environment Clarification

The Docker image mimics the official Hailo software environment (DFC, Model Zoo, HailoRT, tools, examples).

This is enough to:
- prepare data and scripts,
- run compilation flows,
- generate artifacts in mounted volumes.

For accelerated runtime execution, target hardware is still required.

### Validated Facts

- It is possible to generate `HEF` in Docker/Lenovo **without local Hailo PCIe**, as long as the DFC/Model Zoo environment is correctly configured.
- Running accelerated inference with `HEF` requires real Hailo hardware on the target (for example, Raspberry Pi with Hailo).

---

## 3) Requirements

### Host (Lenovo)
- Linux x86_64
- Docker install permissions on host user
- Disk space (>= 30 GB recommended)

### Hailo Artifacts
- `hailo8_ai_sw_suite_2025-10_docker.zip`

### Recommended Working Structure

```bash
~/Documents/AI/hailo/
├── shared_with_docker/
│   ├── COCO/
│   │   └── val2017/
│   ├── calibration_images/
│   ├── models/
│   │   ├── yolov8n.pt
│   │   ├── yolov8n.onnx
│   │   ├── yolov8n.har
│   │   └── yolov8n.hef
│   ├── logs/
│   ├── outputs/
│   ├── scripts/
│   └── ops/
└── docker_custom/   # optional for customizations
```

---

## Tutorial (Step-by-Step)

This is the practical, command-driven sequence to achieve:

`PT/ONNX -> (Model Zoo + DFC) -> HAR/HEF -> Raspberry Pi (HailoRT) -> Inference`

### Step 0A — Install Docker on Lenovo (Ubuntu)

```bash
sudo apt-get update
sudo apt-get install -y ca-certificates curl gnupg

sudo install -m 0755 -d /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
sudo chmod a+r /etc/apt/keyrings/docker.gpg

echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] \
https://download.docker.com/linux/ubuntu $(. /etc/os-release && echo \"$VERSION_CODENAME\") stable" | \
sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

sudo apt-get update
sudo apt-get install -y docker-ce docker-ce-cli containerd.io docker-compose-plugin

sudo usermod -aG docker $USER
newgrp docker
```

### Step 0B — Validate Docker works

```bash
docker --version
docker run hello-world
```

If `docker.sock` permission error appears, logout/login once and retry.

### Step 1 — Create full workspace tree from scratch

```bash
mkdir -p "$HOME/Documents/AI/hailo"
cd "$HOME/Documents/AI/hailo"

mkdir -p shared_with_docker/{COCO/val2017,calibration_images,outputs,scripts,models,logs}
mkdir -p docker_custom

cd "$HOME/Documents/AI/hailo/shared_with_docker"
pwd
find . -maxdepth 3 -type d | sort
```

### Step 1.1 — (Optional) Normalize an existing messy structure

If this is not a clean machine, run the cleanup scripts from this repository on the host:

```bash
cd "/path/to/SEA-ME_Team6_2025-26"

chmod +x src/setup/scripts/hailo-audit-duplicates.sh \
  src/setup/scripts/hailo-reorganize.sh \
  src/setup/scripts/hailo-reorganize-host.sh

./src/setup/scripts/hailo-audit-duplicates.sh "$HOME/Documents/AI/hailo/shared_with_docker"

DRY_RUN=1 ./src/setup/scripts/hailo-reorganize.sh "$HOME/Documents/AI/hailo/shared_with_docker"
DRY_RUN=1 ./src/setup/scripts/hailo-reorganize-host.sh "$HOME/Documents/AI/hailo"

DRY_RUN=0 ./src/setup/scripts/hailo-reorganize.sh "$HOME/Documents/AI/hailo/shared_with_docker"
DRY_RUN=0 ./src/setup/scripts/hailo-reorganize-host.sh "$HOME/Documents/AI/hailo"
```

These scripts move files to backup folders under `ops/backup*`; they do not hard-delete data.

If you still want to run cleanup inside Docker (not recommended due to file ownership):

```bash
docker run -it --rm \
  -v "/Users/joaojesussilva/Documents/untitled folder/SEA-ME_Team6_2025-26:/repo" \
  -v "$HOME/Documents/AI/hailo:/host_hailo" \
  ubuntu:22.04 bash

cd /repo
DRY_RUN=1 ./src/setup/scripts/hailo-reorganize.sh /host_hailo/shared_with_docker
DRY_RUN=1 ./src/setup/scripts/hailo-reorganize-host.sh /host_hailo
```

### Step 2 — Prepare required inputs from zero

#### 2.1 Download COCO validation images (if folder is empty)

```bash
cd "$HOME/Documents/AI/hailo/shared_with_docker"

if [ -z "$(find COCO/val2017 -type f 2>/dev/null)" ]; then
  curl -L -o COCO/val2017.zip "http://images.cocodataset.org/zips/val2017.zip"
  unzip -o COCO/val2017.zip -d COCO/
fi

find COCO/val2017 -type f | wc -l
```

#### 2.2 Ensure custom model exists (optional path)

If you want custom compile (`--ckpt`), put `yolov8n.onnx` at:

`$HOME/Documents/AI/hailo/shared_with_docker/models/yolov8n.onnx`

Example (generate ONNX from scratch):

```bash
cd "$HOME/Documents/AI/hailo/shared_with_docker"
python3 -m venv .venv
source .venv/bin/activate
python -m pip install --upgrade pip
python -m pip install ultralytics onnx
python -c "from ultralytics import YOLO; YOLO('yolov8n.pt').export(format='onnx', opset=11)"
cp -f yolov8n.onnx models/
```

### Step 3 — Prepare Hailo docker image (if not already loaded)

If you already have `hailo_custom_suite:latest`, skip to Step 4.

```bash
mkdir -p "$HOME/hailo-dfc"
cd "$HOME/hailo-dfc"

# Move your Hailo suite ZIP here before running next commands
unzip -o hailo8_ai_sw_suite_2025-10_docker.zip
docker load < hailo8_ai_sw_suite_2025-10.tar.gz
docker images | grep -i hailo
```

### Step 4 — Start container with mounted workspace

```bash
docker run -it --rm \
  -v "$HOME/Documents/AI/hailo/shared_with_docker:/local/workspace/shared" \
  hailo_custom_suite:latest
```

Inside container, validate tools:

```bash
which hailomz
which hailo
hailomz --help | head -n 20
hailo --help | head -n 20
```

### Step 5 — Populate calibration dataset (if empty)

Run on host (outside container):

```bash
cd "$HOME/Documents/AI/hailo/shared_with_docker"
find calibration_images -type f | wc -l

find COCO/val2017 -type f \( -iname "*.jpg" -o -iname "*.jpeg" -o -iname "*.png" \) \
  | head -n 256 \
  | xargs -I{} cp "{}" calibration_images/

find calibration_images -type f | wc -l
```

### Step 6 — Compile baseline first (recommended)

Run inside container:

```bash
set -e
find /local/workspace/shared/calibration_images -type f | wc -l

hailomz compile yolov8n \
  --hw-arch hailo8 \
  --calib-path /local/workspace/shared/calibration_images \
  2>&1 | tee /local/workspace/shared/logs/compile_baseline.log
```

### Step 7 — Persist generated HEF (critical)

Run inside container before exit:

```bash
cp /local/workspace/yolov8n.hef /local/workspace/shared/models/
ls -lh /local/workspace/shared/models/yolov8n.hef
```

Exact short flow validated by the team (also valid):

```bash
cp /local/workspace/yolov8n.hef /local/workspace/shared/
ls -lh /local/workspace/shared/yolov8n.hef
```

### Step 8 — Compile custom ONNX (optional)

Run inside container after baseline succeeds:

```bash
hailomz compile yolov8n \
  --ckpt /local/workspace/shared/models/yolov8n.onnx \
  --hw-arch hailo8 \
  --calib-path /local/workspace/shared/calibration_images \
  2>&1 | tee /local/workspace/shared/logs/compile_custom.log

cp /local/workspace/yolov8n.hef /local/workspace/shared/models/
```

### Step 8.1 — Quick Plan (critical after successful compile)

After `HEF file written to yolov8n.hef`, run this exact sequence.

Inside container (before exit):

```bash
ls -lh /local/workspace/yolov8n.hef
cp /local/workspace/yolov8n.hef /local/workspace/shared/
ls -lh /local/workspace/shared/yolov8n.hef
```

Important: run each command on a separate line. Do not paste `cp` + `ls` as a single line.

If source file is not found, locate it first:

```bash
find /local/workspace -maxdepth 3 -name "yolov8n.hef"
```

On host (Lenovo):

```bash
ls -lh "$HOME/Documents/AI/hailo/shared_with_docker/yolov8n.hef"
```

Send to Raspberry Pi (note: `/data/` is the correct Pi storage path):

```bash
scp -O "$HOME/Documents/AI/hailo/shared_with_docker/yolov8n.hef" root@10.21.220.191:/data/yolov8n.hef
```

Validate on Raspberry Pi:

```bash
ls -lh /data/yolov8n.hef
hailortcli parse-hef /data/yolov8n.hef
```

**Expected parse-hef output:**
```
Architecture HEF was compiled for: HAILO8
Network group name: yolov8n, Single Context
    Network name: yolov8n/yolov8n
        VStream infos:
            Input  yolov8n/input_layer1 UINT8, NHWC(640x640x3)
            Output yolov8n/yolov8_nms_postprocess FLOAT32, HAILO NMS BY CLASS(number of classes: 80, maximum bounding boxes per class: 100, maximum frame size: 160320)
            Operation:
                Op YOLOV8
                Name: YOLOV8-Post-Process
                Score threshold: 0.200
                IoU threshold: 0.70
```

Automation shortcut (inside container):

```bash
/path/to/SEA-ME_Team6_2025-26/src/setup/scripts/hailo-compile-and-deploy.sh baseline

PI_TARGET=root@10.21.220.191:/data/ \
ONNX_PATH=/local/workspace/shared/models/yolov8n.onnx \
/path/to/SEA-ME_Team6_2025-26/src/setup/scripts/hailo-compile-and-deploy.sh custom
```

### Step 9 — Deploy HEF to Raspberry Pi

Run on host:

```bash
ls -lh "$HOME/Documents/AI/hailo/shared_with_docker/yolov8n.hef"
# or, if you persisted inside models/
ls -lh "$HOME/Documents/AI/hailo/shared_with_docker/models/yolov8n.hef"

# Use -O flag for legacy SSH mode (required for some Pi configurations)
scp -O "$HOME/Documents/AI/hailo/shared_with_docker/yolov8n.hef" root@10.21.220.191:/data/yolov8n.hef
```

Run on Raspberry Pi:

```bash
ls -lh /data/yolov8n.hef
hailortcli parse-hef /data/yolov8n.hef
```

### Step 10 — Run inference with new HEF

Update your runtime pipeline/script to point to `yolov8n.hef` (for example in `demo.py` or GStreamer `hailonet hef-path=...`).

If your `demo.py` currently references `yolov8s.hef`, quick replacement command:

```bash
sed -i 's|HEF_PATH = "/data/yolov8s.hef"|HEF_PATH = "/data/yolov8n.hef"|' /data/demo.py
grep 'HEF_PATH' /data/demo.py  # verify change
python3 /data/demo.py
```

**Expected real-time output on Hailo-8:**
```
=== YOLOv8n Hailo-8 Live Demo ===
[1/3] Loading HEF...
[2/3] Starting camera...
[3/3] Opening display...
Display: saving frames to /data/output/ (no display)
=== Running (Ctrl+C to stop) ===
FPS:30.3 | Infer:12.9ms | Det:0 []
FPS:30.1 | Infer:12.7ms | Det:0 []
```

Performance achieved:
- **FPS:** ~30 (real-time)
- **Inference latency:** 12-14 ms (Hailo-8 hardware acceleration)
- **Output:** Frames saved to `/data/output/` with detections

Use sections `4` through `12` for deeper troubleshooting, alternatives, and automation.

---

## 4) Clean Installation (Official AI Software Suite)

```bash
mkdir -p ~/hailo-dfc
mv ~/Downloads/hailo8_ai_sw_suite_2025-10_docker.zip ~/hailo-dfc
cd ~/hailo-dfc
unzip hailo8_ai_sw_suite_2025-10_docker.zip
# generates: hailo8_ai_sw_suite_2025-10.tar.gz and run script

docker load < hailo8_ai_sw_suite_2025-10.tar.gz
```

If you use the custom image already validated in the project (`hailo_custom_suite:latest`), move to step 5.

---

## 5) Start Container with Mounted Volume

```bash
docker run -it --rm \
  -v "$HOME/Documents/AI/hailo/shared_with_docker:/local/workspace/shared" \
  hailo_custom_suite:latest
```

Inside the container, validate the environment:

```bash
which hailomz
which hailo
hailo --help | head -n 20
hailomz --help | head -n 20
```

---

## 6) Prepare Calibration Dataset

If `calibration_images` is empty, populate it with a COCO subset:

```bash
cd "$HOME/Documents/AI/hailo/shared_with_docker"
mkdir -p calibration_images

find COCO/val2017 -type f \( -iname "*.jpg" -o -iname "*.jpeg" -o -iname "*.png" \) \
  | head -n 256 \
  | xargs -I{} cp "{}" calibration_images/

find calibration_images -type f | wc -l
```

Expected: `> 0` (in this flow, `256`).

---

## 7) Baseline Compile (Recommended First Test)

> This command compiles the default Model Zoo checkpoint (`yolov8n`) and validates the full pipeline.

```bash
set -e
find /local/workspace/shared/calibration_images -type f | wc -l

hailomz compile yolov8n \
  --hw-arch hailo8 \
  --calib-path /local/workspace/shared/calibration_images \
  2>&1 | tee /local/workspace/shared/logs/compile_baseline.log
```

### Expected Result
- Parse/optimization/allocation/compilation logs
- Final message: `HEF file written to yolov8n.hef`

### Important Note
`yolov8n.hef` is generated in `/local/workspace` (ephemeral). Before exiting the container:

```bash
cp /local/workspace/yolov8n.hef /local/workspace/shared/models/
ls -lh /local/workspace/shared/models/yolov8n.hef
```

---

## 8) Custom Compile with Your ONNX

> Use this command when you want to compile a specific ONNX file.

```bash
hailomz compile yolov8n \
  --ckpt /local/workspace/shared/models/yolov8n.onnx \
  --hw-arch hailo8 \
  --calib-path /local/workspace/shared/calibration_images \
  2>&1 | tee /local/workspace/shared/logs/compile_custom.log
```

### Difference Between Baseline and Custom
- Without `--ckpt`: uses the official internal Model Zoo checkpoint.
- With `--ckpt`: uses your specified local ONNX.

---

## 9) Common Errors and Quick Fixes

### `cfg file is missing in ...yaml`
Cause: using `--yaml` with a custom file that does not match the format expected by `hailomz compile`.

Solution:
- Prefer `hailomz compile yolov8n` (baseline), or
- Use `--ckpt` with ONNX without forcing a custom YAML.

### `calibration_images: No such file or directory`
Cause: incorrect volume mount.

Solution: confirm `-v ...:/local/workspace/shared` and the internal path.

### `calibration_images` is empty
Cause: folder created without images.

Solution: copy a subset from `COCO/val2017` (step 6).

### `expected conv but found concat/activation`
Cause: custom/simplified ONNX incompatible with the Model Zoo YOLOv8n recipe.

Solution:
- Validate baseline first.
- Re-export ONNX in a compatible format (without aggressive simplifications/embedded NMS) when needed.

---

## 10) Deploy to Raspberry Pi (Real Hailo)

On the host:

```bash
ls -lh "$HOME/Documents/AI/hailo/shared_with_docker/yolov8n.hef"
ls -lh "$HOME/Documents/AI/hailo/shared_with_docker/models/yolov8n.hef"

scp "$HOME/Documents/AI/hailo/shared_with_docker/yolov8n.hef" root@10.21.220.191:/root/models/
# or:
scp "$HOME/Documents/AI/hailo/shared_with_docker/models/yolov8n.hef" root@10.21.220.191:/root/models/
```

On the Pi:

```bash
hailortcli parse-hef /root/models/yolov8n.hef
```

Then, in `demo.py`, replace `yolov8s.hef` with `yolov8n.hef`.

---

## 11) Recommended Model Order (Sprint)

To reduce risk and produce a progressive benchmark:
1. `yolov8n`
2. `yolov8s`
3. `yolov8m`

Initial goal: validate pipeline and runtime before increasing model complexity.

Recommended practical rationale:

| Model | Size | Typical Difficulty | Typical FPS (target-dependent) |
|---|---|---|---|
| `yolov8n` | small | easy | ~250 FPS |
| `yolov8s` | medium | medium | ~160 FPS |
| `yolov8m` | larger | higher | ~90 FPS |

For `Hailo Runtime Integration on AGL`, what matters most is:
- working pipeline,
- runtime validation,
- consistent benchmark collection.

That is why starting with `yolov8n` is recommended.

---

## 12) Automation (Repeatable)

Team-ready option (already versioned in this repository):

```bash
chmod +x "/path/to/SEA-ME_Team6_2025-26/src/setup/scripts/hailo-compile-and-deploy.sh"

# run inside container
/path/to/SEA-ME_Team6_2025-26/src/setup/scripts/hailo-compile-and-deploy.sh baseline

# custom compile
ONNX_PATH=/local/workspace/shared/models/yolov8n.onnx \
/path/to/SEA-ME_Team6_2025-26/src/setup/scripts/hailo-compile-and-deploy.sh custom

# custom + deploy
PI_TARGET=root@10.21.220.191:/root/models/ \
ONNX_PATH=/local/workspace/shared/models/yolov8n.onnx \
/path/to/SEA-ME_Team6_2025-26/src/setup/scripts/hailo-compile-and-deploy.sh custom
```

Below is an automation script that:
- ensures a minimum calibration dataset,
- runs baseline/custom compile,
- persists `HEF` in the mounted volume,
- optionally sends it to Raspberry Pi.

Create `shared_with_docker/scripts/hailo_compile_and_deploy.sh`:

```bash
#!/usr/bin/env bash
set -euo pipefail

MODE="${1:-baseline}"   # baseline | custom
HW_ARCH="${HW_ARCH:-hailo8}"
SHARED="/local/workspace/shared"
CALIB_DIR="$SHARED/calibration_images"
COCO_DIR="$SHARED/COCO/val2017"
ONNX_PATH="${ONNX_PATH:-$SHARED/models/yolov8n.onnx}"
PI_TARGET="${PI_TARGET:-}"   # ex: root@10.21.220.191:/root/models/

mkdir -p "$CALIB_DIR"
if [ "$(find "$CALIB_DIR" -type f | wc -l)" -eq 0 ]; then
  echo "[INFO] calibration_images is empty, copying 256 COCO images..."
  find "$COCO_DIR" -type f \( -iname "*.jpg" -o -iname "*.jpeg" -o -iname "*.png" \) \
    | head -n 256 \
    | xargs -I{} cp "{}" "$CALIB_DIR"/
fi

echo "[INFO] Calibration images: $(find "$CALIB_DIR" -type f | wc -l)"

if [ "$MODE" = "baseline" ]; then
  hailomz compile yolov8n \
    --hw-arch "$HW_ARCH" \
    --calib-path "$CALIB_DIR" \
    2>&1 | tee "$SHARED/logs/compile_baseline.log"
else
  hailomz compile yolov8n \
    --ckpt "$ONNX_PATH" \
    --hw-arch "$HW_ARCH" \
    --calib-path "$CALIB_DIR" \
    2>&1 | tee "$SHARED/logs/compile_custom.log"
fi

mkdir -p "$SHARED/models"
cp /local/workspace/yolov8n.hef "$SHARED/models/"
ls -lh "$SHARED/models/yolov8n.hef"

# keep a flat copy in shared root as well (matches quick deploy flow)
cp /local/workspace/yolov8n.hef "$SHARED/"
ls -lh "$SHARED/yolov8n.hef"

if [ -n "$PI_TARGET" ]; then
  echo "[INFO] Sending HEF to $PI_TARGET"
  scp "$SHARED/yolov8n.hef" "$PI_TARGET"
fi

echo "[OK] Process completed"
```

Inside the container:

```bash
chmod +x /local/workspace/shared/scripts/hailo_compile_and_deploy.sh

# baseline
/local/workspace/shared/scripts/hailo_compile_and_deploy.sh baseline

# custom
ONNX_PATH=/local/workspace/shared/models/yolov8n.onnx \
/local/workspace/shared/scripts/hailo_compile_and_deploy.sh custom

# custom + deploy
PI_TARGET=root@10.21.220.191:/root/models/ \
ONNX_PATH=/local/workspace/shared/models/yolov8n.onnx \
/local/workspace/shared/scripts/hailo_compile_and_deploy.sh custom
```

---

## 13) Future Integration in the ADAS Stack

Recommended flow for project evolution:

`libcamera -> GStreamer -> Hailo inference -> detections -> KUKSA -> Qt Cluster`

Starting with `yolov8n` accelerates integration and initial benchmarking.

---

## 14) Final Checklist

- [ ] `calibration_images` contains files (`wc -l > 0`)
- [ ] baseline compile completed successfully
- [ ] `yolov8n.hef` copied to `shared_with_docker/models`
- [ ] `scp` to Pi completed
- [ ] `hailortcli parse-hef` on Pi completed
- [ ] `demo.py` updated to `yolov8n.hef`

---

## 15) Important Team Notes

- Avoid mixing old tutorials (manual-only) with the official AI Software Suite flow unless strictly needed.
- Prefer reproducible commands with a single mount to reduce path errors.
- Always save logs (`tee`) for fast troubleshooting.

---

## 16) Appendix: Legacy Paths and Rationale

This appendix preserves historical context from previous iterations without polluting the main, reproducible workflow.

### A) Why Manual Installation Was Not Kept as Main Flow

The manual path (`hailo_dataflow_compiler-*.whl` + `hailort_*.deb`) can work for specific cases, but in this project it created frequent setup drift and missing SDK internals for Model Zoo automation (for example `versions.py`-related issues in prior attempts).

The main flow therefore standardizes on the official AI Software Suite container, because it is:
- more reproducible across machines,
- closer to vendor-supported structure,
- faster to onboard teammates,
- less error-prone for Model Zoo operations.

### B) Docker Permission Error (`docker.sock`)

If this appears on host:

`permission denied while trying to connect to the docker API at unix:///var/run/docker.sock`

Use one of the following:

```bash
sudo usermod -aG docker $USER
newgrp docker
docker run hello-world
```

Or re-login (logout/login) after adding the user to the `docker` group.

### C) Historical Contradiction Resolved

Earlier notes stated that HEF generation required local Hailo PCIe. In this project, practical validation showed that `HEF` compilation succeeded on Lenovo/Docker with the correct DFC/Model Zoo environment.

Final position for this tutorial:
- `HEF` compilation can run in host/container with proper toolchain.
- accelerated inference with that `HEF` still requires real Hailo hardware on target runtime.

### D) What to Keep vs What to Archive

Keep in main tutorial:
- single validated workflow,
- baseline/custom compile commands,
- calibration prep,
- deploy and validation commands,
- automation script.

Archive as historical notes only:
- duplicate Dockerfiles from early experiments,
- mixed manual/automatic setup instructions in the same linear flow,
- contradictory statements disproven by validated runs.

### E) Why the Initial colega Tutorial Could Fail (and Why This Took Longer)

This section is meant for technical justification and peer alignment, not blame.

Potential failure points in the initial flow for this environment:
- **Path and mount ambiguity**: host paths and container paths were not consistently standardized, causing missing files (`calibration_images`, custom ONNX, output artifacts).
- **Manual-vs-suite overlap**: combining manual SDK installation and official suite steps in one sequence introduced conflicting assumptions about available binaries and internal SDK files.
- **Model Zoo config mismatch risk**: custom YAML and custom ONNX were mixed into commands expecting Model Zoo-native structure, leading to errors like missing cfg expectations.
- **Calibration dataset readiness**: compilation depends on real calibration data; empty or wrong folders caused hard stops even when DFC was correctly installed.
- **Output persistence gap**: generated `HEF` initially landed in ephemeral container paths, so successful compiles could still be “lost” unless copied to mounted storage.

Why implementation took longer than expected:
- multiple iterations were needed to separate environment issues from model-graph issues,
- several errors were sequentially dependent (fixing one exposed the next),
- contradictory assumptions from legacy notes required re-validation with real logs,
- the final workflow had to be proven end-to-end (compile, persist, deploy, parse on target), not just compile once.

Summary argument for project communication:
- The delay was mainly due to **integration and reproducibility hardening**, not a single command mistake.
- The current tutorial reduces rework by codifying the exact validated path and explicitly isolating legacy pitfalls.

### F) Why João 2.0 Also Failed Before Final Success

João 2.0 improved the initial flow, but it still failed at first because execution combined valid steps with incompatible assumptions.

Primary root causes:
- **Mixed execution modes**: manual SDK path and official suite path were both present, causing environment drift.
- **Host/container path divergence**: files existed on host but not in the mounted container path at runtime.
- **Calibration readiness issues**: `calibration_images` existed but was empty during compile attempts.
- **Config-model mismatch**: custom YAML and custom ONNX were used with commands expecting Model Zoo-native structure.
- **Graph compatibility issues**: simplified/custom ONNX produced output layers incompatible with expected YOLOv8 recipe stages.
- **Artifact persistence oversight**: successful `HEF` output initially remained in ephemeral container storage.

What finally unlocked success:
- single standardized execution path,
- single mounted workspace path,
- populated calibration dataset,
- baseline compile first, then custom compile,
- explicit copy of generated `HEF` to mounted storage before container exit.

Communication-ready summary:
- João 2.0 did not fail because the approach was wrong in principle.
- It failed initially due to integration friction across environment, paths, data readiness, and model/config alignment.

---

## 17) Legacy Reference: colega Initial Tutorial

> This section preserves the initial colega workflow as a historical reference.
> It is not the recommended main path for this project, but it is kept for traceability and team context.

### Scope

- Docker-based DFC setup on Linux x86_64 host.
- Build and run a custom container.
- Install Model Zoo and compile YOLOv8 model.
- Deploy generated `HEF` to Raspberry Pi.

### Step 1 — Docker setup on Ubuntu host

```bash
sudo apt-get update
sudo apt-get install -y ca-certificates curl gnupg

sudo install -m 0755 -d /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
sudo chmod a+r /etc/apt/keyrings/docker.gpg

echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] \
  https://download.docker.com/linux/ubuntu $(. /etc/os-release && echo \"$VERSION_CODENAME\") stable" | \
  sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

sudo apt-get update
sudo apt-get install -y docker-ce docker-ce-cli containerd.io docker-compose-plugin

sudo usermod -aG docker $USER
```

Validation:

```bash
docker --version
docker run hello-world
```

If `docker.sock` permission error appears, run:

```bash
newgrp docker
docker run hello-world
```

### Step 2 — Download Hailo artifacts

From `https://hailo.ai/developer-zone`, download:

- `hailo_dataflow_compiler-<version>-py3-none-linux_x86_64.whl`
- `hailort_<version>_amd64.deb`

Copy into working folder:

```bash
mkdir -p ~/hailo-dfc && cd ~/hailo-dfc
ls -la
```

### Step 3 — Create Dockerfile (manual path)

```bash
cd ~/hailo-dfc
cat > Dockerfile << 'EOF'
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    python3.10 python3.10-dev python3.10-venv \
    python3-pip python3-tk \
    libfuse2 graphviz libgraphviz-dev \
    libgstreamer1.0-0 \
    libgstreamer-plugins-base1.0-0 \
    git wget \
    && rm -rf /var/lib/apt/lists/*

RUN python3 -m pip install --upgrade pip setuptools wheel

WORKDIR /app

RUN python3.10 -m venv /venv
ENV VIRTUAL_ENV=/venv
ENV PATH="/venv/bin:/opt/hailo/usr/bin:$PATH"

COPY hailo_dataflow_compiler-*.whl /tmp/
RUN pip install /tmp/hailo_dataflow_compiler-*.whl && rm /tmp/*.whl

COPY hailort_*_amd64.deb /opt/hailo/

CMD ["/bin/bash"]
EOF
```

### Step 4 — Create docker-compose

```bash
cat > docker-compose.yaml << 'EOF'
version: '3.9'
services:
  dfc:
    image: hailo-dfc:latest
    build:
      context: .
      dockerfile: Dockerfile
    volumes:
      - ./workspace:/app/workspace
    stdin_open: true
    tty: true
EOF

mkdir -p workspace
```

### Step 5 — Build and enter container

```bash
cd ~/hailo-dfc
docker compose build
docker compose run dfc bash
```

Validate:

```bash
hailo --version
python3 -c "from hailo_sdk_client import ClientRunner; print('DFC OK')"
```

### Step 6 — Install Hailo Model Zoo

```bash
cd /app/workspace
git clone --branch v2.14.0 https://github.com/hailo-ai/hailo_model_zoo.git
cd hailo_model_zoo
pip install -e .

hailomz --help
hailomz info yolov8m
```

### Step 7 — Export YOLOv8 ONNX on host

```bash
pip install ultralytics
python3 -c "
from ultralytics import YOLO
model = YOLO('yolov8m.pt')
model.export(format='onnx', imgsz=640, opset=11)
"

cp yolov8m.onnx ~/hailo-dfc/workspace/
```

### Step 8 — Prepare calibration images

```bash
mkdir -p ~/hailo-dfc/workspace/calib
# Copy 500-1000 representative images into this folder
```

### Step 9 — Compile model

```bash
docker compose run dfc bash
cd /app/workspace/hailo_model_zoo

hailomz compile yolov8m \
    --ckpt /app/workspace/yolov8m.onnx \
    --hw-arch hailo8 \
    --calib-path /app/workspace/calib/ \
    --classes 80 \
    --performance
```

### Step 10 — Deploy to Raspberry Pi

```bash
scp ~/hailo-dfc/workspace/*.hef pi@10.21.220.192:/home/pi/models/
```

On Pi:

```bash
hailortcli run /home/pi/models/yolov8m.hef
```

### Step 11 — Useful commands

```bash
cd ~/hailo-dfc && docker compose run dfc bash
hailomz info <model>
hailomz profile <model> --hw-arch hailo8
hailomz eval <model> --hw-arch hailo8 --target emulator
ls hailo_model_zoo/cfg/networks/
```

### Step 12 — Legacy troubleshooting notes

- `ModuleNotFoundError: hailo_sdk_client`: venv/container mismatch.
- `FileNotFoundError: versions.py`: common when mixing manual SDK installs with Model Zoo assumptions.
- long compile times on medium/large models.
- calibration quality and quantity heavily affect final quality.

### Historical status note

This tutorial was a valid technical starting point. In this project, it was superseded by the validated AI Software Suite flow in sections `1-16` to improve reproducibility and reduce setup drift.
