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

## Sprint 13 - Comparacao Real (`.pt` vs `HEF` em AGL/Hailo)

Data de consolidacao: 2026-05-07.

### Prova de que a Fase F correu em AGL + Hailo
- Execucao remota: `ssh root@10.21.220.191 "bash /data/yolo_benchmark/scripts/run_video_benchmark_sprint13.sh"`.
- Todos os artefactos de runtime estao em caminhos AGL: `/data/yolo_benchmark/...`.
- O runtime mostra streams do HEF e tempos de inferencia no acelerador (`Hailo medio`), por exemplo `10.5 ms`, `11.5 ms`, `14.5 ms`.

### Fase F (HEF no AGL/Hailo) - latencia e throughput

Valores abaixo sao medias dos 2 videos (`teste1` + `teste2`) com base nos logs finais da execucao.

| Modelo | Tipo | FPS real medio | FPS pipeline medio | Hailo medio (ms) | p50 medio (ms) | p95 medio (ms) | p99 medio (ms) |
|---|---|---:|---:|---:|---:|---:|---:|
| `yolov8s_detect_sprint13.hef` | detect | 28.3 | 31.7 | 14.4 | 31.4 | 36.2 | 40.5 |
| `yolo26n_detect_sprint13.hef` | detect | 46.0 | 55.4 | 10.5 | 17.8 | 22.2 | 25.3 |
| `yolov8n_seg_sprint13.hef` | seg (boxes) | 32.5 | 36.6 | 11.5 | 27.1 | 32.0 | 35.0 |
| `yolo26n_seg_sprint13.hef` | seg (boxes) | 39.2 | 45.2 | 14.5 | 21.9 | 26.1 | 28.6 |

Tabela por video (sem media):

| Modelo | Video | FPS real | FPS pipeline | Hailo medio (ms) | p50 (ms) | p95 (ms) | p99 (ms) |
|---|---|---:|---:|---:|---:|---:|---:|
| `yolov8s_detect_sprint13.hef` | `teste1` | 28.34 | 31.54 | 14.39 | 31.35 | 36.62 | 40.80 |
| `yolov8s_detect_sprint13.hef` | `teste2` | 28.31 | 31.76 | 14.33 | 31.30 | 35.81 | 40.19 |
| `yolo26n_detect_sprint13.hef` | `teste1` | 45.82 | 55.69 | 10.53 | 17.57 | 22.17 | 25.69 |
| `yolo26n_detect_sprint13.hef` | `teste2` | 46.20 | 55.15 | 10.53 | 17.86 | 22.14 | 24.93 |
| `yolov8n_seg_sprint13.hef` | `teste1` | 32.40 | 36.50 | 11.49 | 27.29 | 30.99 | 34.92 |
| `yolov8n_seg_sprint13.hef` | `teste2` | 32.51 | 36.56 | 11.40 | 26.84 | 32.88 | 35.06 |
| `yolo26n_seg_sprint13.hef` | `teste1` | 39.16 | 45.37 | 14.55 | 21.88 | 25.98 | 28.20 |
| `yolo26n_seg_sprint13.hef` | `teste2` | 39.09 | 45.11 | 14.53 | 21.94 | 26.23 | 28.86 |

### Fase C (`best.pt` no Lenovo) - runtime + cobertura + confidence

Esta secao usa uma re-execucao controlada em 2026-05-07 para obter os valores em falta (`phase_c_detailed_pt_metrics.json`).

| Modelo `.pt` | FPS real medio | Inferencia media (ms) | Confidence medio | Confidence p50 | Confidence p95 | Cobertura | Deteccoes/frame |
|---|---:|---:|---:|---:|---:|---:|---:|
| `yolov8s_detect` | 183.26 | 3.361 | 0.686 | 0.736 | 0.896 | 78.10% | 1.909 |
| `yolo26n_detect` | 183.35 | 3.961 | 0.685 | 0.746 | 0.950 | 70.89% | 1.851 |
| `yolov8n_seg` | 197.12 | 2.970 | 0.677 | 0.710 | 0.979 | 91.09% | 1.325 |
| `yolo26n_seg` | 152.73 | 4.810 | 0.651 | 0.650 | 0.987 | 77.59% | 1.391 |

Tabela por video (sem media):

| Modelo `.pt` | Video | FPS real | Inferencia media (ms) | Confidence medio | Confidence p50 | Confidence p95 | Cobertura | Deteccoes/frame |
|---|---|---:|---:|---:|---:|---:|---:|---:|
| `yolov8s_detect` | `teste1` | 166.98 | 3.410 | 0.688 | 0.733 | 0.889 | 72.84% | 2.109 |
| `yolov8s_detect` | `teste2` | 199.54 | 3.312 | 0.685 | 0.740 | 0.903 | 83.36% | 1.710 |
| `yolo26n_detect` | `teste1` | 178.89 | 3.994 | 0.671 | 0.700 | 0.943 | 72.16% | 2.194 |
| `yolo26n_detect` | `teste2` | 187.81 | 3.928 | 0.698 | 0.792 | 0.957 | 69.61% | 1.508 |
| `yolov8n_seg` | `teste1` | 196.14 | 2.960 | 0.670 | 0.744 | 0.981 | 93.72% | 1.509 |
| `yolov8n_seg` | `teste2` | 198.11 | 2.979 | 0.684 | 0.675 | 0.978 | 88.46% | 1.141 |
| `yolo26n_seg` | `teste1` | 152.58 | 4.769 | 0.609 | 0.577 | 0.983 | 87.78% | 1.725 |
| `yolo26n_seg` | `teste2` | 152.87 | 4.851 | 0.693 | 0.723 | 0.991 | 67.40% | 1.056 |

### Medicao de processo completo (`decode -> preprocess -> infer -> postprocess -> render`)

- `avg_total_ms` da Fase F cobre `preprocess + infer(Hailo) + postprocess`.
- `real_fps` da Fase F e o melhor proxy de E2E no estado atual porque inclui loop completo com leitura de frame, desenho e escrita.
- Para medicao E2E estrita por etapa (incluindo decode e render como tempos separados), o script deve passar a exportar `decode_ms` e `render_ms` por frame num proximo ajuste.

### Investigacao do `0.50` nos HEFs

Porque aparece tanto `0.50` nos videos HEF enquanto no `.pt` os scores variam mais:

1. O overlay arredonda para 2 casas (`{score:.2f}`), comprimindo variacao visual.
2. O threshold da Fase F (`conf=0.45`) deixa muitos scores perto do limiar.
3. A pipeline HEF esta quantizada (INT8), o que tende a concentrar logits perto de zero em alguns cenarios.
4. A calibracao BYOM foi feita com imagens `0..1`, mas o runtime envia `UINT8 0..255`; esta diferenca de escala pode enviesar confiancas para valores proximos do limiar.

Acao aplicada:
- `src/hailo/scripts/inference_video_sprint13.py` agora guarda no `*_stats.json`:
	- por frame: `mean_conf`, `p50_conf`, `p95_conf`
	- global: `mean_conf`, `p50_conf`, `p95_conf`

### Leitura direta
- Em runtime no AGL/Hailo, `yolo26n_detect` foi o mais rapido (melhor FPS e menor p95).
- Nos modelos `seg`, `yolo26n_seg` ficou mais rapido que `yolov8n_seg`.
- Na fase `.pt`, `yolov8n_seg` teve maior cobertura de frames com deteccao (91.1%) e melhor inferencia media.
