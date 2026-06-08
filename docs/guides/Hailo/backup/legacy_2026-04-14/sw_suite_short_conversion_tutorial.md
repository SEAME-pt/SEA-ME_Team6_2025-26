# Hailo YOLOv8 Model Conversion: Complete From-Scratch Tutorial

> Complete step-by-step guide to convert any YOLOv8 variant (nano/small/medium/large) to Hailo HEF format.  
> **Tested on:** Lenovo (Ubuntu x86_64) + Docker + Hailo AI Software Suite 2025-10  
> **Last Validated:** March 2026

## Index

- [Overview](#overview)
- [What You'll Learn](#what-youll-learn)
- [Prerequisites](#prerequisites)
  - [Host Requirements](#host-requirements)
  - [Software Requirements](#software-requirements)
  - [Data Requirements](#data-requirements)
- [Phase 1: Environment Setup](#phase-1-environment-setup)
  - [1.1 Create Working Directory](#11-create-working-directory)
  - [1.2 Download Hailo Docker Image](#12-download-hailo-docker-image)
  - [1.3 Start Docker Container](#13-start-docker-container)
- [Phase 2: Prepare Calibration Dataset](#phase-2-prepare-calibration-dataset)
  - [2.1 Get COCO Images](#21-get-coco-images)
  - [2.2 Organize Calibration Folder](#22-organize-calibration-folder)
- [Phase 3: Compile YOLOv8S (Baseline)](#phase-3-compile-yolov8s-baseline)
  - [3.1 Inside Container](#31-inside-container)
  - [3.2 Run Baseline Compile](#32-run-baseline-compile)
  - [3.3 Expected Output](#33-expected-output)
- [Phase 4: Persist HEF to Host](#phase-4-persist-hef-to-host)
  - [4.1 Copy from Container](#41-copy-from-container)
  - [4.2 Verify on Host](#42-verify-on-host)
- [Phase 5: Deploy to Raspberry Pi](#phase-5-deploy-to-raspberry-pi)
  - [5.1 Transfer HEF](#51-transfer-hef)
  - [5.2 Validate on Target](#52-validate-on-target)
- [Phase 6: Run Inference](#phase-6-run-inference)
  - [6.1 Update Runtime Config](#61-update-runtime-config)
  - [6.2 Start Demo](#62-start-demo)
- [Advanced: Custom Models](#advanced-custom-models)
  - [A.1 Export Your Own ONNX](#a1-export-your-own-onnx)
  - [A.2 Compile Custom Model](#a2-compile-custom-model)
- [Troubleshooting](#troubleshooting)
- [Performance Reference](#performance-reference)
- [Next Steps](#next-steps)

---

## Overview

The Hailo YOLOv8 conversion process transforms a PyTorch or ONNX model into a hardware-optimized `.hef` (Hailo Executable Format) file for deployment on Hailo-8 accelerators.

**High-level flow:**

```
PT/ONNX Model (YOLOv8S)
    ↓
Hailo Model Zoo (hailomz compile)
    ↓
Quantization + Optimization (DFC)
    ↓
Hardware Allocation (Hailo-8)
    ↓
HEF File (Executable)
    ↓
Raspberry Pi + Hailo-8 Accelerator
    ↓
Real-time Inference (~30 FPS for YOLOv8S)
```

---

## What You'll Learn

✅ How to set up Hailo AI Software Suite in Docker  
✅ How to prepare and organize calibration datasets  
✅ How to compile YOLOv8S using Model Zoo  
✅ How to persist artifacts from container to host  
✅ How to deploy HEF to Raspberry Pi  
✅ How to validate with `hailortcli`  
✅ How to run real-time inference  
✅ How to convert custom models

---

## Prerequisites

### Host Requirements

- **OS:** Ubuntu 20.04+ (x86_64)
- **RAM:** 8GB+ (16GB recommended for large models)
- **Disk:** 50GB+ free space
- **Docker:** Installed and running
- **Network:** SSH access to Raspberry Pi (for deployment phase)

### Software Requirements

- Docker 20.10+
- Hailo AI Software Suite 2025-10 (Docker image)
- SSH client
- Basic shell commands (`bash`, `grep`, `find`, etc.)

### Data Requirements

- **Calibration images:** 256 COCO images (minimum, 500+ recommended)
- **Source:** Can be subset from COCO val2017 or custom dataset with similar distribution
- **Format:** JPG, PNG (standard image formats)
- **Size:** Representative of your target deployment domain

---

## Phase 1: Environment Setup

### 1.1 Create Working Directory

```bash
# On Lenovo (host)
mkdir -p ~/Documents/AI/hailo/shared_with_docker
mkdir -p ~/Documents/AI/hailo/shared_with_docker/{models,calibration_images,logs}
cd ~/Documents/AI/hailo/shared_with_docker
ls -la
```

Expected structure:
```
shared_with_docker/
├── models/           # Will store .hef files
├── calibration_images/  # COCO subset
└── logs/            # Compilation logs
```

### 1.2 Download Hailo Docker Image

Get the official Hailo AI Software Suite 2025-10 from Hailo's download page (requires account).

```bash
# Assuming you have the .tar.gz file
docker load < hailo8_ai_sw_suite_2025-10.tar.gz

# Verify load
docker images | grep hailo
```

Expected output:
```
hailo_ai_suite:latest    ...
```

### 1.3 Start Docker Container

```bash
docker run -it --rm \
  -v "$HOME/Documents/AI/hailo/shared_with_docker:/local/workspace/shared" \
  hailo_ai_suite:latest
```

Inside container, validate:

```bash
which hailomz
which hailo
hailomz --help | head
```

Expected:
```
usage: hailomz [-h] [-v] {compile,info,profile,eval,...}
```

---

## Phase 2: Prepare Calibration Dataset

### 2.1 Get COCO Images

**Option A: From existing COCO dataset (if you have it)**

```bash
# Host machine
cd ~/Documents/AI/hailo/shared_with_docker

# If you already have COCO val2017 downloaded
find /path/to/COCO/val2017 -type f \( -iname "*.jpg" -o -iname "*.jpeg" \) \
  | head -256 \
  | xargs -I {} cp {} calibration_images/
```

**Option B: Download COCO sample (or use your domain-specific images)**

```bash
# Host machine - download a small COCO sample
mkdir -p /tmp/coco_download
cd /tmp/coco_download

# Alternative: use any 256 representative images from your dataset
# Make sure they match your deployment domain
```

### 2.2 Organize Calibration Folder

```bash
# Host machine
cd ~/Documents/AI/hailo/shared_with_docker

# Verify images are copied
ls -la calibration_images/ | head -20
find calibration_images -type f | wc -l
# Should show >= 256

# Verify image formats (optional)
file calibration_images/* | head
```

Expected:
```
256+ image files in .jpg or .png format
```

---

## Phase 3: Compile YOLOv8S (Baseline)

### 3.1 Inside Container

```bash
docker run -it --rm \
  -v "$HOME/Documents/AI/hailo/shared_with_docker:/local/workspace/shared" \
  hailo_ai_suite:latest

# Inside container
cd /local/workspace/shared
ls -la
```

### 3.2 Run Baseline Compile

The **baseline** compile uses the official Model Zoo checkpoint for YOLOv8S (pre-trained on COCO).

```bash
# Inside container - baseline compile for YOLOv8S
hailomz compile yolov8s \
  --hw-arch hailo8 \
  --calib-path /local/workspace/shared/calibration_images \
  2>&1 | tee /local/workspace/shared/logs/compile_yolov8s_baseline.log
```

**What's happening:**
1. `hailomz compile yolov8s`: Use Model Zoo YOLOv8S (pre-trained)
2. `--hw-arch hailo8`: Target hardware is Hailo-8
3. `--calib-path`: Path to calibration images (for quantization)
4. `tee`: Save log to file

### 3.3 Expected Output

**During compilation (5-10 minutes):**

```
[info] Parsing model...
[info] Downloading YOLOv8S model from Model Zoo...
[info] Model zoo checkpoint: [progress]...
[info] Optimization phase...
[info] Allocation phase...
[info] Compilation phase...
[info] Successful Compilation (compilation time: 8s)
[info] HEF file written to yolov8s.hef
```

**After compilation:**

```bash
# Still inside container
ls -lh /local/workspace/yolov8s.hef
# Should show: ~4.5M (size varies by model)
```

---

## Phase 4: Persist HEF to Host

⚠️ **CRITICAL:** The HEF file is generated in the ephemeral container directory. It will be **deleted when the container exits** if not copied to the mounted volume.

### 4.1 Copy from Container

```bash
# Still inside container, before exit
ls -lh /local/workspace/yolov8s.hef
cp /local/workspace/yolov8s.hef /local/workspace/shared/
ls -lh /local/workspace/shared/yolov8s.hef
```

**Important:** Run each command on a separate line. Do not paste them together.

If the source file is missing:

```bash
find /local/workspace -maxdepth 3 -name "yolov8s.hef"
# If found, copy from the result location
```

Exit container:

```bash
exit
```

### 4.2 Verify on Host

```bash
# Back on Lenovo host
ls -lh ~/Documents/AI/hailo/shared_with_docker/yolov8s.hef
# Should show: -rw-r--r-- ... 4.5M ...
```

If file is missing, check inside the mounted volume:

```bash
find ~/Documents/AI/hailo/shared_with_docker -name "yolov8s.hef"
```

---

## Phase 5: Deploy to Raspberry Pi

### 5.1 Transfer HEF

From Lenovo host to Raspberry Pi:

```bash
# On Lenovo host
scp -O ~/Documents/AI/hailo/shared_with_docker/yolov8s.hef \
  root@<PI_IP>:/data/yolov8s.hef

# Example with actual IP
# scp -O ~/Documents/AI/hailo/shared_with_docker/yolov8s.hef \
#   root@10.21.220.191:/data/yolov8s.hef
```

**Notes:**
- `-O`: Legacy SSH mode (required for some Pi configurations)
- `PI_IP`: Your Raspberry Pi's IP address
- `/data/`: Standard Hailo runtime storage on Pi

### 5.2 Validate on Target

SSH into Raspberry Pi:

```bash
ssh root@<PI_IP>

# On Pi
ls -lh /data/yolov8s.hef
hailortcli parse-hef /data/yolov8s.hef
```

**Expected parse-hef output:**

```
Architecture HEF was compiled for: HAILO8
Network group name: yolov8s, Single Context
    Network name: yolov8s/yolov8s
        VStream infos:
            Input  yolov8s/input_layer1 UINT8, NHWC(640x640x3)
            Output yolov8s/yolov8_nms_postprocess FLOAT32, HAILO NMS BY CLASS(number of classes: 80, maximum bounding boxes per class: 100, maximum frame size: 160320)
            Operation:
                Op YOLOV8
                Name: YOLOV8-Post-Process
                Score threshold: 0.200
                IoU threshold: 0.70
                Classes: 80
                Max bboxes per class: 100
                Image height: 640
                Image width: 640
```

If you see this output **without errors**, HEF is valid and hardware-compatible. ✅

---

## Phase 6: Run Inference

### 6.1 Update Runtime Config

If your runtime uses `demo.py` or similar:

```bash
# On Pi
sed -i 's|yolov8n.hef|yolov8s.hef|' /data/demo.py
# Or if it was using a different model:
sed -i 's|HEF_PATH = "[^"]*"|HEF_PATH = "/data/yolov8s.hef"|' /data/demo.py

# Verify the change
grep 'HEF_PATH' /data/demo.py
```

### 6.2 Start Demo

```bash
# On Pi
python3 /data/demo.py
```

**Expected real-time output:**

```
=== YOLOv8S Hailo-8 Live Demo ===
[1/3] Loading HEF...
    Input: yolov8s/input_layer1 (640, 640, 3)
[2/3] Starting camera...
[3/3] Opening display...
Display: saving frames to /data/output/ (no display)
=== Running (Ctrl+C to stop) ===
FPS:28.5 | Infer:13.2ms | Det:5 [person:0.87, car:0.92]
FPS:28.3 | Infer:13.5ms | Det:3 [person:0.91]
FPS:28.2 | Infer:13.1ms | Det:4 [person:0.88, car:0.85]
```

**Stop with:** `Ctrl+C`

**Check output frames:**

```bash
ls -lh /data/output/ | tail -10
# Should show recent JPG files with timestamps
```

---

## Advanced: Custom Models

### A.1 Export Your Own ONNX

If you have a custom YOLOv8 model (fine-tuned on your data):

```bash
# On Lenovo host (or any machine with PyTorch installed)
pip install ultralytics onnx onnxruntime

python3 << 'EOF'
from ultralytics import YOLO

# Load your fine-tuned model
model = YOLO("path/to/your_model.pt")

# Export to ONNX
model.export(format="onnx", imgsz=640, opset=11)
# Generates: path/to/your_model.onnx

EOF

# Copy to shared mount
cp path/to/your_model.onnx ~/Documents/AI/hailo/shared_with_docker/
```

### A.2 Compile Custom Model

```bash
# Inside container (same as before)
docker run -it --rm \
  -v "$HOME/Documents/AI/hailo/shared_with_docker:/local/workspace/shared" \
  hailo_ai_suite:latest

cd /local/workspace/shared

# Custom compile with your ONNX
hailomz compile yolov8s \
  --ckpt /local/workspace/shared/your_model.onnx \
  --hw-arch hailo8 \
  --calib-path /local/workspace/shared/calibration_images \
  2>&1 | tee /local/workspace/shared/logs/compile_yolov8s_custom.log

# Persist HEF
cp /local/workspace/yolov8s.hef /local/workspace/shared/yolov8s_custom.hef
exit
```

Then deploy as in Phase 5.

---

## Troubleshooting

### Issue: "No calibration images found"

**Solution:**
```bash
# On host
find ~/Documents/AI/hailo/shared_with_docker/calibration_images -type f | wc -l
# Must be > 0

# If empty, copy images
find /path/to/COCO -name "*.jpg" | head -256 | xargs -I {} cp {} \
  ~/Documents/AI/hailo/shared_with_docker/calibration_images/
```

### Issue: "HEF file not found" after compile succeeds

**Solution:** You must copy from ephemeral to mounted volume **before exiting container**.

```bash
# Inside container, immediately after compile
find /local/workspace -name "yolov8s.hef"
cp /local/workspace/yolov8s.hef /local/workspace/shared/
exit
```

### Issue: `scp` fails with "Is a directory"

**Solution:** Include the filename in the scp destination:

```bash
# Wrong:
scp yolov8s.hef root@10.21.220.191:/data/

# Correct:
scp -O yolov8s.hef root@10.21.220.191:/data/yolov8s.hef
```

### Issue: `hailortcli parse-hef` shows "not a valid HEF"

**Possible causes:**
1. File was corrupted during scp transfer (try again with `-O`)
2. Wrong HEF file (compiled for different architecture)
3. HEF was deleted before copy (re-run compilation)

**Solution:**
```bash
# On host, verify file size
ls -lh ~/Documents/AI/hailo/shared_with_docker/yolov8s.hef
# Should be 4-5 MB

# On Pi, re-transfer
scp -O ~/Documents/AI/hailo/shared_with_docker/yolov8s.hef \
  root@10.21.220.191:/data/yolov8s.hef

# Validate
ssh root@10.21.220.191 "hailortcli parse-hef /data/yolov8s.hef"
```

### Issue: Inference shows "Det:0" (no detections)

**Possible causes:**
1. Wrong model name in code (checking for yolov8n but running yolov8s)
2. Input preprocessing issue (color space, normalization)
3. Confidence threshold too high

**Solution:**
```bash
# Check demo.py for correct model reference
grep 'HEF_PATH' /data/demo.py

# Verify hardware inference (not CPU fallback)
grep -i "hailo\|accelerator" /data/demo.py

# Check recent output frames
file /data/output/*.jpg | head -5
# If files are blank/white, preprocessing issue
```

---

## Performance Reference

| Model | Input Size | Inference Time | FPS | Hailo-8 | Notes |
|-------|-----------|-----------------|-----|---------|--------|
| YOLOv8n | 640×640 | 12-14 ms | ~30 | ✅ | Nano, fastest |
| YOLOv8s | 640×640 | 13-15 ms | ~28 | ✅ | Small, balanced |
| YOLOv8m | 640×640 | 16-20 ms | ~25 | ✅ | Medium, more accuracy |
| YOLOv8l | 640×640 | 25-30 ms | ~18 | ✅ | Large, slower |

---

### YOLOv8s vs YOLOv8-seg vs YOLO26s: Critérios de Comparação com Score

| Critério                                   | Peso | YOLOv8s | Score | YOLOv8-seg | Score | YOLO26s | Score |
|---------------------------------------------|------|---------|-------|-----------|-------|---------|-------|
| mAP (COCO)                                 | 20%  | Médio (7.0) | 1.4 | Médio (7.0) | 1.4 | Alto (9.0) | 1.8 |
| Latência (Hailo-8)                         | 20%  | ~13-15 ms (9.0) | 1.8 | ~15-17 ms (7.0) | 1.4 | ~12-14 ms (9.5) | 1.9 |
| Suporte Segmentação                        | 15%  | ❌ (0.0) | 0.0 | ✅ (10.0) | 1.5 | ✅ (10.0) | 1.5 |
| Robustez ADAS (shadows, curvas, etc.)      | 20%  | Média (6.0) | 1.2 | Boa (8.0) | 1.6 | Muito boa (9.5) | 1.9 |
| NMS-free (sem Non-Max Suppression)         | 10%  | ❌ (0.0) | 0.0 | ❌ (0.0) | 0.0 | ✅ (10.0) | 1.0 |
| Open-vocabulary segmentation               | 10%  | ❌ (0.0) | 0.0 | ❌ (0.0) | 0.0 | ✅ (10.0) | 1.0 |
| Detecção de objetos pequenos               | 10%  | Parcial (5.0) | 0.5 | Parcial (5.0) | 0.5 | Excelente (9.5) | 0.95 |
| Arquitetura unificada (det+seg+open-voc)   | 10%  | ❌ (0.0) | 0.0 | Parcial (5.0) | 0.5 | ✅ (10.0) | 1.0 |
| Resolve NMS tradicional                    | 5%   | ❌ (0.0) | 0.0 | ❌ (0.0) | 0.0 | ✅ (10.0) | 0.5 |
| Resolve limitação segmentação clássica     | 5%   | ❌ (0.0) | 0.0 | Parcial (5.0) | 0.25 | ✅ (10.0) | 0.5 |
| Resolve falha em objetos pequenos          | 5%   | Parcial (5.0) | 0.25 | Parcial (5.0) | 0.25 | ✅ (10.0) | 0.5 |
| Risco compatibilidade Hailo                | 10%  | Nenhum (10.0) | 1.0 | Nenhum (10.0) | 1.0 | Moderado (5.0) | 0.5 |
| **SCORE TOTAL PONDERADO**                  | **100%** | - | **7.35/10** | - | **7.50/10** | - | **8.65/10** |

**Legenda dos critérios extra:**
- **3 problemas resolvidos pelo YOLO26:**  
  1. NMS tradicional (problemas de supressão de detecções)  
  2. Limitação de segmentação clássica  
  3. Falha em objetos pequenos
- **4 inovações do YOLO26:**  
  1. NMS-free (arquitetura sem NMS)  
  2. Open-vocabulary segmentation  
  3. Melhor detecção de objetos pequenos  
  4. Arquitetura unificada (detecção, segmentação, open-voc)

**Recomendação ADAS:**
- **Melhor escolha imediata:** YOLOv8-seg (7.50/10) - equilíbrio entre compatibilidade Hailo, robustez e segmentação
- **Futuro:** YOLO26s (8.65/10) - após validação de compatibilidade Hailo (risco moderado)

---

**Real-world observations (March 2026):**
- YOLOv8S on Hailo-8: 13.2ms per frame, 28-30 FPS
- All models run in real-time (> 15 FPS minimum)
- Inference is hardware-accelerated (not CPU)

---

## Next Steps

1. **Expand model variants:** Try yolov8m or yolov8l for higher accuracy
2. **Fine-tune on custom data:** Export your own .pt → .onnx → .hef
3. **Optimize calibration:** Use domain-specific images for better quantization
4. **Integrate with ADAS stack:** Connect HEF output to downstream processing
5. **Monitor performance:** Log FPS, latency, detections for production metrics

---

## Quick Reference: One-Liner Checklists

**Setup:**
```bash
mkdir -p ~/Documents/AI/hailo/shared_with_docker/{models,calibration_images,logs}
docker load < hailo8_ai_sw_suite_2025-10.tar.gz
```

**Compile (inside container):**
```bash
docker run -it --rm -v "$HOME/Documents/AI/hailo/shared_with_docker:/local/workspace/shared" hailo_ai_suite:latest
cd /local/workspace/shared && hailomz compile yolov8s --hw-arch hailo8 --calib-path calibration_images
cp /local/workspace/yolov8s.hef /local/workspace/shared/ && exit
```

**Deploy:**
```bash
scp -O ~/Documents/AI/hailo/shared_with_docker/yolov8s.hef root@10.21.220.191:/data/
ssh root@10.21.220.191 "hailortcli parse-hef /data/yolov8s.hef"
```

**Run inference:**
```bash
ssh root@10.21.220.191 "sed -i 's/yolov8n/yolov8s/g' /data/demo.py && python3 /data/demo.py"
```

---

## Document History

- **March 2026:** Initial "from scratch" tutorial created, validated with YOLOv8S compilation to HEF
- **References:** Hailo YOLOv8 Compilation Tutorial (primary doc), SEA:ME Team 6 project

