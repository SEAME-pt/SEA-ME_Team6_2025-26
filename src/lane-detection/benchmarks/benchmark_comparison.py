"""
benchmark_comparison.py
Compara as três implementações de post-processing:
  1. NumPy (baseline)
  2. C++ com OpenBLAS (ufldv2_postprocess.so)
  3. C++ com OpenCV  (ufldv2_postprocess_opencv.so)

Corre sem e com câmara activa para medir o impacto da contenção de RAM.

Uso:
    python3 benchmark_comparison.py \
        --weights  /data/weights.bin \
        --npz      /data/culane_postprocess_weights_mini.npz \
        --blas_so  /data/ufldv2_postprocess.so \
        --opencv_so /data/ufldv2_postprocess_opencv.so \
        --iters    200

SEAME Team 6
"""

import os
os.environ["OMP_NUM_THREADS"]      = "1"
os.environ["OPENBLAS_NUM_THREADS"] = "1"

import numpy as np
import ctypes
import time
import argparse

FC2_OUT  = 29376
FEAT_DIM = 4000

# ── Carregar biblioteca C++ ───────────────────────────────────────────────────

def load_lib(so_path, weights_bin):
    lib = ctypes.CDLL(so_path)
    lib.ufldv2_load_weights.restype  = ctypes.c_int
    lib.ufldv2_load_weights.argtypes = [ctypes.c_char_p]
    lib.ufldv2_get_fc2.restype  = ctypes.c_int
    lib.ufldv2_get_fc2.argtypes = [ctypes.POINTER(ctypes.c_uint8),
                                    ctypes.POINTER(ctypes.c_float)]
    rc = lib.ufldv2_load_weights(weights_bin.encode())
    if rc != 0:
        raise RuntimeError(f"ufldv2_load_weights falhou: {rc}")
    return lib

# ── Benchmark ─────────────────────────────────────────────────────────────────

def run_benchmark(name, fn, iters, fake_input):
    # warm-up
    for _ in range(10):
        fn(fake_input)

    times = []
    for _ in range(iters):
        t0 = time.perf_counter()
        fn(fake_input)
        times.append((time.perf_counter() - t0) * 1000.0)

    arr = np.array(times)
    return {
        "name":     name,
        "mean_ms":  float(arr.mean()),
        "p50_ms":   float(np.percentile(arr, 50)),
        "p95_ms":   float(np.percentile(arr, 95)),
        "p99_ms":   float(np.percentile(arr, 99)),
        "min_ms":   float(arr.min()),
        "fps":      float(1000.0 / arr.mean()),
    }

def print_result(r):
    print(f"\n  {r['name']}")
    print(f"    mean = {r['mean_ms']:.2f}ms   "
          f"p50 = {r['p50_ms']:.2f}ms   "
          f"p95 = {r['p95_ms']:.2f}ms   "
          f"p99 = {r['p99_ms']:.2f}ms   "
          f"FPS = {r['fps']:.1f}")

def print_table(results):
    print("\n" + "─" * 70)
    print(f"{'Implementação':<25} {'mean':>8} {'p50':>8} {'p95':>8} {'p99':>8} {'FPS':>7}")
    print("─" * 70)
    for r in results:
        print(f"{r['name']:<25} "
              f"{r['mean_ms']:>7.1f}ms "
              f"{r['p50_ms']:>7.1f}ms "
              f"{r['p95_ms']:>7.1f}ms "
              f"{r['p99_ms']:>7.1f}ms "
              f"{r['fps']:>6.1f}")
    print("─" * 70)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--weights",    default="/data/weights.bin")
    parser.add_argument("--npz",        default="/data/culane_postprocess_weights_mini.npz")
    parser.add_argument("--blas_so",    default="/data/ufldv2_postprocess.so")
    parser.add_argument("--opencv_so",  default="/data/ufldv2_postprocess_opencv.so")
    parser.add_argument("--iters",      type=int, default=200)
    args = parser.parse_args()

    print("=" * 70)
    print("  SEAME — Benchmark Post-Processing: NumPy vs OpenBLAS vs OpenCV")
    print("=" * 70)

    # ── Input falso ───────────────────────────────────────────────────────────
    fake = np.random.randint(0, 256, FEAT_DIM, dtype=np.uint8)
    fake_hwc = np.random.randint(0, 256, (10, 50, 8), dtype=np.uint8)

    # ── NumPy ─────────────────────────────────────────────────────────────────
    print("\nA carregar pesos NumPy...")
    w = np.load(args.npz)
    ln_weight = w["ln_weight"]
    ln_bias   = w["ln_bias"]
    W1 = w["W1"]; b1 = w["b1"]
    W2 = w["W2"]; b2 = w["b2"]

    def numpy_fn(hailo_out):
        fea      = hailo_out.astype(np.float32).transpose(2, 0, 1).flatten()
        mean     = fea.mean()
        var      = fea.var()
        fea_norm = (fea - mean) / np.sqrt(var + 1e-5)
        fea_norm = fea_norm * ln_weight + ln_bias
        fc1_out  = np.maximum(W1 @ fea_norm + b1, 0)
        return W2 @ fc1_out + b2

    # ── OpenBLAS C++ ──────────────────────────────────────────────────────────
    print("A carregar biblioteca OpenBLAS...")
    try:
        lib_blas = load_lib(args.blas_so, args.weights)
        buf_blas = (ctypes.c_float * FC2_OUT)()
        ptr      = fake.ctypes.data_as(ctypes.POINTER(ctypes.c_uint8))

        def blas_fn(hailo_out):
            arr = np.ascontiguousarray(hailo_out.flatten(), dtype=np.uint8)
            p   = arr.ctypes.data_as(ctypes.POINTER(ctypes.c_uint8))
            lib_blas.ufldv2_get_fc2(p, buf_blas)

        blas_ok = True
        print("  ✅ OpenBLAS carregado")
    except Exception as e:
        print(f"  ❌ OpenBLAS falhou: {e}")
        blas_ok = False

    # ── OpenCV C++ ────────────────────────────────────────────────────────────
    print("A carregar biblioteca OpenCV...")
    try:
        lib_cv = load_lib(args.opencv_so, args.weights)
        buf_cv = (ctypes.c_float * FC2_OUT)()

        def opencv_fn(hailo_out):
            arr = np.ascontiguousarray(hailo_out.flatten(), dtype=np.uint8)
            p   = arr.ctypes.data_as(ctypes.POINTER(ctypes.c_uint8))
            lib_cv.ufldv2_get_fc2(p, buf_cv)

        opencv_ok = True
        print("  ✅ OpenCV carregado")
    except Exception as e:
        print(f"  ❌ OpenCV falhou: {e}")
        opencv_ok = False

    # ── Correr benchmarks ─────────────────────────────────────────────────────
    print(f"\nA correr {args.iters} iterações por implementação...\n")

    results = []

    r = run_benchmark("NumPy", lambda x: numpy_fn(x), args.iters, fake_hwc)
    results.append(r)
    print_result(r)

    if blas_ok:
        r = run_benchmark("C++ OpenBLAS", lambda x: blas_fn(x), args.iters, fake_hwc)
        results.append(r)
        print_result(r)

    if opencv_ok:
        r = run_benchmark("C++ OpenCV", lambda x: opencv_fn(x), args.iters, fake_hwc)
        results.append(r)
        print_result(r)

    # ── Tabela comparativa ────────────────────────────────────────────────────
    print_table(results)

    # ── Comparação relativa ───────────────────────────────────────────────────
    if len(results) > 1:
        baseline = results[0]["mean_ms"]
        print("\n  Speedup relativo ao NumPy:")
        for r in results[1:]:
            ratio = baseline / r["mean_ms"]
            sinal = "mais rápido" if ratio > 1 else "mais lento"
            print(f"    {r['name']:<20} {ratio:.2f}x {sinal}")

    print("\n  NOTA: Estes valores são sem câmara activa.")
    print("  Para medir o impacto real, correr com a câmara a capturar em paralelo.")
    print("  Ver ADR-003 para análise completa da contenção de RAM.\n")


if __name__ == "__main__":
    main()
