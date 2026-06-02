#!/usr/bin/env python3
import os

os.environ.setdefault("MODEL_LABEL", "YOLOv8n")
os.environ.setdefault("HEF_PATH", "/data/yolo_benchmark/models/hef/yolov8n_h8.hef")
os.environ.setdefault("INPUT_STREAM", "yolov8n/input_layer1")
os.environ.setdefault("OUTPUT_STREAM", "yolov8n/yolov8_nms_postprocess")
os.environ.setdefault("OUTPUT_PATH", "/data/yolo_benchmark/results/demo_yolov8n_scalercrop.mp4")

import inference_camera_scalercrop_yolov8s as base


if __name__ == "__main__":
    parser = base.demo_io.build_arg_parser(f"SEAME {base.MODEL_LABEL} | ScalerCrop")
    args = parser.parse_args()
    output_path = args.output or base.OUTPUT_PATH

    print(f"SEAME {base.MODEL_LABEL} | ScalerCrop | HEF 640×640")
    print(
        f"Câmara: {base.FULL_W}×{base.FULL_H} (sensor crop 40% topo) | "
        f"Modelo: {base.MODEL_W}×{base.MODEL_H}\n"
    )
    base.run_demo(
        duration_seconds=args.duration,
        save_video=args.save,
        images_dir=args.images_dir,
        loop=args.loop,
        output_path=output_path,
    )