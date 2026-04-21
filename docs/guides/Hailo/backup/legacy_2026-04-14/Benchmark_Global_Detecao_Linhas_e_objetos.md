# Comparativo de Modelos para Detecção de Linhas e Objetos em Hailo-8

> Data: 24/03/2026  
> Contexto: pista com fundo preto, linhas brancas e amarelas, retas/curvas/contra-curvas, passadeiras, marcações de STOP e estacionamento, sombras, semáforos, sinais e barreiras.

## 1) Resposta curta (recomendação)

Para o teu cenário, a melhor estratégia prática em Hailo-8 é **2 modelos**:

1. **Modelo de linhas (lane segmentation/line parsing)**: `YOLOP` *ou* `YOLOv8-seg (nano/small) treinado para classes de marcação`  
2. **Modelo de objetos**: `YOLOv8n` (ou `YOLOv8s` se houver margem de latência)

Se tiver de escolher **um único modelo** para tudo, a melhor opção realista é **YOLOP**, mas com compromisso na qualidade de deteção de objetos face a um detector dedicado.

---

## 2) "Quantos modelos existem?"

Não existe um número fechado de "todos os modelos existentes" (novos papers e variantes aparecem continuamente).

Para decisão de engenharia em Hailo-8, o conjunto realmente relevante (maduro e usado em produção/protótipos) é aproximadamente:

- **6-10 famílias de lane detection** (ex.: UFLD/UFLDv2, LaneATT, CLRNet, SCNN, ENet-SAD, YOLOP)
- **8-12 famílias de object detection** (ex.: YOLOv5/v7/v8/v10/v11, PP-YOLOE, NanoDet, etc.)
- **3-5 opções multitarefa** (ex.: YOLOP e variações)

Ou seja, para seleção prática em Hailo-8, costumas comparar **~15 a 25 candidatos viáveis** (não "todos os que existem no mundo").

---

## 3) Parâmetros usados para justificar a escolha

A recomendação foi feita com os critérios abaixo (peso entre parêntesis):

1. **Latência na edge (Hailo-8)** (30%)  
2. **Robustez em curva/contra-curva e marcações mistas** (20%)  
3. **Robustez a sombras/variação de iluminação** (15%)  
4. **Qualidade em linhas tracejadas + passadeiras + STOP no chão** (15%)  
5. **Complexidade de treino e manutenção** (10%)  
6. **Facilidade de deploy/integração no pipeline atual** (10%)

Escala usada: 1 (fraco) a 5 (excelente).  
Score final: soma ponderada.

---

## 4) Comparativo — Detecção de Linhas

## 4.1 Candidatos principais

### A) UFLD / UFLDv2 (Ultra-Fast Lane Detection)
**Prós**
- Muito rápido (bom para edge)
- Bom em lanes estruturadas
- Leve e eficiente

**Contras**
- Mais fraco em marcações não-lane (STOP, passadeira, estacionamento)
- Pode perder robustez em sombras fortes e desgaste de pintura
- Normalmente focado em "lane geometry" e não semântica rica do chão

### B) LaneATT / CLRNet
**Prós**
- Melhor modelação de geometrias complexas
- Bom em curvas e contra-curvas

**Contras**
- Normalmente mais pesado e menos trivial para otimizar em edge
- Pipeline de treino pode ser mais complexo

### C) SCNN / ENet-SAD (clássicos)
**Prós**
- Fundamentais, bem estudados
- Bons como baseline académico

**Contras**
- Em geral, superados por arquiteturas mais recentes
- Menor relação desempenho/latência para uso atual

### D) YOLOv8-seg (classes de marcação)
**Prós**
- Pipeline unificado com ecossistema YOLO
- Segmentação permite distinguir classes no chão (linha branca, linha amarela, passadeira, STOP)
- Boa integração com dataset custom

**Contras**
- Exige dataset de segmentação bem anotado
- Se muito pequeno (n), pode perder detalhe fino de traço

### E) YOLOP (multitarefa: detecção + drivable + lane)
**Prós**
- Um modelo só para objetos + lanes + área conduzível
- Excelente simplicidade de deployment
- Boa solução "all-in-one" para protótipo ADAS

**Contras**
- Em geral, objetos ficam abaixo de detector dedicado (YOLOv8n/s)
- Menos flexível para evoluir cada tarefa separadamente

---

## 5) Comparativo — Detecção de Objetos

### A) YOLOv8n
**Prós**
- Excelente latência em edge
- Fácil deploy e manutenção
- Bom compromisso para tempo real

**Contras**
- Menor precisão que versões maiores em objetos pequenos/distantes

### B) YOLOv8s
**Prós**
- Melhor precisão que v8n
- Ainda viável em tempo real em muitos casos

**Contras**
- Mais custo computacional
- Menor margem para correr em paralelo com modelo de lanes

### C) YOLOv8m/l
**Prós**
- Mais precisão global

**Contras**
- Custo geralmente alto para pipeline duplo em Hailo-8
- Pode comprometer FPS final do sistema

---

## 6) Matriz de decisão (para a tua pista)

| Opção | Latência | Curvas/contra-curvas | Sombras | Marcação de chão complexa | Integração | Score final (0-5) |
|---|---:|---:|---:|---:|---:|---:|
| UFLDv2 + YOLOv8n | 4.8 | 4.0 | 3.6 | 3.4 | 4.0 | **4.09** |
| UFLDv2 + YOLOv8s | 4.2 | 4.1 | 3.7 | 3.6 | 3.9 | **4.00** |
| YOLOP (modelo único) | 4.3 | 4.1 | 3.8 | 4.0 | 4.8 | **4.17** |
| YOLOv8-seg(n/s) + YOLOv8n | 4.0 | 4.4 | 4.1 | 4.6 | 4.2 | **4.24** |
| YOLOv8-seg(s) + YOLOv8s | 3.5 | 4.5 | 4.2 | 4.7 | 3.8 | **4.09** |

### Leitura da tabela
- **Melhor score técnico**: `YOLOv8-seg(n/s) + YOLOv8n`
- **Melhor simplicidade operacional**: `YOLOP` (modelo único)
- **Melhor compromisso global para o teu cenário**: **2 modelos (seg + detector)**

Nota sobre `YOLOv8n` vs `YOLOv8s` na linha com `UFLDv2`:
- Sim, nesta matriz o `UFLDv2 + YOLOv8n` ficou ligeiramente acima do `UFLDv2 + YOLOv8s`.
- Isto acontece porque os pesos priorizam **latência edge/FPS e integração** (40% combinados), onde `YOLOv8n` tende a ter vantagem.
- O `YOLOv8s` normalmente melhora precisão de objetos, mas paga em custo computacional; nesta ponderação essa troca não compensou.
- Se a tua prioridade principal for mAP de objetos (e não FPS/latência), podes reponderar a matriz e o `YOLOv8s` pode passar à frente.

### 6.1 Tabela expandida de parâmetros (mais relevantes)

> Valores de FPS/latência/carga são **intervalos esperados** para comparação relativa em Hailo-8 e devem ser validados no teu pipeline final.
> Score (0-5) nesta tabela: escala agregada alinhada com a matriz da secção 6 (quanto maior, melhor no cenário descrito).

| Opção | FPS (fim-a-fim) | Precisão Objetos (mAP50) | Precisão em Curvas (F1/IoU linhas) | Peso/Complexidade do Pipeline | Carga no CPU (Host/Pi) | Compatibilidade Hailo (ONNX→HAR→HEF) | Latência Edge (p95) | Sinais/STOP/Vagas | Reação a Sombras | Melhor escolha para Hailo-8 | Score (0-5) | Observações |
|---|---:|---:|---:|---|---|---|---:|---|---|---|---:|---|
| UFLDv2 + YOLOv8n | 24-32 | média/boa | boa | média | baixa/média | alta (normalmente viável) | 35-50 ms | média | média | boa | **4.09** | Melhor quando queres lane geometry rápida com boa margem de FPS |
| UFLDv2 + YOLOv8s | 20-28 | boa/muito boa | boa | média/alta | média | alta (normalmente viável) | 40-60 ms | média/boa | média | boa (se houver margem) | **4.00** | Sobe precisão de objetos, mas reduz margem de latência |
| YOLOv8-seg(n/s) + YOLOv8n | 18-28 | boa | boa/muito boa | alta | média | média/alta (depende da cabeça seg) | 45-65 ms | boa/muito boa | boa | **recomendada** no teu cenário | **4.24** | Melhor para semântica de chão (passadeira/STOP/linhas por cor) |
| YOLOv8-seg(s) + YOLOv8s | 14-24 | muito boa | muito boa | alta | média/alta | média (mais sensível a otimização) | 55-80 ms | muito boa | boa/muito boa | condicional | **4.09** | Mais pesada; usar só com orçamento de FPS confortável |
| YOLOP (modelo único) | 20-30 | média | boa | baixa/média | baixa/média | média (varia por implementação) | 40-60 ms | boa | média/boa | ótima para POC | **4.17** | Excelente simplicidade de deploy, menos flexível para escalar |

Parâmetros mais relevantes para decidir no teu caso (ordem recomendada):
1. **Falhas críticas em curva + sombra** (segurança funcional)
2. **Semântica de chão** (`passadeira`, `stop_chao`, vagas, linhas por cor)
3. **FPS e latência p95 fim-a-fim**
4. **Precisão de objetos** (sinais/semafóros/barreiras)
5. **Complexidade de manutenção e integração**

---

## 7) A tua pergunta crítica: "Mesmo com essa pista, é mesmo o melhor?"

**Sim, para este cenário específico, continuo a recomendar 2 modelos.**

Motivo:
- Tens **muita semântica no chão** (linhas por cor, passadeira, STOP, estacionamento)
- Tens também **objetos verticais e dinâmicos** (sinais, semáforos, barreiras, eventualmente pessoas/veículos)
- Um único modelo tende a comprometer uma das tarefas

Em termos de engenharia ADAS, separar tarefas dá:
- Melhor qualidade por tarefa
- Melhor capacidade de afinação independente
- Maior robustez em condições difíceis (sombras + contraste alto do piso preto)

---

## 8) Prós e contras: 1 modelo vs 2 modelos

## 8.1 Um modelo (YOLOP)
**Prós**
- Deploy mais simples
- Menos componentes para manter
- Time-to-demo mais rápido

**Contras**
- Objetos geralmente menos precisos que YOLO dedicado
- Menos controlo fino por tarefa
- Escalabilidade mais limitada para requisitos de segurança

## 8.2 Dois modelos (Lane + Object)
**Prós**
- Qualidade superior por tarefa
- Melhor adaptação ao teu cenário complexo
- Fácil trocar apenas um modelo sem rebentar o pipeline todo

**Contras**
- Maior integração (sincronização de outputs)
- Mais gestão de latência/FPS
- Mais trabalho de MLOps (treino, versionamento, validação)

---

## 9) Configuração recomendada (final)

### Recomendação principal
- **Linhas/marcações de chão**: `YOLOv8-seg` (classes: `linha_branca_continua`, `linha_branca_tracejada`, `linha_amarela_berma_direita`, `passadeira`, `stop_chao`, `estacionamento`, `fundo_pista`)
- **Objetos**: `YOLOv8n` (classes de sinais/semafóros/barreiras/peões/veículos, conforme necessidade)

### Recomendação de arranque rápido (POC)
- Começar por **YOLOP + YOLOv8n** (ou só YOLOP para prova rápida)
- Depois migrar para **seg dedicado + detector dedicado**

---

## 10) Metodologia de validação recomendada

Para decidir objetivamente, testar em 3 conjuntos:
1. **Dia com luz estável**
2. **Sombras fortes**
3. **Curvas/contra-curvas + passadeira + STOP**

Métricas mínimas a medir:
- FPS e latência (p50/p95)
- F1/IoU de classes de linha
- mAP de objetos
- taxa de falha por cenário crítico (ex.: sombra em curva)

### 10.1 Mini tabela A/B com limiares objetivos

Comparação pedida: `UFLDv2 + YOLOv8s` vs `YOLOv8-seg(n/s) + YOLOv8n`.

Método de score nesta A/B:
- Escala por critério: `0` (não cumpre limiar), `3` (cumpre mínimo), `5` (cumpre com margem).
- Pesos usados (total 100%): FPS 15, Latência p95 15, IoU/F1 linhas 15, F1 passadeira 10, F1 stop_chao 10, mAP50 objetos 15, Falhas críticas 15, Integração/manutenção 5.
- Score final ponderado (0-5): $\sum (peso \times score\_criterio)/100$.

| Critério | Peso (%) | Limiar de aprovação | UFLDv2 + YOLOv8s | YOLOv8-seg(n/s) + YOLOv8n | Regra de decisão |
|---|---:|---:|---:|---:|---|
| FPS fim-a-fim | 15 | >= 20 FPS | medir (0/3/5) | medir (0/3/5) | Se um falhar, perde |
| Latência p95 fim-a-fim | 15 | <= 50 ms | medir (0/3/5) | medir (0/3/5) | Menor é melhor |
| IoU/F1 linhas contínuas e tracejadas | 15 | >= 0.80 | medir (0/3/5) | medir (0/3/5) | Maior é melhor |
| F1 classe `passadeira` | 10 | >= 0.75 | medir (0/3/5) | medir (0/3/5) | Maior é melhor |
| F1 classe `stop_chao` | 10 | >= 0.75 | medir (0/3/5) | medir (0/3/5) | Maior é melhor |
| mAP50 objetos (sinais, semáforos, barreiras) | 15 | >= 0.70 | medir (0/3/5) | medir (0/3/5) | Maior é melhor |
| Falhas críticas (curva+sombras) | 15 | <= 2% dos frames | medir (0/3/5) | medir (0/3/5) | Menor é melhor |
| Custo de integração/manutenção | 5 | qualitativo | médio (0/3/5) | médio/alto (0/3/5) | Menor esforço favorece |
| **Score final ponderado (0-5)** | **100** |  | **calcular após medição** | **calcular após medição** | **Maior score vence** |

Interpretação prática:
- Se o foco principal for semântica rica de chão (passadeira, STOP, estacionamento, cor de linha), tende a ganhar `YOLOv8-seg(n/s) + YOLOv8n`.
- Se o foco principal for lane geometry simples + mais precisão em objetos distantes, `UFLDv2 + YOLOv8s` pode ganhar.
- Em caso de empate técnico, escolher a opção com menor taxa de falhas críticas no cenário `curva + sombra`.

Se o pipeline 2-modelos mantiver tempo real e melhorar erro em cenários críticos, fica validado como escolha final.

### 10.2 Quando usar 3 modelos (e quando não usar)

Arquitetura de 3 modelos recomendada apenas para casos em que existe um requisito crítico extra não coberto de forma robusta por 2 modelos.

Exemplo de arquitetura:
1. **Modelo de chão/linhas**: `YOLOv8-seg(n/s)` (linhas por cor, passadeira, `stop_chao`, vagas)
2. **Modelo de objetos gerais**: `YOLOv8n/s` (sinais, semáforos, barreiras, peões/veículos)
3. **Modelo especialista**: detector dedicado para classe crítica (ex.: semáforo, sinal `STOP`, ou vaga/ocupação)

Quando faz sentido usar 3 modelos:
- A opção com 2 modelos falha limiar crítico de segurança (ex.: `Falhas críticas > 2%` em `curva + sombra`)
- Existe KPI regulatório/projeto que exige ganho adicional numa classe específica
- O sistema ainda mantém tempo real após integração do 3º modelo

Quando **não** faz sentido usar 3 modelos:
- O pipeline com 2 modelos já cumpre todos os limiares
- A adição do 3º modelo derruba FPS abaixo de `20`
- A latência p95 fim-a-fim ultrapassa `50 ms` sem ganho relevante de segurança/perceção

Budget recomendado para decidir (fim-a-fim):
- **FPS alvo:** `>= 20`
- **Latência p95 alvo:** `<= 50 ms`
- **Headroom para 3º modelo:** reservar ~`8-12 ms` p95

Regra de decisão prática:
- Começar por 2 modelos
- Adicionar 3º modelo só se houver ganho mensurável em KPI crítico **e** manutenção dos limites de FPS/latência
- Se não cumprir ambos, recuar para 2 modelos e otimizar dataset, thresholds ou pós-processamento

---

## 11) Conclusão

- Não há "um melhor modelo universal" para todos os cenários.
- Para **o teu cenário específico**, a escolha mais robusta em Hailo-8 é **2 modelos**.
- Se a prioridade for simplicidade imediata, **YOLOP** é a melhor alternativa de modelo único.
- Se a prioridade for qualidade e robustez em produção, **seg dedicado + detector dedicado** ganha.
- Arquitetura com **3 modelos** é opcional e só deve avançar com ganho comprovado em KPI crítico, sem quebrar o orçamento de tempo real.

---

## 12) Nota de transparência

Este comparativo é uma análise técnica orientada a deployment em edge (Hailo-8), baseada em famílias de modelos consolidadas e em critérios de engenharia de sistema.  
O número total de modelos "existentes" no estado-da-arte é aberto e evolui constantemente; por isso a decisão foi feita sobre o conjunto de modelos realmente viáveis para o teu contexto e restrições de hardware/latência.
