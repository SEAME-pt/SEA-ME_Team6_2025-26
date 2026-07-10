# 30_benchmarks

Benchmark comparável e rastreável:
- protocolo de medição,
- métricas (FPS, latência, CPU, estabilidade),
- critérios de aceitação,
- tabelas de resultados.

## Documento principal

- `benchmark_protocol.md`
- `benchmark_step_by_step.md`
- `benchmark_matrix.md`
- `yolo8s_vs_yolo8seg_vs_yolo26seg.md`
- `../../Benchmark_3_modelos_2026-04-28.md` (Sprint 12: 3-model comparison on Vasco dataset)

## Regra-chave

Separar sempre:
- `infer-only` (ex.: `hailortcli benchmark`),
- `E2E` (câmara -> preprocess -> infer -> pós-process -> render/CAN).

## Estado atual (2026-04-21)
- `YOLO26n-seg` já exporta `p99/jitter/drops/stats.csv` no script.
- `YOLOv8n-seg` e `YOLOv8s` exigem parsing de log para fechar `p99/jitter/drops`.


