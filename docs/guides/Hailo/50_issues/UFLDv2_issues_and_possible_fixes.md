# UFLDv2 Issues and Possible Fixes

## Objetivo
Registar de forma rastreável os problemas encontrados no UFLDv2 (incluindo os reportados pelo David), e as soluções tentadas.

## Como usar este ficheiro
- Registar uma issue sempre que um comportamento for repetível e bloquear a pipeline.
- Separar problemas de `decode/preprocess`, `infer`, `postprocess`, `render` e `benchmark`.
- Indicar se o problema afeta A, B, C ou D.

## Template de issue
### Issue ID
- **Data:**
- **Contexto:**
- **Sintoma:**
- **Impacto:**
- **Causa provável:**
- **Tentativas feitas:**
- **Resultado de cada tentativa:**
- **Solução provisória/final:**
- **Ações em aberto:**

## Issue log
### UFLD-001
- **Data:** 14/04/2026
- **Contexto:** E2E comparativo para a pista real; `UFLDv2` usado como baseline de trajetória.
- **Sintoma:** pós-processamento Python/NumPy demasiado lento ou difícil de estabilizar quando a cena fica mais densa.
- **Impacto:** aumento de `p95`, jitter e risco de crash em cenários com muitas marcações ou sombras.
- **Causa provável:** pós-processamento ainda não convertido para C++ e/ou demasiado acoplado ao render.
- **Tentativas feitas:** separar o pós-processamento do render; mover cálculo pesado para NumPy; reduzir trabalho por frame.
- **Resultado de cada tentativa:** melhora parcial, mas ainda sem garantia de latência estável a longo prazo.
- **Solução provisória/final:** migrar o pós-processamento crítico para C++ quando a versão Python estabilizar o pipeline.
- **Ações em aberto:** medir impacto real por cenário e comparar com a versão C++ assim que existir.

### UFLD-002
- **Data:** 14/04/2026
- **Contexto:** Opção B (`UFLDv2 + YOLO26n-seg`) e C (`UFLDv2 + YOLOv8n-seg`) no E2E.
- **Sintoma:** host-NMS pode tornar-se gargalo quando a variante é `split / no-NMS`.
- **Impacto:** FPS E2E desce, `p95` sobe e a estabilidade geral do loop fica pior.
- **Causa provável:** número de objetos/máscaras por frame e custo do NMS no CPU.
- **Tentativas feitas:** testar `full INT8` quando possível; comparar com `no-NMS`; reduzir custo do post-process.
- **Resultado de cada tentativa:** `full INT8` pode eliminar o gargalo, mas nem sempre compila; `split` é mais flexível, mas paga CPU.
- **Solução provisória/final:** escolher a variante que mantenha `p95 <= 50 ms` sem perder deteção crítica.
- **Ações em aberto:** medir o custo do NMS em cenários com passadeiras/setas/cruzamentos.

### UFLD-003
- **Data:** 14/04/2026
- **Contexto:** dataset labelling da pista real.
- **Sintoma:** classes como passadeiras, setas e sinais podem estar incompletas ou com cobertura irregular.
- **Impacto:** falsos negativos ou decisões injustas entre modelos.
- **Causa provável:** distribuição do dataset ainda não cobre todos os cenários críticos com a mesma qualidade.
- **Tentativas feitas:** usar cenários normal e degradado; validar manualmente as labels; comparar apenas runs com o mesmo protocolo.
- **Resultado de cada tentativa:** melhora a comparabilidade, mas não resolve labels em falta.
- **Solução provisória/final:** fixar o subset de avaliação e documentar as lacunas de label antes da comparação final.
- **Ações em aberto:** validar se o Vasco já cobriu todas as classes críticas do plano.

### UFLD-004
- **Data:** 14/04/2026
- **Contexto:** conversão ONNX → HAR → HEF para os modelos `YOLO26n-seg` e `YOLOv8n-seg`.
- **Sintoma:** mismatch nos nós de entrada/saída ou export ONNX não compatível com `hailomz`.
- **Impacto:** compilação falha ou artefacto inválido.
- **Causa provável:** `start-node-names` / `end-node-names` incorretos, ou export com `imgsz` diferente do pipeline.
- **Tentativas feitas:** validar no Netron; alinhar `imgsz`; guardar naming explícito; compilar baseline antes do custom.
- **Resultado de cada tentativa:** baseline ajuda a isolar problema de ambiente vs modelo custom.
- **Solução provisória/final:** corrigir export e nó de entrada/saída antes de voltar a compilar.
- **Ações em aberto:** manter um log por tentativa e não misturar artefactos de runs diferentes.

### UFLD-005
- **Data:** 14/04/2026
- **Contexto:** comparação A/B/C/D para E2E.
- **Sintoma:** comparação injusta entre modelos quando um inclui render pesado ou outro inclui segmentação muito mais densa.
- **Impacto:** decisão técnica enviesada.
- **Causa provável:** protocolo não fixado ou métricas incompletas.
- **Tentativas feitas:** freeze de resolução, cenários, thresholds e métricas (`FPS`, `p50/p95/p99`, `CPU`, drops, qualidade).
- **Resultado de cada tentativa:** melhora a comparabilidade, mas exige disciplina no logging.
- **Solução provisória/final:** manter a matriz de benchmark sincronizada com o sprint e registar tudo por cenário.
- **Ações em aberto:** criar/usar a matriz de decisão ao longo de cada execução.
