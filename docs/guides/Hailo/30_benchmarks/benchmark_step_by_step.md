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
