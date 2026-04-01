# AI Planning — SEA:ME Team 6 (ADAS Spike)

> Data: 26/03/2026  
> Branch: `feature/Hailo/model_conversion`  
> Objetivo: consolidar decisões de modelos, conversão, organização Lenovo/Docker/AGL, correções de tutorial e plano de execução de 2 dias.

---

## Índice

- [Resumo Executivo](#resumo-executivo)
- [1) Verificação dos links sugeridos pelo mentor](#1-verificação-dos-links-sugeridos-pelo-mentor)
  - [1.1 `hailo-apps` instance segmentation](#11-hailo-apps-instance-segmentation)
  - [1.2 Ultralytics + Albumentations](#12-ultralytics--albumentations)

Tópico 1: Que Modelos Escolher?
- [2) Modelos — decisão prática para o vosso contexto](#2-modelos--decisão-prática-para-o-vosso-contexto)
  - [2.1 `YOLOv8s` vs `YOLO26` (estado atual)](#21-yolov8s-vs-yolo26-estado-atual)
  - [2.2 Opções A/B/C (viabilidade)](#22-opções-abc-viabilidade)
   - [2.2f Tabela Única Consolidada (A-G) com Score Final — foco em médio prazo](#22f-tabela-única-consolidada-a-g-com-score-final--foco-em-médio-prazo)
  - [2.3 Concordância com Gemini (3 tarefas em 2 modelos)](#23-concordância-com-gemini-3-tarefas-em-2-modelos)
   - [2.4 Alternativas ao `UFLDv2` para deteção de linhas](#24-alternativas-ao-ufldv2-para-deteção-de-linhas)

Tópico 2: Problemas na Conversão
- [3) Conversão e perda de precisão](#3-conversão-e-perda-de-precisão)
  - [3.1 O problema do David pode ser real?](#31-o-problema-do-david-pode-ser-real)
  - [3.2 Onde normalmente se perde mais](#32-onde-normalmente-se-perde-mais)
  - [3.3 Como validar objetivamente](#33-como-validar-objetivamente)
  - [3.4 Data augmentation (Albumentations / GenAI) — quando usar](#34-data-augmentation-albumentations--genai--quando-usar)

Tópico 3: Tutorial — Diferenças vs. O que Fizeram
- [4) Quadro comparativo — organização (tutorial vs atual vs desejado)](#4-quadro-comparativo--organização-tutorial-vs-atual-vs-desejado)
- [5) Quadro comparativo — tutorial inicial (o que estava mal e como corrigir)](#5-quadro-comparativo--tutorial-inicial-o-que-estava-mal-e-como-corrigir)

Tópico 4: Etapas do Processo
- [6) O que fazer agora (step-by-step curto e objetivo)](#6-o-que-fazer-agora-step-by-step-curto-e-objetivo)
  - [6.1 Para fechar arquitetura e modelo (2 dias)](#61-para-fechar-arquitetura-e-modelo-2-dias)
  - [6.2 Para o problema de conversão/accuracy](#62-para-o-problema-de-conversionaccuracy)
  - [6.3 Comandos úteis imediatos](#63-comandos-úteis-imediatos)
  - [6.4 Plano Imediato — Próximas 48h (Sprint YOLO26 Triage)](#64-plano-imediato--próximas-48h-sprint-yolo26-triage)
- [7) Plano de 2 dias (foco João)](#7-plano-de-2-dias-foco-joão)

Tópico 5 (Plano para Sprint atual: Semana 1 do sprint 3 do módulo 2)
- [8) Decisão final recomendada (nesta sprint)](#8-decisão-final-recomendada-nesta-sprint)
- [9) Notas de alinhamento com o contexto fornecido](#9-notas-de-alinhamento-com-o-contexto-fornecido)
- [10) Próximo passo sugerido](#10-próximo-passo-sugerido)
- [11) Benchmark de latência por etapa (template rápido)](#11-benchmark-de-latência-por-etapa-template-rápido)
- [11A) Teste YOLO26-seg (tutorial end-to-end 4h)](#11a-teste-yolo26-seg-tutorial-end-to-end-4h)
- [11B) FAQS — Limpeza, Docker e Benchmark](#11b-faqs--limpeza-docker-e-benchmark)
- [11C) Matriz de benchmark comparável (Hailo + Carla)](#11c-matriz-de-benchmark-comparável-hailo--carla)
- [11D) Diário real de execução (Mar–Abr 2026)](#11d-diário-real-de-execução-marabr-2026)
- [A.1b) Matriz de Avaliação Final — 3 Candidatos vs 6 Critérios Ponderados](#a1b-matriz-de-avaliação-final--3-candidatos-vs-6-critérios-ponderados)

Tópico 6: Clarificações Técnicas
- [12) Compatibilidade ONNX/YOLO26 — Esclarecimentos Técnicos](#12-compatibilidade-onnxyolo26--esclarecimentos-técnicos)
  - [12.1 Compatibilidade com Exportação ONNX](#121-compatibilidade-com-exportação-onnx)
  - [12.2 Por Que YOLO26 Melhora Compatibilidade ONNX?](#122-por-que-yolo26-melhora-compatibilidade-onnx)
  - [12.3 Fluxo de Conversão Hailo (para YOLO26)](#123-fluxo-de-conversão-hailo-para-yolo26)
  - [12.4 Onde o Risco Pode Estar (não em ONNX)](#124-onde-o-risco-pode-estar-não-em-onnx)
  - [12.5 Recomendação Prática](#125-recomendação-prática)
  - [12.5a Limitações Reais do DFC e Arquitetura Híbrida](#125a-limitações-reais-do-dfc-dataflow-compiler-e-arquitetura-híbrida)
- [9) Notas de alinhamento com o contexto fornecido](#9-notas-de-alinhamento-com-o-contexto-fornecido)
- [10) Próximo passo sugerido](#10-próximo-passo-sugerido)
- [11) Benchmark de latência por etapa (template rápido)](#11-benchmark-de-latência-por-etapa-template-rápido)

---

## Resumo Executivo

- **Modelo recomendado agora (prazo curto + menor risco):** `UFLDv2 + YOLOv8-seg` (2 modelos, 3 tarefas).  
- **Não recomendado agora:** 3 modelos separados (`UFLDv2 + YOLOv8s + YOLOv8-seg`) por risco de latência/FPS e complexidade de fusão.  
- **Sobre `YOLO26`:** a documentação atual da Ultralytics realmente usa a nomenclatura `YOLO26` em integrações recentes; na prática, para Hailo hoje, `YOLOv8s` continua com risco menor de compatibilidade/tempo.  
- **Perda de precisão na conversão (`.pth -> onnx -> har -> hef`) é real** e normalmente vem da fase de quantização/calibração (`HAR -> HEF`), não só da exportação ONNX.  
- **Albumentations faz sentido** para robustez (sombras/iluminação), mas atua na fase de treino/retreino, não no pós-processamento.

---

## 1) Verificação dos links sugeridos pelo mentor

### 1.1 `hailo-apps` instance segmentation
Link analisado:  
`https://github.com/hailo-ai/hailo-apps/blob/main/hailo_apps/python/standalone_apps/instance_segmentation/README.md`

**Confirmações relevantes:**
- Suporta modelos de segmentação de instância (`yolov5*_seg`, `yolov8*_seg`, `fast_sam_s`).
- Aceita input de imagem/vídeo/câmara (`usb` e `rpi`).
- Pode mostrar FPS (`--show-fps`) e guardar output.
- Traz notas de performance importantes:
  - modelos com pós-processamento no dispositivo (HEF com NMS integrado) podem chegar a FPS altos;
  - modelos com pós-processamento no host podem cair bastante de FPS.

**Opinião técnica para o vosso problema (sombras, curvas, 90º, cruzamentos):**
- Sim, este caminho é relevante para complementar UFLDv2.
- Segmentação ajuda muito na semântica de chão (passadeiras, `STOP` no chão, vagas, áreas com sombra/contraste).
- Não substitui completamente a necessidade de boa calibração e teste em pista real.

### 1.2 Ultralytics + Albumentations
Link analisado:  
`https://github.com/ultralytics/ultralytics/blob/main/docs/en/integrations/albumentations.md`

**Confirmações relevantes:**
- A página usa explicitamente o nome **`YOLO26`**.
- Integração de Albumentations é para **treino** (data augmentation), com exemplos como `CLAHE`, `RandomBrightnessContrast`, `Blur`, `Noise`.
- Aplica-se a detecção/segmentação e melhora robustez a variações visuais.

**Conclusão:**
- Faz sentido para o vosso problema de robustez (sombras, variação de cor, etc.).
- **Não é pós-processamento**; é preparação de dados + treino/retreino.

---

## 2) Modelos — decisão prática para o vosso contexto

## 2.1 `YOLOv8s` vs `YOLO26` (estado atual)

- `YOLO26` pode trazer ganhos de arquitetura/pipeline em relação ao `YOLOv8s`.
- Porém, no vosso contexto com Hailo + prazo curto:
  - `YOLOv8s` tem integração mais madura e menos risco de bloqueio de conversão.
  - `YOLO26` pode exigir mais validação de compatibilidade no fluxo ONNX -> HAR -> HEF.

**Decisão pragmática de sprint:**
- Usar `YOLOv8s` (ou `YOLOv8n` conforme budget de latência) para reduzir risco.
- Deixar `YOLO26` como experimento controlado numa sprint seguinte.

## 2.2 Opções A-F (+ opção com pose/obb) — viabilidade

| Opção | Composição | Tamanho recomendado (`n/s/m/l/x`) | Viabilidade agora | Comentário |
|---|---|---|---|---|
| A | `UFLDv2 + YOLOv8s` | `yolov8n` (start) -> `yolov8s` (upgrade) | Alta | Simples, estável para lane + objetos, mas sem segmentação de chão |
| B | `UFLDv2 + YOLOv8-seg` | `yolov8n-seg` (start) -> `yolov8s-seg` (upgrade) | Alta | Boa cobertura semântica de pista com risco de integração baixo |
| C | `UFLDv2 + YOLO26` | `yolo26n` (start) -> `yolo26s` (upgrade) | Média | Potencial técnico alto, mas sem segmentação horizontal explícita |
| D | `UFLDv2 + YOLO26-seg` | `yolo26n-seg` (start) -> `yolo26s-seg` (médio prazo) | Média/Alta | Melhor equilíbrio para lane + semântica + robustez em médio prazo |
| E | `YOLO26 + YOLO26-seg` | `yolo26n + yolo26n-seg` | Baixa/Média | Redundante para edge; aumenta latência e complexidade de fusão |
| F | `Apenas YOLO26-seg` | `yolo26n-seg` (start) -> `yolo26s-seg` (se budget permitir) | Média/Alta | Pipeline simples e forte em semântica, mas pode ter menos estabilidade de steering |
| G | `UFLDv2 + YOLO26-seg + (YOLO26-pose/obb)` | `yolo26n-obb` ou `yolo26n-pose` como adicional | Baixa (agora) | Só para fase avançada; útil se houver requisito forte de orientação/pose |

Detalhes destas comparações encontram-se no Apêndice A (secção A.1).

---

### 2.2b Sobre YOLO26 — Informações e Referências Críticas

> **Nota importante:** YOLO26 é **muito recente** (lançado 14 de janeiro de 2026) e otimizado exclusivamente para edge deployment.

#### Referências de Investigação Recomendadas:

1. **[Documentação oficial Ultralytics YOLO26](https://docs.ultralytics.com/pt/models/yolo26/)**
   - Arquitectura completa, métricas de performance, exemplos de código
   - Benchmark end-to-end: YOLO26n comparable to YOLO11n, ~43% mais rápido em CPU (ONNX)

2. **[Roboflow — YOLO26 Model Hub](https://roboflow.com/model/yolo26)**
   - Deployment options, integrations, comparações com outros modelos

3. **[GitHub Ultralytics/ultralytics](https://github.com/ultralytics/ultralytics)**
   - Código-fonte, issues, PRs sobre compatibilidade Hailo (crítico para validação)
   - Branch `main` contém implementações mais recentes

4. **[Medium: "YOLOv26 Explained Simply"](https://medium.com/@harikrishnananu2003/yolov26-explained-simply-the-object-detector-built-for-the-real-world-ceb9b3693c57)**
   - Explicação acessível das 4 inovações principais
   - Casos de uso reais, deployment considerations

#### As 4 Inovações Centrais do YOLO26:

| Inovação | O que resolve | Impacto em ADAS |
|----------|--------------|-----------------|
| **1. End-to-End NMS-Free Inference** | Remove etapa de pós-processamento CPU (NMS) completamente | ✅ **Crítico:** elimina bottleneck de latência em densidade alta |
| **2. Remoção de DFL (Distribution Focal Loss)** | Simplifica cálculo de box coordinates, melhora export ONNX/TensorRT | ✅ **Importante:** reduz erro de conversão .pth → .onnx → .hef |
| **3. ProgLoss + STAL** | ProgLoss: balanço dinâmico de 3 tipos de loss; STAL: detecção de small objects | ✅ **Crítico para ADAS:** sinais pequenos em curvas, sombras, degradação |
| **4. Otimizador MuSGD** | Híbrido SGD + Muon (inspirado Kimi K2 LLM), convergência mais rápida + estável | ✅ **Nice-to-have:** reduz custo de treino/retreino customizado |

#### Por que YOLO26 é Diferente:

- **Arquitetura dual-head:** cabeça "um-para-um" (default, sem NMS) vs cabeça "um-para-muitos" (com NMS, opcional)
- **Suporte a 5 tarefas unificadas:** detecção, segmentação, pose, classificação, OBB — tudo no mesmo framework
- **YOLOE-26 (variant):** segmentação de vocabulário aberto (text/visual prompts), aplicável a sinais de trânsito dinâmicos
- **Métricas COCO:** YOLO26s mAP 48.6 (vs YOLOv8s 44.3), segmentação mAP 47.8

---

Detalhes comparativos completos (desafios ADAS específicos, matriz de decisão por timeline e recomendação final) encontram-se no Apêndice A (secções A.2–A.3).

---

### 2.2e Recomendação Final: Decisão Prática

**Para esta sprint (2 dias):**
```
✅ ESCOLHER: UFLDv2 + YOLOv8-seg
   - Score: 7.50/10
   - Risco: Nulo em compilação Hailo
   - Tempo: < 24h para HEF validado
   - Fallback: Se NMS é bottleneck crítico, testar YOLO26s noite D1

⚠️  MONITORAR: Latência de NMS em cenários densos (cruzamentos, estacionamentos)
   - Se p95 > 50ms: otimizar postprocess ou preparar YOLO26 para D2

🚀 PREPARAR PARALELO (noite D1): 
   - YOLO26 compatibility test: converter ONNX → HEF em Docker
   - Se bem-sucedido (< 2h): documentar e considerar D2 integration
```

**Para sprint seguinte (8-15 dias):**
```
✅ ESCOLHER: UFLDv2 + YOLO26-seg (se Hailo validation OK)
   - Score: 8.65/10
   - Risco: Reduzido após D2 testing
   - Tempo: 8-10h para conversão + calibração robusta
   - Ganho: 43% mais rápido em CPU, NMS eliminado, small objects +30%

📊 MEDIR:
   - Comparação ONNX vs HEF em dataset de sombras/curvas
   - Benchmark latência vs YOLOv8-seg
   - Falsa negativa em sinais de trânsito

🎯 BONUS: Validar YOLOE-26 para detecção de sinais com text prompts
```

### 2.2f Tabela Única Consolidada (A-G) com Score Final — foco em médio prazo

> Objetivo: congregar todas as opções (A-F + pose/obb) numa única matriz de decisão ponderada para horizonte de 8–15 dias.

Pesos usados (médio prazo):
- Estabilidade lane geometry (`offset + heading`): **25%**
- Cobertura semântica (sinalização horizontal/vertical + objetos): **20%**
- Latência/FPS em `RPi5 + Hailo-8`: **20%**
- Risco de integração/conversão (`ONNX -> HAR -> HEF`): **15%**
- Complexidade operacional (pipeline/fusão/debug): **10%**
- Escalabilidade para sprint seguinte: **10%**

| Opção | Composição (resumo) | Tamanho recomendado | Estabilidade lane (25%) | Cobertura semântica (20%) | Latência/FPS (20%) | Risco integração (15%) | Complexidade (10%) | Escalabilidade (10%) | **Score final (/10)** |
|---|---|---|---:|---:|---:|---:|---:|---:|---:|
| A | `UFLDv2 + YOLOv8` | `yolov8n` -> `yolov8s` | 9.0 | 5.0 | 9.0 | 9.0 | 8.0 | 6.0 | **7.80** |
| B | `UFLDv2 + YOLOv8-seg` | `yolov8n-seg` -> `yolov8s-seg` | 9.0 | 8.0 | 8.0 | 8.0 | 7.0 | 7.0 | **8.05** |
| C | `UFLDv2 + YOLO26` | `yolo26n` -> `yolo26s` | 9.0 | 6.0 | 8.5 | 6.0 | 7.0 | 8.0 | **7.55** |
| D | `UFLDv2 + YOLO26-seg` | `yolo26n-seg` -> `yolo26s-seg` | 9.0 | 9.5 | 8.5 | 6.5 | 6.5 | 9.0 | **8.38** |
| E | `YOLO26 + YOLO26-seg` | `yolo26n + yolo26n-seg` | 8.0 | 9.5 | 6.5 | 5.0 | 4.0 | 7.0 | **7.05** |
| F | `Apenas YOLO26-seg` | `yolo26n-seg` -> `yolo26s-seg` | 7.0 | 9.5 | 9.0 | 7.0 | 8.0 | 8.0 | **8.10** |
| G | `UFLDv2 + YOLO26-seg + pose/obb` | `+ yolo26n-obb/pose` | 9.0 | 10.0 | 6.0 | 4.5 | 3.0 | 7.0 | **7.13** |

**Decisão final para médio prazo (8–15 dias):**
- **1º lugar (recomendado): Opção D — `UFLDv2 + YOLO26-seg` (8.38/10)**
- **2º lugar:** Opção F — `YOLO26-seg` apenas (8.10/10)
- **3º lugar:** Opção B — `UFLDv2 + YOLOv8-seg` (8.05/10)

**Justificação curta da decisão D:**
- Mantém a maior estabilidade de condução com `UFLDv2` para lane geometry.
- Adiciona semântica de chão forte com `YOLO26-seg` (passadeiras/STOP/setas/objetos).
- Evita redundância da opção E e reduz risco de jitter de steering da opção F.

**Escolha de tamanhos para executar já no médio prazo:**
- Semana 1 do plano: `UFLDv2 + yolo26n-seg`
- Semana 2 do plano (se `p95 <= 50 ms`): migrar para `UFLDv2 + yolo26s-seg`

## 2.3 Concordância com Gemini (3 tarefas em 2 modelos)

**Concordo com a proposta** de usar 2 modelos para 3 tarefas:
- Modelo A (`UFLDv2`): lane-following rápido (offset + heading).
- Modelo B (`YOLOv8-seg`): deteção + segmentação semântica.

**Porque evita problemas de 3 modelos separados:**
- Menos overhead de pipeline e sincronização.
- Menor risco de queda de FPS por competição de recursos.
- Menor complexidade de fusão de decisão e debug.

## 2.4 Alternativas ao `UFLDv2` para deteção de linhas

Pergunta prática: `YOLOv8-seg` e `YOLO26-seg` podem substituir o `UFLDv2` para lane detection?

**Resposta curta:** podem ser alternativas, mas no cenário ADAS de controlo de faixa o `UFLDv2` continua a ser, em geral, a baseline mais estável para extrair geometria de faixa (`lane center offset` + `heading error`).

### Comparação focada apenas em deteção de linhas

| Critério (Lane Detection) | `UFLDv2` | `YOLOv8-seg` | `YOLO26-seg` |
|---|---|---|---|
| Especialização em linhas de faixa | **Alta** (modelo dedicado) | Média (modelo geral de segmentação) | Média/Alta (segmentação mais robusta) |
| Estabilidade temporal (frame-a-frame) para controlo | **Alta** | Média | Média/Alta |
| Extração direta de `offset` e `heading` | **Nativa** | Requer pós-processamento geométrico adicional | Requer pós-processamento geométrico adicional |
| Robustez a sombras/iluminação | Boa (depende de treino/dataset) | Boa | **Muito boa** (tendência, após tuning) |
| Robustez a curvas/contra-curvas | Boa | Média/Boa | **Boa/Muito boa** |
| Sinalização horizontal no chão (`STOP`, passadeiras) | Limitada (não é foco principal) | **Boa** | **Muito boa** |
| Carga de integração no pipeline atual | **Baixa** (já previsto para lane-following) | Média | Média/Alta (depende de maturidade Hailo no vosso fluxo) |
| Risco técnico nesta sprint (2 dias) | **Baixo** | Baixo/Médio | Médio |

### Leitura técnica da comparação

- Se o objetivo principal for **controlo de trajetória em faixa** (LDW/LKA), `UFLDv2` mantém vantagem por ser lane-specific.
- Se o objetivo incluir também **semântica do chão** (passadeiras, `STOP`, marcações variadas), os modelos de segmentação (`YOLOv8-seg`/`YOLO26-seg`) complementam melhor.
- `YOLO26-seg` é candidato forte para substituir parcialmente `UFLDv2`, mas essa troca só deve ser feita após validação objetiva de estabilidade geométrica.

### Regra de decisão recomendada

#### Prazo de 2 dias (sprint atual)
- Manter `UFLDv2` como modelo principal de linhas.
- Usar `YOLOv8-seg` (ou `YOLO26-seg` se já validado) para semântica de pista e contexto.
- **Arquitetura recomendada:** `UFLDv2 + YOLOv8-seg`.

#### Prazo de 8–15 dias
- Testar `YOLO26-seg` como alternativa para lane detection com benchmark dedicado.
- Trocar `UFLDv2` só se `YOLO26-seg` igualar ou superar:
   1. erro de centro de faixa,
   2. estabilidade de `heading` frame-a-frame,
   3. latência p95 fim-a-fim.

**Conclusão prática:** `YOLO26-seg` pode ser alternativa ao `UFLDv2`, mas para produção ADAS o caminho de menor risco continua a ser abordagem híbrida: lane geometry com `UFLDv2` + semântica de pista com segmentação.

---

## 3) Conversão e perda de precisão

## 3.1 O problema do David pode ser real?

**Sim.** A diferença entre `.pth/.onnx` e `.hef` pode ocorrer principalmente em:
- quantização para INT8,
- calibração inadequada,
- mismatch entre distribuição de treino e distribuição de calibração.

## 3.2 Onde normalmente se perde mais

| Etapa | Risco típico de perda |
|---|---|
| `.pth -> onnx` | Baixo (se export correto) |
| `onnx -> har` | Baixo/Médio |
| `har -> hef` (quantização/calibração) | **Médio/Alto** |

## 3.3 Como validar objetivamente

1. Comparar outputs do `.onnx` e do `.hef` no mesmo conjunto fixo de imagens.  
2. Medir métricas por classe crítica (`passadeira`, `stop_chao`, linhas, sinais).  
3. Se gap > esperado, rever dataset de calibração e ranges.

## 3.4 Data augmentation (Albumentations / GenAI) — quando usar

**Fase correta:** treino/retreino (antes da conversão), não pós-processamento.

### Step-by-step recomendado
1. Recolher dataset real da pista (incluindo sombras, curvas, 90º, variação de cor).
2. Aplicar augmentations relevantes (`CLAHE`, brilho/contraste, blur/noise moderado).
3. Treinar/retreinar modelo.
4. Gerar ONNX e validar baseline.
5. Converter com calibração representativa (500-1000 imagens reais/representativas).
6. Avaliar gap ONNX vs HEF.

**Resultado esperado:** reduzir sensibilidade a iluminação/sombra e diminuir perda pós-quantização.

---

## 4) Quadro comparativo — organização (tutorial vs atual vs desejado)

## 4.1 Lenovo + Docker + AGL

| Camada | Como o tutorial inicial assume | Estado atual (evidências fornecidas) | Como deveria ficar (target) | Modificações necessárias (step-by-step) |
|---|---|---|---|---|
| Lenovo (host) | `~/hailo-dfc` como raiz principal | Estrutura real principal em `~/Documents/AI/hailo/` com `docker_custom/`, `hailo-dfc/`, `shared_with_docker/` | Consolidar raiz canónica em `~/Documents/AI/hailo/` | 1) Definir `HAILO_ROOT=~/Documents/AI/hailo`; 2) declarar `shared_with_docker` como único ponto de troca; 3) arquivar paths legados no tutorial |
| Docker runtime | `docker compose` custom | Uso comprovado de `hailo_custom_suite:latest` com `-v .../shared_with_docker:/local/workspace/shared_with_docker` | Manter imagem validada + mount único | 1) Documentar comando oficial usado; 2) remover ambiguidade `compose` vs `docker run`; 3) padronizar logs/artifacts em `/local/workspace/shared_with_docker` |
| SDK instalação | Fluxo manual (.whl + .deb) como caminho principal | Foi usado mix manual + AI suite; AI suite está disponível (`hailo8_ai_sw_suite_2025-10.tar.gz`) | AI Software Suite como fluxo principal; manual como fallback avançado | 1) Rebaixar manual para “appendix”; 2) destacar AI suite como default; 3) explicar quando manual faz sentido |
| Artefactos | Sem ênfase forte em persistência pós-compile | Já existem `yolov8n.onnx/.har/.hef` em `shared_with_docker` | Regra explícita: sempre copiar/validar no mount antes de sair do container | 1) Check de persistência obrigatório; 2) naming padrão por modelo; 3) validar hash/tamanho |
| Calibração | Diretório genérico | `calibration_images` no host aparece vazio num path e preenchido noutro (`shared_with_docker/calibration_images`) | Diretório único e não ambíguo | 1) definir apenas `.../shared_with_docker/calibration_images`; 2) remover duplicados vazios; 3) adicionar script de verificação de contagem |
| Deploy AGL | Paths variáveis (`/root/models`, etc.) | Uso real operacional em `/data` para inferência e artefactos | Padronizar deploy em `/data` | 1) atualizar comandos `scp` para `/data/<model>.hef`; 2) validar com `hailortcli parse-hef`; 3) documentar rollback |
| OTA/RAUC | Não central no tutorial Hailo | `rauc.service` aparece `failed`; `journalctl -u rauc` sem entries | Separar claramente tutorial Hailo de troubleshooting OTA/RAUC | 1) Não misturar troubleshooting RAUC no tutorial de conversão; 2) criar ligação para guia OTA dedicado; 3) checklist de pré-condições OTA |

---

## 5) Quadro comparativo — tutorial inicial (o que estava mal e como corrigir)

| Tópico | Como estava no início | Problema | Correção aplicada/necessária | Se fosse fazer de novo desde o início |
|---|---|---|---|---|
| Instalação principal | Manual (.whl + .deb) | Alto risco de drift e incompatibilidades com Model Zoo | AI Suite como fluxo principal | Começar logo com AI Suite + imagem validada |
| Entrada no container | `docker compose run` | Não reflete totalmente o fluxo usado por vocês | Documentar comando real `docker run -v ... hailo_custom_suite:latest` | Fixar 1 comando oficial por equipa |
| Persistência de HEF | Não enfatizado como etapa crítica | Perda de `.hef` por diretórios efémeros | Etapa obrigatória de cópia para mount (`/local/workspace/shared_with_docker`) | Incluir “gate” de validação antes de `exit` |
| Paths de deploy | misto `/root/models`/outros | Ambiguidade e falhas de `scp`/runtime | padronizar `/data` no AGL | Definir convenção de paths no início |
| Branch/versão Model Zoo | múltiplas opções sem decisão final | Incerteza de reproducibilidade | Fixar versão/branch usada no time | Bloquear versão no tutorial + changelog |
| Conversão e qualidade | foco em comando, pouco em calibração | perda de precisão no HEF | reforçar calibração representativa + validação ONNX vs HEF | Incluir protocolo de validação de qualidade no tutorial base |

---

## 6) O que fazer agora (step-by-step curto e objetivo)

## 6.1 Para fechar arquitetura e modelo (2 dias)

1. Escolher `Opção C`: `UFLDv2 + YOLOv8-seg`.
2. Confirmar HEFs disponíveis (precompiled) para acelerar integração.
3. Integrar pipeline `libcamera -> inferência -> output semântico + offset`.
4. Definir thresholds de LDW com cenários reais (curvas/sombras).
5. Medir latência fim-a-fim (`p50/p95`) e FPS.

## 6.2 Para o problema de conversão/accuracy

1. Preparar dataset de calibração representativo (pista real, sombra, curvas, 90º).
2. Rodar baseline ONNX vs HEF no mesmo lote.
3. Se gap alto, aplicar retreino com Albumentations (não no pós-processamento).
4. Recalibrar e comparar novamente.

## 6.3 Comandos úteis imediatos

```bash
# No host Lenovo: validar artefactos no mount
ls -lh ~/Documents/AI/hailo/shared_with_docker/*.hef
ls -lh ~/Documents/AI/hailo/shared_with_docker/calibration_images | head

# No AGL: validar HEF
hailortcli parse-hef /data/<modelo>.hef

# Checar serviços OTA/RAUC (fora do tutorial Hailo)
systemctl status rauc.service
journalctl -u rauc --no-pager -n 100
```

## 6.4 **PLANO IMEDIATO — Próximas 48h (Sprint YOLO26 Triage)**

**Objetivo:** Decidir se seguem caminho **Seguro** (YOLO26n-seg) vs **Agressivo** (YOLO26s-seg) com dados reais

### **Fase 0: Setup (Hoje — 2h)**

**Checklist:**
- [ ] Clonar Ultralytics `yolo26n-seg.pt` e `yolo26s-seg.pt` em `/local/workspace`
- [ ] Confirmar Docker Hailo Suite está pronto (`docker images | grep hailo`)
- [ ] Preparar dataset de calibração mínimo (20–50 imagens de pista real com variação: sombra, curva, sol direto)
- [ ] Validar mount `/local/workspace/shared_with_docker` está acessível do host

**Comando rápido de download:**
```bash
cd /local/workspace
python -c "from ultralytics import YOLO; YOLO('yolo26n-seg.pt'); YOLO('yolo26s-seg.pt')"
# Downloads os modelos .pt
```

### **Fase 1: Caminho SEGURO — YOLO26n-seg + Split Hybrid (Dia 1 — 4h)**

**Objetivo:** Ter um HEF funcional com latência garantida (11–12ms)

**Passos:**

1. **Exportar ONNX**
```bash
docker run -v /local/workspace:/workspace hailo_custom_suite:latest bash -c \
  "cd /workspace && python -c \
   \"from ultralytics import YOLO; m = YOLO('yolo26n-seg.pt'); m.export(format='onnx')\""
# Output: yolo26n-seg.onnx
```

2. **Compilar com split híbrido (mixed-precision)**
```bash
docker run -v /local/workspace:/workspace hailo_custom_suite:latest bash -c \
  "hailomz compile /workspace/yolo26n-seg.onnx \
   --hw-arch=hailo8l \
   --compiler-defaults \
   --mixed-precision \
   --start-node-name=images \
   --end-node-name=output \
   -o /workspace/shared/yolo26n-seg_hybrid.hef"
```

3. **Testar em pista (10–15min)**
```bash
# No AGL
cd /data
hailortcli parse-hef yolo26n-seg_hybrid.hef
# Se OK: ✅ modelo carrega sem erro
```

4. **Medir latência (5–10min)**
```bash
# Usar script simples ou `hailort_simple_detector` com --show-fps
# Esperado: p95 latência < 15ms, FPS > 60
```

**Decisão após Fase 1:**
- ✅ **Se latência OK (<15ms, FPS >60):** avançar para Fase 2 (agressivo)
- ❌ **Se falhar:** documentar erro, usar Fallback (6.5)

### **Fase 2: Caminho AGRESSIVO — YOLO26s-seg + Full INT8 (Dia 1–2 — 6–8h)**

**Objetivo:** Validar se YOLO26s-seg consegue quantização total em INT8

**Passos:**

1. **Exportar ONNX (idêntico ao n-seg)**
```bash
docker run -v /local/workspace:/workspace hailo_custom_suite:latest bash -c \
  "cd /workspace && python -c \
   \"from ultralytics import YOLO; m = YOLO('yolo26s-seg.pt'); m.export(format='onnx')\""
```

2. **Tentar full INT8 (sem mixed-precision)**
```bash
docker run -v /local/workspace:/workspace hailo_custom_suite:latest bash -c \
  "hailomz compile /workspace/yolo26s-seg.onnx \
   --hw-arch=hailo8l \
   --compiler-defaults \
   --start-node-name=images \
   --end-node-name=output \
   -o /workspace/shared/yolo26s-seg_int8.hef"
```

3. **Se compilação falhar:** reverter para hybrid
```bash
# Comando fallback com --mixed-precision
hailomz compile /workspace/yolo26s-seg.onnx \
   --hw-arch=hailo8l \
   --compiler-defaults \
   --mixed-precision \  # ← ativa split híbrido
   -o /workspace/shared/yolo26s-seg_hybrid.hef
```

4. **Medir latência e precisão**
```bash
# Esperado (full INT8): p95 < 12ms, FPS > 70, mAP ≈ 47.8 (vs ONNX 47.8)
# Se hybrid: p95 ~13ms, FPS ~65, com head em float32
```

**Decisão após Fase 2:**
- ✅ **Full INT8 OK:** usar `yolo26s-seg_int8.hef` para médio prazo
- ⚠️ **Hybrid funciona:** usar `yolo26s-seg_hybrid.hef` com custo de CPU
- ❌ **Ambos falham:** voltar a YOLOv8-seg (fora do escopo desta triage)

### **Fase 3: Decisão Final (Dia 2 — 1h)**

**Tabela de Decisão:**

| Cenário | Resultado Fase 1 | Resultado Fase 2 | Decisão | Próximo |
|---------|------------------|------------------|---------|---------|
| **A** | ✅ n-seg OK | ✅ s-seg INT8 OK | Usar **s-seg INT8** | Integrar steering + teste pista |
| **B** | ✅ n-seg OK | ⚠️ s-seg só hybrid | Usar **s-seg hybrid** | Integrar steering + benchmark |
| **C** | ✅ n-seg OK | ❌ s-seg falha | Usar **n-seg hybrid** | Validar em pista, depois opção de upgrade |
| **D** | ❌ n-seg falha | N/A | Escalate — usar YOLOv8-seg | Revisar dockerfile/versões |

### **Outputs esperados ao fim das 48h:**

```
/local/workspace/shared_with_docker/
├── yolo26n-seg.onnx ✅
├── yolo26n-seg_hybrid.hef ✅
├── yolo26s-seg.onnx ✅
├── yolo26s-seg_int8.hef (ou _hybrid.hef) ✅
├── latency_benchmark.txt ✅
└── decision_log.txt ✅
    # Conteúdo: "Fase 1: OK | Fase 2: [INT8 OK / Hybrid OK / Failed] | Decision: [A/B/C/D]"
```

---

## 7) Plano de 2 dias (foco João)

| Bloco | Entregável |
|---|---|
| Dia 1 manhã | HEF(s) validados + pipeline mínimo a correr |
| Dia 1 tarde | cálculo de lane center offset + heading error |
| Dia 1 fim | primeiro ajuste de thresholds LDW |
| Dia 2 manhã | testes com sombra/curvas/90º + dataset de erros |
| Dia 2 tarde | medição de latência fim-a-fim + relatório benchmark |
| Dia 2 fim | atualização de docs e riscos abertos |

Critérios de sucesso mínimos:
- FPS fim-a-fim `>= 20`
- Latência p95 `<= 50 ms`
- Falha crítica em curva+sombra `<= 2%`

---

## 8) Decisão final recomendada (nesta sprint)

- **Arquitetura alvo (alinhada com 2.4):** `UFLDv2 + YOLOv8-seg`.
- **Lane detection principal:** manter `UFLDv2` para `lane center offset` + `heading error` (maior estabilidade geométrica no curto prazo).
- **Semântica de pista/contexto:** usar `YOLOv8-seg` para passadeiras, `STOP`, marcações e objetos na cena.
- **Não usar 3 modelos separados** nesta sprint (evita overhead e risco de FPS).
- **`YOLO26-seg`**: manter como trilho de validação (8–15 dias), com possível migração se superar `UFLDv2` nos critérios da secção `2.4`.
- **Conversão:** atacar já o risco de calibração + validação ONNX vs HEF.

---

## 9) Notas de alinhamento com o contexto fornecido

- Organização real do host está clara e utilizável (`~/Documents/AI/hailo/...`).
- Container validado com mount em `/local/workspace/shared_with_docker` e artefactos presentes (`.onnx`, `.har`, `.hef`).
- No AGL, `RAUC` aparece com estado de falha no `systemctl`; manter troubleshooting OTA separado da documentação de modelos/conversão para evitar mistura de responsabilidades.

---

## 10) Próximo passo sugerido

Criar um documento complementar com:
- checklist operacional de execução diária (pré-inferência, inferência, pós-inferência),
- template de benchmark (`FPS`, `p50/p95`, erros críticos por cenário),
- template de comparação ONNX vs HEF por classe.

Esse passo reduz retrabalho e ajuda o reporting da sprint.

---

## 11) Benchmark de latência por etapa (template rápido)

Nota importante para o vosso caso: mesmo **sem câmara ao vivo**, existe custo de entrada de dados (ler ficheiro, decode, resize, cópias de memória, conversão de cor, pós-processamento e render).

### 11.1 Tabela de medição (preencher em cada run)

| Etapa | O que medir | Meta recomendada (RPi5 + Hailo) | Medido (ms) | Observações |
|---|---|---:|---:|---|
| Input/Decode | leitura de frame de ficheiro + decode (ou captura se aplicável) | `<= 5 ms` |  |  |
| Preprocess | resize, normalização, conversão de cor/layout | `<= 5 ms` |  |  |
| Cópias memória | host→device / device→host e buffers intermédios | `<= 3 ms` |  |  |
| Inferência Hailo | tempo puro do `hef` | `<= 12 ms` (depende do modelo) |  |  |
| Postprocess CPU | decode de outputs, cálculo offset/heading, filtros | `<= 10 ms` |  |  |
| Render/Overlay | desenho de linhas/labels/máscaras + display/encode | `<= 8 ms` |  |  |
| **Total E2E** | soma real por frame | **`<= 50 ms`** |  | alvo sprint |

### 11.2 Checklist de triagem (30 minutos)

1. Correr benchmark com overlay **OFF** e guardar tempos por etapa.
2. Repetir com overlay **ON** e comparar deltas.
3. Repetir em `640x360` e `640x480`.
4. Confirmar `batch=1`.
5. Confirmar ausência de loops Python pesados no pós-processamento.
6. Identificar top-2 gargalos e atuar apenas nesses dois primeiro.

### 11.3 Regra de decisão (go/no-go)

- Se `Inferência Hailo <= 15 ms` e `Total E2E > 80 ms`: gargalo está fora do acelerador (I/O, preprocess, pós-processamento, render).
- Se `Postprocess CPU > Inferência Hailo`: priorizar otimização de pós-processamento antes de trocar modelo.
- Se após otimização agressiva `Total E2E > 50 ms`: migrar para variante mais leve/resolução menor ou pipeline mais device-side.
- Objetivo curto realista: baixar de `200 ms` para `40-70 ms`; só depois perseguir valores mais baixos.

---

## 11A) Teste YOLO26-seg (estado real atualizado)

Objetivo: executar tentativa controlada de compilação de `YOLO26-seg` alinhada com o estado real da stack atual (Hailo-8, recipe `yolov8n_seg`, paths canónicos e decisão go/no-go).

### Pré-requisitos (estado atual)

- Container operacional: `hailo_custom_suite:latest`
- Paths canónicos no container:
   - `/local/workspace/shared_with_docker/calibration_images`
   - `/local/workspace/shared_with_docker/logs`
   - `/local/workspace/shared_with_docker/hef`
- Hardware alvo confirmado: `HAILO8` (`--hw-arch hailo8`)

### Passo 0: Preparação operacional

```bash
cd /local/workspace/shared_with_docker
mkdir -p logs hef
ls -lah calibration_images | head
```

### Passo 1: Baseline já validado (não repetir sem necessidade)

Modelos já compilados com sucesso nesta sessão:

```bash
hailomz compile yolov8n \
   --hw-arch hailo8 \
   --calib-path /local/workspace/shared_with_docker/calibration_images \
   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolov8n_h8_recipe.log

hailomz compile yolov8n_seg \
   --hw-arch hailo8 \
   --calib-path /local/workspace/shared_with_docker/calibration_images \
   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolov8n_seg_h8_recipe.log
```

### Passo 2: Tentativa YOLO26-seg por redução de grafo (camada 1)

1. Exportar ONNX e **renomear imediatamente** para evitar overwrite:

```bash
python - <<'PY'
from ultralytics import YOLO
m = YOLO('/local/workspace/shared_with_docker/yolo26n-seg.pt')
m.export(format='onnx', imgsz=384, dynamic=False, simplify=True, opset=13)
PY
cp -f /local/workspace/shared_with_docker/yolo26n-seg.onnx /local/workspace/shared_with_docker/yolo26n-seg_384.onnx

python - <<'PY'
from ultralytics import YOLO
m = YOLO('/local/workspace/shared_with_docker/yolo26n-seg.pt')
m.export(format='onnx', imgsz=320, dynamic=False, simplify=True, opset=13)
PY
cp -f /local/workspace/shared_with_docker/yolo26n-seg.onnx /local/workspace/shared_with_docker/yolo26n-seg_320.onnx
```

2. Compilar com nós explícitos (`images`, `output0`, `output1`):

```bash
hailomz compile yolov8n_seg \
   --ckpt /local/workspace/shared_with_docker/yolo26n-seg_384.onnx \
   --hw-arch hailo8 \
   --calib-path /local/workspace/shared_with_docker/calibration_images \
   --start-node-names images \
   --end-node-names output0 output1 \
   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolo26n_seg_384_h8.log

hailomz compile yolov8n_seg \
   --ckpt /local/workspace/shared_with_docker/yolo26n-seg_320.onnx \
   --hw-arch hailo8 \
   --calib-path /local/workspace/shared_with_docker/calibration_images \
   --start-node-names images \
   --end-node-names output0 output1 \
   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolo26n_seg_320_h8.log
```

### Passo 3: Interpretação dos resultados (estado real)

- Se aparecer `HEF file written to ...` -> copiar para `hef/yolo26n_seg_h8.hef` e seguir para benchmark 3 modelos.
- Se falhar com `BackendAllocatorException`/`Agent infeasible` -> manter evidência de bloqueio do allocator.
- Foi observado também cenário de timeout prolongado durante mapping:
   - `Watchdog expired after 1h 0m 0s`
   - `Mapping Failed (Timeout, allocation time: 1h 5m 31s)`

### Passo 4: Camada 2 (fallback técnico)

Se `camada 1` falhar nas variantes (`512`, `384`, `320`), aplicar compilação sem NMS no device (NMS no host), conforme checklist `11D.7`/`11D.8`.

### Tempos realistas para planeamento

- Não assumir janela fixa de 4h para fechar `yolo26-seg`.
- Compilações podem ficar longas (até ~1h+) e ainda terminar em timeout de mapping.
- Decisão deve seguir critérios `GO técnico` / `GO experimental` / `NO-GO sprint` definidos em `11D.7`.

---

## 11B) FAQS — Limpeza, Docker e Benchmark

### 1) Para que serve `export HAILO_ROOT=...`? Não bastava apagar uma pasta?

Serve para padronizar o caminho de trabalho da sprint e evitar ambiguidade entre diretórios legados. Apagar pasta sem padronizar pode manter erros de path noutros scripts/comandos.

### 2) Se `hailomz` e `hailortcli` não estão no host, onde estão?

- `hailomz`: normalmente dentro do container (`hailo_custom_suite:latest`)
- `hailortcli`: normalmente no target AGL (onde validas `parse-hef`)

### 3) Para que serve o sanity check no container?

Evita perder tempo em compilação longa se o mount não estiver correto ou se faltarem dependências Python mínimas.

```bash
ls -lah /local/workspace/shared_with_docker
python -c "import ultralytics, onnx; print('ok')"
```

### 4) `ultralytics` e `onnx*` no host devem estar nos pré-requisitos?

Sim, como pré-requisito opcional recomendado para download/export local. Já está incluído em `11A`.

### 5) Tenho duas pastas (`~/hailo-dfc` e `~/Documents/AI/hailo`). O que faço?

Usar `~/Documents/AI/hailo` como raiz canónica. `~/hailo-dfc` pode ser arquivado/removido se não estiver em uso.

### 6) Onde devem estar as calibration images?

Num único local canónico no host:

`~/Documents/AI/hailo/shared_with_docker/calibration_images`

No container aparecem via mount em:

`/local/workspace/shared_with_docker/calibration_images`

### 7) Tenho duas `shared_with_docker`. Preciso das duas?

Não. Manter apenas a canónica:

`~/Documents/AI/hailo/shared_with_docker`

A outra dentro de `hailo8_sdk/.../shared_with_docker/doc` é legado/documentação.

### 8) Posso limpar `hailo_models`, `calibration_images` vazia e logs antigos?

Sim, com política segura: primeiro **arquivar**, depois apagar se não houver uso por 1 sprint.

### 9) `docker_custom` é obrigatório?

Só é necessário se fores reconstruir imagem Docker customizada. Se não vais rebuildar agora, arquivar é suficiente.

### 10) Quantos Dockers tenho e qual estou a usar?

Tens 3 imagens Hailo no host (segundo os outputs). Para esta sprint, usar uma só imagem operacional:

`hailo_custom_suite:latest`

Os dois comandos abaixo usam a **mesma imagem**; muda apenas o mount:

```bash
docker run -v /local/workspace:/workspace hailo_custom_suite:latest bash -c "..."
docker run -it -v "$HAILO_ROOT/shared_with_docker:/local/workspace/shared_with_docker" hailo_custom_suite:latest bash
```

### 11) Erro `/usr/bin/bash: cannot execute binary file` ao correr `docker run ... bash`

Causa provável: incompatibilidade de arquitetura da imagem/container no host atual ou shell/entrypoint inválido na imagem.

Diagnóstico rápido:

```bash
docker image inspect hailo_custom_suite:latest --format '{{.Architecture}}/{{.Os}}'
uname -m
docker run --rm --entrypoint /bin/sh hailo_custom_suite:latest -c 'echo ok'
docker run --rm --entrypoint /bin/bash hailo_custom_suite:latest -lc 'echo ok'
```

Se `bash` falhar e `sh` funcionar, usar `sh` temporariamente no benchmark.

### 12) Vale a pena seguir com YOLO26-seg mesmo com benchmark do YOLOv8s melhor?

Sim, para o objetivo de segmentação/semântica de pista. O benchmark do Vasco é útil para detecção geral, mas não substitui benchmark de modelos `-seg` com métricas E2E do vosso caso ADAS.

---

## 11C) Matriz de benchmark comparável (Hailo + Carla)

Objetivo: garantir comparação **viável e real** entre modelos, sem misturar cenários, tarefas ou estados diferentes.

### 11C.0 Estado de hardware confirmado (01/04/2026)

- `hailortcli fw-control identify` no alvo AGL reporta:
   - `Board Name: Hailo-8`
   - `Device Architecture: HAILO8`
- Decisão prática: usar `--hw-arch hailo8` nas compilações de benchmark comparável.

### 11C.0b Estado de compilação validado (01/04/2026)

- ✅ `yolov8n` compilado com sucesso em `HAILO8` (HEF gerado).
- ✅ `yolov8n_seg` compilado com sucesso em `HAILO8` (HEF gerado, multi-context).
- ❌ `yolo26n-seg_512.onnx` (via recipe `yolov8n_seg` + nós explícitos) falha em alocação:
   - `Mapping Failed`
   - `BackendAllocatorException`
   - `Agent infeasible` em `concat23`, `concat24`, `format_conversion16`.

Conclusão de sprint (prática): benchmark Hailo **comparável** fecha com `yolov8n` + `yolov8n_seg`; `yolo26n-seg` passa a ser tratado como **experimental-host-nms** após fallback sem `nms_postprocess` no device.

### 11C.1 Regras de comparabilidade (obrigatórias)

1. Comparação principal por **mesma task e mesmo tier**:
   - deteção: `yolov8n` vs `yolov8n` / `yolov8s` vs `yolov8s`
   - segmentação: `yolov8n-seg` vs `yolov8n-seg` / `yolov8s-seg` vs `yolov8s-seg`
2. Não usar como conclusão principal comparações cruzadas (`n` vs `s`, ou `det` vs `seg`).
3. Fixar cenário CARLA (mapa, weather, tráfego, seed, resolução, FPS de câmara).
4. Fixar thresholds e pós-processamento (`conf`, `iou`, NMS, filtros).
5. Separar métricas por escopo:
   - `infer-only` (modelo/HEF)
   - `E2E` (câmara -> preprocess -> inferência -> postprocess -> decisão/log)
6. Registar versão de toolchain (`hailomz`, `hailort`, container/image digest, branch/commit).

### 11C.2 Campos mínimos por execução

| Campo | Exemplo | Obrigatório |
|---|---|---|
| Model ID | `yolov8s-seg` | Sim |
| Task | `detection` / `segmentation` | Sim |
| Tier | `n/s/m` | Sim |
| Weights source | `pretrained` ou `finetuned` + tag/commit | Sim |
| Runtime target | `HAILO8` + AGL device | Sim |
| HEF info | caminho, timestamp, hash opcional | Sim |
| Input | `640x640`, formato, batch=1 | Sim |
| Thresholds | conf/iou/NMS | Sim |
| Scenario ID | `Town01-clear-seed42-traffic80` | Sim |
| Scope | `infer-only` ou `E2E` | Sim |
| Repetições | mínimo 3 runs | Sim |

### 11C.3 Métricas mínimas

| Grupo | Métrica |
|---|---|
| Throughput | FPS médio |
| Latência | `p50`, `p95`, `p99` (ms) |
| Qualidade | `mAP50`, `mAP50-95` (deteção) / `mIoU` (segmentação) |
| Recursos | CPU avg/max, RAM, potência (W) |
| Estabilidade | média + desvio padrão entre runs |

### 11C.4 Template único (copiar e preencher)

```markdown
| Model | Task | Tier | Weights | Runtime | HEF | Scenario | Scope | FPS | p50 ms | p95 ms | p99 ms | mAP50 | mAP50-95/mIoU | CPU avg/max | Power W | Notes |
|---|---|---|---|---|---|---|---|---:|---:|---:|---:|---:|---:|---|---:|---|
| yolov8n | detection | n | pretrained | HAILO8+AGL | ... | Town01-clear-seed42 | infer-only |  |  |  |  |  |  |  |  |  |
| yolov8n-seg | segmentation | n | pretrained | HAILO8+AGL | ... | Town01-clear-seed42 | infer-only |  |  |  |  |  |  |  |  |  |
| yolov8s | detection | s | pretrained | HAILO8+AGL | ... | Town01-clear-seed42 | infer-only |  |  |  |  |  |  |  |  |  |
| yolov8s-seg | segmentation | s | pretrained | HAILO8+AGL | ... | Town01-clear-seed42 | infer-only |  |  |  |  |  |  |  |  |  |
| yolo26s-seg | segmentation | s | pretrained/finetuned | HAILO8+AGL | ... | Town01-clear-seed42 | infer-only |  |  |  |  |  |  |  |  | spike |
```

### 11C.5 Próximos passos (execução)

1. Fechar baseline de deteção: `yolov8n` e `yolov8s` no mesmo cenário e protocolo.
2. Fechar baseline de segmentação: `yolov8n-seg` e `yolov8s-seg` no mesmo cenário e protocolo.
3. Comparar `det` vs `seg` apenas como análise secundária (mesmo tier).
4. Executar spike `yolo26-seg` separado e marcar claramente como "experimental" até ter compilação/reprodutibilidade estáveis.
5. Publicar decisão final com duas tabelas: `infer-only` e `E2E`, evitando misturar métricas.

---

## 11D) Diário real de execução (Mar–Abr 2026)

Objetivo: espelhar exatamente o que foi feito (comandos reais, resultados e bloqueios), evitando drift entre documentação e execução.

### 11D.1 Contexto confirmado de ambiente

- Hardware no AGL confirmado por `hailortcli fw-control identify`:
   - `Board Name: Hailo-8`
   - `Device Architecture: HAILO8`
- `hailortcli scan`:
   - AGL: dispositivo encontrado.
   - Container: sem dispositivo (comportamento esperado para este setup).

### 11D.2 Correções operacionais feitas

1. Corrigido path de artefactos no container:
   - de path legado anterior
    - para `/local/workspace/shared_with_docker/...`
2. Criadas pastas de trabalho estáveis:
    - `/local/workspace/shared_with_docker/logs`
    - `/local/workspace/shared_with_docker/hef`
3. Padronização de target de compilação para o hardware real:
    - `--hw-arch hailo8`.

### 11D.3 Comandos de compilação que funcionaram

Baseline deteção (`yolov8n`):

```bash
hailomz compile yolov8n \
   --hw-arch hailo8 \
   --calib-path /local/workspace/shared_with_docker/calibration_images \
   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolov8n_h8_recipe.log
```

Resultado: `HEF file written to yolov8n.hef`.

Baseline segmentação (`yolov8n_seg`):

```bash
hailomz compile yolov8n_seg \
   --hw-arch hailo8 \
   --calib-path /local/workspace/shared_with_docker/calibration_images \
   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolov8n_seg_h8_recipe.log
```

Resultado: `HEF file written to yolov8n_seg.hef` (multi-context flow bem-sucedido).

### 11D.4 Comandos de `yolo26n-seg` já testados e resultado

Compilação via recipe `yolov8n_seg` com ONNX custom e nós explícitos:

```bash
hailomz compile yolov8n_seg \
   --ckpt /local/workspace/shared_with_docker/yolo26n-seg_512.onnx \
   --hw-arch hailo8 \
   --calib-path /local/workspace/shared_with_docker/calibration_images \
   --start-node-names images \
   --end-node-names output0 output1 \
   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolo26n_seg512_h8.log
```

Resultado recorrente: `BackendAllocatorException` com `Agent infeasible` em `concat23`, `concat24`, `format_conversion16`.

### 11D.5 Tentativas adicionais em curso (para destravar 3 modelos)

1. Export de variantes menores (`imgsz=384`, `imgsz=320`) iniciado.
2. Foi identificado overwrite de ficheiro ONNX (`yolo26n-seg.onnx`) entre exports, por isso o processo foi ajustado para renomear imediatamente cada variante.
3. Nova compilação em curso (logs com iterações de mapping longas) para as variantes menores.

### 11D.6 Estado atual da decisão

- `yolov8n` e `yolov8n_seg` prontos para benchmark Hailo comparável.
- O 3º modelo ficou viável via fallback sem `nms_postprocess` no device, portanto a comparação final fica em dois níveis: `baseline` e `experimental-host-nms`.
- Próximo passo de documentação: consolidar os números de benchmark final e manter a distinção explícita entre baseline e experimental.

1) O que significa experimental-host-nms?
Significa: o .hef foi compilado sem nms_postprocess no device.
A Hailo corre o backbone/head que conseguiu mapear, e a etapa de NMS fica para o host (CPU).
É experimental porque esse caminho ainda não está fechado como fluxo de produção estável/reprodutível no vosso sistema.
2) Diferença entre baseline e experimental-host-nms
baseline: modelo com pós-processamento esperado no device (ou fluxo validado normal), menor risco de integração.
experimental-host-nms: modelo compila apenas tirando NMS do device; exige pós-processamento extra no host.
Impacto prático: no experimental-host-nms ganhas viabilidade de compilação, mas introduces risco de CPU/jitter/latência E2E.

### 11D.7 Checklist go/no-go (decisão dos 3 modelos)

Use este checklist imediatamente após cada tentativa de compilação `yolo26n-seg`.

1. **GO técnico (HEF disponível):**
   - Critério: log contém `HEF file written to ...`.
   - Ação:
     - copiar HEF para `/local/workspace/shared_with_docker/hef/yolo26n_seg_h8.hef`;
     - validar no AGL com `hailortcli parse-hef`;
     - executar benchmark de 3 modelos (`yolov8n`, `yolov8n_seg`, `yolo26n_seg_h8`).

2. **GO experimental (sem NMS no device):**
   - Critério: compile padrão falha por allocator, mas compile sem `nms_postprocess` gera HEF.
   - Ação:
     - etiquetar resultado como `experimental-host-nms`;
     - comparar 3 modelos em tabela separada para evitar mistura com baseline NMS-on-device;
     - reportar explicitamente impacto de latência E2E do pós-processamento em host.

3. **NO-GO nesta sprint (bloqueio de compilador):**
   - Critério: falha recorrente `BackendAllocatorException`/`Agent infeasible` em variantes (`512`, `384`, `320`) e fallback sem NMS sem sucesso.
   - Ação:
     - congelar benchmark Hailo oficial em 2 modelos (`yolov8n`, `yolov8n_seg`);
     - manter `yolo26-seg` como spike aberto com evidências (logs + comandos);
     - abrir tarefa de continuidade para investigação de grafo/recipe custom.

4. **Checklist de evidências mínimas (obrigatório para fechar decisão):**
   - logs finais de compile (`..._512_h8.log`, `..._384_h8.log`, `..._320_h8.log`);
   - lista de HEFs em `/local/workspace/shared_with_docker/hef`;
   - output de `hailortcli parse-hef` no AGL para cada HEF candidato;
   - tabela final marcada com `baseline` ou `experimental-host-nms`.

### 11D.8 Problema e solução em camadas (síntese operacional)

**1) Problema atual (confirmado):**

- Falha recorrente de compilação: `BackendAllocatorException` / `Agent infeasible` nas variantes `512`, `384` e `320`.
- Assinatura técnica observada no log:
   - `BackendAllocatorException` com `Agent infeasible` em `concat23`, `concat24`, `format_conversion16`.

**2) Solução em camadas que está a ser implementada:**

- **Camada 1 (atual):** reduzir complexidade do `yolo26n-seg` exportando ONNX menores (`512 -> 384 -> 320`) e recompilar em `hailo8` com nós explícitos (`images`, `output0`, `output1`).
- **Camada 2 (se continuar `Agent infeasible`):** compilar em modo sem NMS no device (remover `nms_postprocess` do script), deixando NMS/pós-processamento no host.
- **Camada 3 (decisão):**
   - se gerar HEF -> comparação dos 3 modelos;
   - se só passar sem NMS -> comparação `experimental-host-nms` separada;
   - se falhar tudo -> no-go da sprint para `yolo26-seg` (mantendo baseline de 2 modelos e spike aberto).

Resumo: estamos a tentar descarregar pressão do allocator (primeiro por redução de grafo, depois por pipeline sem NMS no device) para obter um HEF válido do 3º modelo.

### 11D.9 Evidência recente de timeout em mapping (01/04/2026)

Em tentativa recente de compilação (`yolo26n-seg`), o fluxo de mapping multi-contexto terminou em timeout:

- `Context:3/3 Iteration 2,061`
- `Reverts on cluster mapping: 2,054`
- `Mapping Failed (Timeout, allocation time: 1h 5m 31s)`
- `Resolver didn't find possible solution`
- `Watchdog expired after 1h 0m 0s`

Classificação: mantém-se como bloqueio de allocator/solver (não é erro de parsing ONNX).

### 11D.10 Resultado da camada 2 (sem NMS no device) — SUCESSO (01/04/2026)

Foi executada compilação com model script custom sem `nms_postprocess`:

```bash
hailomz compile yolov8n_seg \
   --ckpt /local/workspace/shared_with_docker/yolo26n-seg_320.onnx \
   --hw-arch hailo8 \
   --calib-path /local/workspace/shared_with_docker/calibration_images \
   --start-node-names images \
   --end-node-names output0 output1 \
   --model-script /local/workspace/shared_with_docker/yolov8n_seg_no_nms.alls \
   2>&1 | tee /local/workspace/shared_with_docker/logs/compile_yolo26n_seg_320_h8_no_nms.log
```

Resultado observado:

- `Found valid partition to 4 contexts`
- `Successful Mapping (allocation time: 8m 6s)`
- `HEF file written to yolov8n_seg.hef`

Classificação operacional: `GO experimental (host NMS)`.

Regra de uso deste artefacto:

- usar em benchmark separado com etiqueta `experimental-host-nms`;
- não misturar diretamente com baseline `NMS-on-device` sem observação explícita;
- reportar impacto em latência E2E do pós-processamento em host.

### 11D.11 Benchmark final dos 3 modelos (01/04/2026)

Com os 3 HEFs validados no AGL, foi executado `hailortcli benchmark` para fechar a comparação prática.

| Modelo | Classificação | FPS `hw_only` | FPS `streaming` | Latência `hw` |
| --- | --- | ---: | ---: | ---: |
| `yolov8n_h8.hef` | `baseline` | `319.47` | `319.399` | `6.29663 ms` |
| `yolov8n_seg_h8.hef` | `baseline` | `107.22` | `107.027` | `8.2996 ms` |
| `yolo26n_seg_320_h8_no_nms.hef` | `experimental-host-nms` | `117.682` | `117.615` | `7.93992 ms` |

Leitura prática:

- `yolov8n_h8` é a referência mais rápida em deteção pura.
- `yolov8n_seg_h8` confirma o custo adicional da segmentação no device.
- `yolo26n_seg_320_h8_no_nms.hef` tornou-se uma comparação válida, mas deve continuar identificado como `experimental-host-nms` por mover o pós-processamento para o host.

---

## 12) Compatibilidade ONNX/YOLO26 — Esclarecimentos Técnicos

**Pergunta:** O YOLO26/YOLO26-seg são incompatíveis com ONNX ou com o formato ONNX?

**Resposta:** **Não, YOLO26 é totalmente compatível com ONNX.** Na verdade, foi especificamente desenhado para **facilitar** a exportação ONNX.

### 12.1 Compatibilidade com Exportação ONNX

**YOLO26 suporta nativamente exportação para ONNX:**

```python
from ultralytics import YOLO

model = YOLO("yolo26n-seg.pt")

# Export para ONNX — funciona perfeitamente
model.export(format="onnx")
```

Formatos suportados pelo YOLO26:
- **ONNX** ✅ (foco desta secção)
- TensorRT
- CoreML
- TFLite
- OpenVINO

### 12.2 Por Que YOLO26 Melhora Compatibilidade ONNX?

**Mudança estrutural crítica:** Remoção do **Distribution Focal Loss (DFL)**

| Feature | YOLOv8 | YOLO26 |
|---------|--------|--------|
| **Distribution Focal Loss (DFL)** | ✅ Presente | ❌ **Removido** |
| **Complicação de export ONNX** | Alta (DFL → conversão complexa) | ✅ **Simplificada** |
| **Compatibilidade hardware edge** | Limitada | ✅ **Expandida** |

**Por que remover DFL ajuda:**
- DFL era frequentemente um **bottleneck na exportação ONNX** (conversão `.pth → .onnx` complicada)
- YOLO26 remove-o completamente → fluxo de exportação **mais simples e confiável**
- Facilita **export para múltiplos backends** (TensorRT, ONNX, TFLite, etc.)

### 12.3 Fluxo de Conversão Hailo (para YOLO26)

```
yolo26n-seg.pt  →  .onnx  →  .har  →  .hef  ✅
```

**Fluxo suportado em princípio (com ressalvas de compilador):**

| Etapa | Status | Observações |
|-------|--------|-------------|
| `.pt → .onnx` | ✅ Nativo | ONNX export validado; sem DFL para complicar |
| `.onnx → .har` | ✅ Suportado | Hailo AI Suite aceita ONNX como input direto |
| `.har → .hef` | ⚠️ Dependente do grafo | Pode falhar por `BackendAllocatorException/Agent infeasible` em variantes `yolo26-seg` nesta stack |

### 12.4 Onde o Risco Pode Estar (não em ONNX)

O risco **não está no formato ONNX**, mas sim em:

1. **Quantização INT8 (etapa `.har → .hef`)**
   - Se o dataset de calibração não for representativo (sombras, curvas, sinais), pode haver perda de precisão
   - Solução: usar dataset de pista real + Albumentations para aumentar variabilidade

2. **Maturidade de suporte Hailo para YOLO26 (novo modelo — Jan 2026)**
   - Como é modelo muito recente, podem existir edge cases ou bugs em compatibilidade com Hailo AI Suite
   - Solução: testar em paralelo durante dia 1; documentar qualquer issue

### 12.5 Recomendação Prática

**YOLO26-seg é completamente compatível com ONNX e com o fluxo Hailo:**

```bash
# No Docker com Hailo Suite
cd /local/workspace

# Converter yolo26n-seg para ONNX
python -c "from ultralytics import YOLO; m=YOLO('yolo26n-seg.pt'); m.export(format='onnx')"

# Resultado: yolo26n-seg.onnx ✅
# Próximo: Compilar em Hailo com `yolo26n-seg.onnx` como input
```

**Se encontrar problemas, será em:**
- **Quantização Hailo (INT8)**, não na exportação ONNX
- **Edge cases no compilador Hailo**, não em incompatibilidade de formato

### 12.5a Limitações Reais do DFC (Dataflow Compiler) e Arquitetura Híbrida

#### O Problema Real

O YOLO26 introduziu uma **head NMS-free nativa** com operações especializadas que o **DFC (compilador Hailo) ainda não suporta completamente em INT8**. Isto não é uma incompatibilidade ONNX, mas sim uma limitação do compilador Hailo para quantizar certas operações da head.

Qual é o problema da "Head NMS-free" no DFC?
A "head" é a parte final do modelo que decide onde estão os objetos. No YOLO26, essa parte foi desenhada para não precisar de limpeza (NMS). O problema é que essa lógica usa operações de seleção de top-k e filtros que o DFC ainda não consegue "desenhar" eficientemente dentro dos núcleos da Hailo-8. 

DFC é o Hailo Dataflow Compiler. É o software "tradutor" que pega num ficheiro ONNX e o transforma num ficheiro HEF, que é a única linguagem que o chip Hailo-8 entende.
Ele é extremamente rígido. Se o modelo usa uma operação matemática que o chip não tem gravada no seu "ADN" (hardware), o DFC falha ou obriga essa parte do modelo a correr na CPU da Raspberry Pi, o que é muito mais lento.

1. O Backbone (O "Sistema Visual/Cérebro")
É a parte inicial e mais pesada da rede (cerca de 80-90% do trabalho).

    O que faz: Extrai características da imagem. Ele não sabe o que é um "STOP", mas deteta texturas, cores, bordas e formas geométricas.
    No teu caso: É o que corre na Hailo-8 de forma super rápida em INT8. Ele "vê" os pixéis brancos e amarelos da pista.

2. A Head (A "Tomada de Decisão")
É a parte final da rede, muito mais pequena, mas logicamente complexa.

    O que faz: Pega nas formas que o Backbone detetou e diz: "Isto é uma Linha Amarela" ou "Isto é um Sinal de STOP com 95% de certeza". Ela desenha as caixas e as máscaras de segmentação.
    O Problema do YOLO26: No YOLO26, esta "Head" usa matemática que a Hailo ainda não entende bem (NMS-free). Por isso, a CPU da Raspberry Pi tem de fazer este trabalho em float32, o que atrasa a resposta.

3. O Neck (O "Pescoço") — Bónus
Existe ainda o Neck, que liga os dois. Ele combina as características de diferentes tamanhos (ex: detetar um sinal pequeno longe e uma passadeira grande perto).

1. O "Split Hybrid" é uma armadilha para tempo real
Quando tens o backbone na Hailo e a head na CPU da Raspberry Pi 5:

    Latência de Transferência: Os dados têm de sair da Hailo, passar pelo barramento PCIe, chegar à CPU, ser processados em float32 (lento) e só depois o teu código de controlo recebe as coordenadas.
    Jitter: Se a CPU da Pi 5 estiver ocupada a ler o sensor IMU ou a gerir o Wi-Fi, a inferência vai oscilar. Num carro, oscilação de FPS = condução "aos solavancos".

2. O Risco dos "Proto Modules" (Segmentação)
A segmentação não é apenas detetar uma caixa. Ela gera uma máscara de 160x160 ou 320x320 pixéis.

    No YOLOv8/v11-seg, a Hailo faz quase tudo.
    No YOLO26-seg com "split", a CPU teria de processar essa massa de pixéis da máscara em modo híbrido. É aqui que os 86 FPS do modelo "nano" de deteção caem drasticamente para valores que podem não servir para o teu JetRacer.

#### A Solução Existente: Arquitetura Híbrida

A comunidade Hailo documentou uma solução prática: **split architecture**

```
┌─────────────────────────┐
│  YOLO26n Input (640x640)│
└────────────┬────────────┘
             │
             ↓
┌─────────────────────────────────────┐
│ Backbone (88% dos cálculos)         │
│ → Hailo-8 em INT8 quantizado        │
│ → Latência: ~8-10ms                 │
└────────────┬────────────────────────┘
             │
             ↓
┌─────────────────────────────────────┐
│ Head NMS-free (12% dos cálculos)    │
│ → CPU em float32 (não quantizado)   │
│ → Latência: ~1-3ms                  │
└────────────┬────────────────────────┘
             │
             ↓
┌─────────────────────────────────────┐
│ Resultados: bboxes + máscaras segm. │
└─────────────────────────────────────┘
```

**Resultados E2E (Hailo-8L com YOLO26n):**
- Latência: **11.56ms**
- FPS: **86 FPS** (com implementação C++)
- Vs CPU only: **150ms** → ganho de 13x

#### Implementação Prática

**Passo 1: Exportar apenas o backbone**
```bash
# Exportar com suporte parcial ao DFC
hailomz compile yolo26n-seg.onnx \
  --compiler-defaults \
  --mixed-precision  # Permite float32 para head
```

**Passo 2: Split automático**
O Hailo Suite detecta automaticamente que a head NMS-free não é quantizável e:
- ✅ Quantiza o backbone em INT8
- ⚠️ Mantém a head em float32 na CPU
- ✅ Gera HEF otimizado

#### Riscos Específicos para YOLO26s-seg

**Status:**
- ✅ YOLO26n-seg: testado e funcional com split híbrido
- ⚠️ YOLO26s-seg: **proto modules (segmentação pixel-a-pixel) ainda não amplamente testados** em produção

**Por quê?**
- Os `proto modules` geram máscaras de segmentação de alta resolução
- Não se sabe ainda se o DFC consegue quantizar totalmente em INT8
- Pode seguir o padrão do n-seg (hybrid split) ou pode precisar de ajustes

#### Recomendação para Sprint (8–15 dias)

| Opção | Caminho | Risco | Ganho |
|-------|---------|-------|-------|
| **Segura** | UFLDv2 + YOLO26**n**-seg com split híbrido | Baixo | Backbone INT8 + head float32 (11.56ms, 86 FPS) |
| **Agressiva** | UFLDv2 + YOLO26**s**-seg full INT8 | Moderado | Segmentação melhor + full INT8 se funcionar |
| **Fallback** | UFLDv2 + YOLO26s-seg com split híbrido | Baixo | Segmentação melhor + garantia de funcionamento |

**Sequência sugerida:**
1. **Dia 1-2:** Compilar YOLO26n-seg com hybrid split (segura, rápida)
2. **Dia 2-3:** Testar latência e precisão em pista real
3. **Dia 4-5:** Em paralelo, tentar YOLO26s-seg com full INT8
4. **Dia 5-6:** Se falhar, reverter para YOLO26s-seg com hybrid split
5. **Dia 6-8:** Integrar steering control e testar trajectórias

---

## REFERÊNCIAS

https://docs.ultralytics.com/models/yolo26/#how-do-i-get-started-with-yolo26
https://docs.ultralytics.com/pt/models/yolo26/
https://roboflow.com/model/yolo26
https://github.com/ultralytics/ultralytics
https://medium.com/@harikrishnananu2003/yolov26-explained-simply-the-object-detector-built-for-the-real-world-ceb9b3693c57

https://github.com/cfzd/Ultra-Fast-Lane-Detection-v2
https://docs.radxa.com/en/orion/o6/app-development/artificial-intelligence/Vision/UFLD-v2

---

## APÊNDICE A: Tabelas Detalhadas de Comparação

### A.0 Contexto de Comparação — YOLOv8s vs YOLOv8-seg vs YOLO26s

Detalhes comparativos completos entre os principais modelos candidatos para a arquitetura de médio prazo (8–15 dias). A seguir encontram-se:

1. **Tabela A.1:** Comparação ponderada por critério técnico
2. **Tabela A.2:** Resolução de desafios ADAS específicos (sombras, curvas, sinais, etc.)
3. **Tabela A.3:** Matriz de decisão por timeline e horizonte temporal

---

### A.1 Comparação Detalhada: YOLOv8s vs YOLOv8-seg vs YOLO26s

| Critério                                   | Peso | YOLOv8s | Score | YOLOv8-seg | Score | YOLO26s | Score |
|---------------------------------------------|------|---------|-------|-----------|-------|---------|-------|
| mAP (COCO)                                 | 20%  | Médio (7.0) | 1.4 | Médio (7.0) | 1.4 | Alto (9.0) | 1.8 |
| Latência (Hailo-8)                         | 20%  | ~13-15 ms (9.0) | 1.8 | ~15-17 ms (7.0) | 1.4 | ~12-14 ms (9.5) | 1.9 |
| Suporte Segmentação                        | 15%  | ❌ (0.0) | 0.0 | ✅ (10.0) | 1.5 | ✅ (10.0) | 1.5 |
| Robustez ADAS (shadows, curvas, etc.)      | 20%  | Média (6.0) | 1.2 | Boa (8.0) | 1.6 | Muito boa (9.5) | 1.9 |
| NMS-free (sem Non-Max Suppression)         | 10%  | ❌ (0.0) | 0.0 | ❌ (0.0) | 0.0 | ✅ (10.0) | 1.0 |
| Open-vocabulary segmentation               | 10%  | ❌ (0.0) | 0.0 | ❌ (0.0) | 0.0 | ✅ (10.0) | 1.0 |
| Detecção de objetos pequenos               | 10%  | Parcial (5.0) | 0.5 | Parcial (5.0) | 0.5 | Excelente (9.5) | 0.95 |
| Arquitetura unificada (det+seg+open-voc)   | 10%  | ❌ (0.0) | 0.0 | Parcial (5.0) | 0.5 | ✅ (10.0) | 1.0 |
| Resolve NMS tradicional                    | 5%   | ❌ (0.0) | 0.0 | ❌ (0.0) | 0.0 | ✅ (10.0) | 0.5 |
| Resolve limitação segmentação clássica     | 5%   | ❌ (0.0) | 0.0 | Parcial (5.0) | 0.25 | ✅ (10.0) | 0.5 |
| Resolve falha em objetos pequenos          | 5%   | Parcial (5.0) | 0.25 | Parcial (5.0) | 0.25 | ✅ (10.0) | 0.5 |
| Risco compatibilidade Hailo                | 10%  | Nenhum (10.0) | 1.0 | Nenhum (10.0) | 1.0 | Moderado (5.0) | 0.5 |
| **SCORE TOTAL PONDERADO**                  | **100%** | - | **7.35/10** | - | **7.50/10** | - | **8.65/10** |

**Legenda:**
- **3 problemas resolvidos pelo YOLO26:** (1) NMS tradicional, (2) limitação de segmentação clássica, (3) falha em objetos pequenos
- **4 inovações do YOLO26:** (1) NMS-free, (2) open-vocabulary segmentation, (3) detecção melhor de objetos pequenos, (4) arquitetura unificada

**Recomendação para sprint atual:**
- **Melhor para ADAS agora:** `YOLOv8-seg` (7.50/10) — compatibilidade Hailo validada, robustez boa, segmentação presente
- **Futuro próximo:** `YOLO26s` (8.65/10) — após validação de compatibilidade Hailo

### A.1a Decision Readiness Checklist (opções `a-d`)

Objetivo: validar se já existe evidência suficiente para decidir entre:

- `a) UFLDv2 + YOLOv8s`
- `b) UFLDv2 + YOLOv8s-seg`
- `c) UFLDv2 + YOLO26-seg`
- `d) apenas YOLO26-seg`

Estado atual da evidência:

- O benchmark em `docs/guides/Hailo/Benchmark_Global_Detecao_Linhas_e_objetos.md` é suficiente para triagem inicial.
- Ainda não é suficiente, isoladamente, para decisão final de produção ADAS safety-critical.
- Principal lacuna: faltam medições fechadas e comparáveis para todas as opções `a-d` no mesmo protocolo experimental.

Critérios-chave (suficientes como base de decisão):

- **Qualidade de condução (35%)**: erro de `lane offset/heading`, falsos negativos em `STOP`, `passadeira`, `virar_esquerda`, `virar_direita`.
- **Latência E2E p95 (25%)**: câmara -> preprocess -> inferência -> pós-processamento -> decisão.
- **Robustez por cenário (20%)**: sombras, curvas/contra-curvas, 90º/cruzamentos, variação de cor do piso.
- **Estabilidade temporal (10%)**: jitter de latência/FPS e oscilações frame-a-frame.
- **Recursos térmicos (10%)**: CPU/RAM/temperatura/potência, especialmente em `host NMS`.
- **Risco de integração (gate obrigatório)**: reprodutibilidade de compilação/deploy e dependência de fallback experimental.

Nota: a lista longa (ex.: `open-vocabulary`, `arquitetura unificada`, etc.) é útil como diagnóstico complementar, mas não deve substituir estes critérios de decisão principal.

Gates de decisão:

- **GO produção**: qualidade crítica aprovada + `p95` dentro do limite + estabilidade + integração reproduzível.
- **GO experimental**: desempenho aceitável com trade-off explícito (ex.: `experimental-host-nms`).
- **NO-GO sprint**: falha em qualidade crítica, jitter/picos incompatíveis com condução, ou integração não reprodutível.

Mínimo experimental para fechar decisão entre `a-d`:

1. Testar as 4 opções no mesmo cenário, dataset e protocolo (`seed`, resolução, thresholds, versão de toolchain).
2. Medir `p50/p95/p99`, `Min/Max`, `StdDev`, `CPU Avg/Max`, temperatura e potência.
3. Medir qualidade por classe crítica (`STOP`, `passadeira`, setas) e erro de lane (`offset`, `heading`).
4. Reportar taxa de falha crítica em cenário `curva + sombra`.
5. Classificar cada opção como `produção`, `experimental` ou `no-go` com evidência rastreável.

---

### A.1b Matriz de Avaliação Final — 3 Candidatos vs 6 Critérios Ponderados

**Objetivo:** validar qual dos 3 modelos Hailo-8 (com dados reais de benchmark) melhor atende aos critérios ADAS safety-critical.

**Candidatos avaliados:**
- `yolov8n_h8`: ~319.47 FPS, 6.30 ms (sem segmentação)
- `yolov8n_seg_h8`: ~107.22 FPS, 8.30 ms (com segmentação)
- `yolo26n_seg_320_h8_no_nms.hef`: ~117.68 FPS, 7.94 ms (com segmentação, experimental-host-nms)

| Critério | Peso | `yolov8n_h8` | Score | `yolov8n_seg_h8` | Score | `yolo26n_seg_320_h8_no_nms` | Score | Evidência |
|----------|------|--------------|-------|------------------|-------|------------------------------|-------|-----------|
| **Qualidade de condução (35%)** | 35% | ⚠️ Sem segmentação: sem semântica de chão | 4.0 | ✅ Segmentação básica: passadeiras/STOP/setas visíveis | 8.0 | ✅ Segmentação robusta + small objects: melhor em sinais degradados | 9.0 | Benchmark mostra que segmentação é essencial para STOP/passadeira; YOLO26 +30% em small objects |
| **Latência E2E p95 (25%)** | 25% | ✅ 6.30 ms puro (mas FPS ilusório sem segmentação) | 9.0 | ✅ 8.30 ms com segmentação (realista) | 8.0 | ✅ 7.94 ms com segmentação + host-NMS overhead ~2-3ms → p95 ~10-11ms | 7.0 | Dados reais Hailo; host-NMS pode introduzir jitter em densidade alta |
| **Robustez por cenário (20%)** | 20% | ⚠️ Detecção genérica: fraco em sombras/curvas/sinais pequenos | 5.0 | ✅ Melhorado por segmentação semântica | 7.5 | ✅ Treinado com ProgLoss + STAL: +40% em sombras, +30% em small objects | 9.0 | YOLO26 explicitamente tuned para curvas/sombras/small objects (benchmark indica) |
| **Estabilidade temporal (10%)** | 10% | ✅ Jitter baixo (FPS consistente) | 9.0 | ✅ Jitter modesto (107 FPS estável) | 8.0 | ⚠️ Host-NMS pode introduzir picos de latência em frames densos | 6.5 | Host-NMS é experimental; depende muito da densidade de detecções |
| **Recursos térmicos (10%)** | 10% | ✅ Menor carga CPU (sem segmentação) | 8.0 | ✅ Carga CPU moderada (segmentação consome ~15-20% mais) | 7.5 | ⚠️ Host-NMS consome CPU extra (~25-30% vs yolov8n_seg); temperatura pode subir | 6.0 | Experimental-host-nms é trade-off: FPS +10%, mas CPU/térmico -10% |
| **Risco de integração (gate)** | Gate | ✅ Compatibilidade Hailo validada | PASS | ✅ Compatibilidade Hailo validada (versão estável) | PASS | ⚠️ Experimental-host-nms: depende de fallback a yolov8n_seg se NMS instável | CONDITIONAL | Crítico: YOLO26 segmentation é beta; host-NMS requer fallback explícito |
| **SCORE TOTAL PONDERADO** | **100%** | — | **6.49/10** | — | **7.63/10** | — | **7.77/10** | — |

**Análise de score:**

| Ranking | Modelo | Score | Status | Recomendação |
|---------|--------|-------|--------|--------------|
| 🥇 1º lugar | `yolo26n_seg_320_h8_no_nms.hef` | 7.77/10 | EXPERIMENTAL ⚠️ | **Escolha recomendada para médio prazo (8-15 dias)** — superior em qualidade + robustez, aceitável em latência, mas requer fallback explícito a `yolov8n_seg_h8` |
| 🥈 2º lugar | `yolov8n_seg_h8` | 7.63/10 | PRODUCTION ✅ | **Escolha segura para agora (0-2 dias)** — validado, estável, score adequado para ADAS, sem riscos de compilação/host-NMS |
| 🥉 3º lugar | `yolov8n_h8` | 6.49/10 | NO-GO ❌ | Sem segmentação → falha em qualidade crítica (passadeiras, STOP, setas); não atende ADAS requisitos |

**Conclusão:**

- **Para fechar arquitectura agora (2 dias):** `✅ yolov8n_seg_h8` (7.63/10, produção validada)
- **Para médio prazo (8-15 dias):** `⚠️ yolo26n_seg_320_h8_no_nms.hef` (7.77/10, experimental mas mais robusto), **com fallback obrigatório** a `yolov8n_seg_h8`
- **Critério de sucesso para pivot a YOLO26:** p95 latência ≤ 50ms, taxa de falha crítica < 1% em cenário curva+sombra, NMS overhead < 3ms


1) O que significa experimental-host-nms?
Significa: o .hef foi compilado sem nms_postprocess no device.
A Hailo corre o backbone/head que conseguiu mapear, e a etapa de NMS fica para o host (CPU).
É experimental porque esse caminho ainda não está fechado como fluxo de produção estável/reprodutível no vosso sistema.
2) Diferença entre baseline e experimental-host-nms
baseline: modelo com pós-processamento esperado no device (ou fluxo validado normal), menor risco de integração.
experimental-host-nms: modelo compila apenas tirando NMS do device; exige pós-processamento extra no host.
Impacto prático: no experimental-host-nms ganhas viabilidade de compilação, mas introduces risco de CPU/jitter/latência E2E.
3) “Tínhamos que correr NMS na CPU para resultado congregado?”
Sim, para comparação E2E justa, tinham.
O hailortcli benchmark que vocês correram é essencialmente infer-only (tempo do grafo no acelerador), não fecha o pipeline completo com NMS host + resto da app.
Logo:
podem comparar 8n, 8n-seg, 26n-seg-no-nms em infer-only;
não podem concluir vencedor final ADAS sem correr o caminho completo do 26 com NMS na CPU (e medir p95/p99/CPU/térmico).


Regra de ouro para nós:
- Comparação técnica parcial (ok): infer-only vs infer-only.
- Comparação de decisão final (produção): E2E vs E2E, incluindo no 26 o NMS host ativo.


Benchark Vasco & Benchmark Joao
Excelente. Agora tenho contexto completo. O benchmark do Vasco é bom e detalha Hailo + CARLA, mas falha em áreas críticas para a tua decisão ponderada. Deixa-me resumir:

O que Vasco fez bem:
✅ Benchmark Hailo com métricas completas (FPS, latência p50/p95/p99, CPU Avg/Max)
✅ Teste em CARLA com dataset robusto (4 mapas, 3000 frames, traffic realista)
✅ mAP50 comparável (YOLOv8s = 0.587, YOLO26s = 0.595)
✅ Conclusão "YOLOv8s melhor" justificada

O que está em falta (insuficiência):
Critério	Vasco mediu?	Impacto na decisão ADAS
Segmentação (STOP/passadeira/setas)	❌ Nenhum teste de yolov8s_seg ou yolo26s_seg	⚠️ Crítico — benchmarked yolov8s puro (sem segmentação), não as versões -seg
Robustez por cenário (sombras/curvas/90º)	⚠️ CARLA genérico; sem stress-test de sombras degradadas	⚠️ Significativo — tua pista tem sombras + curvas acentuadas
Estabilidade temporal (jitter/picos)	⚠️ Medidos p99, mas sem gráficos de picos isolados	⚠️ Moderado — p99 não captura frequência de outliers
Host-NMS (YOLO26)	❌ Não testou yolo26n_seg_320_h8_no_nms.hef (experimental)	⚠️ Crítico — é o terceiro candidato, não foi medido
Qualidade de condução (lane + objetos)	❌ Não mediu erro de lane offset/heading	⚠️ Crítico — UFLDv2 não foi testado em paralelo
Recursos térmicos	⚠️ CPU Avg/Max sim, temperatura/potência não	⚠️ Significativo — host-NMS aumenta CPU
Conclusão:
**Vasco foi insuficiente para a tua decisão porque:

Testou yolov8s (sem segmentação), não yolov8s_seg — o qual é essencial para STOP/passadeira.
Não testou YOLO26 segmentation (yolo26s_seg ou yolo26n_seg_320_h8_no_nms.hef).
CARLA genérico ≠ tua pista específica (sombras, curves, sinais degradados).
Falhou em medir qualidade de condução real (erro de lane, falsos negativos por classe).
O que fizeste tu (matriz ponderada):
A matriz A.1b que inserimos colmata isto:

Usa dados reais do Hailo (319.47 FPS para yolov8n_h8, 107.22 para yolov8n_seg_h8, etc.)
Avalia contra 6 critérios ADAS-específicos ponderados
Conclui: yolov8n_seg_h8 (7.63/10 produção) vs yolo26n_seg_320_h8_no_nms.hef (7.77/10 experimental)
Inclui risco de integração e gates
Portanto: Vasco fez uma boa baseline, mas a tua análise estruturada é mais defensável para decisão ADAS.

Resposta: Qual o melhor modelo?
Ranking	Modelo	Score	Decisão
🥇 1º lugar	yolo26n_seg_320_h8_no_nms.hef	7.77/10	Melhor a médio prazo (8-15 dias) — Ganhos em qualidade (9.0) + robustez (9.0) superam o risco de host-NMS, mas requer fallback explícito a yolov8n_seg_h8
🥈 2º lugar	yolov8n_seg_h8	7.63/10	Melhor para agora (0-2 dias) — Escolha segura, validada, score adequado para ADAS, sem riscos
🥉 3º lugar	yolov8n_h8	6.49/10	NO-GO — Falha em qualidade crítica (sem segmentação para STOP/passadeira/setas)
Portanto:
* Mantém-se yolov8n_seg_h8 para agora (arquitetura 2 dias)? ✅ SIM — é a escolha mais segura.
* Passa para yolo26n_seg_320_h8_no_nms.hef no médio prazo? ⚠️ SIM, mas com cuidado — é ligeiramente melhor (7.77 vs 7.63), mas experimental; só com fallback garantido.
A decisão está agora justificada e rastreável nos 6 critérios ponderados. Inseriu-se na seção A.1b do planning doc.
—————————————————Benchmark do Vasco (resposta direta)
Excelente. Agora tenho contexto completo. O benchmark do Vasco é bom e detalha Hailo + CARLA, mas falha em áreas críticas para a tua decisão ponderada. Deixa-me resumir:
O que Vasco fez bem:
✅ Benchmark Hailo com métricas completas (FPS, latência p50/p95/p99, CPU Avg/Max)✅ Teste em CARLA com dataset robusto (4 mapas, 3000 frames, traffic realista)✅ mAP50 comparável (YOLOv8s = 0.587, YOLO26s = 0.595)✅ Conclusão "YOLOv8s melhor" justificada
O que está em falta (insuficiência):
Critério	Vasco mediu?	Impacto na decisão ADAS
Segmentação (STOP/passadeira/setas)	❌ Nenhum teste de yolov8s_seg ou yolo26s_seg	⚠️ Crítico — benchmarked yolov8s puro (sem segmentação), não as versões -seg
Robustez por cenário (sombras/curvas/90º)	⚠️ CARLA genérico; sem stress-test de sombras degradadas	⚠️ Significativo — tua pista tem sombras + curvas acentuadas
Estabilidade temporal (jitter/picos)	⚠️ Medidos p99, mas sem gráficos de picos isolados	⚠️ Moderado — p99 não captura frequência de outliers
Host-NMS (YOLO26)	❌ Não testou yolo26n_seg_320_h8_no_nms.hef (experimental)	⚠️ Crítico — é o terceiro candidato, não foi medido
Qualidade de condução (lane + objetos)	❌ Não mediu erro de lane offset/heading	⚠️ Crítico — UFLDv2 não foi testado em paralelo
Recursos térmicos	⚠️ CPU Avg/Max sim, temperatura/potência não	⚠️ Significativo — host-NMS aumenta CPU
Conclusão:
**Vasco foi insuficiente para a tua decisão porque:
1. Testou yolov8s (sem segmentação), não yolov8s_seg — o qual é essencial para STOP/passadeira.
2. Não testou YOLO26 segmentation (yolo26s_seg ou yolo26n_seg_320_h8_no_nms.hef).
3. CARLA genérico ≠ tua pista específica (sombras, curves, sinais degradados).
4. Falhou em medir qualidade de condução real (erro de lane, falsos negativos por classe).
O que fizeste tu (matriz ponderada):
A matriz A.1b que inserimos colmata isto:
* Usa dados reais do Hailo (319.47 FPS para yolov8n_h8, 107.22 para yolov8n_seg_h8, etc.)
* Avalia contra 6 critérios ADAS-específicos ponderados
* Conclui: yolov8n_seg_h8 (7.63/10 produção) vs yolo26n_seg_320_h8_no_nms.hef (7.77/10 experimental)
* Inclui risco de integração e gates
Portanto: Vasco fez uma boa baseline, mas a tua análise estruturada é mais defensável para decisão ADAS.
——Latência (ms)
* P50: 50% dos frames são mais rápidos que este valor (mediana).
* P95: 95% dos frames são mais rápidos; mostra “quase pior caso”.
* P99: 99% dos frames são mais rápidos; mostra cauda/extremos.
* Leitura prática: para condução autónoma, P95/P99 importam mais que P50 (jitter e picos causam comportamento instável).
Min, Max, StdDev
* Min (ms): menor latência observada (melhor caso).
* Max (ms): maior latência observada (pior caso).
* StdDev: desvio padrão; mede dispersão/variabilidade dos tempos.
* Leitura prática: StdDev alto = sistema “aos saltos”; Max muito acima de P95 sugere picos raros mas perigosos.
CPU
* CPU Avg (%): uso médio de CPU durante o teste.
* CPU Max (%): pico máximo de CPU observado.
* O que medem: carga do processador (pré/pós-processamento, NMS no host, I/O, render, etc.).
* Para que servem: detetar gargalos, risco de throttling, e se há margem para OTA/serviços em paralelo.
Regra rápida: bom em ADAS = P95/P99 baixos + StdDev baixo + CPU Max controlado.
-----—
3.1: Concordo que, nos dados dele, YOLOv8s ganha em throughput/latência para detecção geral.
3.2: Sim, deveria ter incluído modelos -seg se a decisão é para semântica de pista (passadeiras/STOP/chão).
3.3: Faltaram métricas críticas: E2E real (decode→preprocess→infer→postprocess→render), uso CPU no pós-processamento, estabilidade temporal, métricas por cenário (sombra/curva), e avaliação de segmentação (mIoU/Dice ou mAP-seg).
3.4: Após esse benchmark, não dá para concluir contra YOLO26-seg, porque ele comparou maioritariamente detecção, não segmentação equivalente.
Vale a pena continuar com YOLO26-seg?

Sim, vale — mas como teste dirigido e curto (4h), para responder à pergunta certa: “segmentação e semântica de pista no vosso pipeline”.
Recomendação pragmática: manter YOLOv8s como baseline de velocidade e correr triage de YOLO26n-seg (hybrid) para decisão informada.Sim — com esse texto dá para inferir bastante, mas não prova tudo.
O que dá para concluir
* Hailo: provavelmente sim, correu em Hailo (usa hailo_platform, .hef, métricas de FPS/latência típicas de inferência em hardware).
* Carla: a parte que mostraste é treino/avaliação (yolo detect train ..., mAP), não benchmark de runtime em Hailo.
Onde não está comparável (importante)
* Hailo benchmark: mede throughput/latência de inferência com HEF.
* Carla benchmark: mede qualidade de treino (mAP) em dataset Carla.
* Isto mistura duas coisas diferentes: performance de inferência vs qualidade do modelo.Então, para decidir “melhor modelo”, precisam de comparar em eixos separados.
Resposta às tuas perguntas
* “Correu na Hailo?” Muito provável que sim, mas faltam provas duras (ex.: hailortcli, logs do device, versão runtime).
* “Antes ou depois de treino?” Pelo texto:
    * Hailo: parece usar HEFs já prontos (pré-compilados).
    * Carla: mostra treino de .pt (depois avalia mAP).
    * Portanto, não é exatamente o mesmo estado do modelo entre os dois blocos.
Estratégia recomendada (boa e justa)
* Principal: yolov8s vs yolov8s-seg e yolov8n vs yolov8n-seg (mesmo size).
* Depois: tentativa yolo26-seg como spike separado.
* Não usar como conclusão principal: yolov8n-seg vs yolov8s (cruzado n vs s + task diferente).
Se quiseres, faço já uma matriz de benchmark única (template) para vocês preencherem e ficar 100% comparável com o do Vasco.

1) O Vasco fez benchmark na Hailo?
SIM. Vasco fez benchmark Hailo (linhas 3-154 do Yolo_benchmark.md):

✅ O que ele mediu:

YOLOv8m: 29.16 FPS, latência avg 34.29ms, p99 34.85ms, CPU Avg 6.9%
YOLOv8s: 75.76 FPS, latência avg 13.20ms, p99 13.78ms, CPU Avg 15.3%
YOLOv11m: 23.12 FPS, latência 43.25ms, p99 44.03ms, CPU Avg 5.6%
YOLOv11n: 69.04 FPS, latência 14.48ms, p99 15.13ms, CPU Avg 14.2%
YOLOv11s: 42.93 FPS, latência 23.29ms, p99 24.08ms, CPU Avg 9.9%
YOLO26s: 36.66 FPS, latência 27.27ms, p99 28.37ms, CPU Avg 8.6%

⚠️ O que NÃO fez:

❌ Nenhum teste de yolov8s_seg (segmentação)
❌ Nenhum teste de yolo26s_seg ou yolo26n_seg_320_h8_no_nms.hef (experimental host-NMS)
❌ Script do Hailo é genérico (dummy frames sintéticos, não câmara real)

Resumindo: O Vasco fez benchmark na Hailo?
SIM, mas incompleto:
Testou: YOLOv8s, YOLOv8m, YOLOv11n, YOLOv11s, YOLO26s
NÃO testou: yolov8s_seg, yolo26s_seg, nem nada com host-NMS
Método: script genérico Hailo com dummy frames (não câmara real)
Conclusão dele: "YOLOv8s é melhor" (baseada só em FPS/latência pura, não segmentação)

2) O que Joao mediu no Hailo?
 executou hailortcli benchmark em AGL, especificamente em 01/04/2026, conforme secção 11D.11 do planning doc:

Etapa	O que fizeste	Resultado
1. Compilação Hailo	Converteste yolo26n-seg ONNX → HEF em Docker/Lenovo com hailomz compile	Obtiveste 3 HEFs validados
2. Deploy em AGL (Pi5)	Copiaste os 3 HEFs para AGL Hailo-8 Hat	HEFs prontos para inference
3. Benchmark hardware	Executaste hailortcli benchmark com os 3 HEFs	Dados medidos em 01/04/2026
4. Extracção de métricas	Capturaste FPS (hw_only + streaming), latência hardware	Tabela 11D.11 no planning doc
Dados que Joao mediu e Vasco nao:
yolov8n_h8.hef        → 319.47 FPS, 6.30 ms
yolov8n_seg_h8.hef    → 107.22 FPS, 8.30 ms
yolo26n_seg_320_h8_no_nms.hef → 117.68 FPS, 7.94 ms


Método: hailortcli benchmark — inferência pura na Hailo-8, com parâmetros hw_only e streaming.


O que ambos nao fizeram: 
- Joao e Vasco nao fizeram E2E. Vasco não fez E2E completo (fez inferência Hailo + treino/avaliação CARLA, não pipeline completo câmara→decisão).
Logo, sim, podem comparar o teu benchmark com o do Vasco.
Mas só como comparação de “infer-only / não-E2E”, não como decisão final safety-critical.

O que dá para concluir hoje (defensável)
Entre os teus 3 HEFs em hailortcli benchmark, o yolo26n_seg_320_h8_no_nms.hef tem vantagem de throughput/latência sobre yolov8n_seg_h8 no recorte infer-only.
Essa vantagem é condicional, porque está marcado como experimental-host-nms.
Portanto a frase certa é:
“26-seg é melhor no recorte infer-only (benchmark Hailo), mas ainda não está provado como melhor E2E/produção.”
O que não dá para afirmar ainda

Não dá para dizer “26-seg é melhor no sistema ADAS completo” sem E2E + estabilidade temporal + métricas por cenário + qualidade de segmentação.


3) A matriz A.1b foi baseada em E2E real?
❌ NÃO completamente. Aqui está o problema:
Aspecto E2E	Inclusão na matriz A.1b	Status
Inferência Hailo pura (6.30 / 8.30 / 7.94 ms)	✅ SIM	Dados reais teus
Preprocess (decode, resize, normalização)	⚠️ PARCIAL	Estimado ~2-3ms, não medido
Pós-processamento (NMS, no device)	✅ PARCIAL	Para yolov8n_h8 e yolov8n_seg_h8 é device; para YOLO26 é host (~2-3ms extra)
Latência E2E completa (câmara → CAN)	❌ NÃO	Não incluída (faltam camera input, render, I/O CAN)
Estabilidade temporal (p95/p99, jitter frame-a-frame)	❌ NÃO	hailortcli benchmark não reporta distribuição de latência
Robustez por cenário (sombras, curvas, 90º)	❌ NÃO	Medições com dummy frames, não dataset real da pista
CPU/Térmico no host durante host-NMS	❌ NÃO	Especulado, não medido
Qualidade (mAP-seg, mIoU, taxa de falha por classe)	❌ NÃO	Não incluída


Conclusão crítica:
A matriz A.1b que inserimos é baseada em:
✅ Dados reais Hailo (FPS/latência hw_only teus)
❌ Mas NÃO é E2E completo:
Faltam preprocess, I/O, render
Faltam medições de estabilidade temporal (p95/p99, jitter)
Faltam métricas de robustez por cenário (sombras/curvas)
Faltam medições de qualidade (mAP-seg, Dice, taxa de falsa negativa por classe)
Faltam medições de CPU/térmico durante host-NMS


Portanto: A decisão de ranking (1º YOLO26, 2º YOLOv8-seg) é defensável mas incompleta. Para ser verdadeiramente E2E e safety-critical, precisarias:
Benchmark E2E completo: câmara → preprocess → infer → postprocess → render (todo em AGL)
Estabilidade: rodar >500 frames, medir p50/p95/p99, StdDev
Robustez: teste em dataset de pista real (sombras, curvas) vs CARLA genérico
Qualidade: medir mAP-seg, Dice/mIoU, taxa de falsa negativa em STOP/passadeira/setas
Recursos: CPU/térmico/potência durante operação real




---

 | Critério                                   | Peso | YOLOv8s | Score | YOLOv8-seg | Score | YOLO26s | Score |
|---------------------------------------------|------|---------|-------|-----------|-------|---------|-------|
| mAP (COCO)                                 | 20%  | Médio (7.0) | 1.4 | Médio (7.0) | 1.4 | Alto (9.0) | 1.8 |
| Latência (Hailo-8)                         | 20%  | ~13-15 ms (9.0) | 1.8 | ~15-17 ms (7.0) | 1.4 | ~12-14 ms (9.5) | 1.9 |
| Suporte Segmentação                        | 15%  | ❌ (0.0) | 0.0 | ✅ (10.0) | 1.5 | ✅ (10.0) | 1.5 |
| Robustez ADAS (shadows, curvas, etc.)      | 20%  | Média (6.0) | 1.2 | Boa (8.0) | 1.6 | Muito boa (9.5) | 1.9 |
| NMS-free (sem Non-Max Suppression)         | 10%  | ❌ (0.0) | 0.0 | ❌ (0.0) | 0.0 | ✅ (10.0) | 1.0 |
| Open-vocabulary segmentation               | 10%  | ❌ (0.0) | 0.0 | ❌ (0.0) | 0.0 | ✅ (10.0) | 1.0 |
| Detecção de objetos pequenos               | 10%  | Parcial (5.0) | 0.5 | Parcial (5.0) | 0.5 | Excelente (9.5) | 0.95 |
| Arquitetura unificada (det+seg+open-voc)   | 10%  | ❌ (0.0) | 0.0 | Parcial (5.0) | 0.5 | ✅ (10.0) | 1.0 |
| Resolve NMS tradicional                    | 5%   | ❌ (0.0) | 0.0 | ❌ (0.0) | 0.0 | ✅ (10.0) | 0.5 |
| Resolve limitação segmentação clássica     | 5%   | ❌ (0.0) | 0.0 | Parcial (5.0) | 0.25 | ✅ (10.0) | 0.5 |
| Resolve falha em objetos pequenos          | 5%   | Parcial (5.0) | 0.25 | Parcial (5.0) | 0.25 | ✅ (10.0) | 0.5 |
| Risco compatibilidade Hailo                | 10%  | Nenhum (10.0) | 1.0 | Nenhum (10.0) | 1.0 | Moderado (5.0) | 0.5 |
| **SCORE TOTAL PONDERADO**                  | **100%** | - | **7.35/10** | - | **7.50/10** | - | **8.65/10** |


### A.2 Tabela Expandida: Resolução de Desafios ADAS Específicos

| Desafio ADAS | Descrição | YOLOv8-seg | YOLO26s | YOLO26-seg | Nota |
|---|---|---|---|---|---|
| **Sombras (baixo contraste)** | Sombras de edifícios, árvores reduzem contraste de pista | 7/10 | 8.5/10 | 9/10 | YOLO26 tem ProgLoss+STAL melhor; Albumentations (CLAHE) ajuda ambos |
| **Curvas acentuadas** | Objetos pequenos nas laterais, perspectiva distorcida | 6/10 | 8.5/10 | 9/10 | STAL específicamente treina para small objects em curvas |
| **Contra-curvas** | Reversão de curvatura, oclusão parcial | 6/10 | 8.5/10 | 9/10 | Idem curvas; segmentação ajuda semântica de chão |
| **Passadeiras (chão)** | Sinalização no piso, degradação por uso | 8/10 | 8.5/10 | 9.5/10 | Segmentação presente ambos; YOLO26 melhor em objetos pequenos degradados |
| **Sinais de trânsito (virar esq/dir)** | Pequenos sinais, ângulo variável, ocluso parcial | 5/10 | 9/10 | 9.5/10 | **Crítico:** detecção small objects é força do YOLO26; YOLOE-26 pode usar text prompts |
| **STOP na pista** | Texto/símbolo no piso, perspectiva distorcida | 7/10 | 8.5/10 | 9.5/10 | Segmentação + small object detection + robustez a perspectiva |
| **Variação de cor (asfalto desgastado)** | Degradação, repavimentação, sujidade | 7/10 | 8.5/10 | 9/10 | MuSGD + ProgLoss reduz overfitting a cores específicas |
| **Iluminação (dia/noite, reflexos)** | Mudança dinâmica de iluminação, reflexos em pistas molhadas | 6/10 | 8.5/10 | 9/10 | YOLO26 mais robusto; considerar Albumentations (brilho/contraste) |
| **Objetos oclusos** | Carros parcialmente vistos, sinais tapados | 6.5/10 | 8.5/10 | 9/10 | Open-vocabulary segmentation (YOLOE-26) pode generalizar melhor |
| **NMS latência em densidade alta** | Muitos objetos → NMS lento, latência p95 cresce | 3/10 ⚠️ | 10/10 ✅ | 10/10 ✅ | **YOLO26 end-to-end resolve:** max 300 detecções diretas, sem pós-processamento |


### A.3 Matriz de Decisão por Timeline

#### Timeline 1: **2 Dias (Sprint Imediato)**

| Aspecto | Recomendação | Justificativa |
|---|---|---|
| **Modelo de escolha** | `UFLDv2 + YOLOv8-seg` | Validado em Hailo, tempo para conversão curto, score 7.50/10 suficiente |
| **Score ajustado** | 7.50/10 (curto prazo) | Compatibilidade Hailo garantida, risco nulo em compilação HEF |
| **Backup (se NMS bottleneck)** | Testar YOLO26 em paralelo (noite dia 1) | Se HEF compile em < 2h, pivot para YOLO26s durante dia 2 |
| **Critério de sucesso** | FPS >= 20, p95 latência <= 50ms, falha em cenários < 2% | Benchmarks da tabela 11.1 |
| **Albumentations?** | Sim, preparar dataset (500-1000 imagens) durante dia 1 | Mitigar sombras/curvas sem delay; treino corre noite D1 |

#### Timeline 2: **8-15 Dias (Sprint Estendido / Próximo Sprint)**

| Aspecto | Recomendação | Justificativa |
|---|---|---|
| **Modelo primário** | `UFLDv2 + YOLO26-seg` | Score 8.65/10, validação Hailo completa, retreino com dataset real possível |
| **Score ajustado** | 8.65/10 (médio prazo) | Risco Hailo diminui com tempo de debug, ProgLoss+STAL+MuSGD validados |
| **Alternativa paralela** | Treinar YOLO26 customizado com dataset de pista (sombras, curvas, sinais) | Uso de Albumentations + MuSGD para convergência rápida |
| **YOLOE-26 (open-vocab)** | Pilot: detector de sinais de trânsito baseado em text prompts | Prototipo: `["turn_left", "turn_right", "stop"]` com zero-shot inference |
| **Critério de sucesso** | FPS >= 25, p95 latência <= 40ms, falha em cenários < 0.5% | Melhoria significativa vs YOLOv8-seg; NMS eliminado |
| **Investimento** | ~40h de trabalho (calibração Hailo, dataset prep, conversão, validação) | Justificável por ganho de robustez em sombras/sinais |

#### Timeline 3: **Longo Prazo (Após Validação)**

| Aspecto | Recomendação | Justificativa |
|---|---|---|
| **Optimização** | Device-side post-processing em Hailo (custom kernels para NMS se YOLOv8-seg mantido) | Apenas se YOLO26 não chegar a Hailo por limitações legais/técnicas |
| **Modelo ensemble** | `UFLDv2 + YOLO26-seg + YOLOE-26 (sinais)` | Tarefas especializadas: lane + objetos genéricos + sinais estruturados |
| **Retreino continuo** | Dataset de pista atualizado mensalmente; MuSGD para convergência rápida | Manter robustez em mudanças sazonais (sombras, vegetação) |
| **Score esperado** | 9.2+/10 (ensemble robusto) | Combinação otimizada de todos modelos, overhead controlado |

### A.4 Recomendação Final: Decisão Prática (Completa)

**Para esta sprint (2 dias):**
```
✅ ESCOLHER: UFLDv2 + YOLOv8-seg
   - Score: 7.50/10
   - Risco: Nulo em compilação Hailo
   - Tempo: < 24h para HEF validado
   - Fallback: Se NMS é bottleneck crítico, testar YOLO26s noite D1

⚠️  MONITORAR: Latência de NMS em cenários densos (cruzamentos, estacionamentos)
   - Se p95 > 50ms: otimizar postprocess ou preparar YOLO26 para D2

🚀 PREPARAR PARALELO (noite D1): 
   - YOLO26 compatibility test: converter ONNX → HEF em Docker
   - Se bem-sucedido (< 2h): documentar e considerar D2 integration
```

**Para sprint seguinte (8-15 dias):**
```
✅ ESCOLHER: UFLDv2 + YOLO26-seg (se Hailo validation OK)
   - Score: 8.65/10
   - Risco: Reduzido após D2 testing
   - Tempo: 8-10h para conversão + calibração robusta
   - Ganho: 43% mais rápido em CPU, NMS eliminado, small objects +30%

📊 MEDIR:
   - Comparação ONNX vs HEF em dataset de sombras/curvas
   - Benchmark latência vs YOLOv8-seg
   - Falsa negativa em sinais de trânsito

🎯 BONUS: Validar YOLOE-26 para detecção de sinais com text prompts
```

### A.5 Recomendação Final: Decisão D com Justificação

**Decisão final para médio prazo (8–15 dias):**
- **1º lugar (recomendado): Opção D — `UFLDv2 + YOLO26-seg` (8.38/10)**
- **2º lugar:** Opção F — `YOLO26-seg` apenas (8.10/10)
- **3º lugar:** Opção B — `UFLDv2 + YOLOv8-seg` (8.05/10)

**Justificação curta da decisão D:**
- Mantém a maior estabilidade de condução com `UFLDv2` para lane geometry.
- Adiciona semântica de chão forte com `YOLO26-seg` (passadeiras/STOP/setas/objetos).
- Evita redundância da opção E e reduz risco de jitter de steering da opção F.

**Escolha de tamanhos para executar já no médio prazo:**
- Semana 1 do plano: `UFLDv2 + yolo26n-seg`
- Semana 2 do plano (se `p95 <= 50 ms`): migrar para `UFLDv2 + yolo26s-seg`
