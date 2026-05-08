# YOLOv8s vs YOLOv8n-seg vs YOLO26n-seg

## Objetivo
Comparar os três scripts real-time testados no AGL com câmara CSI e `ScalerCrop`, usando os resultados reais dos runs de 60s.

## Resumo rápido
- `YOLO26n-seg` foi o mais rápido e o mais leve em latência.
- `YOLOv8n-seg` foi um bom equilíbrio entre rapidez e simplicidade do fluxo.
- `YOLOv8s` foi o mais pesado, mas serve como baseline sólido e já validado.

## Resultados medidos
| Modelo | HEF / modo | FPS real | FPS pipeline | Latência média | Latência mín | Latência máx | `p50` | `p95` | `p99` | Vídeo |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---|
| `YOLOv8s` | `640x640` / deteção | `29.0` | `49.5` | `20.2 ms` | `18.1 ms` | `24.8 ms` | `20.0 ms` | `22.1 ms` | `n/a` | `/data/demo_yolov8s_scalercrop.mp4` |
| `YOLOv8n-seg` | `640x640` / `Device-side NMS` | `29.0` | `67.6` | `14.8 ms` | `12.5 ms` | `21.6 ms` | `14.6 ms` | `16.9 ms` | `n/a` | `/data/results/demo_yolov8n_seg_rt.mp4` |
| `YOLO26n-seg` | `320x320` / `Host-side NMS` | `29.0` | `101.9` | `9.8 ms` | `9.1 ms` | `13.7 ms` | `9.7 ms` | `10.7 ms` | `n/a` | `/data/results/demo_yolo26n_seg_rt.mp4` |

## Leitura prática
- `YOLO26n-seg` é o melhor candidato se a prioridade for desempenho e margem de latência.
- `YOLOv8n-seg` é o candidato mais equilibrado se quiseres manter o ecossistema `YOLOv8`.
- `YOLOv8s` é o baseline de controlo para comparar qualidade e custo computacional.

## Observações importantes
- Estes runs medem bem o desempenho E2E, mas ainda não fecham a avaliação de qualidade semântica em cenários variados.
- A coluna `p99` ainda não foi extraída destes logs; deve ser acrescentada numa bateria de testes dedicada.
- Para decisão final, falta cruzar estes números com qualidade visual, estabilidade e comportamento em cenários degradados.

## O que ainda falta testar
- `CPU avg` e `CPU max` durante cada run.
- `jitter` e quedas por run.
- Qualidade por cenário: normal, degradado, curva, sombra e cruzamento.
- Métricas de falha por classe: passadeira, setas, linhas e sinais.

## Próximo passo operacional (após já teres corrido os modelos)
1. Repetir os 3 modelos nos 5 cenários fixos.
2. Guardar `run.log` + `pidstat.log` por cenário/modelo.
3. Extrair `p99`, `jitter` e `fps_min` dos logs quando não vierem no resumo.
4. Avaliar vídeos para FN por classe.
5. Atualizar esta tabela e a `benchmark_matrix.md`.

## Estado de instrumentação atual
- `YOLO26n-seg`: já imprime `p99`, `jitter`, `drops` e guarda `stats.csv`.
- `YOLOv8n-seg`: ainda depende de parsing de log para `p99/jitter/drops`.
- `YOLOv8s`: ainda depende de parsing de log para `p99/jitter/drops`.

## Ligação com a matriz principal
- `benchmark_matrix.md`
- `../60_planning/current_sprint_plan.md`
- `../50_issues/UFLDv2_issues_and_possible_fixes.md`

## Sprint 13 Update (2026-05-07) - Offline Video em AGL/Hailo

Execucao concluida para 4 modelos em 2 videos (`8 runs`) no AGL.

| Modelo | Tipo | FPS real medio | p95 medio | p99 medio | Observacao |
|---|---|---:|---:|---:|---|
| `yolov8s_detect_sprint13.hef` | detect | 28.3 | 36.2 ms | 40.5 ms | Baseline mais pesado em latencia |
| `yolo26n_detect_sprint13.hef` | detect | 46.0 | 22.2 ms | 25.3 ms | Melhor throughput global |
| `yolov8n_seg_sprint13.hef` | seg (boxes) | 32.5 | 32.0 ms | 35.0 ms | Mais estavel que o baseline detect |
| `yolo26n_seg_sprint13.hef` | seg (boxes) | 39.2 | 26.1 ms | 28.6 ms | Melhor seg em desempenho |

Detalhe por video (HEF em AGL/Hailo):

| Modelo | Video | FPS real | p95 | p99 |
|---|---|---:|---:|---:|
| `yolov8s_detect_sprint13.hef` | `teste1` | 28.34 | 36.62 ms | 40.80 ms |
| `yolov8s_detect_sprint13.hef` | `teste2` | 28.31 | 35.81 ms | 40.19 ms |
| `yolo26n_detect_sprint13.hef` | `teste1` | 45.82 | 22.17 ms | 25.69 ms |
| `yolo26n_detect_sprint13.hef` | `teste2` | 46.20 | 22.14 ms | 24.93 ms |
| `yolov8n_seg_sprint13.hef` | `teste1` | 32.40 | 30.99 ms | 34.92 ms |
| `yolov8n_seg_sprint13.hef` | `teste2` | 32.51 | 32.88 ms | 35.06 ms |
| `yolo26n_seg_sprint13.hef` | `teste1` | 39.16 | 25.98 ms | 28.20 ms |
| `yolo26n_seg_sprint13.hef` | `teste2` | 39.09 | 26.23 ms | 28.86 ms |

Comparacao com fase `.pt` (media dos 2 videos):

| Modelo `.pt` | FPS real | Inferencia media | Confidence medio | Confidence p50 | Confidence p95 |
|---|---:|---:|---:|---:|---:|
| `yolov8s_detect` | 183.26 | 3.361 ms | 0.686 | 0.736 | 0.896 |
| `yolo26n_detect` | 183.35 | 3.961 ms | 0.685 | 0.746 | 0.950 |
| `yolov8n_seg` | 197.12 | 2.970 ms | 0.677 | 0.710 | 0.979 |
| `yolo26n_seg` | 152.73 | 4.810 ms | 0.651 | 0.650 | 0.987 |

Detalhe por video (`.pt`):

| Modelo `.pt` | Video | FPS real | Inferencia media | Confidence medio | Confidence p50 | Confidence p95 |
|---|---|---:|---:|---:|---:|---:|
| `yolov8s_detect` | `teste1` | 166.98 | 3.410 ms | 0.688 | 0.733 | 0.889 |
| `yolov8s_detect` | `teste2` | 199.54 | 3.312 ms | 0.685 | 0.740 | 0.903 |
| `yolo26n_detect` | `teste1` | 178.89 | 3.994 ms | 0.671 | 0.700 | 0.943 |
| `yolo26n_detect` | `teste2` | 187.81 | 3.928 ms | 0.698 | 0.792 | 0.957 |
| `yolov8n_seg` | `teste1` | 196.14 | 2.960 ms | 0.670 | 0.744 | 0.981 |
| `yolov8n_seg` | `teste2` | 198.11 | 2.979 ms | 0.684 | 0.675 | 0.978 |
| `yolo26n_seg` | `teste1` | 152.58 | 4.769 ms | 0.609 | 0.577 | 0.983 |
| `yolo26n_seg` | `teste2` | 152.87 | 4.851 ms | 0.693 | 0.723 | 0.991 |

Nota:
- Nesta passagem, os modelos `seg` foram avaliados com render de boxes (nao com mascaras), porque o HEF BYOM nao inclui o head `cv4` de coeficientes de mascara.
- `avg_total_ms` e `real_fps` da Fase F devem ser lidos como benchmark de pipeline real; para E2E estrito por etapa falta separar `decode_ms` e `render_ms` no JSON.
