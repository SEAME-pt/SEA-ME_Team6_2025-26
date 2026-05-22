#!/usr/bin/env python3
"""
Calibração empírica do K de distância — classe 9 (ou qualquer classe YOLO).
Método: K_i = dist_m × box_h_px → K_final = median(K_i)

Modo headless (sem display) — guarda frame capturado como /tmp/calib_K_N.jpg.

Uso:
  python3 calibrate_K.py [--camera 1] [--flip -1] [--class 9] [--frames 5]

Workflow:
  1. Posiciona obstáculo a distância conhecida (ex: 30cm)
  2. Prime ENTER → script captura --frames frames, detecta classe, usa melhor detecção
  3. Insere distância real em cm quando pedido
  4. Repete para 50, 80, 100cm
  5. Script mostra K_median + valores para copiar para o código
"""

import sys, os, argparse
import numpy as np
import cv2

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "lane-detection"))

from hailo_platform import (HEF, VDevice, HailoStreamInterface,
                             InferVStreams, ConfigureParams,
                             InputVStreamParams, OutputVStreamParams,
                             FormatType)
from config import FULL_W, FULL_H, CAM_FPS

YOLO_HEF_PATH = "/data/yolov8s.hef"
YOLO_INPUT    = "best_model/input_layer1"
MODEL_W = MODEL_H = 640
CONF_THRESH   = 0.30
REG_MAX       = 16

YOLO_STREAMS = [
    ("best_model/conv41", "best_model/conv42", 8),
    ("best_model/conv52", "best_model/conv53", 16),
    ("best_model/conv62", "best_model/conv63", 32),
]


def decode_yolo(raw_outputs):
    boxes_all, scores_all, cls_all = [], [], []
    for cv2_key, cv3_key, stride in YOLO_STREAMS:
        cv2_out    = raw_outputs[cv2_key][0]
        cv3_out    = raw_outputs[cv3_key][0]
        cls_scores = 1.0 / (1.0 + np.exp(-cv3_out))
        max_scores = cls_scores.max(axis=2)
        cls_ids    = cls_scores.argmax(axis=2)
        mask = max_scores >= CONF_THRESH
        if not mask.any():
            continue
        ys, xs = np.where(mask)
        reg = cv2_out[ys, xs, :].reshape(-1, 4, REG_MAX)
        reg = np.exp(reg - reg.max(axis=2, keepdims=True))
        reg = reg / reg.sum(axis=2, keepdims=True)
        reg = (reg * np.arange(REG_MAX)).sum(axis=2)
        cx  = (xs + 0.5) * stride;  cy = (ys + 0.5) * stride
        x1  = np.clip((cx - reg[:, 0] * stride) / MODEL_W, 0, 1)
        y1  = np.clip((cy - reg[:, 1] * stride) / MODEL_H, 0, 1)
        x2  = np.clip((cx + reg[:, 2] * stride) / MODEL_W, 0, 1)
        y2  = np.clip((cy + reg[:, 3] * stride) / MODEL_H, 0, 1)
        boxes_all.append(np.stack([x1, y1, x2, y2], axis=1))
        scores_all.append(max_scores[ys, xs])
        cls_all.append(cls_ids[ys, xs])
    if not boxes_all:
        return np.empty((0, 4)), np.empty((0,)), np.empty((0,), dtype=int)
    return (np.vstack(boxes_all), np.concatenate(scores_all), np.concatenate(cls_all))


def infer_frame(ng, inp, outp, frame, target_class):
    """Corre YOLO num frame, devolve melhor detecção da classe alvo ou None."""
    img = cv2.resize(frame, (MODEL_W, MODEL_H))
    img = np.ascontiguousarray(cv2.cvtColor(img, cv2.COLOR_BGR2RGB).astype(np.uint8))
    with ng.activate():
        with InferVStreams(ng, inp, outp) as pipe:
            raw = pipe.infer({YOLO_INPUT: img[np.newaxis]})
    boxes, scores, classes = decode_yolo(raw)
    best_box, best_score = None, 0.0
    for box, score, cls_id in zip(boxes, scores, classes):
        if int(cls_id) == target_class and score > best_score:
            best_score = score
            best_box   = box
    return best_box, best_score


def save_debug_frame(frame, box, idx):
    """Guarda frame com bounding box desenhado em /tmp/calib_K_N.jpg."""
    disp = frame.copy()
    if box is not None:
        x1 = int(box[0] * FULL_W); y1 = int(box[1] * FULL_H)
        x2 = int(box[2] * FULL_W); y2 = int(box[3] * FULL_H)
        bh = (box[3] - box[1]) * FULL_H
        cv2.rectangle(disp, (x1, y1), (x2, y2), (0, 255, 0), 2)
        cv2.putText(disp, f"h={bh:.1f}px", (x1, max(y1 - 6, 14)),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)
    path = f"/tmp/calib_K_{idx}.jpg"
    cv2.imwrite(path, disp)
    return path


def main():
    p = argparse.ArgumentParser()
    p.add_argument("--camera",  type=int, default=1)
    p.add_argument("--flip",    type=int, default=None)
    p.add_argument("--class",   type=int, default=9, dest="target_class")
    p.add_argument("--frames",  type=int, default=5,
                   help="Nº de frames a capturar por medição (usa melhor detecção)")
    args = p.parse_args()

    sys.path.insert(0, "/opt/seame/adas")
    from camera_broker import CameraBroker
    import queue as Q

    broker = CameraBroker(width=FULL_W, height=FULL_H, fps=CAM_FPS,
                          camera=args.camera, flip=args.flip)
    q = broker.register("calib_k")
    broker.start()

    hef  = HEF(YOLO_HEF_PATH)
    measurements = []

    print(f"\n=== Calibração K — Classe {args.target_class} ===")
    print(f"Distâncias sugeridas: 30, 50, 80, 100 cm")
    print(f"Frame debug guardado em /tmp/calib_K_N.jpg após cada captura\n")

    with VDevice() as target:
        cfg  = ConfigureParams.create_from_hef(hef, interface=HailoStreamInterface.PCIe)
        ng   = target.configure(hef, cfg)[0]
        inp  = InputVStreamParams.make(ng, format_type=FormatType.UINT8)
        outp = OutputVStreamParams.make(ng, format_type=FormatType.FLOAT32)

        meas_idx = 0
        while True:
            print(f"[{len(measurements)} medições] Posiciona obstáculo → ENTER para capturar | 'q' para terminar")
            cmd = input("> ").strip().lower()
            if cmd == 'q':
                break

            # Descarta frames velhos da queue
            while not q.empty():
                try:
                    q.get_nowait()
                except Exception:
                    break

            # Captura --frames frames, fica com a melhor detecção
            best_box, best_score, best_frame = None, 0.0, None
            captured = 0
            print(f"  A capturar {args.frames} frames...", end="", flush=True)
            while captured < args.frames:
                try:
                    frame = q.get(timeout=1.0)
                except Q.Empty:
                    print(" timeout!", end="")
                    continue
                box, score = infer_frame(ng, inp, outp, frame, args.target_class)
                if box is not None and score > best_score:
                    best_score = score
                    best_box   = box
                    best_frame = frame
                captured += 1
                print(".", end="", flush=True)
            print()

            if best_box is None:
                print(f"  Nenhuma detecção da classe {args.target_class}. Reposiciona e tenta de novo.\n")
                continue

            bh      = (best_box[3] - best_box[1]) * FULL_H
            path    = save_debug_frame(best_frame, best_box, meas_idx)
            print(f"  Detecção: box_h={bh:.1f}px  conf={best_score:.2f}")
            print(f"  Frame guardado: {path}")

            try:
                dist_cm = float(input("  Distância real (cm): ").strip())
            except ValueError:
                print("  Inválido, ignorado.\n")
                continue

            dist_m = dist_cm / 100.0
            K_i    = dist_m * bh
            measurements.append((dist_m, bh, K_i))
            print(f"  → K_i = {K_i:.2f}\n")
            meas_idx += 1

    broker.stop()

    if not measurements:
        print("\nSem medições. A sair.")
        return

    print("\n=== Resultados ===")
    print(f"{'Dist(cm)':>10} {'box_h_px':>10} {'K_i':>10}")
    print("─" * 34)
    K_vals, dists_m = [], []
    for dist_m, bh, K_i in measurements:
        print(f"{dist_m*100:>10.1f} {bh:>10.1f} {K_i:>10.2f}")
        K_vals.append(K_i)
        dists_m.append(dist_m)

    K_median = float(np.median(K_vals))
    print("─" * 34)
    print(f"  K median = {K_median:.2f}")
    print(f"  K mean   = {float(np.mean(K_vals)):.2f}")
    print(f"\n→ Substitui em INFERENCE_DUAL_THETA.py:")
    print(f"  _DIST_K   = {K_median:.1f}")
    print(f"  _DIST_MIN = {max(0.1, min(dists_m) * 0.8):.2f}")
    print(f"  _DIST_MAX = {max(dists_m) * 1.5:.2f}")


if __name__ == "__main__":
    main()
