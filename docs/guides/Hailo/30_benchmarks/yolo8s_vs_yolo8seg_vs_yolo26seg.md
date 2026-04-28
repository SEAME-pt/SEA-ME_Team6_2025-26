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
