import numpy as np
from collections import deque
from config import (
    QUANT_SCALE, QUANT_ZP,
    NUM_CELL_ROW, NUM_CELL_COL, NUM_ROW, NUM_COL, NUM_LANES,
    ROW_ANCHOR, ROW_LANE_LIST,
    EXIST_THRESHOLD, EXIST_THRESHOLD_TOP, EXIST_THRESHOLD_SINGLE, LOCAL_WIDTH,
    TEMPORAL_HISTORY, TEMPORAL_MIN_HITS, SPATIAL_WINDOW,
    FULL_W, FULL_H,
    CAMERA_OFFSET_NORM, LATERAL_DEVIATION_N, MIN_LANE_PTS,
)


# ── Pesos ──────────────────────────────────────────────────────────────────────
def load_weights(path):
    """
    Carrega os pesos das FC layers do post-processing.
    W1: (2048, 2000)  b1: (2048,)
    W2: (39576, 2048) b2: (39576,)
    """
    w  = np.load(path)
    W1 = w["W1"]
    b1 = w["b1"]
    W2 = w["W2"]
    b2 = w["b2"]
    print(f"[Weights] W1={W1.shape} b1={b1.shape} W2={W2.shape} b2={b2.shape}")
    return W1, b1, W2, b2


# ── Post-processing CPU ────────────────────────────────────────────────────────
def postprocess(conv37_uint8, W1, b1, W2, b2):
    """
    Recebe o tensor conv37 (10, 25, 8) UINT8 do Hailo e devolve os 4 slices
    flat prontos para decode_lanes.

    Pipeline:
        1. Dequantização: float = (uint8 - zp) * scale
        2. Transpose HWC→CHW: (10,25,8) → (8,10,25) para alinhar com ONNX
        3. Flatten → vector (2000,)
        4. FC1: W1 (2048×2000) @ x + b1 → ReLU
        5. FC2: W2 (39576×2048) @ x + b2
        6. Split em 4 slices: loc_row, loc_col, exist_row, exist_col
    """
    x = (conv37_uint8.astype(np.float32) - QUANT_ZP) * QUANT_SCALE
    x = x.transpose(2, 0, 1).flatten()  # (10,25,8)→(8,10,25)→(2000,)

    x = np.dot(W1, x) + b1
    x = np.maximum(x, 0)               # ReLU

    x = np.dot(W2, x) + b2

    # loc_row:   4 × 56 × 100 = 22400
    # loc_col:   4 × 41 × 100 = 16400
    # exist_row: 4 × 56 × 2   = 448
    # exist_col: 4 × 41 × 2   = 328
    return x[:22400], x[22400:38800], x[38800:39248], x[39248:39576]


# ── Decode ─────────────────────────────────────────────────────────────────────
def _softmax_local(arr, max_idx, local_width):
    """Softmax local em torno do argmax para estimativa sub-célula."""
    lo     = max(0, max_idx - local_width)
    hi     = min(len(arr) - 1, max_idx + local_width)
    ind    = np.arange(lo, hi + 1)
    scores = arr[ind]
    scores = np.exp(scores - scores.max())
    scores /= scores.sum()
    return float((scores * ind).sum()) + 0.5


def decode_lanes(loc_row_flat, loc_col_flat, exist_row_flat, exist_col_flat,
                 single_lane_mode=False):
    """
    Decodifica os 4 slices do FC2 para coordenadas (x, y) no frame original.
    Só lanes 1 e 2 (row-based) são calculadas.
    Devolve lista de 4 lanes, cada uma lista de (x, y).
    """
    loc_row   = loc_row_flat.reshape(NUM_CELL_ROW, NUM_ROW, NUM_LANES)
    exist_row = exist_row_flat.reshape(2, NUM_ROW, NUM_LANES)

    def exist_scores(exist):
        exp_e = np.exp(exist - exist.max(axis=0, keepdims=True))
        return exp_e / exp_e.sum(axis=0, keepdims=True)

    row_score = exist_scores(exist_row)  # (2, NUM_ROW, NUM_LANES)
    lanes = [[] for _ in range(NUM_LANES)]

    for lane_idx in ROW_LANE_LIST:
        pts = []
        for row_idx in range(NUM_ROW):
            if single_lane_mode:
                threshold = min(EXIST_THRESHOLD_SINGLE + 0.2, 0.8) if row_idx <= 20 else EXIST_THRESHOLD_SINGLE
            else:
                threshold = EXIST_THRESHOLD if row_idx > 20 else EXIST_THRESHOLD_TOP
            if row_score[1, row_idx, lane_idx] < threshold:
                continue
            max_idx = int(loc_row[:, row_idx, lane_idx].argmax())
            out_x   = _softmax_local(loc_row[:, row_idx, lane_idx], max_idx, LOCAL_WIDTH)
            x_full  = int(out_x / (NUM_CELL_ROW - 1) * FULL_W)
            y_full  = int(ROW_ANCHOR[row_idx] * FULL_H)
            pts.append((x_full, y_full))
        lanes[lane_idx] = pts

    return lanes


# ── Desvio lateral ─────────────────────────────────────────────────────────────
def calc_lateral_deviation(lanes):
    """
    Calcula o desvio lateral normalizado [-1.0, +1.0] a partir das ego-lanes.
    Usa os N pontos mais baixos (mais próximos do veículo) de cada lane.
    Aplica compensação do descentramento físico da câmara (CAMERA_OFFSET_NORM).

    Retorna (deviation, status) onde status é "both", "left", "right" ou "none".
    """
    frame_center = FULL_W / 2.0

    def bottom_mean_x(pts, n=LATERAL_DEVIATION_N):
        if len(pts) < MIN_LANE_PTS:
            return None
        sorted_pts = sorted(pts, key=lambda p: p[1], reverse=True)
        return float(np.mean([p[0] for p in sorted_pts[:n]]))

    x_left  = bottom_mean_x(lanes[1])
    x_right = bottom_mean_x(lanes[2])

    has_left  = x_left  is not None
    has_right = x_right is not None

    if not has_left and not has_right:
        return None, "none"

    if has_left and has_right:
        lane_width = x_right - x_left
        if lane_width < 50:
            return None, "none"
        lane_center = (x_left + x_right) / 2.0
        deviation   = (lane_center - frame_center) / (lane_width / 2.0)
        return float(np.clip(deviation - CAMERA_OFFSET_NORM, -1.0, 1.0)), "both"

    if has_left:
        deviation = -(frame_center - x_left) / frame_center
        return float(np.clip(deviation - CAMERA_OFFSET_NORM, -1.0, 1.0)), "left"

    deviation = (x_right - frame_center) / frame_center
    return float(np.clip(deviation - CAMERA_OFFSET_NORM, -1.0, 1.0)), "right"


# ── Smoothing ──────────────────────────────────────────────────────────────────
class TemporalLaneSmoother:
    """
    Suavização temporal por anchor.
    Um ponto só é emitido se aparecer em >= min_hits dos últimos `history` frames.
    O x emitido é a média dos valores válidos no histórico.
    """
    def __init__(self, history=TEMPORAL_HISTORY, min_hits=TEMPORAL_MIN_HITS):
        self.history  = history
        self.min_hits = min_hits
        self._hist = [[deque(maxlen=history) for _ in range(NUM_ROW)]
                      for _ in range(NUM_LANES)]
        self._y = [int(ROW_ANCHOR[r] * FULL_H) for r in range(NUM_ROW)]

    def update(self, lanes):
        smoothed = [[] for _ in range(NUM_LANES)]
        for lane_idx in ROW_LANE_LIST:
            y_to_x = {y: x for (x, y) in lanes[lane_idx]}
            for row_idx in range(NUM_ROW):
                self._hist[lane_idx][row_idx].append(y_to_x.get(self._y[row_idx]))
            pts = []
            for row_idx in range(NUM_ROW):
                valid = [x for x in self._hist[lane_idx][row_idx] if x is not None]
                if len(valid) >= self.min_hits:
                    pts.append((int(np.mean(valid)), self._y[row_idx]))
            smoothed[lane_idx] = pts
        return smoothed


def smooth_lane(pts, window=SPATIAL_WINDOW):
    """Suavização espacial: moving average no eixo x entre pontos adjacentes."""
    if len(pts) < 3:
        return pts
    xs = np.array([p[0] for p in pts], dtype=np.float32)
    ys = np.array([p[1] for p in pts], dtype=np.float32)
    pad       = window // 2
    xs_padded = np.pad(xs, pad, mode='edge')
    kernel    = np.ones(window) / window
    xs_smooth = np.convolve(xs_padded, kernel, mode='valid')
    return [(int(x), int(y)) for x, y in zip(xs_smooth, ys)]


def smooth_lanes(lanes):
    return [smooth_lane(lane) for lane in lanes]
