# Benchmark Step by Step (pós-runs)

Guia operacional para fechar a avaliação E2E dos modelos já corridos e atualizar a matriz final com dados comparáveis.

## 0) Regra base
- Não misturar `infer-only` com `E2E` na mesma conclusão.
- Decisão final usa `E2E` por cenário.

## 1) O que fazer agora (ordem recomendada)
1. Repetir runs E2E por cenário com os 3 modelos.
2. Registar logs de inferência + logs de CPU em paralelo.
3. Extrair métricas automáticas e completar as manuais.
4. Atualizar `yolo8s_vs_yolo8seg_vs_yolo26seg.md` e `benchmark_matrix.md`.
5. Só depois decidir se avança para treino com dataset da pista.

## 2) Cenários obrigatórios
- `normal`
- `degradado`
- `curva`
- `sombra`
- `cruzamento`

Executar pelo menos 1 run de 60s por cenário e por modelo.

## 3) Preparar pastas de resultados no AGL
```bash
mkdir -p /data/results/bench_runs/{yolov8s,yolov8n_seg,yolo26n_seg}
```

## 4) Correr o modelo e guardar log

### 4.1 YOLOv8s
```bash
python3 /data/scripts/inference_camera_scalercrop_yolov8s.py 60 --save --output /data/results/bench_runs/yolov8s/demo_yolov8s_normal.mp4 | tee /data/results/bench_runs/yolov8s/yolov8s_normal.log
```

### 4.2 YOLOv8n-seg
```bash
python3 /data/scripts/inference_camera_scalercrop_yolov8n_seg.py 60 --save --output /data/results/bench_runs/yolov8n_seg/demo_yolov8n_seg_normal.mp4 | tee /data/results/bench_runs/yolov8n_seg/yolov8n_seg_normal.log
```

### 4.3 YOLO26n-seg
```bash
python3 /data/scripts/inference_camera_scalercrop_yolo26n_seg.py 60 --save --output /data/results/bench_runs/yolo26n_seg/demo_yolo26n_seg_normal.mp4 | tee /data/results/bench_runs/yolo26n_seg/yolo26n_seg_normal.log
```

## 5) Registar CPU em paralelo (segunda shell)

### Preferido (`pidstat`)
```bash
pidstat -h -u -r -d 1 | tee /data/results/bench_runs/yolov8s/pidstat_yolov8s_normal.log
```

### Fallback (`top`)
```bash
top -b -d 1 | tee /data/results/bench_runs/yolov8s/top_yolov8s_normal.log
```

Repetir com nome de ficheiro do cenário/modelo correspondente.

## 6) Métricas: o que é automático vs manual

### Automático no script (estado atual)
- `YOLO26n-seg`: `FPS real`, `p50`, `p95`, `p99`, `jitter`, `drops`, `stats.csv`.
- `YOLOv8n-seg` e `YOLOv8s`: `FPS real`, `p50`, `p95` no resumo atual.

### Manual/external
- `CPU avg` e `CPU max` (via `pidstat`/`top`).
- Qualidade por classe (passadeira, setas, linhas, sinais).
- Estabilidade por cenário.

## 7) Extrair p99/jitter/fps mínimo de logs (quando faltar no resumo)
```bash
python3 - <<'PY'
import re, statistics, math
log_path = "/data/results/bench_runs/yolov8s/yolov8s_normal.log"
totals = []
with open(log_path, "r", errors="ignore") as file:
	for line in file:
		match = re.search(r'(\d+\.\d+)ms\s+(\d+\.\d+)ms\s+(\d+\.\d+)ms\s+(\d+\.\d+)ms', line)
		if match:
			totals.append(float(match.group(4)))

if not totals:
	print("Sem dados de latência no log")
	raise SystemExit

ordered = sorted(totals)
count = len(ordered)
def percentile(p):
	idx = max(0, min(count - 1, math.ceil((p / 100.0) * count) - 1))
	return ordered[idx]

fps_each = [1000.0 / value for value in totals if value > 0]
print("frames:", count)
print("p50:", round(percentile(50), 3), "ms")
print("p95:", round(percentile(95), 3), "ms")
print("p99:", round(percentile(99), 3), "ms")
print("jitter_std:", round(statistics.pstdev(totals), 3), "ms")
print("fps_avg_pipeline:", round(sum(fps_each) / len(fps_each), 3))
print("fps_min_pipeline:", round(min(fps_each), 3))
PY
```

## 8) Qualidade por classe (inspeção de vídeo)
Para cada vídeo, preencher:
- `FN passadeira`
- `FN setas`
- `FN linhas`
- `FN sinais`
- `estabilidade` (`estável`, `oscila`, `instável`)

Regra prática de FN:
- contar 1 FN quando um objeto visível esperado desaparece por >= 3 frames consecutivos.

## 9) Template único para preencher por cenário
| Modelo | Cenário | FPS avg E2E | FPS min E2E | p50 | p95 | p99 | CPU avg | CPU max | jitter | drops | FN passadeira | FN setas | FN linhas | FN sinais | Estabilidade |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---|
| YOLOv8s | normal |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| YOLOv8n-seg | normal |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
| YOLO26n-seg | normal |  |  |  |  |  |  |  |  |  |  |  |  |  |  |

## 10) Atualização final de documentos
Após fechar os 5 cenários:
1. Atualizar `yolo8s_vs_yolo8seg_vs_yolo26seg.md` com os novos números.
2. Atualizar `benchmark_matrix.md` com decisão GO/NO-GO por modelo.
3. Registar pendências (ex.: treino com dataset pista) no planeamento sprint.

## Ligações úteis
- `README.md`
- `benchmark_protocol.md`
- `benchmark_matrix.md`
- `yolo8s_vs_yolo8seg_vs_yolo26seg.md`

## 11) Sprint 13 fechado (2026-05-07)

### 11.1 Resultado da Fase F
- Benchmark completo executado no AGL (`root@10.21.220.191`) com HEFs em `/data/yolo_benchmark/models/hef/sprint13`.
- Runs completos: `4 modelos x 2 videos = 8 execucoes`.
- Saidas no AGL: `/data/yolo_benchmark/results/sprint13/*.mp4` e `*_stats.json`.

Resumo de desempenho (media dos 2 videos por modelo):

| Modelo | FPS real | p95 (ms) | p99 (ms) |
|---|---:|---:|---:|
| `yolov8s_detect_sprint13.hef` | 28.3 | 36.2 | 40.5 |
| `yolo26n_detect_sprint13.hef` | 46.0 | 22.2 | 25.3 |
| `yolov8n_seg_sprint13.hef` | 32.5 | 32.0 | 35.0 |
| `yolo26n_seg_sprint13.hef` | 39.2 | 26.1 | 28.6 |

Tabela por video (sem media):

| Modelo | Video | FPS real | p95 (ms) | p99 (ms) |
|---|---|---:|---:|---:|
| `yolov8s_detect_sprint13.hef` | `teste1` | 28.34 | 36.62 | 40.80 |
| `yolov8s_detect_sprint13.hef` | `teste2` | 28.31 | 35.81 | 40.19 |
| `yolo26n_detect_sprint13.hef` | `teste1` | 45.82 | 22.17 | 25.69 |
| `yolo26n_detect_sprint13.hef` | `teste2` | 46.20 | 22.14 | 24.93 |
| `yolov8n_seg_sprint13.hef` | `teste1` | 32.40 | 30.99 | 34.92 |
| `yolov8n_seg_sprint13.hef` | `teste2` | 32.51 | 32.88 | 35.06 |
| `yolo26n_seg_sprint13.hef` | `teste1` | 39.16 | 25.98 | 28.20 |
| `yolo26n_seg_sprint13.hef` | `teste2` | 39.09 | 26.23 | 28.86 |

### 11.1.1 Fase C (`.pt`) com valores detalhados

Foi executada uma passagem adicional para recolher os valores que faltavam (FPS/inferencia/confidence) e guardada em:
- `~/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_c_predictions/phase_c_detailed_pt_metrics.json`

Resumo de desempenho (media dos 2 videos por modelo):

| Modelo `.pt` | FPS real | Inferencia media (ms) | Confidence medio | Confidence p50 | Confidence p95 |
|---|---:|---:|---:|---:|---:|
| `yolov8s_detect` | 183.26 | 3.361 | 0.686 | 0.736 | 0.896 |
| `yolo26n_detect` | 183.35 | 3.961 | 0.685 | 0.746 | 0.950 |
| `yolov8n_seg` | 197.12 | 2.970 | 0.677 | 0.710 | 0.979 |
| `yolo26n_seg` | 152.73 | 4.810 | 0.651 | 0.650 | 0.987 |

Tabela por video (sem media):

| Modelo `.pt` | Video | FPS real | Inferencia media (ms) | Confidence medio | Confidence p50 | Confidence p95 |
|---|---|---:|---:|---:|---:|---:|
| `yolov8s_detect` | `teste1` | 166.98 | 3.410 | 0.688 | 0.733 | 0.889 |
| `yolov8s_detect` | `teste2` | 199.54 | 3.312 | 0.685 | 0.740 | 0.903 |
| `yolo26n_detect` | `teste1` | 178.89 | 3.994 | 0.671 | 0.700 | 0.943 |
| `yolo26n_detect` | `teste2` | 187.81 | 3.928 | 0.698 | 0.792 | 0.957 |
| `yolov8n_seg` | `teste1` | 196.14 | 2.960 | 0.670 | 0.744 | 0.981 |
| `yolov8n_seg` | `teste2` | 198.11 | 2.979 | 0.684 | 0.675 | 0.978 |
| `yolo26n_seg` | `teste1` | 152.58 | 4.769 | 0.609 | 0.577 | 0.983 |
| `yolo26n_seg` | `teste2` | 152.87 | 4.851 | 0.693 | 0.723 | 0.991 |

### 11.2 Copiar resultados da Fase F para o Lenovo

Destino local recomendado:

```bash
mkdir -p /home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl
```

Copiar todos os MP4 + JSON:

```bash
scp root@10.21.220.191:/data/yolo_benchmark/results/sprint13/*.mp4 \
	/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl/

scp root@10.21.220.191:/data/yolo_benchmark/results/sprint13/*_stats.json \
	/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl/
```

### 11.3 Porque o confidence no YOLOv8s aparece muitas vezes como `0.50`

Conclusao da investigacao:
- O overlay arredonda para 2 casas (`{score:.2f}`), o que achata variacao visual.
- O threshold de runtime (`conf=0.45`) deixa muitos scores junto ao limiar.
- O caminho HEF esta em INT8 e tende a concentrar scores mais perto de 0.5 do que o `.pt` FP.
- Existe risco adicional de mismatch de escala entre calibracao BYOM (`0..1`) e input runtime (`UINT8 0..255`), que pode empurrar scores para a zona do limiar.

Para medicao objetiva a partir de agora:
- O `inference_video_sprint13.py` foi atualizado para gravar `mean_conf/p50_conf/p95_conf` por frame e globais no `*_stats.json`.

### 11.4 Esta tabela e so inferencia ou E2E?

- `avg_total_ms` na Fase F mede `preprocess + infer + postprocess`.
- `real_fps` reflete o loop completo e funciona como proxy E2E (`decode + preprocess + infer + postprocess + render/write`).
- O runtime atual ja guarda `decode_ms`, `render_ms` e `stage_total_ms` por frame, e tambem `pipeline_fps_stage_total` no resumo.

### 11.5 Comparacao com script do Vasco (A/B no mesmo HEF)

Objetivo:
- Medir a diferenca de pos-processamento no mesmo artefacto (`yolov8s_detect_sprint13.hef`) para separar efeito de script vs efeito de modelo.

Baseline (nosso modo atual):
- `conf=0.45`, `iou=0.55`
- NMS por classe

Vasco-like (modo opcional no mesmo script):
- `conf=0.30`, `iou=0.40`
- `--min-box-size 0.04`
- `--global-nms-iou 0.25`

Pasta de saida no AGL:
- `/data/yolo_benchmark/results/fase_f_comparacao_com_vasco/`

Comando recomendado:

```bash
bash /home/seame/Documents/SEA-ME_Team6_2025-26/src/hailo/scripts/Vasquinho/run_phase_f_comparacao_com_vasco.sh
```

Script usado para modo Vasco-like:
- `src/hailo/scripts/Vasquinho/run_best_offline.py`
