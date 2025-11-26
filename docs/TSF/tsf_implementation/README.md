# TSF Minimal Implementation (exemplo)

Este é um exemplo mínimo e funcional para começar a implementar o Trustable
Software Framework (TSF) no teu projeto `Sea:ME`.

Objetivo: fornecer uma estrutura simples com quatro tipos de ficheiros:
- Expectation (EX-...)
- Assertion (AS-...)
- Assumption (ASU-...)
- Evidence (EV-...)

Cada item é um ficheiro Markdown com front-matter mínimo. As ligações entre
itens são descritas no `graph/graph.dot` (formato DOT). Esta abordagem:

- é suficiente para começar hoje
- permite evolução para `trudag`/`dotstop` depois

Estrutura criada:

```
docs/TSF/tsf_implementation/
├─ README.md
├─ items/
│  ├─ EX-001_expectation.md
│  ├─ AS-001_assertion.md
│  ├─ ASU-001_assumption.md
│  └─ EV-001_evidence.md
└─ graph/
   └─ graph.dot
```

Próximos passos rápidos (no terminal, a partir do root do repo):

```bash
# Ver ficheiros criados
ls -la docs/TSF/tsf_implementation

# Adicionar e commitar (se quiseres guardar no repo)
git add docs/TSF/tsf_implementation
git commit -m "Add minimal TSF implementation example"
git push origin development
```

Como usar:
- Abre os ficheiros em `items/` e ajusta as declarações para o teu projeto
  Sea:ME.
- Edita `graph/graph.dot` para ligar outros items.
- Mais tarde podes trocar para `trudag`/`dotstop` quando estiveres pronto.

Se quiseres, eu adapto estes exemplos a requisitos concretos do `Sea:ME` —
diz quais são as expectativas que queres representar e eu escrevo as items.
# TSF implementation

This folder contains the Trustable Software Framework implementation for the project.

Structure:
- references/
  - expectations/
  - assertions/
  - assumptions/
  - evidences/
- validators/

Each item is a markdown file using the Trudag/Trustable template (YAML front matter). Use `trudag manage` to create / link / lint / score.
