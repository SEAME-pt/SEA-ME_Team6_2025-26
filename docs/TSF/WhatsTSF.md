# 🧩 Trustable Software Framework (TSF) — Resumo e Implementação

## 1. O que é o TSF (Trustable Software Framework)

O **Eclipse Trustable Software Framework (TSF)** é um **modelo e metodologia para avaliar o “grau de confiança” (trustability)** de um software, com base em **evidências verificáveis** sobre como ele é desenvolvido, testado e mantido.

Em vez de depender de documentos Word, Excel ou de ferramentas de requisitos proprietárias, o TSF integra **os metadados de confiança diretamente no repositório Git** do projeto (junto ao código, documentação e testes).  

O TSF foca-se em **sistemas críticos** — onde **segurança, performance, disponibilidade e fiabilidade** são aspetos essenciais — e permite:
- **Rastrear expectativas e evidências** (quem garante o quê e com base em que provas);
- **Quantificar confiança** (através de “scores”);
- **Manter coerência** entre o que o software afirma fazer e o que realmente faz.

---

## 2. O que se sabe até hoje (estado atual do projeto)

- O TSF está **em incubação no Eclipse Foundation**, com o **desenvolvimento ativo pela Codethink**.  
- É **open source**, com licenças **EPL 2.0** e **CC BY-SA 4.0**.  
- O desenvolvimento principal ocorre no GitLab da Codethink:  
  👉 [https://gitlab.com/CodethinkLabs/trustable/trustable](https://gitlab.com/CodethinkLabs/trustable/trustable)
- O **tooling oficial** chama-se **TruDAG** (Trustable Directed Acyclic Graph tool), implementado em Python.  
- O modelo baseia-se em **gráficos acíclicos direcionados (DAGs)** compostos por **Statements (declarações)** ligadas por relações lógicas.

### 🔹 Estrutura conceitual

Cada projeto é descrito por um **grafo de confiança**, composto por:
- **Expectations** → requisitos ou objetivos definidos pelos stakeholders.  
- **Assertions** → afirmações que ligam expectativas e evidências.  
- **Premises / Evidence** → provas concretas (documentos, código, resultados de testes, auditorias, etc).  
- **Assumptions** → condições externas ao projeto, mas necessárias (ex: dependência de um SO específico).

A partir destes elementos, o TSF constrói um **modelo rastreável**, permitindo:
- Justificar cada requisito com evidências.
- Ligar resultados de testes e análises automáticas.
- Avaliar **Confidence Scores** (níveis de confiança) automaticamente via CI/CD.

---

## 3. Como implementar o TSF na prática

### 🧾 Etapas resumidas da metodologia

1. **Definir Expectativas (Expectations)**  
   → O que o software deve fazer (funcional e não funcional).  
   Exemplo: “O sistema deve responder em menos de 200ms em 95% das requisições.”

2. **Identificar Evidências (Evidence)**  
   → Quais artefactos provam essas afirmações?  
   Exemplo: testes automatizados, logs de performance, revisões de código, documentação de segurança.

3. **Documentar Assumptions (Premissas externas)**  
   → O que depende de fatores fora do teu controlo (ex: infraestrutura do cliente).

4. **Registar a Lógica (Assertions)**  
   → Criar ligações entre Expectations e Evidence, formando um **grafo acíclico (DAG)**.  
   Cada ligação representa uma dedução lógica (“esta evidência suporta esta expectativa”).

5. **Avaliar Confiança (Confidence Assessment)**  
   → Automatizar a recolha de métricas e gerar pontuações.  
   (Isto pode ser integrado no pipeline CI/CD.)

---

## 4. Ferramentas para implementar: TruDAG

O **TruDAG** (Trustable DAG Tool) é o **software oficial** para operacionalizar o TSF.  
Serve para **criar, gerir e avaliar os “Trustable Graphs”** no teu repositório Git.

### 🔹 Instalação

```bash
pipx install trustable --index-url https://gitlab.com/api/v4/projects/66600816/packages/pypi/simple«
```

OU

```bash
pip install trustable --index-url https://gitlab.com/api/v4/projects/66600816/packages/pypi/simple
```

## 🔹 Uso básico

Depois de instalado, podes:

- Criar **Statements** (*Expectations*, *Assertions*, *Evidence*, *Assumptions*);
- Definir **links** entre eles;
- Associar **artefactos** (ficheiros, testes, outputs);
- Gerar **Confidence Scores** e relatórios.

> 🧩 Cada Statement e as suas ligações ficam registados no próprio repositório **Git**, garantindo **rastreabilidade nativa**.

---

## 🧮 5. Relação com Traceability Matrix e outras abordagens

O **TSF** substitui (ou generaliza) a tradicional **traceability matrix** (matriz de rastreabilidade de requisitos), mas de forma **automatizada e viva**, não documental.

| Método | Forma tradicional | TSF / TruDAG |
|--------|------------------|--------------|
| **Traceability Matrix** | Feita manualmente em Excel ou ferramenta dedicada | Representada como um **grafo (DAG)** em Git |
| **Evidência** | Ligada via documentos | Ligada via **artefactos e metadados verificáveis** |
| **Atualização** | Manual, sujeita a erro | **Automatizada** via CI/CD |
| **Avaliação** | Subjetiva | **Quantificável** com “Confidence Scores” |
| **Escalabilidade** | Difícil | **Elevada**, com composição entre projetos |

Portanto, **não é preciso usar uma matriz de rastreabilidade tradicional** — o **grafo do TSF** já a substitui, e o **TruDAG** é a ferramenta prática para gerir isso.

---

## ⚙️ 6. Exemplo simplificado de implementação

Imagina que tens um projeto `XYZ` com o objetivo de provar que é **seguro e confiável**:

### 🪄 Cria o repositório

```bash
git init XYZ
cd XYZ
```

### 📦 Instala o TruDAG

```bash
pipx install trustable
```

### 🧱 Adiciona Statements
```bash
trustable add "The software passes all critical security tests" --type Expectation
trustable add "Security tests are executed automatically in CI" --type Assertion
trustable add "CI results are published and reviewed weekly" --type Evidence
```

### 🔗 Liga os Statements
```bash
trustable link "Security tests are executed automatically in CI" "The software passes all critical security tests"
trustable link "CI results are published and reviewed weekly" "Security tests are executed automatically in CI"
```

### 🧩 Executa a avaliação
```bash
trustable evaluate
```

### ➡️ Resultado: um relatório com confidence score, evidências e dependências lógicas rastreáveis.

## 🧭 7. Conclusão

O TSF é uma abordagem moderna e formal para avaliar confiança em software, substituindo documentos e matrizes manuais por uma estrutura declarativa e rastreável, integrada no ciclo de desenvolvimento.

✅ Para implementar na prática:

Usar o TruDAG (ferramenta oficial em Python);

Modelar o projeto como um grafo de Statements;

Ligar evidências e artefactos concretos;

Automatizar a análise via CI/CD;

Dispensar folhas planeamento e ferramentas de rastreabilidade tradicionais.

## 📚 Referências

Eclipse TSF Project Page

Codethink TSF GitLab

Trustable Methodology

TruDAG Installation Guide

TSF Documentation Index


https://projects.eclipse.org/projects/technology.tsf

https://gitlab.com/CodethinkLabs/trustable/trustable

https://codethinklabs.gitlab.io/trustable/trustable/index.html

instalar a ferramenta da eclipse para o TSF
https://codethinklabs.gitlab.io/trustable/trustable/trudag/install.html#user

metodologia e aboardagem do TSF
https://codethinklabs.gitlab.io/trustable/trustable/methodology.html



## 💡 “Any consideration of trust must be based on evidence.”
— Trustable Software Framework
