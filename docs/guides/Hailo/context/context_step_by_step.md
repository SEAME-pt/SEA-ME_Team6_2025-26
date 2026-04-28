# Context Step by Step

Este guia mostra como usar o contexto do projeto sem o reinventar a cada tarefa.

## 1) Ler o panorama
- Abrir `context.md` e identificar o programa, a equipa e o estado do módulo.

## 2) Fixar as restrições
- Rever hardware, software e dependências de integração.
- Confirmar o que está dentro e fora do âmbito atual.

## 3) Relacionar com a tarefa
- Ligar o contexto ao objetivo do momento: setup, conversão, benchmark, E2E ou issue.

## 4) Atualizar só o que mudou
- Acrescentar decisões novas.
- Não reescrever o histórico já validado sem necessidade.

## 5) Resultado esperado
- Um ponto de entrada estável para qualquer pessoa retomar o trabalho rapidamente.

## 6) Checkpoint atual

### Estado guardado
- A documentação `yolo_step_by_step/` foi criada para consolidar o fluxo E2E e o runbook de replicação.
- O foco atual é manter o benchmark YOLO localmente enquanto a sessão remota está limitada.
- O script fixado do `yolo26n-seg` continua como referência experimental com host-side NMS.

### Como retomar depois
1. Abrir `yolo_step_by_step/README.md` para o índice principal.
2. Ler `yolo_step_by_step/00_full_lifecycle_e2e_step_by_step.md` para o contexto completo.
3. Seguir `yolo_step_by_step/02_replication_runbook.md` para executar os passos.
4. Consultar `yolo_step_by_step/05_known_issues_and_fixes.md` se houver erro.

### Próximo passo sugerido
- Validar localmente se há ficheiros novos a guardar em git e, quando a sessão remota voltar, continuar com a replicação do benchmark ou com a promoção dos scripts estáveis para `src/hailo/scripts`.
