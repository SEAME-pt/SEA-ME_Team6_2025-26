# Environment Setup Checklist

## 1) Host (Lenovo)
- [ ] Docker funcional.
- [ ] Paths normalizados para partilha de artefactos.
- [ ] Ferramentas de export ONNX instaladas.

## 2) Container (Hailo Suite)
- [ ] Acesso ao `hailomz` e utilitários de compilação.
- [ ] Pastas de trabalho montadas corretamente.
- [ ] Estrutura de logs/artefactos criada.

## 3) Target (AGL)
- [ ] Dispositivo Hailo detetado.
- [ ] HEFs acessíveis no target.
- [ ] Comando de validação de HEF executado sem erro.

## 4) Convenções de paths
Documentar aqui os paths oficiais da equipa (host, container, target) para evitar deriva operacional.

## 5) Sanity checks mínimos
- validar leitura de ficheiros de calibração;
- validar presença dos ONNX esperados;
- validar parse do HEF no target.
