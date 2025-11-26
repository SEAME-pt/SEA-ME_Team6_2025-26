# TSF Implementation (Sea:ME) — Clean Bootstrap

Esta pasta contém uma implementação limpa e inicial do Trustable Software Framework
(TSF) para o projeto Sea:ME.

Estrutura:
- `items/expectations/` — Expectativas (EX-...)
- `items/assertions/` — Afirmações / Assertions (AS-...)
- `items/assumptions/` — Premissas / Assumptions (ASU-...)
- `items/evidences/` — Evidências (EV-...)
- `graph/graph.dot` — Grafo DOT que liga os items
- `validators/validate_items.py` — Validador simples front-matter
- `scripts/` — Scripts de utilidade (gerar graph, etc.)

Uso rápido:
- Edita ou acrescenta ficheiros em `items/*` usando o template de front-matter YAML.
- Atualiza `graph/graph.dot` para ligar items.
- Usa `validators/validate_items.py` para verificar a presença de front-matter.

Objetivo imediato: proporcionar um ambiente reproduzível para mapear os requisitos L0
(veja `docs/TSF/requirements/LO_requirements.md`) para Expectations → Assertions → Evidence.

Depois de validarmos estas statements, posso gerar um `.dotstop.dot` e preparar validação/CI.
