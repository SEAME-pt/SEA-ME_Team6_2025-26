#!/usr/bin/env python3
"""
Sprint 13 Phase C: Generate annotated prediction videos for all 4 models
Uses Ultralytics predict + OpenCV for robust video generation
"""

import os
import sys
from pathlib import Path
from ultralytics import YOLO
import cv2

RESULTS_DIR = Path("/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs")
VIDEOS_DIR = Path("/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset")
OUTPUT_DIR = RESULTS_DIR / "phase_c_predictions"

OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

models_config = [
    ("yolov8s_detect_sprint13", "detect", "teste1"),
    ("yolov8s_detect_sprint13", "detect", "teste2"),
    ("yolo26n_detect_sprint13", "detect", "teste1"),
    ("yolo26n_detect_sprint13", "detect", "teste2"),
    ("yolov8n_seg_sprint13", "segment", "teste1"),
    ("yolov8n_seg_sprint13", "segment", "teste2"),
    ("yolo26n_seg_sprint13", "segment", "teste1"),
    ("yolo26n_seg_sprint13", "segment", "teste2"),
]

def generate_annotated_video(model_name, task, video_name, idx, total):
    """Generate annotated prediction video"""
    
    model_pt = RESULTS_DIR / model_name / "weights" / "best.pt"
    video_path = VIDEOS_DIR / f"{video_name}.mp4"
    output_mp4 = OUTPUT_DIR / f"{model_name}_{video_name}.mp4"
    
    print(f"\n[{idx}/{total}] {model_name} + {video_name}")
    print(f"  Input:  {video_path}")
    print(f"  Output: {output_mp4}")
    
    # Check files exist
    if not model_pt.exists():
        print(f"  ERROR: Model not found at {model_pt}")
        return False
    if not video_path.exists():
        print(f"  ERROR: Video not found at {video_path}")
        return False
    
    try:
        # Load model
        print(f"  Loading model...")
        model = YOLO(str(model_pt))
        
        # Run prediction (returns generator)
        print(f"  Running predictions...")
        results = model.predict(
            source=str(video_path),
            task=task,
            imgsz=640,
            conf=0.25,
            device=0,
            verbose=False,
            stream=True,  # Stream mode to save memory
        )
        
        # Get video properties
        cap = cv2.VideoCapture(str(video_path))
        fps = cap.get(cv2.CAP_PROP_FPS)
        width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        cap.release()
        
        print(f"  Video info: {width}x{height}, {fps} fps")
        
        # Create video writer
        fourcc = cv2.VideoWriter_fourcc(*'mp4v')
        out = cv2.VideoWriter(str(output_mp4), fourcc, fps, (width, height))
        
        if not out.isOpened():
            print(f"  ERROR: Could not open video writer")
            return False
        
        # Process and annotate frames
        frame_count = 0
        for result in results:
            # Get annotated frame from result
            frame = result.plot()  # Returns RGB annotated frame
            
            # Convert RGB to BGR for OpenCV
            frame_bgr = cv2.cvtColor(frame, cv2.COLOR_RGB2BGR)
            
            # Resize if needed
            if frame_bgr.shape[0] != height or frame_bgr.shape[1] != width:
                frame_bgr = cv2.resize(frame_bgr, (width, height))
            
            # Write frame
            out.write(frame_bgr)
            frame_count += 1
            
            if frame_count % 50 == 0:
                print(f"    Processed {frame_count} frames...")
        
        out.release()
        
        if output_mp4.exists():
            size_mb = output_mp4.stat().st_size / (1024*1024)
            print(f"  ✅ Saved: {output_mp4} ({size_mb:.1f} MB, {frame_count} frames)")
            return True
        else:
            print(f"  ERROR: Output file not created")
            return False
            
    except Exception as e:
        print(f"  ERROR: {e}")
        import traceback
        traceback.print_exc()
        return False

# Main execution
if __name__ == "__main__":
    print("\n" + "="*100)
    print("SPRINT 13 — PHASE C: Generate Annotated Prediction Videos")
    print("="*100)
    
    success_count = 0
    total = len(models_config)
    
    for idx, (model_name, task, video_name) in enumerate(models_config, 1):
        if generate_annotated_video(model_name, task, video_name, idx, total):
            success_count += 1
    
    print("\n" + "="*100)
    print(f"PHASE C COMPLETE: {success_count}/{total} videos generated successfully")
    print(f"Output directory: {OUTPUT_DIR}")
    print("="*100 + "\n")
    
    # List outputs
    mp4_files = list(OUTPUT_DIR.glob("*.mp4"))
    if mp4_files:
        print(f"Generated {len(mp4_files)} MP4 files:")
        for mp4 in sorted(mp4_files):
            size_mb = mp4.stat().st_size / (1024*1024)
            print(f"  • {mp4.name} ({size_mb:.1f} MB)")
    else:
        print("No MP4 files generated!")
        sys.exit(1)
