#!/usr/bin/env python3
"""
Calibração de CAMERA_OFFSET_PX e FOV_H — modo headless.

Workflow:
  1. Script captura frame e guarda em /tmp/calib_geo_N.jpg
  2. Abre o ficheiro no teu PC (scp ou partilha) para ver o cx_px do objecto
     OU usa o valor impresso no terminal se YOLO detectar o objecto
  3. Insere cx_px quando pedido

Modo OFFSET (o):
  Coloca objecto directamente à frente do carro (eixo central).
  theta deve ser 0° → CAMERA_OFFSET_PX = cx_px - FULL_W/2

Modo FOV (f):
  Coloca objecto a posição conhecida: lateral_cm (+= direita) e forward_cm.
  FOV_H = atan(lateral/forward) * (180/pi) * FULL_W / (cx_px - car_center_px)

Uso:
  python3 calibrate_geometry.py [--camera 1] [--flip -1]
"""

import sys, os, argparse
import numpy as np
import cv2

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "lane-detection"))
sys.path.insert(0, "/opt/seame/adas")

from config import FULL_W, FULL_H, CAM_FPS

CAMERA_OFFSET_PX_CURRENT = 85
FOV_H_CURRENT            = 50.9

# Tenta usar YOLO para auto-detectar (opcional — se não disponível, pede cx_px manual)
YOLO_HEF_PATH = "/data/yolov8s.hef"
YOLO_INPUT    = "best_model/input_layer1"
MODEL_W = MODEL_H = 640
CONF_THRESH   = 0.40
REG_MAX       = 16
YOLO_STREAMS  = [
    ("best_model/conv41", "best_model/conv42", 8),
    ("best_model/conv52", "best_model/conv53", 16),
    ("best_model/conv62", "best_model/conv63", 32),
]


def decode_yolo_all(raw_outputs):
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


def capture_frame(q):
    import queue as Q
    while not q.empty():
        try: q.get_nowait()
        except Exception: break
    return q.get(timeout=2.0)


def save_annotated(frame, boxes, idx):
    disp  = frame.copy()
    img_cx = FULL_W // 2
    car_cx = int(img_cx + CAMERA_OFFSET_PX_CURRENT)
    cv2.line(disp, (img_cx, 0), (img_cx, FULL_H), (80, 80, 80), 1)
    cv2.line(disp, (car_cx, 0), (car_cx, FULL_H), (0, 255, 255), 1)
    cv2.putText(disp, f"img_cx={img_cx}", (img_cx + 2, 15),
                cv2.FONT_HERSHEY_SIMPLEX, 0.4, (80, 80, 80), 1)
    cv2.putText(disp, f"car_cx={car_cx}", (car_cx + 2, 30),
                cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0, 255, 255), 1)
    for box, score, cls_id in zip(*boxes):
        x1 = int(box[0] * FULL_W); y1 = int(box[1] * FULL_H)
        x2 = int(box[2] * FULL_W); y2 = int(box[3] * FULL_H)
        cx = (x1 + x2) // 2
        cv2.rectangle(disp, (x1, y1), (x2, y2), (0, 255, 0), 2)
        cv2.line(disp, (cx, 0), (cx, FULL_H), (0, 165, 255), 1)
        cv2.putText(disp, f"cls{int(cls_id)} cx={cx}", (x1, max(y1-6, 14)),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.45, (0, 255, 0), 1)
    path = f"/tmp/calib_geo_{idx}.jpg"
    cv2.imwrite(path, disp)
    return path


def main():
    p = argparse.ArgumentParser()
    p.add_argument("--camera", type=int, default=1)
    p.add_argument("--flip",   type=int, default=None)
    p.add_argument("--class",  type=int, default=None, dest="target_class",
                   help="Filtrar detecções por classe (default: mostra todas)")
    p.add_argument("--no-yolo", action="store_true",
                   help="Não usar YOLO — pede cx_px manualmente")
    args = p.parse_args()

    from camera_broker import CameraBroker
    import queue as Q

    use_yolo = not args.no_yolo
    ng = inp = outp = None

    broker = CameraBroker(width=FULL_W, height=FULL_H, fps=CAM_FPS,
                          camera=args.camera, flip=args.flip)
    q = broker.register("calib_geo")
    broker.start()

    if use_yolo:
        try:
            from hailo_platform import (HEF, VDevice, HailoStreamInterface,
                                         InferVStreams, ConfigureParams,
                                         InputVStreamParams, OutputVStreamParams,
                                         FormatType)
            hef_yolo  = HEF(YOLO_HEF_PATH)
            _vdev     = VDevice()
            cfg       = ConfigureParams.create_from_hef(hef_yolo, interface=HailoStreamInterface.PCIe)
            ng        = _vdev.configure(hef_yolo, cfg)[0]
            inp       = InputVStreamParams.make(ng, format_type=FormatType.UINT8)
            outp      = OutputVStreamParams.make(ng, format_type=FormatType.FLOAT32)
            print("YOLO activo — detecções automáticas.")
        except Exception as e:
            print(f"YOLO não disponível ({e}) — modo manual.")
            use_yolo = False

    offset_samples = []
    fov_samples    = []
    idx            = 0

    cls_str = f"classe {args.target_class}" if args.target_class is not None else "todas as classes"
    print(f"\n=== Calibração Geometria (headless) — {cls_str} ===")
    print(f"Imagem  center: {FULL_W//2}px")
    print(f"Car     center: {int(FULL_W//2 + CAMERA_OFFSET_PX_CURRENT)}px  (offset actual={CAMERA_OFFSET_PX_CURRENT})")
    print(f"FOV_H actual:   {FOV_H_CURRENT}°\n")
    print("Comandos: o=OFFSET  f=FOV  q=terminar\n")

    while True:
        cmd = input("Comando (o/f/q): ").strip().lower()
        if cmd == 'q':
            break
        if cmd not in ('o', 'f'):
            print("  o=offset  f=fov  q=sair")
            continue

        print("  A capturar frame...", end="", flush=True)
        try:
            frame = capture_frame(q)
        except Q.Empty:
            print(" timeout. Tenta de novo.")
            continue
        print(" ok")

        # Inferência YOLO (opcional)
        det_info = ""
        boxes_for_save = (np.empty((0,4)), np.empty((0,)), np.empty((0,), dtype=int))
        if use_yolo and ng is not None:
            img = cv2.resize(frame, (MODEL_W, MODEL_H))
            img = np.ascontiguousarray(cv2.cvtColor(img, cv2.COLOR_BGR2RGB).astype(np.uint8))
            with ng.activate():
                with InferVStreams(ng, inp, outp) as pipe:
                    raw = pipe.infer({YOLO_INPUT: img[np.newaxis]})
            b, s, c = decode_yolo_all(raw)
            # Filtra por classe se --class fornecido
            if args.target_class is not None and len(b):
                mask = c == args.target_class
                b, s, c = b[mask], s[mask], c[mask]
            boxes_for_save = (b, s, c)
            if len(b):
                lines = []
                for box, score, cls_id in zip(b, s, c):
                    cx_det = int(((box[0] + box[2]) / 2.0) * FULL_W)
                    lines.append(f"    cls{int(cls_id)} cx={cx_det}px  conf={score:.2f}")
                print(f"  Detecções YOLO:\n" + "\n".join(lines))
            else:
                filt = f" (classe {args.target_class})" if args.target_class is not None else ""
                print(f"  Nenhuma detecção YOLO{filt}.")

        path = save_annotated(frame, boxes_for_save, idx)
        print(f"  Frame anotado: {path}  (linhas: cinza=img_center  ciano=car_center  laranja=cx detecção)")
        idx += 1

        # Lê cx_px
        try:
            cx_px = int(input("  cx_px do objecto (px): ").strip())
        except ValueError:
            print("  Inválido, ignorado.")
            continue

        if cmd == 'o':
            offset = cx_px - FULL_W / 2
            offset_samples.append(offset)
            print(f"  → CAMERA_OFFSET_PX ≈ {offset:+.1f}px\n")

        elif cmd == 'f':
            try:
                lat = float(input("  Lateral real (cm, +=direita, -=esquerda): "))
                fwd = float(input("  Forward real (cm): "))
            except ValueError:
                print("  Inválido, ignorado.")
                continue
            theta_real = np.degrees(np.arctan2(lat, fwd))
            car_cx     = FULL_W / 2 + CAMERA_OFFSET_PX_CURRENT
            delta_px   = cx_px - car_cx
            if abs(delta_px) < 5:
                print("  Objecto demasiado perto do centro — usa posição mais lateral.")
                continue
            fov_i = theta_real * FULL_W / delta_px
            fov_samples.append((theta_real, delta_px, fov_i))
            print(f"  theta_real={theta_real:.2f}°  delta={delta_px:.1f}px  FOV_H≈{fov_i:.2f}°\n")

    broker.stop()
    if use_yolo and ng is not None:
        try: _vdev.release()
        except Exception: pass

    print("\n=== Resultados ===")

    if offset_samples:
        med = float(np.median(offset_samples))
        print(f"\nCAMERA_OFFSET_PX:")
        for i, v in enumerate(offset_samples):
            print(f"  medição {i+1}: {v:+.1f}px")
        print(f"  median = {med:+.1f}px  (actual={CAMERA_OFFSET_PX_CURRENT:+d}px)")
        print(f"  → CAMERA_OFFSET_PX = {int(round(med))}")
    else:
        print("\nCAMERA_OFFSET_PX: sem medições")

    if fov_samples:
        fov_vals = [s[2] for s in fov_samples]
        med_fov  = float(np.median(fov_vals))
        print(f"\nFOV_H:")
        for i, (tr, dp, fi) in enumerate(fov_samples):
            print(f"  medição {i+1}: theta_real={tr:.2f}°  delta={dp:.1f}px  FOV_i={fi:.2f}°")
        print(f"  median = {med_fov:.2f}°  (actual={FOV_H_CURRENT:.1f}°)")
        print(f"  → FOV_H = {med_fov:.1f}")
    else:
        print("\nFOV_H: sem medições")


if __name__ == "__main__":
    main()
