# Dual Inference Pipeline — Optimizações (v4 → v10)

## Resultados

| Versão | Optimização | Latência média | FPS real | FPS pipeline puro |
|--------|-------------|:--------------:|:--------:|:-----------------:|
| v4 | Baseline (sequencial) | ~77ms | ~10 | ~13 |
| v5 | Overlap YPre/LHailo (Python threads) | ~73ms | ~10 | ~13.7 |
| v6 | Multi-network scheduler (ROUND_ROBIN) | ~85ms | ~10.5 | ~11.7 |
| v7 | Undistort no frame pequeno (800×320) | ~71ms | ~10 | ~14.1 |
| v8 | v7 + Overlap YPre/LHailo | ~67ms | ~10–11 | ~14.9 |
| v9 | v8 + Overlap LPost/YHailo (Python) | ~76ms | ~10 | ~13.2 |
| **v10** | **v8 + Overlap LPost/YHailo (C++ OpenBLAS)** | **~72ms** | **~13.0** | **~13.9** |

> v6 descartado — multi-network scheduler divide os clusters do NPU e degrada a performance.
> v9 sem ganho real — GIL do CPython impedia o paralelismo efectivo entre threads Python.

---

## Optimizações activas no v10

| Optimização | Versão introduzida | Poupança |
|-------------|:-----------------:|:--------:|
| Undistort no frame pequeno (800×320) | v7 | ~6ms |
| Overlap YPre/LHailo (Python thread) | v8 | ~4ms |
| Overlap LPost/YHailo (C++ + OpenBLAS, sem GIL) | v10 | ~13–18ms |

> v9 tentou o overlap LPost/YHailo em Python puro — o GIL do CPython tornava-o ineficaz. O v10 resolve com extensão C++.

---

## Descrição das Optimizações

### v5 — Overlap YPre/LHailo
O pré-processamento do YOLO (resize + cvtColor, ~4ms CPU) é submetido a uma `ThreadPoolExecutor` antes da inferência lane no Hailo. O Hailo bloqueia o main thread ~25ms, tempo suficiente para o CPU completar o YPre em paralelo.

```
LHailo  [=======25ms=======]
YPre         [==4ms==]          ← paralelo, escondido
```

**Poupança: ~4ms/frame.**

---

### v7 — Undistort no frame pequeno
A matriz de câmara intrínseca é reescalada para o espaço do modelo (800×320):

```python
scale_x = TRAIN_WIDTH  / FULL_W          # 800 / 820
scale_y = TRAIN_HEIGHT / (FULL_H - top)  # 320 / (616 - top)
K_small[0,0] *= scale_x   # fx
K_small[1,1] *= scale_y   # fy
K_small[0,2] *= scale_x   # cx
K_small[1,2]  = (cy - top) * scale_y
```

O `cv2.remap` é aplicado directamente no frame 800×320 (~1ms) em vez do frame completo 820×616 (~6.8ms).

**Poupança: ~6ms/frame.**

---

### v10 — Extensão C++ + Overlap LPost/YHailo

#### Problema no v9
O post-processing do UFLDv2 (FC1→ReLU→FC2→decode, ~13ms) foi submetido à thread worker durante o YHailo, mas o **GIL** do CPython impedia o paralelismo real — numpy mantinha o GIL durante operações curtas, causando fragmentação e aumentando o tempo efectivo para 16–30ms.

#### Solução: extensão C++ com pybind11 + OpenBLAS
O post-processing foi reescrito em C++ (`postprocess_cpp`):

- **`cblas_sgemv`** (OpenBLAS) para FC1 (2048×2000) e FC2 (11424×2048)
- **`py::gil_scoped_release`** liberta o GIL durante o cálculo BLAS → o main thread (a correr YHailo) não bloqueia
- **`openblas_set_num_threads(4)`** aproveita os 4 cores CORTEXA76

```
          Main thread                  Worker thread (C++)
          ───────────────────          ────────────────────
LHailo    [======26ms======]
YPre           [==4ms==]              ← paralelo com LHailo
YHailo                    [====32ms====]
LPost (C++)               [====18ms====]  ← paralelo com YHailo (GIL livre)
YPost                                     [=2ms=]
          ─────────────────────────────────────────────────
Total     ≈ 72ms
```

**Poupança: ~13–18ms/frame (LPost escondido dentro do YHailo).**

---

## Bottleneck Actual

| Componente | Tempo | % do Total |
|------------|:-----:|:----------:|
| LHailo (Hailo-8 NPU) | ~26ms | 36% |
| YHailo (Hailo-8 NPU) | ~32ms | 44% |
| LPre + YPre + YPost + Coords | ~14ms | 20% |
| **LPost C++ (overlap)** | **~18ms** | **0%*** |

*escondido dentro do YHailo

O bottleneck passou do CPU para o **Hailo-8** (LHailo + YHailo ≈ 58ms dos 72ms totais). Não é optimizável por software — requereria hardware adicional ou um modelo mais leve.
