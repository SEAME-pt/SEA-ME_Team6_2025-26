# YOLO Metrics — Step by Step (AGL)

Este ficheiro mantém-se por compatibilidade histórica.

## Documento canónico
- Usar: `benchmark_step_by_step.md`

## Motivo
- Evitar divergência entre dois guias semelhantes.
- Garantir um único fluxo oficial para execução e atualização de tabelas.

## Escopo do fluxo oficial
- runs E2E por cenário;
- recolha de logs de inferência e CPU;
- extração de `p50/p95/p99`, `jitter`, `drops`, `fps avg/min`;
- avaliação manual por classe crítica;
- atualização de `benchmark_matrix.md` e `yolo8s_vs_yolo8seg_vs_yolo26seg.md`.
