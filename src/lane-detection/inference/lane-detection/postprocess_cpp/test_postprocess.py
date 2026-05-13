"""
Verifica que postprocess_cpp dá os mesmos resultados que postprocess.py

Uso: python3 test_postprocess.py
"""
import sys
import os
import time
import numpy as np

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))
sys.path.insert(0, os.path.dirname(__file__))

from postprocess import load_weights, postprocess as py_postprocess, decode_lanes as py_decode
import postprocess_cpp as cpp

WEIGHTS_PATH = "/data/tusimple_postprocess_weights.npz"

print("A carregar pesos...")
W1, b1, W2, b2, keep_rows = load_weights(WEIGHTS_PATH)

# Garante arrays C-contiguous float32
W1 = np.ascontiguousarray(W1, dtype=np.float32)
b1 = np.ascontiguousarray(b1, dtype=np.float32)
W2 = np.ascontiguousarray(W2, dtype=np.float32)
b2 = np.ascontiguousarray(b2, dtype=np.float32)

# Input sintético
rng = np.random.default_rng(42)
conv37 = rng.integers(100, 200, size=(10, 25, 8), dtype=np.uint8)

# ── Correctness ───────────────────────────────────────────────────────────────
print("\n── Correctness ──────────────────────────────────────────────────")
py_loc, py_exist = py_postprocess(conv37, W1, b1, W2, b2, keep_rows)
cpp_loc, cpp_exist = cpp.postprocess(conv37, W1, b1, W2, b2)

loc_diff   = np.max(np.abs(py_loc   - cpp_loc))
exist_diff = np.max(np.abs(py_exist - cpp_exist))
print(f"loc   max diff: {loc_diff:.2e}   {'OK' if loc_diff < 1e-4 else 'FALHOU'}")
print(f"exist max diff: {exist_diff:.2e}  {'OK' if exist_diff < 1e-4 else 'FALHOU'}")

py_lanes  = py_decode(py_loc, py_exist)
cpp_lanes = cpp.decode_lanes(cpp_loc, cpp_exist)

all_match = True
for lane_idx in [1, 2]:
    py_pts  = py_lanes[lane_idx]
    cpp_pts = cpp_lanes[lane_idx]
    if py_pts != cpp_pts:
        print(f"Lane {lane_idx} DIVERGE: py={py_pts[:3]} cpp={cpp_pts[:3]}")
        all_match = False
    else:
        print(f"Lane {lane_idx}: {len(py_pts)} pontos — OK")

if all_match:
    print("Todos os pontos coincidem.")

# ── Benchmark ─────────────────────────────────────────────────────────────────
print("\n── Benchmark (100 iterações) ────────────────────────────────────")
N = 100

# Python
t0 = time.perf_counter()
for _ in range(N):
    loc, exist = py_postprocess(conv37, W1, b1, W2, b2, keep_rows)
    py_decode(loc, exist)
t_py = (time.perf_counter() - t0) / N * 1000

# C++
t0 = time.perf_counter()
for _ in range(N):
    loc, exist = cpp.postprocess(conv37, W1, b1, W2, b2)
    cpp.decode_lanes(loc, exist)
t_cpp = (time.perf_counter() - t0) / N * 1000

print(f"Python:  {t_py:.1f}ms/frame")
print(f"C++:     {t_cpp:.1f}ms/frame")
print(f"Speedup: {t_py/t_cpp:.1f}x")
