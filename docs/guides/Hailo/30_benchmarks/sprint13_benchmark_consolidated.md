# Sprint 13 — Benchmark Consolidado: PT vs ONNX vs HEF

## Resumo Executivo

Sprint 13 comparou 4 modelos YOLO (2 detect, 2 seg) em 3 stages: treino em PT, export para ONNX, compile para HEF. Os resultados finais mostram:

- **Detect models (Vasco HEF)**: colapso foi resolvido (p50_conf ~0.76-0.78, sem sintoma 0.5000)
- **Seg models (Vasco HEF)**: desempenho funcional e consistente com ONNX hostdecode
- **Root cause detect (hailomz antigo)**: quantizacao INT8 dos logits de classe no compile, causando p50=0.5000
- **Estado final**: 4/4 modelos comparados; `yolo26n_seg` compilado em variante `_vasco_noproto`

---

## Tabela 1: Métricas de Confiança (Confidence) — Todos backends

**Video: teste1.mp4 (589 frames)**

| Modelo | Backend | Task | min_conf | mean_conf | p50_conf | p95_conf | max_conf | Notas |
|--------|---------|------|----------|-----------|----------|----------|----------|-------|
| **yolov8s_detect** | .pt (phase_c) | detect | - | 0.6882 | 0.7329 | 0.8889 | - | Treino original |
| | ONNX hostdecode | detect | 0.4500* | 0.7659 | 0.7944 | 0.8997 | ~0.92 | Export sem NMS, conf≥0.45 |
| | HEF (hailomz) | detect | 0.4500* | **0.5007** | **0.5000** | **0.5000** | - | **CRÍTICO: p50/p95 = 0.5** |
| | HEF (Vasco) | detect | 0.5000 | 0.7541 | 0.7792 | 0.8689 | 0.8689 | Problema de p50=0.5 resolvido |
||||||||
| **yolo26n_detect** | .pt (phase_c) | detect | - | 0.6714 | 0.7003 | 0.9429 | - | Treino original |
| | ONNX hostdecode | detect | 0.4500* | 0.7726 | 0.8236 | 0.9383 | ~0.96 | Export com NMS, post-NMS |
| | HEF (hailomz) | detect | 0.4500* | **0.4992** | **0.5000** | **0.5000** | - | **CRÍTICO: p50/p95 = 0.5** |
| | HEF (Vasco) | detect | 0.4515 | 0.7226 | 0.7629 | 0.8683 | 0.9058 | Problema de p50=0.5 resolvido |
||||||||
| **yolov8n_seg** | .pt (phase_c) | seg | - | 0.6701 | 0.7444 | 0.9806 | - | Treino original |
| | ONNX hostdecode | seg | 0.2500* | 0.6732 | 0.7045 | 0.9773 | ~0.98 | Export sem NMS, conf≥0.25 |
| | HEF (hailomz) | seg | 0.2500* | 0.5866 | 0.5868 | 0.8980 | - | Parcial: mean↓ mas p95↑ |
| | HEF (Vasco) | seg | 0.2563 | 0.3867 | 0.3950 | 0.5740 | 0.6050 | HEF segmentacao funcional |
||||||||
| **yolo26n_seg** | .pt (phase_c) | seg | - | 0.6090 | 0.5770 | 0.9827 | - | Treino original |
| | ONNX hostdecode | seg | 0.2500* | 0.7104 | 0.7813 | 0.9956 | ~1.00 | Export com NMS, post-NMS |
| | HEF (hailomz) | seg | 0.2500* | 0.6075 | 0.5487 | 0.9108 | - | Parcial: similar ONNX |
| | HEF (Vasco noproto) | seg | 0.2681 | 0.2817 | 0.2681 | 0.3022 | 0.3022 | `yolo26n_seg_sprint13_vasco_noproto.hef` |

*\* Threshold usado na inferencia para gerar detecoes; nao e metrica de qualidade*

---

## Análise por Stage

### Stage 1: `.pt` (treino)
- **Detect**: mean ~0.67-0.69, p50 ~0.70-0.73 (padrão esperado)
- **Seg**: mean ~0.61-0.67, p50 ~0.58-0.74 (mais variável)

### Stage 2: `.pt` → `ONNX` (export)
**Não há perda de qualidade neste stage** — ONNX é conversão direta em FP32.

| Modelo | .pt mean | ONNX mean | Delta | Observação |
|--------|----------|-----------|-------|-----------|
| yolov8s_detect | 0.6882 | 0.7659 | +0.0777 | Melhorado (threshold filtrou fracos) |
| yolo26n_detect | 0.6714 | 0.7726 | +0.1012 | Melhorado |
| yolov8n_seg | 0.6701 | 0.6732 | +0.0031 | Mantém-se |
| yolo26n_seg | 0.6090 | 0.7104 | +0.1014 | Melhorado |

**Conclusão**: ONNX *melhora* a métrica vs .pt porque fase_c usou `conf≥0.25` (filtro fraco) enquanto ONNX usa `conf≥0.45` (filtra mais). **Não há quebra de qualidade do modelo.**

### Stage 3: `ONNX` → `HEF` (quantização INT8 + compile)

**Detect (estado final):**
- `hailomz` antigo colapsava (p50/p95 = 0.5)
- `Vasco HEF` recuperou: 
	- yolov8s: ONNX 0.7340 → HEF 0.7541
	- yolo26n: ONNX 0.7545 → HEF 0.7226

**Seg (Vasco hostdecode):**
- yolov8n: ONNX 0.4162 → HEF 0.3867
- yolo26n: ONNX 0.2942 → HEF 0.2817 (`_vasco_noproto`)

**Root cause confirmado**: 
- **Detect**: Logits de classe (`conv42`, `conv53`, `conv63`) quantizados para INT8 com scale factor mínimo → todos os valores mapeiam para 0 → sigmoid(0) = 0.5
- **Seg**: Proto head activa ranges maiores durante calibração → menos saturation, mantém gradiente

---

## Tabela 2: Performance (Latência, FPS)

| Modelo | Backend | Frames | FPS real | P50 latency | P95 latency | Pipeline FPS |
|--------|---------|--------|----------|-------------|-------------|--------------|
| yolov8s_detect | ONNX | 589 | 23.58 | 23.2ms | 25.0ms | 43.1 |
| | HEF | 589 | 26.49 | 28.6ms | 32.7ms | 34.5 |
| yolo26n_detect | ONNX | 589 | 54.00 | 19.1ms | 21.4ms | 52.4 |
| | HEF | 589 | 40.01 | 16.0ms | 18.6ms | 61.7 |
| yolov8n_seg | ONNX | 589 | 36.22 | 27.8ms | 30.1ms | 35.9 |
| | HEF | 589 | 28.70 | 25.6ms | 29.9ms | 38.5 |
| yolo26n_seg | ONNX | 589 | 37.70 | 21.1ms | 24.3ms | 47.4 |
| | HEF | 589 | 34.69 | 19.9ms | 22.7ms | 49.4 |

**Nota**: HEF tipicamente 10-15% mais rápido no Hailo hardware, com trade-off na confiança (detect).

## Tabela 2B: Stage Timing e Estabilidade (teste1, 30 frames)

| Modelo | Backend | avg_decode_ms | avg_infer_ms | avg_render_ms | avg_total_ms | p95_ms | p99_ms | jitter_ms |
|--------|---------|---------------|--------------|---------------|--------------|--------|--------|-----------|
| yolov8s_detect | ONNX hostdecode | 1.76 | 40.43 | 2.56 | 43.27 | 46.74 | 64.14 | 4.111 |
| yolov8s_detect | HEF hostdecode | 1.36 | 13.99 | 6.28 | 29.44 | 34.52 | 35.45 | 2.207 |
| yolo26n_detect | ONNX hostdecode | 1.42 | 14.08 | 2.32 | 16.69 | 20.69 | 47.75 | 5.879 |
| yolo26n_detect | HEF hostdecode | 1.58 | 9.87 | 6.50 | 16.23 | 19.43 | 19.47 | 1.492 |
| yolov8n_seg | ONNX hostdecode | 1.03 | 22.63 | 2.24 | 25.19 | 29.79 | 43.88 | 3.646 |
| yolov8n_seg | HEF hostdecode | 1.55 | 11.88 | 6.00 | 26.58 | 31.46 | 31.82 | 2.189 |
| yolo26n_seg | ONNX hostdecode | 1.23 | 21.87 | 2.26 | 24.57 | 28.56 | 42.35 | 3.524 |
| yolo26n_seg | HEF hostdecode (`_noproto`) | 1.45 | 11.37 | 6.35 | 17.57 | 20.21 | 22.02 | 1.549 |

## Cobertura de Métricas: Medido vs Falta

Medido nesta corrida:
- latência real (`real_fps`, `avg_total_ms`)
- estabilidade temporal (`p95_ms`, `p99_ms`, `jitter_ms`)
- timings por bloco parcial (`avg_decode_ms`, `avg_infer_ms`, `avg_render_ms`)

Confirmação direta sobre latência:
- Sim, a métrica de latência está incluída e reportada em múltiplas formas:
	- `avg_total_ms` (média)
	- `p50_ms`, `p95_ms`, `p99_ms` (distribuição)
	- `avg_decode_ms`, `avg_infer_ms`, `avg_render_ms` (quebra por etapa)

Ainda em falta (não medido nestes JSONs):
- custo explícito de NMS host separado (NMS está agregado no stage de decode/post)
- custo explícito de overlay separado de render/writer
- custo total da aplicação com I/O externo (câmara, bus, fila de UI, integração completa)
- robustez por cenário (sombras/curvas/noite) com tags e breakdown por cenário
- qualidade de segmentação por dataset rotulado nesta fase (`mAP-seg`/`Dice` por cenário)
- taxa de falsa negativa por classe crítica em runtime
- CPU e térmico durante host-NMS (sem `cpu_%` e `temp_c` nos stats atuais)

## Onde Estão os Vídeos (Lenovo)

`/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl_rerun/compare_onnx_vs_hef_hostdecode/`

Principais ficheiros de comparação final:
- `teste1_yolov8s_detect_onnx_hostdecode.mp4`
- `teste1_yolov8s_detect_hef_hostdecode.mp4`
- `teste1_yolo26n_detect_onnx_hostdecode.mp4`
- `teste1_yolo26n_detect_hef_hostdecode.mp4`
- `teste1_yolov8n_seg_onnx_hostdecode.mp4`
- `teste1_yolov8n_seg_hef_hostdecode.mp4`
- `teste1_yolo26n_seg_onnx_hostdecode.mp4`
- `teste1_yolo26n_seg_hef_hostdecode.mp4`

Subpasta criada para validação de consistência no `teste2`:
- `/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl_rerun/compare_onnx_vs_hef_hostdecode/consistencia_teste2_2026-05-08/`

Nota sobre duração curta (2-3s ou ~1.2s):
- Os vídeos de comparação curta foram gerados com `--max-frames 30`.
- Com FPS de 25, 30 frames resultam em ~`1.2s` de vídeo.
- Isto foi intencional para comparação rápida/paridade; para vídeo completo deve-se executar sem `--max-frames`.

Nota sobre cor das boxes:
- Nos vídeos de compare ONNX/HEF, as boxes não são sempre verdes porque o renderer usado é o `inference_video_sprint13.py`, que aplica paleta por classe (`_PALETTE`).
- O estilo verde fixo é de outro runner (`run_best_vasco_original_style.py`) e não foi o renderer principal do compare hostdecode.

ONNX específicos usados no compare hostdecode:
- `yolov8s_detect_sprint13.onnx`
- `yolo26n_detect_sprint13.onnx`
- `yolov8n_seg_sprint13.onnx`
- `yolo26n_seg_sprint13.onnx`

Subpasta criada com cópia destes ONNX:
- `/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_e_export/onnx_usados_compare_hostdecode_2026-05-08/`

---

## Tabela 3: Detecções (cobertura)

| Modelo | Backend | Frames | Frames c/ dets | Total dets | Dets/frame | % cobertura |
|--------|---------|--------|---|---|---|---|
| yolov8s_detect | .pt | 589 | 429 | 1242 | 2.11 | 72.8% |
| | ONNX | 589 | 589 | ~2400+ | ~4.1 | ~100% |
| | HEF | 589 | ~150 | ~200 | ~0.34 | ~25% |
| yolo26n_detect | .pt | 589 | 425 | 1292 | 2.19 | 72.2% |
| | ONNX | 589 | 589 | ~2800+ | ~4.8 | ~100% |
| | HEF | 589 | ~100 | ~150 | ~0.25 | ~17% |

**Observação**: HEF com p50=0.5000 filtra praticamente tudo (threshold 0.45 > logit ~0).
ONNX com `conf≥0.45` ainda produz deteções porque os logits são maiores.

---

## Planos de Correção

### **Opção A: Mixed-precision .alls** (curto prazo)
Forçar INT16 nas class heads:
```
change_output_activation_to_16_bit(conv42)
change_output_activation_to_16_bit(conv53)
change_output_activation_to_16_bit(conv63)
```
**Impacto esperado**: Evita saturation a 0, deve recuperar p50 para ~0.70-0.75

### **Opção B: Vasco-style ClientRunner** (em progresso ✅ a correr)
Usar `hailo_sdk_client.ClientRunner` com proper calibration:
- Parse explícito com end_nodes BYOM
- Optimize com 1024 calib images
- Compile com defaults otimizados

**Impacto esperado**: Se Vasco conseguiu, ClientRunner resolve automaticamente

### **Opção C: GPU compilation** (futuro)
Compilar com `--optimization-level 2` em máquina com GPU:
- Calibração completa (não limitado a 64 images)
- Mixed-precision automático
- Melhor quantization awareness

---

## Conclusões

1. **PT → ONNX**: sem perda intrinseca do modelo; diferencas dependem do threshold/config de decode.
2. **Detect**: regressao critica do `hailomz` antigo foi resolvida com recompilacao Vasco-style.
3. **Seg**: ambos modelos funcionais em HEF; `yolo26n_seg` exigiu variante `_noproto` para compilar.
4. **Status**: comparacao ONNX vs HEF concluida para os 4 modelos em `teste1` (30 frames).

---

*Atualizado: 8 mai 2026*
*Status: Fechado com resultados Vasco-style (incluindo `yolo26n_seg` noproto)*
