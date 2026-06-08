# Hailo Documentation Index

Este diretório está organizado para separar claramente:
- contexto transversal do projeto,
- visão global do projeto,
- setup e conversão,
- benchmark e E2E,
- problemas/soluções,
- planeamento atual,
- histórico bruto/evidência.

## Estrutura

- `context/`: contexto detalhado do projeto SEA:ME Team 6 (hardware, software, constraints, estado).
- `context/context_step_by_step.md`: como usar e atualizar o contexto.
- `00_overview/`: visão geral do projeto, arquitetura e objetivos.
- `00_overview/overview_step_by_step.md`: ordem recomendada para usar a documentação.
- `10_setup/`: instalação e configuração (Docker, Lenovo, AGL, paths).
- `10_setup/setup_step_by_step.md`: execução prática do setup.
- `20_conversion/`: fluxo de conversão (`.pt -> .onnx -> .har -> .hef`), calibração e scripts.
- `20_conversion/conversion_step_by_step.md`: execução prática da conversão.
- `30_benchmarks/`: medições comparáveis, critérios e resultados de benchmark.
- `30_benchmarks/benchmark_step_by_step.md`: execução prática de benchmark.
- `40_e2e/`: narrativa fio a pavio (o que foi feito, como, porquê, resultado).
- `40_e2e/e2e_step_by_step.md`: execução prática E2E.
- `50_issues/`: problemas por modelo/componente (ex.: UFLDv2) e soluções.
- `50_issues/issue_step_by_step.md`: método para registar e fechar issues.
- `60_planning/`: planeamento vivo do sprint atual e próximos passos.
- `60_planning/planning_step_by_step.md`: método para manter o plano vivo.
- `90_history/`: histórico organizado de comandos/logs usados como evidência.
- `90_history/evidence_step_by_step.md`: método para indexar evidência.
- `yolo_step_by_step/`: pacote operacional de onboarding e replicação dos fluxos YOLO (setup -> inferência -> benchmark -> troubleshooting).
- `backup/`: legado documental preservado, sem perda de conteúdo.

## Começar por aqui

1. Ler `context/context.md`.
2. Ler `00_overview/project_scope.md`.
3. Validar ambiente em `10_setup/environment_setup_checklist.md`.
4. Seguir conversão em `20_conversion/conversion_pipeline.md`.
5. Medir com `30_benchmarks/benchmark_protocol.md`.
6. Fechar narrativa em `40_e2e/e2e_execution_story.md`.
7. Registar bloqueios em `50_issues/UFLDv2_issues_and_possible_fixes.md`.
8. Atualizar plano em `60_planning/current_sprint_plan.md`.
9. Indexar evidência em `90_history/evidence_index.md`.
10. Para replicação YOLO de ponta a ponta, seguir `yolo_step_by_step/README.md`.

## Fluxo E2E de referência

1. Câmara
2. Preprocess
3. Inferência (Hailo)
4. Pós-processamento
5. Render/CAN

Cada etapa deve responder:
- o que foi feito,
- como foi feito,
- por que foi feito,
- resultado obtido.

## Conteúdo legado

Os documentos anteriores foram movidos para:
- `backup/legacy_2026-04-14/`

Inclui também conteúdo histórico já existente (ex.: `historical/commands`).
