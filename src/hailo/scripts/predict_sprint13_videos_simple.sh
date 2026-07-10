#!/usr/bin/env bash
# Sprint 13 — Phase C (v2): Generate predictions with annotated videos
# Simple and robust approach using yolo CLI directly

set -euo pipefail

RESULTS_DIR="/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs"
VIDEOS_DIR="/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset"
OUTPUT_DIR="$RESULTS_DIR/phase_c_predictions"

mkdir -p "$OUTPUT_DIR"

echo "=========================================="
echo "Phase C: Predictions (annotated videos)"
echo "=========================================="

# YOLOv8s detect + teste1
echo "[1/8] YOLOv8s detect + teste1..."
yolo detect predict model="$RESULTS_DIR/yolov8s_detect_sprint13/weights/best.pt" \
  source="$VIDEOS_DIR/teste1.mp4" imgsz=640 conf=0.25 save=True device=0 --verbose
cp ~/runs/detect/predict/teste1.mp4 "$OUTPUT_DIR/yolov8s_detect_teste1.mp4" 2>/dev/null || echo "Warning: video not found"

# YOLOv8s detect + teste2
echo "[2/8] YOLOv8s detect + teste2..."
rm -rf ~/runs/detect/predict*
yolo detect predict model="$RESULTS_DIR/yolov8s_detect_sprint13/weights/best.pt" \
  source="$VIDEOS_DIR/teste2.mp4" imgsz=640 conf=0.25 save=True device=0 --verbose
cp ~/runs/detect/predict/teste2.mp4 "$OUTPUT_DIR/yolov8s_detect_teste2.mp4" 2>/dev/null || echo "Warning: video not found"

# YOLO26n detect + teste1
echo "[3/8] YOLO26n detect + teste1..."
rm -rf ~/runs/detect/predict*
yolo detect predict model="$RESULTS_DIR/yolo26n_detect_sprint13/weights/best.pt" \
  source="$VIDEOS_DIR/teste1.mp4" imgsz=640 conf=0.25 save=True device=0 --verbose
cp ~/runs/detect/predict/teste1.mp4 "$OUTPUT_DIR/yolo26n_detect_teste1.mp4" 2>/dev/null || echo "Warning: video not found"

# YOLO26n detect + teste2
echo "[4/8] YOLO26n detect + teste2..."
rm -rf ~/runs/detect/predict*
yolo detect predict model="$RESULTS_DIR/yolo26n_detect_sprint13/weights/best.pt" \
  source="$VIDEOS_DIR/teste2.mp4" imgsz=640 conf=0.25 save=True device=0 --verbose
cp ~/runs/detect/predict/teste2.mp4 "$OUTPUT_DIR/yolo26n_detect_teste2.mp4" 2>/dev/null || echo "Warning: video not found"

# YOLOv8n segment + teste1
echo "[5/8] YOLOv8n segment + teste1..."
rm -rf ~/runs/segment/predict*
yolo segment predict model="$RESULTS_DIR/yolov8n_seg_sprint13/weights/best.pt" \
  source="$VIDEOS_DIR/teste1.mp4" imgsz=640 conf=0.25 save=True device=0 --verbose
cp ~/runs/segment/predict/teste1.mp4 "$OUTPUT_DIR/yolov8n_seg_teste1.mp4" 2>/dev/null || echo "Warning: video not found"

# YOLOv8n segment + teste2
echo "[6/8] YOLOv8n segment + teste2..."
rm -rf ~/runs/segment/predict*
yolo segment predict model="$RESULTS_DIR/yolov8n_seg_sprint13/weights/best.pt" \
  source="$VIDEOS_DIR/teste2.mp4" imgsz=640 conf=0.25 save=True device=0 --verbose
cp ~/runs/segment/predict/teste2.mp4 "$OUTPUT_DIR/yolov8n_seg_teste2.mp4" 2>/dev/null || echo "Warning: video not found"

# YOLO26n segment + teste1
echo "[7/8] YOLO26n segment + teste1..."
rm -rf ~/runs/segment/predict*
yolo segment predict model="$RESULTS_DIR/yolo26n_seg_sprint13/weights/best.pt" \
  source="$VIDEOS_DIR/teste1.mp4" imgsz=640 conf=0.25 save=True device=0 --verbose
cp ~/runs/segment/predict/teste1.mp4 "$OUTPUT_DIR/yolo26n_seg_teste1.mp4" 2>/dev/null || echo "Warning: video not found"

# YOLO26n segment + teste2
echo "[8/8] YOLO26n segment + teste2..."
rm -rf ~/runs/segment/predict*
yolo segment predict model="$RESULTS_DIR/yolo26n_seg_sprint13/weights/best.pt" \
  source="$VIDEOS_DIR/teste2.mp4" imgsz=640 conf=0.25 save=True device=0 --verbose
cp ~/runs/segment/predict/teste2.mp4 "$OUTPUT_DIR/yolo26n_seg_teste2.mp4" 2>/dev/null || echo "Warning: video not found"

# Cleanup
rm -rf ~/runs

echo ""
echo "=========================================="
echo "Predictions Complete"
echo "=========================================="
echo ""
echo "Outputs in: $OUTPUT_DIR"
ls -lh "$OUTPUT_DIR"/*.mp4 2>/dev/null || echo "No MP4 videos found"
