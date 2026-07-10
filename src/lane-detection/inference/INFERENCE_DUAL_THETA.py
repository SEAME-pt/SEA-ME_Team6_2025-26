#!/usr/bin/env python3
"""
SEAME — Dual Inference Pipeline v15
  • UFLDv2 (cut v2)   — Lane Detection   — Hailo-8 PCIe
  • best_model        — Object Detection — Hailo-8 PCIe

Único VDevice, dois network groups, inferência sequencial por frame.
Optimizações acumuladas:
  v7:  undistort no frame pequeno (800×320) — poupa ~6ms
  v8:  YPre corre em thread CPU durante LHailo (NPU idle) — poupa ~4ms
  v9:  LPost corre em thread CPU durante YHailo (NPU idle) — overlap estrutural
  v10: LPost usa extensão C++ (OpenBLAS, sem GIL) — overlap efectivo ~13ms
  v11: fix visualização — re-distort de pontos para display
  v15: fix socket sender — publica em /tmp/adas_lane.sock (ADAS Manager)
       remove KUKSA directo; corrige assinatura publish_deviation(dev, status)
  v15: lane hold com decaimento — quando status="none", mantém o último desvio
       válido por HOLD_FRAMES frames com multiplicador HOLD_DECAY por frame.
       O ADAS Manager não entra em DEGRADED durante o hold.
"""

import numpy as np
import cv2
import threading
import queue
import time
import sys
import os
import argparse
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from hailo_platform import (HEF, VDevice, HailoStreamInterface,
                             InferVStreams, ConfigureParams,
                             InputVStreamParams, OutputVStreamParams,
                             FormatType)

# ── Módulos lane-detection ────────────────────────────────────────────────────
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                "lane-detection"))
from config import (
    HEF_PATH as LANE_HEF_PATH,
    WEIGHTS_PATH,
    FULL_W, FULL_H, CAM_FPS,
    TRAIN_WIDTH, TRAIN_HEIGHT, CROP_RATIO,
    INPUT_STREAM  as LANE_INPUT,
    OUTPUT_LAYER  as LANE_OUTPUT,
    EXIST_THRESHOLD, TEMPORAL_HISTORY, TEMPORAL_MIN_HITS,
    CAMERA_OFFSET_CM,
)
from postprocess import (
    load_weights, postprocess, decode_lanes,
    calc_lateral_deviation, TemporalLaneSmoother, smooth_lanes,
)
from visualization import draw_lanes, draw_drivable_area, draw_overlay as draw_overlay_lane

# ── Extensão C++ (postprocess + decode_lanes sem GIL) ────────────────────────
try:
    sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                    "postprocess_cpp"))
    import postprocess_cpp as _pp_cpp
    _cpp_postprocess  = _pp_cpp.postprocess
    _cpp_decode_lanes = _pp_cpp.decode_lanes
    print("[PostProcess] C++ extension carregada (OpenBLAS, sem GIL)")
except ImportError:
    _cpp_postprocess  = None
    _cpp_decode_lanes = None
    print("[PostProcess] AVISO: C++ não disponível — a usar Python (overlap ineficaz)")

# ── Socket → ADAS Manager ─────────────────────────────────────────────────────
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                '..', '..', 'ADAS-Manager'))
from socket_sender import (start_socket_thread, send_perception as publish_deviation,
                           send_objects as publish_objects,
                           SIGN_UNKNOWN, SIGN_STOP, SIGN_YIELD,
                           SIGN_SPEED_30, SIGN_SPEED_50, SIGN_SPEED_80,
                           SIGN_OBSTACLE, SIGN_PEDESTRIAN,
                           SIGN_TL_GREEN, SIGN_TL_RED, SIGN_TL_YELLOW)


# ══════════════════════════════════════════════════════════════════════════════
# Configuração YOLO (best_model)
# ══════════════════════════════════════════════════════════════════════════════
YOLO_HEF_PATH = "/data/yolov8s.hef"
OUTPUT_PATH   = "/data/demo_dual_v15.mp4"

MODEL_W = 640
MODEL_H = 640

YOLO_INPUT   = "best_model/input_layer1"
CONF_THRESH  = 0.30
IOU_THRESH   = 0.40
MIN_BOX_SIZE = 0.04

YOLO_CLASS_MAP = {
    0:  SIGN_SPEED_50,      # 50_maxspeed
    1:  SIGN_SPEED_80,      # 80_maxspeed
    2:  SIGN_PEDESTRIAN,    # Crosswalk
    3:  SIGN_UNKNOWN,       # Gate
    4:  SIGN_PEDESTRIAN,    # Pedestrians_crossing
    5:  SIGN_STOP,          # Stop_sign
    6:  SIGN_YIELD,         # Traffic_priority
    7:  SIGN_UNKNOWN,       # both_arrow
    8:  SIGN_OBSTACLE,      # car
    9:  SIGN_UNKNOWN,       # cars not allowed
    10: SIGN_UNKNOWN,       # left_cross
    11: SIGN_OBSTACLE,      # obstacle
    12: SIGN_UNKNOWN,       # right_cross
    13: SIGN_TL_GREEN,      # traffic_lights_green
    14: SIGN_UNKNOWN,       # traffic_lights_off
    15: SIGN_TL_RED,        # traffic_lights_red
    16: SIGN_TL_YELLOW,     # traffic_lights_yellow
}

_K_SIGN          = 37.8   # calibrado por Vasco com classe 9 (sinais), erro ~8%
_K_CAR           = 420.2   # TODO: calibrar com classe 8 (car)
_K_OBSTACLE      = 51.5 
_K_CROSSWALK     = 420.2   # TODO: calibrar com classe 2 (passadeira — marcação no chão)
_K_GATE          = 420.2   # TODO: calibrar com classe 3 (gate — estrutura física)

_DIST_K_PER_CLASS = {
    0:  _K_SIGN,       # 50_maxspeed
    1:  _K_SIGN,       # 80_maxspeed
    2:  _K_CROSSWALK,  # Crosswalk (passadeira no chão — tamanho diferente)
    3:  _K_GATE,       # Gate (estrutura física — tamanho diferente)
    4:  _K_SIGN,       # Pedestrians_crossing
    5:  _K_SIGN,       # Stop_sign
    6:  _K_SIGN,       # Traffic_priority
    7:  _K_SIGN,       # both_arrow
    8:  _K_CAR,        # car
    9:  _K_SIGN,       # cars not allowed
    10: _K_SIGN,       # left_cross
    11: _K_OBSTACLE,   # obstacle
    12: _K_SIGN,       # right_cross
    13: _K_SIGN,       # traffic_lights_green
    14: _K_SIGN,       # traffic_lights_off
    15: _K_SIGN,       # traffic_lights_red
    16: _K_SIGN,       # traffic_lights_yellow
}
_DIST_K   = _K_SIGN  # fallback para classes não mapeadas
_DIST_MIN = 0.24      # distância mínima válida (m)
_DIST_MAX = 1.35      # distância máxima válida (m)
FOV_H            = 50.9    # FOV calibrado empiricamente
CAMERA_OFFSET_PX = 85      # calibrado empiricamente — câmara deslocada à esquerda do centro do carro

# Classes YOLO que recebem overlay de distância/theta (expansível)
OBSTACLE_VIS_CLASSES = {8, 11}  # car, obstacle

YOLO_STREAMS = [
    ("best_model/conv41", "best_model/conv42", 8),
    ("best_model/conv52", "best_model/conv53", 16),
    ("best_model/conv62", "best_model/conv63", 32),
]
REG_MAX = 16

BOX_COLOR  = (0, 255, 255)
TEXT_COLOR = (0, 255, 255)

CALIB_DIR_FRONT = Path("/data/seame-configs/camera")
CALIB_DIR_REAR  = Path("/data/seame-configs/camera_rear")
INTRINSIC_DIR   = Path("/data/seame-configs/camera")

_resize_h   = int(TRAIN_HEIGHT / CROP_RATIO)
_top_native = round(FULL_H * ((_resize_h - TRAIN_HEIGHT) / _resize_h))

# ── Diagnóstico ───────────────────────────────────────────────────────────────
# Quando True, salta toda a inferência YOLO (preprocess + Hailo + postprocess).
# Liberta o NPU + CPU para o lane → ~29 Hz em vez de ~18 Hz dual.
# Trade-off: sem TSR (sinais) durante o teste. Reverter pondo a False.
SKIP_YOLO = False

# ── Lane hold parameters ──────────────────────────────────────────────────────
# 2 frames @ 22Hz = ~90ms. Suficiente p/ cobrir flicker mas curto o bastante p/ o
# manager registar perda quando a passadeira ocupa toda a vista (status=none
# chega rapidamente ao manager → DEGRADED → target=0 → travagem rápida).
HOLD_FRAMES = 2
HOLD_DECAY  = 0.85  # multiplicador por frame — ~20% do valor original ao fim

# ── Tier 1 — robustez em curva / single-line ─────────────────────────────────
# A: Em single-line mode, exige pontos suficientemente perto do carro.
MIN_BOTTOM_Y_RATIO = 0.50   # ponto "perto" = y >= 50% da altura da imagem
MIN_BOTTOM_POINTS  = 2      # nº mínimo de pontos perto do carro (single-mode)
# B: Filtro de continuidade lateral entre frames (rejeita saltos suspeitos).
MAX_X_JUMP_CM      = 40.0   # salto máximo aceitável de deviation entre frames
MAX_REJECT_STREAK  = 5      # após N rejeições seguidas, aceita para não ficar preso
# C: Cobertura vertical mínima de uma linha.
MIN_LINE_SPAN_PX   = 60     # y_max - y_min mínimo para considerar a linha válida

# ── Tier 2 — polyfit + outlier rejection ─────────────────────────────────────
# Uma lane real tem pontos que formam uma curva suave x=f(y). Riscas
# horizontais de passadeira espalham-se horizontalmente sem formar curva
# → resíduos altos no polyfit. Rejeita esses outliers e a lane se restarem
# poucos inliers.
POLYFIT_DEG            = 2     # grau do polinómio (quadrático cobre curvas)
POLYFIT_MAX_RESIDUAL   = 25.0  # resíduo |x - poly(y)| em px para inlier
POLYFIT_MIN_Y_SPAN     = 30    # y-span mínimo p/ fit ser bem-condicionado
POLYFIT_MIN_INLIERS    = 4     # nº mínimo de inliers para aceitar lane

# ── Look-ahead deviation (Tier 2.5) ──────────────────────────────────────────
# Em curva, o bottom da lane visível não representa onde a lane vai. Avaliar
# o polyfit num Y mais alto (mais à frente) dá a posição da lane à frente do
# carro → LKA steera na direção da curva, não para o lado errado.
# 0.0 = só bottom (= comportamento antigo)
# 0.5 = meio da gama dos pontos válidos (~1m à frente)
# 1.0 = topo (mais antecipação, menos preciso lateralmente)
LOOKAHEAD_Y_RATIO      = 0.7

# ── Tier 2 (extended) — lane assignment by world-X ───────────────────────────
# UFLDv2 produz 4 slots fixos; em curva apertada, ego_right pode aparecer no
# slot 1 (que historicamente é "left"). Não confiar no slot — atribuir por
# world-X dos pontos perto do carro. Descarta lanes far-left/far-right da via.
PLAUSIBLE_EGO_X_CM     = 60.0  # |world_x| máx aceitável p/ ser ego lane
WORLD_X_BOTTOM_N       = 5     # pontos de fundo usados p/ estimar world-X médio
DEBUG_ASSIGN_EVERY_N   = 30    # print debug de _assign_ego_lanes a cada N chamadas
                               # (=0 para desactivar)
_assign_debug_counter  = 0


# ══════════════════════════════════════════════════════════════════════════════
# Calibração
# ══════════════════════════════════════════════════════════════════════════════
class Calibration:
    def __init__(self, is_rear=False):
        self.enabled      = False
        self._small_map1  = None
        self._small_map2  = None
        self._full_map1   = None
        self._full_map2   = None
        self.H_img2world  = None

        calib_dir = CALIB_DIR_REAR if is_rear else CALIB_DIR_FRONT
        cam_label = "REAR" if is_rear else "FRONT"

        files = {
            "camera_matrix.npy":        INTRINSIC_DIR / "camera_matrix.npy",
            "dist_coeffs.npy":          INTRINSIC_DIR / "dist_coeffs.npy",
            "homography_img2world.npy":  calib_dir    / "homography_img2world.npy",
        }
        missing = [n for n, p in files.items() if not p.exists()]
        if missing:
            print(f"[CALIB] [{cam_label}] Ficheiros em falta: {', '.join(missing)}")
            print(f"[CALIB] A correr SEM calibração")
            return

        camera_matrix    = np.load(files["camera_matrix.npy"])
        dist_coeffs      = np.load(files["dist_coeffs.npy"])
        self.H_img2world = np.load(files["homography_img2world.npy"])

        self._camera_matrix = camera_matrix
        self._dist_coeffs   = dist_coeffs

        new_matrix, _ = cv2.getOptimalNewCameraMatrix(
            camera_matrix, dist_coeffs, (FULL_W, FULL_H), 0, (FULL_W, FULL_H))

        self._new_fx = float(new_matrix[0, 0])
        self._new_fy = float(new_matrix[1, 1])
        self._new_cx = float(new_matrix[0, 2])
        self._new_cy = float(new_matrix[1, 2])

        scale_x = TRAIN_WIDTH  / FULL_W
        scale_y = TRAIN_HEIGHT / (FULL_H - _top_native)

        K_small = new_matrix.copy()
        K_small[0, 0] *= scale_x
        K_small[1, 1] *= scale_y
        K_small[0, 2] *= scale_x
        K_small[1, 2]  = (new_matrix[1, 2] - _top_native) * scale_y

        self._small_map1, self._small_map2 = cv2.initUndistortRectifyMap(
            camera_matrix, dist_coeffs, None, K_small,
            (TRAIN_WIDTH, TRAIN_HEIGHT), cv2.CV_16SC2)

        self._full_map1, self._full_map2 = cv2.initUndistortRectifyMap(
            camera_matrix, dist_coeffs, None, new_matrix,
            (FULL_W, FULL_H), cv2.CV_16SC2)

        self.enabled = True
        print(f"[CALIB] [{cam_label}] OK — small maps {TRAIN_WIDTH}×{TRAIN_HEIGHT} "
              f"+ full maps {FULL_W}×{FULL_H} + extrinsic ({calib_dir.name}/)")

    def undistort_small(self, img_bgr):
        if not self.enabled:
            return img_bgr
        return cv2.remap(img_bgr, self._small_map1, self._small_map2, cv2.INTER_LINEAR)

    def undistort_full(self, frame):
        if not self.enabled:
            return frame
        return cv2.remap(frame, self._full_map1, self._full_map2, cv2.INTER_LINEAR)

    def undistorted_to_distorted(self, points):
        if not self.enabled or not points:
            return points
        pts = np.array([[p[0], p[1]] for p in points], dtype=np.float64)
        x_n = (pts[:, 0] - self._new_cx) / self._new_fx
        y_n = (pts[:, 1] - self._new_cy) / self._new_fy
        pts3d = np.stack([x_n, y_n, np.ones(len(pts))], axis=1).reshape(-1, 1, 3)
        rvec = np.zeros((3, 1)); tvec = np.zeros((3, 1))
        pts_dist, _ = cv2.projectPoints(
            pts3d, rvec, tvec, self._camera_matrix, self._dist_coeffs)
        return [(int(round(p[0][0])), int(round(p[0][1]))) for p in pts_dist]

    def pixels_to_world(self, points_px):
        if not self.enabled or self.H_img2world is None:
            return None
        pts = np.array(points_px, dtype=np.float32).reshape(-1, 1, 2)
        return cv2.perspectiveTransform(pts, self.H_img2world).reshape(-1, 2)


def _clean_lane_polyfit(pts):
    """
    Tier 2 — ajusta x = f(y) com polyfit de grau POLYFIT_DEG e devolve
    apenas os pontos com resíduo < POLYFIT_MAX_RESIDUAL.
    Devolve [] se:
      - poucos pontos para o fit
      - y-span demasiado pequeno (fit mal-condicionado)
      - polyfit falha (matriz singular)
      - inliers < POLYFIT_MIN_INLIERS
    """
    if len(pts) < POLYFIT_DEG + 2:
        return []
    arr = np.asarray(pts, dtype=np.float32)
    xs, ys = arr[:, 0], arr[:, 1]
    if float(ys.max() - ys.min()) < POLYFIT_MIN_Y_SPAN:
        return []
    try:
        coeffs = np.polyfit(ys, xs, POLYFIT_DEG)
    except (np.linalg.LinAlgError, ValueError, TypeError):
        return []
    residuals = np.abs(np.polyval(coeffs, ys) - xs)
    inliers   = residuals < POLYFIT_MAX_RESIDUAL
    if int(inliers.sum()) < POLYFIT_MIN_INLIERS:
        return []
    return [p for p, ok in zip(pts, inliers.tolist()) if ok]


def _assign_ego_lanes(all_lanes, calib):
    """
    Reatribui as 4 slots do UFLDv2 a (ego_left, ego_right) por world-X dos
    pontos de fundo. UFLDv2 não garante "slot 1 = left, slot 2 = right" em
    curva — esta função usa a geometria real no mundo, não o índice do slot.

    Para cada lane:
      1. Tier 2 polyfit cleanup
      2. Pega nos WORLD_X_BOTTOM_N pontos mais baixos
      3. Estima world_x médio via calib.pixels_to_world
      4. Descarta se |world_x| > PLAUSIBLE_EGO_X_CM (provavelmente far-left/right)

    Depois:
      ego_left  = lane com world_x mais próximo de 0 entre as negativas
      ego_right = lane com world_x mais próximo de 0 entre as positivas

    Devolve (left_pts, right_pts) — cada um pode ser None.
    """
    global _assign_debug_counter
    candidates = []
    raw_world_x = []      # debug: incl. as rejeitadas por PLAUSIBLE_EGO_X_CM
    for slot_i, lane in enumerate(all_lanes):
        cleaned = _clean_lane_polyfit(lane)
        if not cleaned:
            raw_world_x.append((slot_i, None, "polyfit-reject"))
            continue
        bottom_pts = sorted(cleaned, key=lambda p: p[1], reverse=True)[:WORLD_X_BOTTOM_N]
        world = calib.pixels_to_world(bottom_pts)
        if world is None or len(world) == 0:
            raw_world_x.append((slot_i, None, "no-world"))
            continue
        world_x = float(np.mean(world[:, 0]))
        if abs(world_x) > PLAUSIBLE_EGO_X_CM:
            raw_world_x.append((slot_i, world_x, "out-of-range"))
            continue
        candidates.append((world_x, cleaned, slot_i))
        raw_world_x.append((slot_i, world_x, "kept"))

    left  = right = None
    if candidates:
        left_cands  = [c for c in candidates if c[0] < 0]
        right_cands = [c for c in candidates if c[0] >= 0]
        # Pega na mais próxima de 0 em cada lado (provável ego, não far-left/far-right)
        left  = max(left_cands,  key=lambda c: c[0])[1] if left_cands  else None
        right = min(right_cands, key=lambda c: c[0])[1] if right_cands else None

    if DEBUG_ASSIGN_EVERY_N > 0 and _assign_debug_counter % DEBUG_ASSIGN_EVERY_N == 0:
        parts = []
        for slot_i, wx, tag in raw_world_x:
            wx_s = f"{wx:+6.1f}cm" if wx is not None else "   --   "
            parts.append(f"s{slot_i}:{wx_s}[{tag}]")
        l_wx = "None"
        r_wx = "None"
        for wx, pts, si in candidates:
            if left  is not None and id(pts) == id(left):  l_wx = f"s{si} {wx:+.1f}cm"
            if right is not None and id(pts) == id(right): r_wx = f"s{si} {wx:+.1f}cm"
        print(f"[assign_dbg] {' | '.join(parts)}  ->  L={l_wx}  R={r_wx}",
              flush=True)
    _assign_debug_counter += 1
    return left, right


def calc_lateral_deviation_cm(lanes, calib, img_h):
    """
    Tier 2 + Tier 1 robustness pipeline:
      Tier 2 — polyfit + outlier rejection (rejeita riscas de passadeira).
      Tier 2 (extended) — lane assignment by world-X (corrige slot swap em curva).
      Tier 1 C — Cobertura vertical mínima.
      Tier 1 A — Em single-line, exige pontos perto do carro.
    """
    # Tier 2 + assignment by world-X (substitui o uso directo de lanes[1], lanes[2])
    lane_left, lane_right = _assign_ego_lanes(lanes, calib)

    bottom_threshold = img_h * MIN_BOTTOM_Y_RATIO

    def has_span(pts):
        if not pts or len(pts) < 3:
            return False
        ys = [p[1] for p in pts]
        return (max(ys) - min(ys)) >= MIN_LINE_SPAN_PX

    def has_bottom_coverage(pts):
        if not pts:
            return False
        return sum(1 for p in pts if p[1] >= bottom_threshold) >= MIN_BOTTOM_POINTS

    def bottom_n(pts, n=5):
        return sorted(pts, key=lambda p: p[1], reverse=True)[:n]

    # C: span check (aplicado sobre inliers do polyfit)
    left_ok  = has_span(lane_left)
    right_ok = has_span(lane_right)

    # A: em single-mode, exige cobertura na zona perto do carro
    if left_ok and not right_ok and not has_bottom_coverage(lane_left):
        return None, "none"
    if right_ok and not left_ok and not has_bottom_coverage(lane_right):
        return None, "none"
    if not left_ok and not right_ok:
        return None, "none"

    left_pts  = bottom_n(lane_left)  if left_ok  else None
    right_pts = bottom_n(lane_right) if right_ok else None
    has_left  = left_pts  is not None
    has_right = right_pts is not None

    def _lookahead_world_x(lane_pts):
        """
        Look-ahead world_x: fit polyfit, avalia a um Y mais alto que o bottom
        (LOOKAHEAD_Y_RATIO da gama de Ys). Captura "para onde a lane vai", não
        apenas onde está agora. Em curva, este sinal tem o sinal CORRECTO para
        o LKA virar na direção da curva.
        Fallback: mean(bottom 5 pts) se polyfit falhar.
        """
        if not lane_pts:
            return None
        arr = np.asarray(lane_pts, dtype=np.float32)
        xs, ys = arr[:, 0], arr[:, 1]
        # Fallback: insufficient pts/span
        if len(lane_pts) < POLYFIT_DEG + 2 or (ys.max() - ys.min()) < POLYFIT_MIN_Y_SPAN:
            bp = sorted(lane_pts, key=lambda p: p[1], reverse=True)[:5]
            w  = calib.pixels_to_world(bp)
            return float(np.mean(w[:, 0])) if w is not None and len(w) else None
        try:
            coeffs = np.polyfit(ys, xs, POLYFIT_DEG)
        except (np.linalg.LinAlgError, ValueError, TypeError):
            bp = sorted(lane_pts, key=lambda p: p[1], reverse=True)[:5]
            w  = calib.pixels_to_world(bp)
            return float(np.mean(w[:, 0])) if w is not None and len(w) else None
        # Look-ahead Y: ratio da gama, do bottom para o topo
        y_lookahead = float(ys.max() - LOOKAHEAD_Y_RATIO * (ys.max() - ys.min()))
        x_lookahead = float(np.polyval(coeffs, y_lookahead))
        w = calib.pixels_to_world([(x_lookahead, y_lookahead)])
        if w is None or len(w) == 0:
            return None
        return float(w[0, 0])

    left_x = right_x = None
    if has_left:
        left_x  = _lookahead_world_x(lane_left)
    if has_right:
        right_x = _lookahead_world_x(lane_right)
    # Safety: se polyfit falhou em ambas, fallback ao bottom mean (legacy)
    if has_left and left_x is None:
        left_world  = calib.pixels_to_world(left_pts)
        left_x      = float(np.mean(left_world[:, 0]))
    if has_right and right_x is None:
        right_world = calib.pixels_to_world(right_pts)
        right_x     = float(np.mean(right_world[:, 0]))

    if has_left and has_right:
        return -((left_x + right_x) / 2.0) - CAMERA_OFFSET_CM, "both"
    if has_left:
        return -(left_x + 15.0) - CAMERA_OFFSET_CM, "left"
    return -(right_x - 15.0) - CAMERA_OFFSET_CM, "right"


# ══════════════════════════════════════════════════════════════════════════════
# Tier 1B — Filtro de continuidade lateral
# ══════════════════════════════════════════════════════════════════════════════
class LateralContinuityFilter:
    """
    Rejeita leituras com salto lateral suspeito (> MAX_X_JUMP_CM por frame).
    Se rejeitar MAX_REJECT_STREAK frames seguidos, aceita o próximo para evitar
    ficar preso num valor antigo se a deviation realmente mudou de regime.
    Quando rejeita, devolve (None, "none") → o LaneHold a jusante mantém o valor
    anterior com decaimento.
    """
    def __init__(self, max_jump_cm=MAX_X_JUMP_CM, max_streak=MAX_REJECT_STREAK):
        self._max_jump   = max_jump_cm
        self._max_streak = max_streak
        self._last_dev   = None
        self._streak     = 0

    def filter(self, deviation, status):
        if status == "none" or deviation is None:
            return deviation, status
        if self._last_dev is None:
            self._last_dev = deviation
            self._streak   = 0
            return deviation, status
        if abs(deviation - self._last_dev) > self._max_jump:
            self._streak += 1
            if self._streak < self._max_streak:
                return None, "none"
            # streak excedido → assume que houve mudança real de regime
        self._last_dev = deviation
        self._streak   = 0
        return deviation, status


# ══════════════════════════════════════════════════════════════════════════════
# Lane Hold
# ══════════════════════════════════════════════════════════════════════════════
class LaneHold:
    """
    Quando status="none", mantém o último desvio válido por HOLD_FRAMES frames
    com decaimento multiplicativo. Devolve o status da última lane válida para
    o ADAS Manager não entrar em DEGRADED durante o hold.
    Ao esgotar o hold, devolve (None, "none") e deixa o ADAS degradar.
    """
    def __init__(self, hold_frames=HOLD_FRAMES, decay=HOLD_DECAY):
        self._hold_frames  = hold_frames
        self._decay        = decay
        self._last_dev     = 0.0
        self._last_status  = "none"
        self._hold_counter = 0

    def update(self, deviation, status):
        if status != "none":
            self._last_dev     = deviation if deviation is not None else 0.0
            self._last_status  = status
            self._hold_counter = 0
            return deviation, status

        if self._hold_counter < self._hold_frames:
            self._hold_counter += 1
            self._last_dev *= self._decay
            return self._last_dev, self._last_status

        return None, "none"


# ══════════════════════════════════════════════════════════════════════════════
# Writer assíncrono
# ══════════════════════════════════════════════════════════════════════════════
class AsyncVideoWriter:
    def __init__(self, path, fps, width, height):
        self._queue  = queue.Queue(maxsize=60)
        self._writer = cv2.VideoWriter(
            path, cv2.VideoWriter_fourcc(*"mp4v"), fps, (width, height))
        if not self._writer.isOpened():
            raise RuntimeError(f"Não foi possível criar: {path}")
        self._stop   = threading.Event()
        self._thread = threading.Thread(target=self._loop, daemon=True)
        self._thread.start()

    def _loop(self):
        while not self._stop.is_set() or not self._queue.empty():
            try:
                frame = self._queue.get(timeout=0.1)
                self._writer.write(frame)
            except Exception:
                continue

    def write(self, frame):
        try:
            self._queue.put_nowait(frame)
        except queue.Full:
            pass

    def release(self):
        self._stop.set()
        self._thread.join(timeout=10)
        self._writer.release()


# ══════════════════════════════════════════════════════════════════════════════
# Pré-processamento
# ══════════════════════════════════════════════════════════════════════════════
def preprocess_lane(frame):
    img = frame[_top_native:, :]
    return cv2.resize(img, (TRAIN_WIDTH, TRAIN_HEIGHT), interpolation=cv2.INTER_AREA)

def preprocess_yolo(frame):
    img = cv2.resize(frame, (MODEL_W, MODEL_H))
    return np.ascontiguousarray(cv2.cvtColor(img, cv2.COLOR_BGR2RGB).astype(np.uint8))


# ══════════════════════════════════════════════════════════════════════════════
# Post-processing YOLO
# ══════════════════════════════════════════════════════════════════════════════
def nms_boxes(boxes, scores, iou_threshold=0.45):
    if len(boxes) == 0:
        return []
    x1, y1, x2, y2 = boxes[:, 0], boxes[:, 1], boxes[:, 2], boxes[:, 3]
    areas = np.maximum(0.0, x2 - x1) * np.maximum(0.0, y2 - y1)
    order = scores.argsort()[::-1]
    keep  = []
    while order.size > 0:
        i = order[0]
        keep.append(i)
        xx1   = np.maximum(x1[i], x1[order[1:]])
        yy1   = np.maximum(y1[i], y1[order[1:]])
        xx2   = np.minimum(x2[i], x2[order[1:]])
        yy2   = np.minimum(y2[i], y2[order[1:]])
        inter = np.maximum(0.0, xx2 - xx1) * np.maximum(0.0, yy2 - yy1)
        iou   = inter / (areas[i] + areas[order[1:]] - inter + 1e-9)
        order = order[np.where(iou <= iou_threshold)[0] + 1]
    return keep


def decode_yolov8_outputs(raw_outputs, conf_thresh=CONF_THRESH, iou_thresh=IOU_THRESH):
    boxes_all, scores_all, cls_all = [], [], []

    for cv2_key, cv3_key, stride in YOLO_STREAMS:
        cv2_out    = raw_outputs[cv2_key][0]
        cv3_out    = raw_outputs[cv3_key][0]
        cls_scores = 1.0 / (1.0 + np.exp(-cv3_out))
        max_scores = cls_scores.max(axis=2)
        cls_ids    = cls_scores.argmax(axis=2)
        mask = max_scores >= conf_thresh
        if not mask.any():
            continue
        ys, xs = np.where(mask)
        reg = cv2_out[ys, xs, :].reshape(-1, 4, REG_MAX)
        reg = np.exp(reg - reg.max(axis=2, keepdims=True))
        reg = reg / reg.sum(axis=2, keepdims=True)
        reg = (reg * np.arange(REG_MAX)).sum(axis=2)
        cx  = (xs + 0.5) * stride
        cy  = (ys + 0.5) * stride
        x1  = np.clip((cx - reg[:, 0] * stride) / MODEL_W, 0, 1)
        y1  = np.clip((cy - reg[:, 1] * stride) / MODEL_H, 0, 1)
        x2  = np.clip((cx + reg[:, 2] * stride) / MODEL_W, 0, 1)
        y2  = np.clip((cy + reg[:, 3] * stride) / MODEL_H, 0, 1)
        boxes_all.append(np.stack([x1, y1, x2, y2], axis=1))
        scores_all.append(max_scores[ys, xs])
        cls_all.append(cls_ids[ys, xs])

    if not boxes_all:
        return np.empty((0, 4)), np.empty((0,)), np.empty((0,), dtype=int)

    boxes   = np.vstack(boxes_all)
    scores  = np.concatenate(scores_all)
    classes = np.concatenate(cls_all)

    w = boxes[:, 2] - boxes[:, 0]
    h = boxes[:, 3] - boxes[:, 1]
    size_mask = (w >= MIN_BOX_SIZE) & (h >= MIN_BOX_SIZE)
    boxes, scores, classes = boxes[size_mask], scores[size_mask], classes[size_mask]

    if len(boxes) == 0:
        return np.empty((0, 4)), np.empty((0,)), np.empty((0,), dtype=int)

    keep_final = []
    for cls_id in np.unique(classes):
        idx  = np.where(classes == cls_id)[0]
        keep = nms_boxes(boxes[idx], scores[idx], iou_thresh)
        keep_final.extend(idx[keep])

    if not keep_final:
        return np.empty((0, 4)), np.empty((0,)), np.empty((0,), dtype=int)

    keep_final              = np.array(keep_final)
    boxes, scores, classes  = boxes[keep_final], scores[keep_final], classes[keep_final]
    keep2 = nms_boxes(boxes, scores, iou_threshold=0.25)
    if not keep2:
        return np.empty((0, 4)), np.empty((0,)), np.empty((0,), dtype=int)

    return boxes[keep2], scores[keep2], classes[keep2]


def scale_boxes(boxes):
    if boxes.size == 0:
        return boxes.astype(int)
    b = boxes.copy()
    b[:, 0] *= FULL_W;  b[:, 2] *= FULL_W
    b[:, 1] *= FULL_H;  b[:, 3] *= FULL_H
    return b.astype(int)


# ══════════════════════════════════════════════════════════════════════════════
# Geometria câmara
# ══════════════════════════════════════════════════════════════════════════════
def get_theta_cam(cx_px: float) -> float:
    """Ângulo horizontal em graus. 0=frente, +=direita, -=esquerda."""
    image_center_x = FULL_W / 2 + CAMERA_OFFSET_PX
    return (cx_px - image_center_x) * (FOV_H / FULL_W)


# ══════════════════════════════════════════════════════════════════════════════
# Visualização
# ══════════════════════════════════════════════════════════════════════════════
def draw_obstacle_overlay(frame, boxes, scores, classes, obj_data):
    """
    Para cada detecção em OBSTACLE_VIS_CLASSES desenha:
      • linha do centro-baixo do carro até ao fundo do bounding box
      • label com distância em cm no ponto médio da linha
      • label com theta (ângulo) sobre o bounding box
      • arco no carro mostrando o ângulo em relação à frente
    boxes:    coordenadas normalizadas [0,1]
    obj_data: lista de (dist_m, theta) pré-calculados no loop principal
    """
    h, w = frame.shape[:2]
    car_cx     = int(w // 2 + CAMERA_OFFSET_PX)
    car_origin = (car_cx, h - 1)
    COLOR      = (0, 165, 255)

    for box_norm, score, cls_id, (dist_m, theta) in zip(boxes, scores, classes, obj_data):
        if int(cls_id) not in OBSTACLE_VIS_CLASSES:
            continue

        x1 = int(box_norm[0] * w)
        y1 = int(box_norm[1] * h)
        x2 = int(box_norm[2] * w)
        y2 = int(box_norm[3] * h)
        cx_px   = (x1 + x2) / 2.0
        dist_cm = dist_m * 100.0

        # Linha car-origin → fundo do bounding box
        target = (int(cx_px), y2)
        cv2.line(frame, car_origin, target, COLOR, 2)

        # Label distância no ponto médio
        mid_x = (car_origin[0] + target[0]) // 2
        mid_y = (car_origin[1] + target[1]) // 2
        d_label = f"{dist_cm:.0f} cm"
        (dw, dh), _ = cv2.getTextSize(d_label, cv2.FONT_HERSHEY_SIMPLEX, 0.55, 2)
        cv2.rectangle(frame, (mid_x + 3, mid_y - dh - 3), (mid_x + 5 + dw, mid_y + 3), (0, 0, 0), -1)
        cv2.putText(frame, d_label, (mid_x + 4, mid_y - 2),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.55, COLOR, 2)

        # Label theta sobre o bounding box
        t_label = f"ang: {theta:+.1f}deg"
        ty = max(y1 - 6, 14)
        (tw, th2), _ = cv2.getTextSize(t_label, cv2.FONT_HERSHEY_SIMPLEX, 0.55, 2)
        cv2.rectangle(frame, (x1, ty - th2 - 2), (x1 + tw + 4, ty + 3), (0, 0, 0), -1)
        cv2.putText(frame, t_label, (x1 + 2, ty),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.55, (0, 220, 255), 2)

        # Arco no carro mostrando o ângulo em relação à frente (270° = cima no OpenCV)
        arc_r    = 45
        arc_base = 270.0
        arc_end  = arc_base + theta
        cv2.ellipse(frame, car_origin, (arc_r, arc_r), 0,
                    min(arc_base, arc_end), max(arc_base, arc_end), COLOR, 2)
        ep_x = int(car_cx + arc_r * np.sin(np.radians(theta)))
        ep_y = int(h - 1  - arc_r * np.cos(np.radians(theta)))
        cv2.line(frame, car_origin, (ep_x, ep_y), COLOR, 1)

    return frame


def draw_detections(frame, boxes, scores, classes):
    font = cv2.FONT_HERSHEY_SIMPLEX
    for box, score, cls in zip(boxes, scores, classes):
        x1, y1, x2, y2 = box
        cv2.rectangle(frame, (x1, y1), (x2, y2), BOX_COLOR, 2)
        label = f"{cls}:{score:.2f}"
        (tw, th), baseline = cv2.getTextSize(label, font, 0.8, 2)
        ty = max(th + 4, y1 - 4)
        cv2.rectangle(frame, (x1, ty - th - baseline - 2), (x1 + tw + 4, ty + 2), (0, 0, 0), -1)
        cv2.putText(frame, label, (x1 + 2, ty - baseline), font, 0.8, TEXT_COLOR, 2)
    return frame


def draw_overlay_dual(frame, fps, frame_idx,
                      t_lane_pre, t_lane_hailo, t_lane_post, t_coords,
                      t_yolo_pre, t_yolo_hailo, t_yolo_post,
                      num_lanes, num_dets, deviation, status, calib_enabled,
                      hold_active=False):
    h, w = frame.shape[:2]
    overlay = frame.copy()
    cv2.rectangle(overlay, (0, 0), (235, 215), (0, 0, 0), -1)
    cv2.addWeighted(overlay, 0.5, frame, 0.5, 0, frame)

    dev_str   = (f"{deviation:+.1f}cm" if calib_enabled and deviation is not None
                 else f"{deviation:+.3f}" if deviation is not None else "N/A")
    hold_str  = " HOLD" if hold_active else ""
    dev_color = (0, 165, 255) if hold_active else (0, 255, 0) if status == "both" else (0, 165, 255)

    lines = [
        (f"FPS:           {fps:.1f}",                          (0, 255, 0)),
        ("── Lane ──────────────────",                         (180, 180, 180)),
        (f"  Pre:         {t_lane_pre:.1f}ms",                 (200, 200, 200)),
        (f"  Hailo:       {t_lane_hailo:.1f}ms",               (200, 200, 200)),
        (f"  Post:        {t_lane_post:.1f}ms",                (200, 200, 200)),
        (f"  Coords:      {t_coords:.1f}ms",                   (200, 200, 200)),
        (f"  Lanes:       {num_lanes}",                        (255, 255, 255)),
        (f"  Dev:         {dev_str} [{status}]{hold_str}",     dev_color),
        ("── YOLO ──────────────────",                         (180, 180, 180)),
        (f"  Pre:         {t_yolo_pre:.1f}ms",                 (200, 200, 200)),
        (f"  Hailo:       {t_yolo_hailo:.1f}ms",               (200, 200, 200)),
        (f"  Post:        {t_yolo_post:.1f}ms",                (200, 200, 200)),
        (f"  Detec.:      {num_dets}",                         (255, 255, 255)),
        (f"Frame: {frame_idx}",                                (255, 255, 255)),
    ]
    for i, (text, color) in enumerate(lines):
        cv2.putText(frame, text, (5, 16 + i * 14),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.33, color, 1)

    cv2.putText(frame, "SEAME | UFLDv2 + best_model (v15 — lane hold + decay)",
                (w - 310, h - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.3, (200, 200, 200), 1)
    return frame


# ══════════════════════════════════════════════════════════════════════════════
# Pipeline principal
# ══════════════════════════════════════════════════════════════════════════════
def run(frame_queue, duration_seconds=60, save_video=False, is_rear=False):
    start_socket_thread()
    W1, b1, W2, b2, keep_rows = load_weights(WEIGHTS_PATH)

    if _cpp_postprocess is not None:
        W1 = np.ascontiguousarray(W1, dtype=np.float32)
        b1 = np.ascontiguousarray(b1, dtype=np.float32)
        W2 = np.ascontiguousarray(W2, dtype=np.float32)
        b2 = np.ascontiguousarray(b2, dtype=np.float32)

    calib          = Calibration(is_rear=is_rear)
    lane_hold      = LaneHold()
    lateral_filter = LateralContinuityFilter()

    pp_backend = "C++ (OpenBLAS)" if _cpp_postprocess is not None else "Python (numpy)"
    print(f"\nFrame câmara:      {FULL_W}×{FULL_H} (via CameraBroker)")
    print(f"Lane modelo:       {TRAIN_WIDTH}×{TRAIN_HEIGHT} (undistort_small — crop+resize+undistort)")
    print(f"YOLO modelo:       {MODEL_W}×{MODEL_H} (best_model)")
    print(f"Undistort:         small frame {TRAIN_WIDTH}×{TRAIN_HEIGHT} (modelo) + re-distort pts (display)")
    print(f"PostProcess:       {pp_backend}")
    print(f"Overlap:           YPre/LHailo | LPost/YHailo")
    print(f"Socket:            DGRAM /tmp/adas_lane.sock → ADAS Manager")

    async_writer = None
    if save_video:
        async_writer = AsyncVideoWriter(OUTPUT_PATH, CAM_FPS, FULL_W, FULL_H)
        print(f"A gravar: {OUTPUT_PATH}")

    hef_lane = HEF(LANE_HEF_PATH)
    hef_yolo = HEF(YOLO_HEF_PATH)

    with VDevice() as target, ThreadPoolExecutor(max_workers=1) as executor:
        cfg_lane = ConfigureParams.create_from_hef(hef_lane, interface=HailoStreamInterface.PCIe)
        cfg_yolo = ConfigureParams.create_from_hef(hef_yolo, interface=HailoStreamInterface.PCIe)

        lane_ng = target.configure(hef_lane, cfg_lane)[0]
        yolo_ng = target.configure(hef_yolo, cfg_yolo)[0]

        lane_inp  = InputVStreamParams.make(lane_ng, format_type=FormatType.UINT8)
        lane_outp = OutputVStreamParams.make(lane_ng, format_type=FormatType.UINT8)
        yolo_inp  = InputVStreamParams.make(yolo_ng, format_type=FormatType.UINT8)
        yolo_outp = OutputVStreamParams.make(yolo_ng, format_type=FormatType.FLOAT32)

        print(f"\nA correr durante {duration_seconds}s — Ctrl+C para parar\n")
        print(f"{'Frame':<7} {'LPre':>6} {'LHailo':>7} {'LPost':>7} {'Coords':>7} "
              f"{'YPre':>6} {'YHailo':>7} {'YPost':>7} "
              f"{'Total':>7} {'Lanes':>6} {'Dets':>5} {'Dev':>9} {'Status'}")
        print("─" * 108)

        frame_idx         = 0
        t_start           = time.time()
        times_all         = []
        fps_acc           = []
        temporal_smoother = TemporalLaneSmoother()
        prev_status       = None
        single_lane_mode  = False

        try:
            while (time.time() - t_start) < duration_seconds:
                try:
                    frame = frame_queue.get(timeout=0.1)
                except queue.Empty:
                    continue

                t_frame_start = time.time()

                # ── LPre: undistort_small (crop+resize+undistort em 1 remap) ─
                t0 = time.time()
                if calib.enabled:
                    img_lane = calib.undistort_small(frame)
                else:
                    img_lane = preprocess_lane(frame)
                img_lane = np.ascontiguousarray(
                    cv2.cvtColor(img_lane, cv2.COLOR_BGR2RGB))
                t_lane_pre = (time.time() - t0) * 1000

                # ── YPre em paralelo com LHailo ───────────────────────────────
                if not SKIP_YOLO:
                    t0_ypre  = time.time()
                    fut_ypre = executor.submit(preprocess_yolo, frame)
                else:
                    fut_ypre = None

                t0 = time.time()
                with lane_ng.activate():
                    with InferVStreams(lane_ng, lane_inp, lane_outp) as pipe:
                        lane_out = pipe.infer({LANE_INPUT: img_lane[np.newaxis]})
                t_lane_hailo = (time.time() - t0) * 1000

                if fut_ypre is not None:
                    img_yolo   = fut_ypre.result()
                    t_yolo_pre = (time.time() - t0_ypre) * 1000
                else:
                    img_yolo   = None
                    t_yolo_pre = 0.0

                # ── LPost em paralelo com YHailo (C++ liberta GIL) ────────────
                _lane_out    = lane_out
                _single_lane = single_lane_mode

                def _run_lane_post(_lo=_lane_out, _slm=_single_lane):
                    t0p    = time.time()
                    conv37 = _lo[LANE_OUTPUT][0]
                    if _cpp_postprocess is not None:
                        loc, exist = _cpp_postprocess(conv37, W1, b1, W2, b2)
                        pts = _cpp_decode_lanes(loc, exist, _slm)
                    else:
                        loc, exist = postprocess(conv37, W1, b1, W2, b2, keep_rows)
                        pts = decode_lanes(loc, exist, single_lane_mode=_slm)
                    return pts, (time.time() - t0p) * 1000

                fut_lpost = executor.submit(_run_lane_post)

                if not SKIP_YOLO and img_yolo is not None:
                    t0 = time.time()
                    with yolo_ng.activate():
                        with InferVStreams(yolo_ng, yolo_inp, yolo_outp) as pipe:
                            yolo_out = pipe.infer({YOLO_INPUT: img_yolo[np.newaxis]})
                    t_yolo_hailo = (time.time() - t0) * 1000
                else:
                    yolo_out     = None
                    t_yolo_hailo = 0.0

                lanes, t_lane_post = fut_lpost.result()

                # ── Coords + temporal smoothing ───────────────────────────────
                t0                = time.time()
                lanes             = temporal_smoother.update(lanes)
                lanes             = smooth_lanes(lanes)
                if calib.enabled:
                    deviation, status = calc_lateral_deviation_cm(lanes, calib, FULL_H)
                else:
                    deviation, status = calc_lateral_deviation(lanes)

                # ── Tier 1B: continuidade lateral entre frames ────────────────
                deviation, status = lateral_filter.filter(deviation, status)
                t_coords          = (time.time() - t0) * 1000

                # ── Lane hold com decaimento ──────────────────────────────────
                raw_status        = status
                deviation, status = lane_hold.update(deviation, status)
                hold_active       = (raw_status == "none" and status != "none")

                # ── Publicar no ADAS Manager via socket ───────────────────────
                publish_deviation(deviation if deviation is not None else 0.0, status)

                # ── Re-distort para display ───────────────────────────────────
                lanes_disp = [calib.undistorted_to_distorted(l) for l in lanes]

                if prev_status == "both" and status in ("left", "right"):
                    single_lane_mode = True
                elif status == "both":
                    single_lane_mode = False
                prev_status = status

                # ── Post-processing YOLO ──────────────────────────────────────
                if yolo_out is not None:
                    t0                     = time.time()
                    boxes, scores, classes = decode_yolov8_outputs(yolo_out)
                    boxes_full             = scale_boxes(boxes)
                    t_yolo_post            = (time.time() - t0) * 1000
                else:
                    boxes       = np.empty((0, 4))
                    scores      = np.empty((0,))
                    classes     = np.empty((0,), dtype=int)
                    boxes_full  = boxes.astype(int)
                    t_yolo_post = 0.0

                # ── Publicar objectos no ADAS Manager ─────────────────────────
                obj_list = []
                obj_data = []   # (dist_m, theta) por detecção — partilhado com overlay
                for box, score, cls_id in zip(boxes, scores, classes):
                    sign_cls = YOLO_CLASS_MAP.get(int(cls_id), SIGN_UNKNOWN)
                    box_h_px = (box[3] - box[1]) * FULL_H
                    K        = _DIST_K_PER_CLASS.get(int(cls_id), _DIST_K)
                    dist_m   = K / box_h_px if box_h_px > 0 else 0.0
                    if dist_m < _DIST_MIN or dist_m > _DIST_MAX:
                        dist_m = 9999.0
                    cx_px    = ((box[0] + box[2]) / 2.0) * FULL_W
                    theta    = get_theta_cam(cx_px)
                    obj_data.append((dist_m, theta))
                    obj_list.append((sign_cls, float(score), dist_m, theta))
                publish_objects(obj_list)

                # ── Métricas ──────────────────────────────────────────────────
                t_total   = (time.time() - t_frame_start) * 1000
                num_lanes = sum(1 for l in lanes if l)
                num_dets  = len(boxes_full)
                times_all.append(t_total)

                fps_acc.append(1000.0 / (t_total + 1e-9))
                if len(fps_acc) > 30:
                    fps_acc.pop(0)
                fps = sum(fps_acc) / len(fps_acc)

                dev_str  = (f"{deviation:+.1f}cm" if calib.enabled and deviation is not None
                            else f"{deviation:+.3f}" if deviation is not None
                            else "   N/A")
                hold_tag = " HOLD" if hold_active else ""
                print(f"{frame_idx:<7} "
                      f"{t_lane_pre:>5.1f}ms "
                      f"{t_lane_hailo:>6.1f}ms "
                      f"{t_lane_post:>6.1f}ms "
                      f"{t_coords:>6.1f}ms "
                      f"{t_yolo_pre:>5.1f}ms "
                      f"{t_yolo_hailo:>6.1f}ms "
                      f"{t_yolo_post:>6.1f}ms "
                      f"{t_total:>6.1f}ms "
                      f"{num_lanes:>6} "
                      f"{num_dets:>5} "
                      f"{dev_str:>9} "
                      f"{status}{hold_tag}")

                if save_video and async_writer is not None:
                    frame_out  = draw_drivable_area(frame.copy(), lanes_disp)
                    frame_out  = draw_lanes(frame_out, lanes_disp)
                    frame_out  = draw_detections(frame_out, boxes_full, scores, classes)
                    frame_out  = draw_obstacle_overlay(frame_out, boxes, scores, classes, obj_data)
                    frame_out  = draw_overlay_dual(
                        frame_out, fps, frame_idx,
                        t_lane_pre, t_lane_hailo, t_lane_post, t_coords,
                        t_yolo_pre, t_yolo_hailo, t_yolo_post,
                        num_lanes, num_dets, deviation, status, calib.enabled,
                        hold_active)
                    async_writer.write(frame_out)

                frame_idx += 1

        except KeyboardInterrupt:
            print("\nInterrompido.")

    if async_writer is not None:
        print("\nA aguardar gravação...")
        async_writer.release()
        print(f"Vídeo guardado: {OUTPUT_PATH}")

    elapsed  = time.time() - t_start
    real_fps = frame_idx / elapsed if elapsed > 0 else 0
    avg      = sum(times_all) / len(times_all) if times_all else 0
    sorted_t = sorted(times_all) if times_all else [0]
    p50      = sorted_t[int(len(sorted_t) * 0.50)]
    p95      = sorted_t[int(len(sorted_t) * 0.95)]

    print("─" * 108)
    print(f"\nResumo ({frame_idx} frames em {elapsed:.1f}s):")
    print(f"   PostProcess:       {pp_backend}")
    print(f"   FPS real:          {real_fps:.1f}")
    print(f"   FPS pipeline puro: {1000/avg:.1f}" if avg > 0 else "")
    print(f"   Latência média:    {avg:.1f}ms")
    print(f"   P50:               {p50:.1f}ms")
    print(f"   P95:               {p95:.1f}ms")
    if save_video:
        print(f"   Vídeo guardado:    {OUTPUT_PATH}")


# ══════════════════════════════════════════════════════════════════════════════
# Entry point
# ══════════════════════════════════════════════════════════════════════════════
if __name__ == "__main__":
    sys.path.insert(0, "/opt/seame/adas")
    from camera_broker import CameraBroker

    p = argparse.ArgumentParser(description="SEAME Dual Inference v15 — socket → ADAS Manager")
    p.add_argument("duration", type=int, nargs="?", default=60)
    p.add_argument("--save",      action="store_true")
    p.add_argument("--camera",    type=int, default=1,
                   help="1=frente (default), 0=trás")
    p.add_argument("--flip",      type=int, default=None,
                   help="0=vertical, 1=horizontal, -1=180°")
    p.add_argument("--vis-class", type=int, nargs="+", default=None,
                   help="Classes com overlay distância/theta (default: usa OBSTACLE_VIS_CLASSES). "
                        "Ex: --vis-class 9  ou  --vis-class 8 9 11")
    args = p.parse_args()

    if args.vis_class is not None:
        OBSTACLE_VIS_CLASSES.clear()
        OBSTACLE_VIS_CLASSES.update(args.vis_class)

    cam_label = "TRASEIRA (flip=-1)" if args.camera == 0 else "FRONTAL"
    print(f"SEAME | Dual Inference v15: UFLDv2 (lane) + best_model (objects)")
    print(f"Câmara: {cam_label} (index={args.camera}) | {FULL_W}×{FULL_H} @ {CAM_FPS}fps")
    print(f"Overlay distância/theta: classes {sorted(OBSTACLE_VIS_CLASSES)}")
    print(f"PostProcess: C++ (OpenBLAS, sem GIL) | Overlap: YPre/LHailo + LPost/YHailo\n")

    broker = CameraBroker(width=FULL_W, height=FULL_H, fps=CAM_FPS, shutter=4000, gain=3.0,
                          camera=args.camera, flip=args.flip)
    q = broker.register("dual")
    broker.start()

    run(q, duration_seconds=args.duration, save_video=args.save,
        is_rear=(args.camera == 0))

    broker.stop()
