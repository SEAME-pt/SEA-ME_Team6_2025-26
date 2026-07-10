# E2E Step by Step

Este guia organiza a narrativa completa de uma execução fim-a-fim.

## 1) Definir o cenário
- Escolher a sequência a testar: câmara, preprocess, inferência, pós-processamento e render/CAN.
- Fixar o objetivo da execução antes de recolher qualquer evidência.

## 2) Executar uma etapa de cada vez
- Documentar o que foi feito.
- Explicar como foi feito.
- Explicar porquê a etapa era necessária.
- Registar o resultado observado.

## 3) Guardar evidência por etapa
- Capturar logs, screenshots ou medições úteis.
- Evitar misturar evidência de etapas diferentes no mesmo bloco.

## 4) Fechar o fio narrativo
- Ligar a saída de uma etapa à entrada da seguinte.
- Mostrar onde o pipeline funcionou e onde falhou.
- Indicar o impacto no comportamento final do sistema.

## 5) Resultado esperado
- Uma história técnica completa e verificável.
- Evidência suficiente para reproduzir a execução.

## Ligações úteis
- `README.md`
- `e2e_execution_story.md`
- `../30_benchmarks/benchmark_step_by_step.md`
