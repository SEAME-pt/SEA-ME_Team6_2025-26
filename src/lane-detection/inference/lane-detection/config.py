import numpy as np

# ── Paths ──────────────────────────────────────────────────────────────────────
HEF_PATH     = "/data/tusimple_res34_cut_v2.hef"
WEIGHTS_PATH = "/data/tusimple_postprocess_weights.npz"
OUTPUT_PATH  = "/data/demo_lane_detection.mp4"

# ── Câmara ─────────────────────────────────────────────────────────────────────
SENSOR_W  = 4608
SENSOR_H  = 2592
ROI_RATIO = 0.6

FULL_W  = 820
FULL_H  = 616
CAM_FPS = 20
ROI_Y   = 0
ROI_H   = FULL_H

# ── Modelo ─────────────────────────────────────────────────────────────────────
TRAIN_WIDTH  = 800
TRAIN_HEIGHT = 320
CROP_RATIO   = 0.8

MODEL_NAME   = "tusimple_res34_cut_v2"
INPUT_STREAM = f"{MODEL_NAME}/input_layer1"
OUTPUT_LAYER = f"{MODEL_NAME}/conv37"

# Dequantização — qp_scale e qp_zp extraídos do HEF compilado
QUANT_SCALE = 0.271904319524765
QUANT_ZP    = 157.0

# ── UFLDv2 TuSimple ────────────────────────────────────────────────────────────
NUM_ROW      = 56
NUM_COL      = 41
NUM_LANES    = 4
GRIDING_NUM  = 100
NUM_CELL_ROW = GRIDING_NUM
NUM_CELL_COL = GRIDING_NUM

# Row anchors: y=160→710 normalizado para 720px
ROW_ANCHOR = np.linspace(160, 710, NUM_ROW) / 720.0
# Col anchors: x=0→1 normalizado
COL_ANCHOR = np.linspace(0, 1, NUM_COL)

# Ego-lanes: 1=esquerda (row), 2=direita (row)
ROW_LANE_LIST = [1, 2]
COL_LANE_LIST = [0, 3]

# ── Detecção ───────────────────────────────────────────────────────────────────
EXIST_THRESHOLD        = 0.5   # threshold base de existência
EXIST_THRESHOLD_TOP    = 0.8   # threshold mais exigente para anchors do topo (row_idx <= 20)
EXIST_THRESHOLD_SINGLE = 0.4   # threshold reduzido para single-lane mode (curvas)
LOCAL_WIDTH            = 14    # janela do softmax local
MIN_LANE_PTS           = 3     # mínimo de pontos para considerar lane válida

# ── Smoothing ──────────────────────────────────────────────────────────────────
TEMPORAL_HISTORY  = 10   # frames de histórico temporal
TEMPORAL_MIN_HITS = 3    # mínimo de detecções válidas no histórico para manter ponto
SPATIAL_WINDOW    = 9    # janela de suavização espacial (moving average)

# ── Desvio lateral ─────────────────────────────────────────────────────────────
CAMERA_OFFSET_NORM   = 0.142   # offset da câmara normalizado (fallback pixel-based)
CAMERA_OFFSET_CM     = -3.2    # offset da câmara em cm (calibrado: carro centrado → desvio=0)
LATERAL_DEVIATION_N  = 10     # nº de pontos base usados no cálculo do desvio lateral

# ── Visualização ───────────────────────────────────────────────────────────────
LANE_COLORS = [
    (0,   0,   0),    # lane 0 — não usada
    (0,   255, 0),    # lane 1 — ego esquerda (verde)
    (0,   0,   255),  # lane 2 — ego direita (vermelho)
    (0,   0,   0),    # lane 3 — não usada
]
DRAW_MAX_Y_GAP = 40   # máxima diferença de y entre pontos consecutivos desenhados

# ── KUKSA ──────────────────────────────────────────────────────────────────────
KUKSA_HOST         = "10.21.220.191"
KUKSA_PORT         = 55555
KUKSA_CA_CERT      = "/etc/kuksa/tls/ca.crt"
KUKSA_TOKEN        = "/etc/kuksa/jwt/publisher.jwt"
LKA_DEVIATION_PATH = "Vehicle.ADAS.LaneKeepAssist.LateralDeviation"
LKA_STATUS_PATH    = "Vehicle.ADAS.LaneKeepAssist.LaneStatus"
