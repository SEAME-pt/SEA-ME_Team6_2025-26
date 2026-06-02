# ONNX Output Tensor Formats — Guia Detalhado

## Contexto: onde isto aparece no pipeline

O pipeline completo de um modelo YOLO vai do treino até à inferência em hardware:

```
[1] Treino         .pt (PyTorch weights, FP32)
        ↓ torch.onnx.export()
[2] ONNX Export    .onnx  ← aqui vivem os formatos descritos neste documento
        ↓ hailo parse
[3] HAR            .har (Hailo Archive — grafo intermédio)
        ↓ hailo optimize (quantização INT8, usa calibration images)
[4] HAR quantizado .har
        ↓ hailo compile (mapeamento para hardware Hailo-8)
[5] HEF            .hef  ← executado no AGL com hailo_platform
        ↓ inference_video_sprint13.py
[6] Inferência     vídeo anotado + stats JSON
```

O formato dos tensors de saída do ONNX (passo 2) determina como o
`run_onnx_hostdecode_sprint13.py` faz o decode local. Os dois formatos
principais que encontrámos nos nossos 4 modelos Sprint13 são:

---

## Formato A — "raw anchors sem NMS" — `[1, C, N]`

**Exemplos nos nossos modelos:**
- `yolov8s_detect`: `output0 [1, 21, 8400]`
- `yolov8n_seg`:    `output0 [1, 55, 8400]`  +  `proto [1, 32, 160, 160]`

### Dimensões

```
output0: [batch=1, channels=C, anchors=N]

Para detect (yolov8s, 17 classes):
  C = 4 (box dist) + 17 (class logits) = 21
  N = 8400 = 80×80 + 40×40 + 20×20  (3 escalas concatenadas)

Para seg (yolov8n, 19 classes, 32 proto):
  C = 4 (box dist) + 19 (class logits) + 32 (mask coefs) = 55
  N = 8400
```

### O que são os 8400 anchors?

O YOLO processa a imagem 640×640 em 3 escalas de resolução spatial:
- **Escala 1 (objetos grandes)**: 20×20 = 400 células → 1 anchor por célula → 400
- **Escala 2 (objetos médios)**: 40×40 = 1600 células → 1600
- **Escala 3 (objetos pequenos)**: 80×80 = 6400 células → 6400
- **Total**: 400 + 1600 + 6400 = **8400**

Cada "anchor" é uma proposta de deteção antes do NMS. A maioria tem baixa
confiança e é eliminada no passo de NMS do lado do host.

### O que são os 4 canais de bounding box? (DFL)

O `yolov8` (e derivados) usa **DFL — Distribution Focal Loss** para o bbox,
diferente do YOLO v5 que usava cx/cy/w/h diretamente.

```
Canais [0:4] = distribuição de probabilidade sobre 16 bins de distância
               (não são offset x/y/w/h diretamente)
```

O decode (em `decode_byom()`) converte estes bins em `[x1, y1, x2, y2]`
absolutas usando `DFL → softmax → weighted sum`.

### Para o seg — os 32 mask coefficients e o proto

```
proto: [1, 32, 160, 160]
  32   = número de "máscaras base" (protótipos low-rank)
  160  = resolução spatial = input_size / 4 = 640 / 4
```

A máscara final para cada deteção constrói-se assim:

```python
# coefs: [32]       — da última parte do canal C do output0
# proto: [32,H,W]   — tensor separado
mask_logits = coefs @ proto.reshape(32, -1)   # [1, H*W]
mask = sigmoid(mask_logits).reshape(H, W)
mask_resized = cv2.resize(mask, (img_w, img_h))
binary_mask = mask_resized > 0.5
```

---

## Formato B — "pós-NMS com NMS embutido no ONNX" — `[1, N, K]`

**Exemplos nos nossos modelos:**
- `yolo26n_detect`: `output0 [1, 300, 6]`
- `yolo26n_seg`:    `output0 [1, 300, 38]`  +  `proto [1, 32, 160, 160]`

### Dimensões

```
output0: [batch=1, max_dets=300, cols=K]

Para detect (yolo26n, 17 classes):
  K = 4 (xyxy) + 1 (score) + 1 (class_id) = 6
  300 = número máximo de deteções pós-NMS (as "vagas" não usadas ficam a zeros)

Para seg (yolo26n, 19 classes, 32 proto):
  K = 4 (xyxy) + 1 (score) + 1 (class_id) + 32 (mask coefs) = 38
  300 = igual ao detect
```

### O que são exatamente as 38 colunas?

```
col 0:3  → [x1, y1, x2, y2]  bounding box em pixels (absolutos, já escalados)
col 4    → score              confiança da deteção (sigmoid já aplicado)
col 5    → class_id           índice da classe (float que se converte a int)
col 6:38 → mask coefs [32]    coeficientes para combinar com proto
```

### Diferença chave face ao Formato A

| Aspeto | Formato A `[1,C,N]` | Formato B `[1,N,K]` |
|---|---|---|
| NMS | **não** feito dentro do ONNX | **já feito** dentro do ONNX |
| Número de deteções | ~8400 propostas brutas | ≤300 deteções finais |
| Box format | distribuição DFL → precisa decode | xyxy absoluto, pronto a usar |
| Score | logit bruto → precisa sigmoid | já é probabilidade 0-1 |
| Usado para Hailo? | ✅ sim (BYOM flow remove NMS) | ❌ não recomendado (NMS no graph é difícil de mapear para Hailo) |

### Por que os nossos modelos exportam formatos diferentes?

A flag `export_with_nms` (ou `nms=True/False` em Ultralytics) controla isto:

```python
# Formato A (sem NMS) — padrão para BYOM/Hailo
model.export(format='onnx', nms=False)   # ou omit nms flag

# Formato B (com NMS) — usado quando só precisamos de inferência ONNX local
model.export(format='onnx', nms=True)
```

O `yolo26n` foi exportado com `nms=True`, daí o Formato B. O `yolov8n/s`
exportou com `nms=False`, ficando com o Formato A.

---

## Formato C — "BYOM heads no HEF" — output multi-tensor sem NMS

Quando o HEF é compilado com o flow BYOM (Bring Your Own Model), o Hailo
separa automaticamente a head em tensors individuais por escala:

```
Para yolov8s_detect HEF:
  yolov8s_detect_sprint13_byom/conv41  (80, 80, 64)  ← box DFL escala 1
  yolov8s_detect_sprint13_byom/conv42  (80, 80, 17)  ← class logits escala 1
  yolov8s_detect_sprint13_byom/conv52  (40, 40, 64)  ← box DFL escala 2
  yolov8s_detect_sprint13_byom/conv53  (40, 40, 17)  ← class logits escala 2
  yolov8s_detect_sprint13_byom/conv62  (20, 20, 64)  ← box DFL escala 3
  yolov8s_detect_sprint13_byom/conv63  (20, 20, 17)  ← class logits escala 3

Para yolov8n_seg HEF (adiciona proto):
  ...conv42/43/55/56/65/66 (igual ao detect mas 19 classes)...
  yolov8n_seg_sprint13_byom/conv45  (160, 160, 32)   ← proto masks
```

O `decode_byom()` em `inference_video_sprint13.py` reconstrói as deteções
partir destes fragmentos.

---

## Tabela resumo dos 4 modelos Sprint13

| Modelo | Output ONNX | Formato | NMS no ONNX | Classes |
|---|---|---|---|---|
| `yolov8s_detect` | `[1,21,8400]` | A (raw) | Não | 17 |
| `yolo26n_detect` | `[1,300,6]` | B (pós-NMS) | Sim | 17 |
| `yolov8n_seg` | `[1,55,8400]` + `proto[1,32,160,160]` | A (raw+mask) | Não | 19 |
| `yolo26n_seg` | `[1,300,38]` + `proto[1,32,160,160]` | B (pós-NMS+mask) | Sim | 19 |

---

## Implicações para o decoder (`_decode_export_output`)

O decoder em `run_onnx_hostdecode_sprint13.py` deteta o formato
automaticamente pela forma do tensor:

```python
# Formato B: shape[-1] é pequeno (6–128) relativo à dim [-2]
if 6 <= pred.shape[-1] <= 128 and pred.shape[1] > pred.shape[2]:
    # pós-NMS: cada linha é uma deteção
    # col4 = score, col5 = class

# Formato A: 2ª dim > 4+num_classes (ex: 21 ou 55) e 3ª dim = 8400
else:
    # raw: transpor para [N, C], extrair scores com sigmoid
```

---

## Por que o detect HEF tem `p50_conf = 0.5000` (problema de quantização)

Quando `p50 = 0.5000` exatamente, isso significa `sigmoid(0) = 0.5` —
os logits de classe saem do Hailo como **zero** em INT8.

Causa raiz: a quantização INT8 nos detect HEFs calibrou o range das
camadas de class score (`conv42`, `conv53`, `conv63`) a partir de apenas
64 imagens em CPU-mode (`--optimization-level 0`). Se as ativações nessas
64 imagens forem fracas, o quantizador escolhe um scale factor muito
pequeno → todos os valores float mapeiam para INT8 zero → sigmoid(0) = 0.5.

Os **seg HEFs** não sofrem do mesmo problema (mean ≈ 0.59–0.61) porque
o proto head e as máscaras introduzem âncoras de gradiente que expandem
o range das ativações de classe durante o treino.

### Fix possível — mixed-precision via `.alls`

Para forçar `int16` nas camadas de class output dos detect models:

```
# yolov8s_detect_mixed_precision.alls
normalization1 = normalization([0, 0, 0], [255, 255, 255])
change_output_activation_to_16_bit(conv42)
change_output_activation_to_16_bit(conv53)
change_output_activation_to_16_bit(conv63)
```

> **Nota**: os nomes das layers no `.alls` referem-se aos nós no grafo HAR
> (depois de `hailo parse`), que podem diferir dos nomes no HEF final.
> Usar `hailo profiler` ou `hailo visualizer` para confirmar os nomes corretos.

A alternativa definitiva é compilar com GPU a `--optimization-level 2`,
que usa calibração completa e mixed-precision automático.
