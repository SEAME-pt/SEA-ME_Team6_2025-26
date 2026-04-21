# E2E 1-Day Checklist (14 April 2026) — Copy/Paste Edition

> **Objetivo:** Comparação E2E real de Opções B (UFLDv2 + YOLO26n-seg) e C (UFLDv2 + YOLOv8n-seg) em 1 dia  
> **Participantes:** João (YOLO26n + YOLOv8n E2E), colega (UFLDv2), colega (pós-processamento + render)  
> **Dataset:** colega (pista real com labels passadeiras, setas, sinais)  
> **Duração total:** 9h30 (8h00–17h30)  
> **Status:** PRONTO PARA EXECUTAR ✅

---

## PRÉ-CHECK (antes de começar — 5 min)

```bash
# Terminal 1 (Lenovo/Host)
cd ~/Documents/AI/hailo/shared_with_docker

# Verificar HEFs existem
ls -lh hef/
# OUTPUT ESPERADO:
# yolov8n_seg_h8.hef (baseline)
# yolo26n_seg_320_h8_no_nms.hef (split hybrid)

# Verificar dataset colega
ls -lh /path/to/colega/dataset/images | head -5
# OUTPUT ESPERADO: 50+ imagens com nomes tipo "pista_001.jpg"

# Terminal 2 (AGL/Rasp5)
ssh root@<AGL_IP>
cd /data

# Validar HEFs em AGL
hailortcli parse-hef yolov8n_seg_h8.hef
hailortcli parse-hef yolo26n_seg_320_h8_no_nms.hef
# OUTPUT ESPERADO: "Model parsed successfully" para ambos
```

---

## BLOCO 0: Pré-requisitos (0h30) — 8h00–8h30

### Task 0.1: Freezar protocolo

```markdown
**Resolução:** 640x640
**Cenários:**
  - Normal: pista reta, iluminação boa, sem sombras acentuadas
  - Degradado: curva acentuada, sombras, iluminação variável

**Thresholds (ficar com estes para toda a comparação):**
  - UFLDv2: conf=0.45, iou=0.5
  - YOLO26n-seg: conf=0.5, iou=0.6
  - YOLOv8n-seg: conf=0.5, iou=0.6

**Métricas a registar (por cenário, por modelo):**
  - FPS E2E: avg, min, max, StdDev
  - Latência: p50, p95, p99, jitter_max (max - p95)
  - CPU: avg, max durante corrida
  - Qualidade: FN em passadeira, FN em setas, FN em sinais
  - Drops/crashes: quantidade, timestamps
```

**Atribuição:** colega (UFLDv2), João+colega (validar dataset)

**Tempo:** 15 min

---

### Task 0.2: Preparar dataset

```bash
# Terminal 1 (Lenovo)
cd /path/to/colega/dataset

# Contar imagens totais
find . -name "*.jpg" -o -name "*.png" | wc -l
# ESPERADO: 50+ imagens

# Validar estrutura (imagens + labels YOLO format)
ls images/ | head -5
ls labels/ | head -5

# Anotar:
# - Cenário Normal: 20-25 imagens (pista reta, luz boa)
# - Cenário Degradado: 20-25 imagens (curva+sombra)

echo "Normal_images=$(find images/ -name "*reta*" -o -name "*normal*" | wc -l)"
echo "Degraded_images=$(find images/ -name "*curva*" -o -name "*sombra*" | wc -l)"
```

**Atribuição:** colega (validar dataset), João (confirmar localização)

**Tempo:** 10 min

---

### Task 0.3: Preparar skeleton E2E

```bash
# Terminal 1 (Lenovo)
# Copiar pipeline de colega e adaptar para B e C

# Structure esperada (pseudo-código):
# pipeline_E2E.py:
#   ├─ def load_video(dataset_path)
#   ├─ def load_hef(hef_path)
#   ├─ def decode_and_preprocess(frame)
#   ├─ def infer_hailo(frame_preprocessed)
#   ├─ def postprocess_nms(infer_output, model_type)  # host-NMS se YOLO26 split
#   ├─ def render_output(frame, detections, masks)
#   ├─ def log_metrics(fps, latency_p50_p95_p99, cpu_avg_max, quality)
#   └─ def run_benchmark(dataset_path, hef_path, model_type, scenario)

# Copiar código de colega e adaptar
cp /path/to/colega/pipeline_E2E.py ./pipeline_YOLO26n_seg.py
cp /path/to/colega/pipeline_E2E.py ./pipeline_YOLOv8n_seg.py

# Adaptar para YOLO26n-seg:
# - Mudar hef_path → "yolo26n_seg_320_h8_no_nms.hef"
# - Ativar host-NMS (flag_host_nms=True)

# Adaptar para YOLOv8n-seg:
# - Mudar hef_path → "yolov8n_seg_h8.hef"
# - Desativar host-NMS (flag_host_nms=False)
```

**Atribuição:** João + colega

**Tempo:** 5 min

---

## BLOCO 1: E2E YOLO26n-seg (Opção B) — 8h30–11h30 (3h00)

### Task 1.1: Tentar compilação full INT8 (NOVO) — 8h30–9h00

```bash
# Terminal 1 (Lenovo/Docker)
cd ~/Documents/AI/hailo/shared_with_docker

docker run -v $PWD:/workspace hailo_custom_suite:latest bash -c "
  cd /workspace
  
  # Tentar compilação full INT8 (sem --mixed-precision)
  hailomz compile yolov8n_seg \
    --ckpt ./yolo26n-seg_320.onnx \
    --hw-arch hailo8 \
    --calib-path ./calibration_images \
    --start-node-names images \
    --end-node-names output0 output1 \
    -o ./hef/yolo26n_seg_320_h8_full_int8.hef \
    2>&1 | tee ./logs/compile_yolo26n_seg_320_h8_full_int8.log
"

# Verificar resultado
if grep -q "HEF file written" ./logs/compile_yolo26n_seg_320_h8_full_int8.log; then
  echo "✅ Full INT8 SUCCESS — copiar para AGL"
  YOLO26_HEF="yolo26n_seg_320_h8_full_int8.hef"
else
  echo "⚠️ Full INT8 FAILED — revert para split hybrid"
  YOLO26_HEF="yolo26n_seg_320_h8_no_nms.hef"
fi

echo "HEF a usar: $YOLO26_HEF"
```

**Atribuição:** João

**Tempo:** 30 min (15 min compile + 15 min decisão/cópia)

**Critério:** Se falhar >15 min, skip e usar split hybrid

---

### Task 1.2: Integrar infer + host-NMS + postprocess + render — 9h00–10h00

```bash
# Terminal 1 (Lenovo)
# Adaptar pipeline_YOLO26n_seg.py para:

# 1) Load HEF (escolher qual conforme Task 1.1)
hef_path = "/data/yolo26n_seg_320_h8_full_int8.hef" or "/data/yolo26n_seg_320_h8_no_nms.hef"
device = hailo_runtime(hef_path)

# 2) Decode + preprocess (já temos de colega)
frame = cv2.imread(image_path)
frame_pp = preprocess(frame)  # resize 640x640, normalize, etc.

# 3) Infer na Hailo
output_raw = device.infer(frame_pp)

# 4) Host-NMS (se split hybrid, caso contrário skip)
if YOLO26_HEF.contains("no_nms"):
    # NMS na CPU
    detections = cpu_nms(output_raw, conf_thresh=0.5, iou_thresh=0.6)
else:
    # Se full INT8, NMS já está no device
    detections = output_raw

# 5) Postprocess
boxes, masks, scores, classes = parse_yolo26_output(detections)
passadeira_masks = masks[classes == CLASS_PASSADEIRA]
setas_masks = masks[classes == CLASS_SETAS]

# 6) Render
frame_rendered = cv2.rectangle(frame, ...)  # boxes
frame_rendered = cv2.drawContours(frame_rendered, masks, ...)  # masks

# 7) Log métricas
log_metrics(fps, latency, cpu_usage, quality_scores)
```

**Atribuição:** João + colega

**Tempo:** 1h (debug pode alongar)

**Critério sucesso:** 10 frames sem crash, latência p50 < 50 ms

---

### Task 1.3: Teste estabilidade inicial — 10h00–10h45

```bash
# Terminal 2 (AGL)
cd /data

# Copiar HEF para AGL
scp <lenovo_user>@<lenovo_IP>:~/Documents/AI/hailo/shared_with_docker/hef/$YOLO26_HEF ./

# Validar HEF
hailortcli parse-hef $YOLO26_HEF
# ESPERADO: "Model parsed successfully"

# Terminal 1 (Lenovo)
# Rodar 10 frames de teste
python pipeline_YOLO26n_seg.py \
  --dataset /path/to/colega/dataset/images \
  --num_frames 10 \
  --hef_path /path/to/hef/$YOLO26_HEF \
  --scenario "test" \
  --output_log logs/test_yolo26n_seg.log

# Verificar output
tail -20 logs/test_yolo26n_seg.log
# ESPERADO:
# Frame 0: FPS=45, latency_p50=12ms, cpu_avg=25%
# Frame 1: FPS=45, latency_p50=12ms, cpu_avg=25%
# ... sem crashes
```

**Atribuição:** João

**Tempo:** 45 min (30 min debug se problemas)

**Critério go/no-go:**
- ✅ GO: 10 frames completaram, latência p50 < 50ms
- ❌ NO-GO: crashes, latência p50 > 60ms → fallback Task 2

---

### Task 1.4: Setup logging para Bloco 3 — 10h45–11h30

```bash
# Terminal 1 (Lenovo)
# Preparar scripts de logging com timestamp + métricas

cat > benchmark_logger.py <<'EOF'
import logging
import time
import psutil

class BenchmarkLogger:
    def __init__(self, scenario, model):
        self.scenario = scenario
        self.model = model
        self.metrics = []
        
    def log_frame(self, frame_id, fps, latency_list, cpu_usage, detections):
        p50 = sorted(latency_list)[len(latency_list)//2]
        p95 = sorted(latency_list)[int(0.95*len(latency_list))]
        p99 = sorted(latency_list)[int(0.99*len(latency_list))]
        
        entry = {
            "frame": frame_id,
            "scenario": self.scenario,
            "model": self.model,
            "fps": fps,
            "latency_p50_ms": p50,
            "latency_p95_ms": p95,
            "latency_p99_ms": p99,
            "cpu_avg": cpu_usage[0],
            "cpu_max": cpu_usage[1],
            "num_detections": len(detections),
            "timestamp": time.time()
        }
        
        self.metrics.append(entry)
        print(f"Frame {frame_id}: FPS={fps:.1f}, p95={p95:.2f}ms, CPU={cpu_usage[0]:.1f}%")
        
    def save_csv(self, output_path):
        import pandas as pd
        df = pd.DataFrame(self.metrics)
        df.to_csv(output_path, index=False)
        print(f"Saved: {output_path}")
EOF

python benchmark_logger.py
```

**Atribuição:** João

**Tempo:** 45 min (criar infrastructure de logging)

---

## BLOCO 2: E2E YOLOv8n-seg (Opção C) — 11h30–13h30 (2h00)

### Task 2.1: Reutilizar código de B, trocar HEF + desativar host-NMS — 11h30–12h30

```bash
# Terminal 1 (Lenovo)
# Copiar pipeline de B, adaptar para YOLOv8n-seg

cp pipeline_YOLO26n_seg.py pipeline_YOLOv8n_seg.py

# Editar pipeline_YOLOv8n_seg.py:
# - hef_path = "/data/yolov8n_seg_h8.hef"
# - flag_host_nms = False  # NMS é device-side, skip aqui

# Teste 10 frames
python pipeline_YOLOv8n_seg.py \
  --dataset /path/to/colega/dataset/images \
  --num_frames 10 \
  --hef_path /data/yolov8n_seg_h8.hef \
  --scenario "test" \
  --output_log logs/test_yolov8n_seg.log

tail -20 logs/test_yolov8n_seg.log
```

**Atribuição:** João

**Tempo:** 1h

---

### Task 2.2: Validar estabilidade device-side NMS — 12h30–13h15

```bash
# Terminal 1 (Lenovo)
# Rodar 20 frames, medir variância

python pipeline_YOLOv8n_seg.py \
  --dataset /path/to/colega/dataset/images \
  --num_frames 20 \
  --hef_path /data/yolov8n_seg_h8.hef \
  --scenario "stability_test" \
  --output_log logs/stability_yolov8n_seg.log

# Verificar jitter
grep "latency_p95" logs/stability_yolov8n_seg.log | \
  awk '{print $NF}' | \
  python3 -c "import sys; vals=[float(x) for x in sys.stdin]; print(f'p95 avg: {sum(vals)/len(vals):.2f}ms, std: {(sum((x-sum(vals)/len(vals))**2 for x in vals)/len(vals))**0.5:.2f}ms')"
# ESPERADO: std < 3ms (muito estável)
```

**Atribuição:** João

**Tempo:** 45 min

---

## BLOCO 3: Rodar 2 Cenários — 13h30–15h15 (1h45)

### Task 3.1: Cenário Normal — 13h30–14h00

```bash
# Terminal 1 (Lenovo)
# Rodar 50 frames (normal scenario: pista reta, boa iluminação)

python pipeline_YOLO26n_seg.py \
  --dataset /path/to/colega/dataset/images \
  --num_frames 50 \
  --filter_scenario "normal" \
  --hef_path /data/$YOLO26_HEF \
  --scenario "Normal" \
  --output_csv logs/benchmark_YOLO26n_seg_Normal.csv \
  --output_log logs/benchmark_YOLO26n_seg_Normal.log

# Verificar throughput
tail -5 logs/benchmark_YOLO26n_seg_Normal.log
```

**Atribuição:** João

**Tempo:** 30 min (20 min corrida + 10 min verificação)

---

### Task 3.2: Cenário Degradado — 14h00–14h30

```bash
# Terminal 1 (Lenovo)
# Rodar 50 frames (degraded: curva+sombra)

python pipeline_YOLO26n_seg.py \
  --dataset /path/to/colega/dataset/images \
  --num_frames 50 \
  --filter_scenario "degraded" \
  --hef_path /data/$YOLO26_HEF \
  --scenario "Degraded" \
  --output_csv logs/benchmark_YOLO26n_seg_Degraded.csv \
  --output_log logs/benchmark_YOLO26n_seg_Degraded.log

# Registar taxa de falha
grep "FN_" logs/benchmark_YOLO26n_seg_Degraded.log | \
  awk -F'[=,]' '{print $2}' > fn_rates.txt
python3 -c "import sys; vals=[float(x) for x in open('fn_rates.txt')]; print(f'Avg FN rate: {sum(vals)/len(vals):.2%}')"
```

**Atribuição:** João

**Tempo:** 30 min

---

### Task 3.3: Repetir YOLOv8n-seg em ambos cenários (se tempo) — 14h30–15h15

```bash
# Terminal 1 (Lenovo)
# Cenário Normal (YOLOv8n-seg)

python pipeline_YOLOv8n_seg.py \
  --dataset /path/to/colega/dataset/images \
  --num_frames 50 \
  --filter_scenario "normal" \
  --hef_path /data/yolov8n_seg_h8.hef \
  --scenario "Normal" \
  --output_csv logs/benchmark_YOLOv8n_seg_Normal.csv

# Cenário Degradado (YOLOv8n-seg)

python pipeline_YOLOv8n_seg.py \
  --dataset /path/to/colega/dataset/images \
  --num_frames 50 \
  --filter_scenario "degraded" \
  --hef_path /data/yolov8n_seg_h8.hef \
  --scenario "Degraded" \
  --output_csv logs/benchmark_YOLOv8n_seg_Degraded.csv
```

**Atribuição:** João

**Tempo:** 45 min (ou skip se sem tempo)

---

## BLOCO 4: Tabela Final + Decisão — 15h15–16h45 (1h30)

### Task 4.1: Compilar resultados em tabela única

```bash
# Terminal 1 (Lenovo)
python3 <<'EOF'
import pandas as pd
import numpy as np

# Carregar CSVs gerados

# Opção A (colega: UFLDv2+YOLOv8s) — conforme colega tiver enviado
try:
    df_a = pd.read_csv("logs/benchmark_A_UFLDv2_YOLOv8s.csv")
except:
    df_a = pd.DataFrame()  # placeholder

# Opção B (João: YOLO26n-seg)
df_b_normal = pd.read_csv("logs/benchmark_YOLO26n_seg_Normal.csv")
df_b_degraded = pd.read_csv("logs/benchmark_YOLO26n_seg_Degraded.csv")

# Opção C (João: YOLOv8n-seg)
try:
    df_c_normal = pd.read_csv("logs/benchmark_YOLOv8n_seg_Normal.csv")
    df_c_degraded = pd.read_csv("logs/benchmark_YOLOv8n_seg_Degraded.csv")
except:
    df_c_normal = df_c_degraded = pd.DataFrame()  # se não teve tempo

# Compilar tabela final
summary = {
    "Model": ["UFLDv2+YOLOv8s", "UFLDv2+YOLO26n-seg", "UFLDv2+YOLOv8n-seg"],
    "Scenario": ["Normal", "Normal", "Normal"],
    "FPS_avg": [
        df_a["fps"].mean() if len(df_a) > 0 else np.nan,
        df_b_normal["fps"].mean(),
        df_c_normal["fps"].mean() if len(df_c_normal) > 0 else np.nan
    ],
    "FPS_min": [
        df_a["fps"].min() if len(df_a) > 0 else np.nan,
        df_b_normal["fps"].min(),
        df_c_normal["fps"].min() if len(df_c_normal) > 0 else np.nan
    ],
    "FPS_max": [
        df_a["fps"].max() if len(df_a) > 0 else np.nan,
        df_b_normal["fps"].max(),
        df_c_normal["fps"].max() if len(df_c_normal) > 0 else np.nan
    ],
    "Latency_p95_ms": [
        df_a["latency_p95_ms"].mean() if len(df_a) > 0 else np.nan,
        df_b_normal["latency_p95_ms"].mean(),
        df_c_normal["latency_p95_ms"].mean() if len(df_c_normal) > 0 else np.nan
    ],
    "CPU_avg": [
        df_a["cpu_avg"].mean() if len(df_a) > 0 else np.nan,
        df_b_normal["cpu_avg"].mean(),
        df_c_normal["cpu_avg"].mean() if len(df_c_normal) > 0 else np.nan
    ],
    "CPU_max": [
        df_a["cpu_max"].max() if len(df_a) > 0 else np.nan,
        df_b_normal["cpu_max"].max(),
        df_c_normal["cpu_max"].max() if len(df_c_normal) > 0 else np.nan
    ]
}

df_summary = pd.DataFrame(summary)

print("="*100)
print("E2E BENCHMARK FINAL — 14 April 2026")
print("="*100)
print(df_summary.to_string(index=False))

# Salvar
df_summary.to_csv("logs/E2E_FINAL_SUMMARY.csv", index=False)
print("\nSaved: logs/E2E_FINAL_SUMMARY.csv")
EOF
```

**Atribuição:** João

**Tempo:** 30 min

---

### Task 4.2: Classificar Go/No-Go

```markdown
**Critérios:**

| Modelo | Latency p95 | CPU avg | FN rate (degraded) | Status |
|--------|-------------|---------|-------------------|--------|
| A (colega) | ? | ? | ? | Pending |
| B (YOLO26n-seg) | ~12-15 ms | ~25-30% | ~1-2% | GO produção se p95<50 |
| C (YOLOv8n-seg) | ~10-12 ms | ~20-25% | ~2-3% | GO produção se p95<50 |

**Decisão:**
- Se A < B < C em latência: manter A
- Se B < A e B < C: promover B (YOLO26n-seg)
- Se C < A e C < B: promover C (YOLOv8n-seg, mais simples)
- Se qualquer um > 60ms em p95: NO-GO, precisa otimização

**Fallback:**
- Se melhor não passar gate: usar A (já validado por colega)
```

**Atribuição:** João + colega + colega (decisão conjunta)

**Tempo:** 30 min

---

### Task 4.3: Documentar report final

```bash
# Terminal 1 (Lenovo)
cat > E2E_REPORT_14apr2026.md <<'EOF'
# E2E Benchmark Report — 14 April 2026

## Resumo

Testadas 3 opções de pipeline ADAS com E2E completo (decode→infer→postprocess→render):
- **A:** UFLDv2 + YOLOv8s (colega)
- **B:** UFLDv2 + YOLO26n-seg (João, split hybrid)
- **C:** UFLDv2 + YOLOv8n-seg (João, device NMS)

## Resultados

[Inserir tabela de E2E_FINAL_SUMMARY.csv aqui]

## Recomendação

[Preencher com decisão final + fallback]

## Notas Técnicas

- B usa split hybrid (host-NMS), overhead estimado 2-3ms
- C usa device NMS, mais estável, mas menos robustez a sombras
- Dataset: [descrição colega]
- Thresholds: UFLDv2 conf=0.45, YOLO conf=0.5

## Próximos Passos

1. Se B ganhar: integrar em produção
2. Se C ganhar: considerar retreino com Albumentations para robustez
3. Se nenhum passar gate: otimizar pós-processamento ou ajustar resolução
EOF

cat E2E_REPORT_14apr2026.md
```

**Atribuição:** João

**Tempo:** 30 min

---

## BUFFER: 1h (16h45–17h45)

- Debug de anomalias (se existirem)
- Ou: iniciar SegFormer (Opção D) se 3h+ sobrar
- Ou: reunião final com mentores

---

## CHECKLIST FINAL (17h45)

```bash
# Verificar todos artefactos foram gerados

[ ] logs/benchmark_YOLO26n_seg_Normal.csv
[ ] logs/benchmark_YOLO26n_seg_Degraded.csv
[ ] logs/benchmark_YOLOv8n_seg_Normal.csv (opcional)
[ ] logs/benchmark_YOLOv8n_seg_Degraded.csv (opcional)
[ ] logs/E2E_FINAL_SUMMARY.csv
[ ] E2E_REPORT_14apr2026.md
[ ] Decisão final documentada (GO / GO experimental / NO-GO)
[ ] Fallback claro se melhor modelo falhar

# Commit aos docs

git add docs/guides/Hailo/E2E_1DAY_CHECKLIST_14apr2026.md
git add docs/guides/Hailo/AI_implementation_planning.md (atualizado com seção 11E)
git add E2E_REPORT_14apr2026.md
git commit -m "E2E benchmark 14apr2026: B (YOLO26n-seg) vs C (YOLOv8n-seg) vs A (colega baseline)"
```

---

## Notas Importantes

1. **Se Task bloquear >30 min:** skip para próxima, registar como "bloqueado" no report
2. **Se B (YOLO26n-seg) não compilar full INT8:** usar split hybrid de 11D.10 imediatamente
3. **Se qualquer modelo crash:** registar erro + timestamp, tentar restart 1x
4. **Se dataset colega não chegar:** usar imagens sintéticas CARLA como fallback temporário
5. **Métricas críticas:** p95 latência E2E (gate 50ms), CPU avg (gate 30%), FN em passadeira (gate 1%)

---

**Data:** 14 April 2026  
**Status:** READY FOR EXECUTION ✅  
**Feedback:** João, colega, colega — alinhados?
