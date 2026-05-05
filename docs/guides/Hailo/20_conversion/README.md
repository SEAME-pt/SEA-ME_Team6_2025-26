# 20_conversion

Fluxo de conversão de modelos:
- export ONNX,
- parsing/compilação HAR/HEF,
- calibração,
- scripts `.alls`,
- validações de artefactos.

## Documento principal

- `conversion_pipeline.md`
- `conversion_step_by_step.md`

## Resultado esperado desta fase

- `ONNX` válido por variante.
- `HAR` gerado sem erros críticos.
- `HEF` gerado e copiado para artefactos.
- Registo do modo de execução (`full-int8` vs `split/no-nms`).

