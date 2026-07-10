# Benchmark Protocol

## Objetivo
Garantir comparação justa entre modelos e entre runs.

## Separação obrigatória
- **Infer-only:** mede throughput/latência de inferência no HEF.
- **E2E:** mede pipeline completo (câmara -> preprocess -> infer -> pós-process -> render/CAN).

## Campos mínimos por run
- modelo/HEF,
- cenário,
- nº frames,
- FPS,
- latência p50/p95/p99,
- CPU avg/max,
- observações (erro/jitter/instabilidade).

## Critérios de aceitação (base)
- p99 < limiar definido pela equipa,
- estabilidade sem quedas anómalas,
- reprodutibilidade em múltiplas runs.

## Template de tabela
| Modelo | Cenário | FPS | p50 | p95 | p99 | CPU avg | CPU max | Notas |
|---|---|---:|---:|---:|---:|---:|---:|---|
| | | | | | | | | |
