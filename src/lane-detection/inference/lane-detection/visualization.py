import cv2
import numpy as np
from config import (
    LANE_COLORS, ROW_LANE_LIST, DRAW_MAX_Y_GAP,
    TRAIN_WIDTH, TRAIN_HEIGHT, EXIST_THRESHOLD,
)


def draw_lanes(frame, lanes):
    """
    Desenha as ego-lanes no frame.
    Não liga pontos com gap vertical > DRAW_MAX_Y_GAP (evita linhas entre gaps de detecção).
    """
    for lane_idx in ROW_LANE_LIST:
        lane = lanes[lane_idx]
        if not lane:
            continue
        color = LANE_COLORS[lane_idx]
        for j in range(len(lane) - 1):
            if abs(lane[j+1][1] - lane[j][1]) < DRAW_MAX_Y_GAP:
                cv2.line(frame, lane[j], lane[j+1], color, 3)
        for pt in lane:
            cv2.circle(frame, pt, 4, color, -1)
    return frame


def draw_drivable_area(frame, lanes):
    """
    Desenha a área conduzível entre as duas ego-lanes:
    - Polígono verde transparente entre lane esquerda e direita
    - Linha central a amarelo
    Só desenha onde há pontos comuns nas duas lanes.
    """
    lane_left  = lanes[1]
    lane_right = lanes[2]

    if not lane_left or not lane_right:
        return frame

    ys_left   = {y: x for (x, y) in lane_left}
    ys_right  = {y: x for (x, y) in lane_right}
    common_ys = sorted(set(ys_left.keys()) & set(ys_right.keys()))

    if len(common_ys) < 2:
        return frame

    pts_left   = [(ys_left[y],  y) for y in common_ys]
    pts_right  = [(ys_right[y], y) for y in common_ys]
    pts_center = [((ys_left[y] + ys_right[y]) // 2, y) for y in common_ys]

    # Polígono da área conduzível
    poly = np.array(pts_left + pts_right[::-1], dtype=np.int32)
    overlay = frame.copy()
    cv2.fillPoly(overlay, [poly], (0, 180, 0))
    cv2.addWeighted(overlay, 0.3, frame, 0.7, 0, frame)

    # Linha central
    for j in range(len(pts_center) - 1):
        cv2.line(frame, pts_center[j], pts_center[j+1], (0, 255, 255), 2)

    return frame


def draw_overlay(frame, fps, frame_idx, t_pre, t_hailo, t_post, t_coords,
                 deviation, status):
    """
    Desenha o painel de telemetria no canto superior esquerdo do frame.
    """
    h, w = frame.shape[:2]
    overlay = frame.copy()
    cv2.rectangle(overlay, (0, 0), (215, 145), (0, 0, 0), -1)
    cv2.addWeighted(overlay, 0.5, frame, 0.5, 0, frame)

    dev_str   = f"{deviation:+.3f}" if deviation is not None else "N/A"
    dev_color = (0, 255, 0) if status == "both" else (0, 165, 255)

    lines = [
        (f"FPS:    {fps:.1f}",              (0, 255, 0)),
        (f"Pre:    {t_pre:.1f}ms",          (200, 200, 200)),
        (f"Hailo:  {t_hailo:.1f}ms",        (200, 200, 200)),
        (f"Post:   {t_post:.1f}ms",         (200, 200, 200)),
        (f"Coords: {t_coords:.1f}ms",       (200, 200, 200)),
        (f"Frame:  {frame_idx}",            (255, 255, 255)),
        (f"Thr:    {EXIST_THRESHOLD}",      (255, 200, 0)),
        (f"Dev:    {dev_str} [{status}]",   dev_color),
    ]
    for i, (text, color) in enumerate(lines):
        cv2.putText(frame, text, (5, 16 + i * 14),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.35, color, 1)
    cv2.putText(frame, f"SEAME | TuSimple cut v2 | {TRAIN_WIDTH}x{TRAIN_HEIGHT}",
                (w - 260, h - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (200, 200, 200), 1)
    return frame
