# Conversion Pipeline

## Fluxo
1. `.pt -> .onnx`
2. `.onnx -> .har`
3. `.har -> .hef`

## Inputs necessários
- modelo fonte (`.pt` ou `.onnx`),
- dataset/pasta de calibração,
- configuração de nós (start/end) quando aplicável,
- script `.alls` (quando necessário, ex.: no-NMS).

## Outputs esperados
- ONNX válido por variante,
- log de compilação por tentativa,
- HEF final identificado por naming consistente.

## Regras de rastreabilidade
- Cada tentativa deve gerar log próprio.
- Cada HEF deve ter origem explícita (modelo, resolução, modo).
- Registrar se o fluxo foi full-INT8 ou híbrido/no-NMS.

## Evidência histórica útil
- `../backup/legacy_2026-04-14/historical/commands/docker_history.md`
- `../backup/legacy_2026-04-14/historical/commands/lenovo_history.md`
