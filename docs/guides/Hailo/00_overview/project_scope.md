# Project Scope (Hailo ADAS)

## Contexto funcional
O veículo deve operar em condução automática numa pista de teste com:
- piso preto,
- linhas brancas (berma esquerda e tracejado central),
- linha amarela (berma direita),
- sinalização vertical,
- sinalização horizontal (`passadeira`, `STOP`, virar esquerda/direita),
- sombras,
- curvas e contracurvas,
- curvas apertadas,
- curvas a 90º/cruzamentos,
- alterações de cor e contraste da pista.

## Objetivo
Construir, integrar e validar um pipeline ADAS em edge com Hailo, capaz de suportar este cenário de pista com robustez e reprodutibilidade, garantindo:
- perceção estável em cenários degradados,
- comportamento consistente entre runs,
- integração correta com a arquitetura distribuída (STM32 + RPi5/AGL + RPi4 cluster),
- rastreabilidade de decisões técnicas (modelo, calibração, benchmark e E2E).

## Como usar este documento
- Ler este ficheiro depois de `../context/context.md`.
- Usá-lo para confirmar a direção do trabalho antes de começar setup, conversão ou benchmark.
- Se o contexto do sistema mudar, atualizar primeiro o `context.md` e só depois este objetivo.

Este scope está alinhado com o estado atual do projeto SEA:ME Team 6:
- módulos 0 e 1 concluídos,
- módulo 2 (ADAS) em evolução,
- OTA e CI/CD já iniciados e em refinamento.

## Arquitetura alvo (alto nível)
1. Câmara
2. Preprocess
3. Inferência (Hailo/HEF)
4. Pós-processamento
5. Render/CAN

E2E real (decode→preprocess→infer→postprocess→render), uso CPU no pós-processamento, estabilidade temporal, métricas por cenário (sombra/curva), e avaliação de segmentação (mIoU/Dice ou mAP-seg).

O que é cada parte
* decode: ler imagem/vídeo e transformar JPEG/H264/etc. em frames brutos.
* preprocess: resize, crop/letterbox, normalização, BGR↔RGB, layout de tensor.
* infer: correr o HEF na Hailo.
* postprocess: interpretar outputs, aplicar thresholds, NMS, reconstruir máscaras/caixas/linhas.
* render: desenhar overlay, máscaras, caixas, texto, FPS, e às vezes enviar saída para GUI/log/controlo.


Métricas:
- latência real,
- jitter,
- custo do NMS no host,
- custo do overlay/render,
- custo total na aplicação.

. medições de estabilidade temporal (p95/p99, jitter)
. métricas de robustez por cenário (sombras/curvas)
. medições de qualidade (mAP-seg, Dice, taxa de falsa negativa por classe)
. medições de CPU/térmico durante host-NMS

## Arquitetura de sistema (resumo)
- **STM32U585AI + ThreadX**: controlo de motores, velocidade e sinais de tempo real.
- **Raspberry Pi 5 (AGL 20.0.2)**: integração veicular, CAN, VSS/KUKSA.
- **Raspberry Pi 4 (Debian/Qt)**: instrument cluster.
- **Comunicação**: STM32 -> MCP2515/MCP2518 -> RPi5/AGL -> KUKSA/VSS -> RPi4/Qt.

Decisão arquitetural relevante: não usar `uProtocol`; usar `COVESA VSS` + `KUKSA Databroker`.

## Decisão técnica esperada
Comparar candidatos em condições homogéneas e fechar decisão com base em:
- latência (p50/p95/p99),
- FPS,
- estabilidade,
- consumo CPU,
- qualidade funcional no caso ADAS (deteção/segmentação em cenários críticos).

## Critério de sucesso do scope
- Pipeline E2E documentado de fio a pavio com evidência.
- Separação explícita entre medições `infer-only` e `E2E`.
- Decisão final de modelo suportada por benchmark comparável.
- Lista de riscos técnicos e mitigação para iteração seguinte.

## Fontes de contexto anterior
- `../context/context.md`
- `../backup/legacy_2026-04-14/AI_implementation_planning.md`
- `../backup/legacy_2026-04-14/Benchmark_Global_Detecao_Linhas_e_objetos.md`
- `../backup/legacy_2026-04-14/Benchmark_segformerYolo8s_vs_UFLv2Yolo8ou26seg.md`
- `overview_step_by_step.md`
