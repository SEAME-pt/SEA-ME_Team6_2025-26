# Current Sprint Plan

## Objetivo do sprint
Validar e comparar pipelines E2E reais para a pista do projeto, com foco em `decode → preprocess → infer → postprocess → render` e comparação justa entre opções.

## Opções em avaliação
- **A — `UFLDv2 + YOLOv8s`**: responsabilidade do David; serve como baseline de referência.
- **B — `UFLDv2 + YOLO26n-seg`**: prioridade do João; alvo principal para testar a rota mais moderna.
- **C — `UFLDv2 + YOLOv8n-seg`**: alternativa do João; fallback mais simples e previsivelmente estável.
- **D — `SegFormer + YOLOv8s`**: spike opcional se houver tempo; focado em passadeiras, setas e semântica de pista.

## Divisão de trabalho
- **David:** continuar a parte do `UFLDv2 e refinar o pós-processamento já existente, migrando a implementação para C++ quando estiver estável.
- **Vasco:** fechar a integração `infer → postprocess → render` no YOLOv8s` testar se consegue detetar passadeiras, stops e setas do chão com bounding boxes.
- **João:** implementar os pipelines E2E de `YOLO26n-seg` e `YOLOv8n-seg`, medir e comparar, e avançar para `SegFormer` se o tempo permitir.

## Entregáveis
- [ ] `setup_step_by_step.md` com comandos e validação real.
- [ ] `conversion_step_by_step.md` com rota baseline e custom.
- [ ] Harness E2E reutilizável para opções B e C.
- [ ] Tabela comparativa A/B/C/D com métricas e notas.
- [ ] Decisão final suportada por dados.

## Plano operacional
1. **Freeze do protocolo**: resolução, thresholds, cenários, métricas e naming.
2. **Fechar setup**: host, container, target, calibração e `hailomz`.
3. **Converter e validar artefactos**: ONNX → HEF com logs preservados.
4. **Executar B e C em E2E**: medir `FPS`, `p50/p95`, `CPU`, drops e qualidade.
5. **Se houver tempo, spike em D**: `SegFormer + YOLOv8s`.
6. **Comparar e decidir**: escolher a melhor opção para o E2E final.

## Métricas a comparar
- `FPS` E2E médio e mínimo.
- Latência `p50`, `p95` e `p99`.
- `CPU` média e máxima.
- `jitter` / drops / crashes.
- Qualidade por classe: passadeira, setas, linhas, sinais.
- Estabilidade por cenário: reta, curva, sombra, cruzamento.

## Critérios de decisão
- **Produção:** `p95 <= 50 ms`, `CPU avg <= 30%`, baixa taxa de falsos negativos em passadeira/setas.
- **Aceitável para iteração:** `p95 <= 60 ms` com estabilidade boa e sem crashes.
- **No-go:** crashes, `p95 > 60 ms`, ou degradação visível na deteção de pista.

## Riscos
- Post-process em Python pode tornar-se gargalo; considerar C++ cedo se a latência subir.
- `SegFormer` pode exigir mais afinação do que o tempo disponível permite.
- Se os labels da pista ainda não cobrirem bem todos os cenários, a comparação pode ficar enviesada.

## Decisões pendentes
- Confirmar se B será full INT8 ou split hybrid dependendo da compilação.
- Confirmar o conjunto final de cenários de benchmark.
- Decidir se D entra apenas como spike ou como candidato real.

## Próximos passos (48h)
1. Finalizar `setup_step_by_step.md` e `conversion_step_by_step.md`.
2. Fechar o harness E2E para B e C com métricas iguais.
3. Preparar a tabela comparativa e o primeiro go/no-go.

## Base documental
- `../backup/legacy_2026-04-14/E2E_1DAY_CHECKLIST_14apr2026.md`
- `../backup/legacy_2026-04-14/E2E_1DIA_RESUMO_14apr.md`
- `../backup/legacy_2026-04-14/AI_implementation_planning.md`
