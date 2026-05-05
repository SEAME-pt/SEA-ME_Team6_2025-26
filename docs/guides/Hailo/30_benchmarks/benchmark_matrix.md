# Benchmark Matrix A/B/C/D

## Objetivo
Comparar as quatro opções E2E de forma justa e decidir qual deve seguir para a implementação final.

## Opções
- **A:** `UFLDv2 + YOLOv8s`
- **B:** `UFLDv2 + YOLO26n-seg`
- **C:** `UFLDv2 + YOLOv8n-seg`
- **D:** `SegFormer + YOLOv8s`

## O que medir
- `FPS` E2E médio e mínimo.
- Latência `p50`, `p95`, `p99`.
- `CPU` média e máxima.
- `jitter` e quedas por run.
- Qualidade: passadeira, setas, linhas, sinais.
- Estabilidade por cenário: normal, degradado, curva, sombra, cruzamento.

## Estado atual dos scripts (2026-04-21)
- `inference_camera_scalercrop_yolo26n_seg.py`: já exporta `p99`, `jitter`, `drops` e `*_stats.csv`.
- `inference_camera_scalercrop_yolov8n_seg.py`: resumo com `p50` e `p95`; `p99/jitter/drops` podem ser extraídos por parser de log.
- `inference_camera_scalercrop_yolov8s.py`: resumo com `p50` e `p95`; `p99/jitter/drops` podem ser extraídos por parser de log.

## Sequência recomendada após runs iniciais
1. Repetir E2E nos 5 cenários para os 3 modelos.
2. Guardar `run.log` + `pidstat.log` por cenário/modelo.
3. Extrair métricas faltantes dos logs (`p99`, `jitter`, `fps_min`).
4. Completar qualidade por classe por inspeção dos vídeos.
5. Atualizar decisão GO/NO-GO com base na tabela abaixo.

## Critérios mínimos
- `p95 <= 50 ms` para GO produção.
- `CPU avg <= 30%` como meta saudável.
- Falsos negativos baixos em passadeira e setas.
- Sem crashes e sem drops repetidos.

## Tabela
| Opção | Pipeline | FPS | p50 | p95 | p99 | CPU avg | CPU max | FN passadeira | FN setas | Estabilidade | Decisão |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---|---|
| A | UFLDv2 + YOLOv8s |  |  |  |  |  |  |  |  |  |  |
| B | UFLDv2 + YOLO26n-seg |  |  |  |  |  |  |  |  |  |  |
| C | UFLDv2 + YOLOv8n-seg |  |  |  |  |  |  |  |  |  |  |
| D | SegFormer + YOLOv8s |  |  |  |  |  |  |  |  |  |  |

## Métricas adicionais recomendadas (3.1)
- `RAM avg/max` por run.
- temperatura do SoC/Hailo durante o run.
- `% frames com latência > p95` (cauda temporal).
- taxa de falsos positivos por classe crítica.
- tempo de arranque do pipeline (cold start).
- reprodutibilidade: desvio entre 3 runs iguais.

## Leitura da matriz
- Se B der o melhor equilíbrio de semântica e latência, segue como principal.
- Se C for mais estável e suficientemente rápido, serve de fallback principal.
- Se D resolver passadeiras/setas com custo aceitável, entra como alternativa forte.
- A serve como baseline de controlo do colega.

## Onde ligar isto
- `yolo8s_vs_yolo8seg_vs_yolo26seg.md`
- `yolo_metrics_step_by_step.md`
- `../60_planning/current_sprint_plan.md`
- `../50_issues/UFLDv2_issues_and_possible_fixes.md`
- `benchmark_protocol.md`
