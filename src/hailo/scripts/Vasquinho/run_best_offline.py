#!/usr/bin/env python3
"""
Offline comparison runner with Vasco-like postprocess defaults.

This script wraps the Sprint 13 runtime and applies:
- conf=0.30
- iou=0.40
- min_box_size=0.04
- global_nms_iou=0.25

It still supports overriding any option via CLI.
"""

import argparse
import importlib.util
from pathlib import Path


def _load_sprint13_module():
    here = Path(__file__).resolve().parent
    target = (here.parent / "inference_video_sprint13.py").resolve()
    spec = importlib.util.spec_from_file_location("inference_video_sprint13", str(target))
    if spec is None or spec.loader is None:
        raise RuntimeError(f"Cannot load Sprint13 runtime module: {target}")
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def main():
    parser = argparse.ArgumentParser(
        description="Run offline video inference with Vasco-like postprocess defaults",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument("--hef", required=True, help="Path to .hef")
    parser.add_argument("--source", required=True, help="Input video path")
    parser.add_argument("--output", required=True, help="Output video path")
    parser.add_argument("--model-type", default="detect", choices=["detect", "seg"])
    parser.add_argument("--model-name", default="vasco_like")
    parser.add_argument("--conf", type=float, default=0.30)
    parser.add_argument("--iou", type=float, default=0.40)
    parser.add_argument("--min-box-size", type=float, default=0.04)
    parser.add_argument("--global-nms-iou", type=float, default=0.25)
    parser.add_argument("--sync-writer", action="store_true")
    parser.add_argument("--label-decimals", type=int, default=3)
    args = parser.parse_args()

    mod = _load_sprint13_module()
    mod.run_inference(args)


if __name__ == "__main__":
    main()
