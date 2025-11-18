# 🧩 Trustable Software Framework (TSF) — Overview and Implementation

## 1. What is TSF (Trustable Software Framework)

The Trustable Software Framework (TSF) is an open-source toolset designed to manage requirements, evidence, and verification activities for safety- and mission-critical software — especially in aerospace, defense, automotive, and medical domains, where the software must be demonstrably trustworthy.

The **Eclipse Trustable Software Framework (TSF)** is a **model and methodology to assess the "trustability" of software**, based on **verifiable evidence** of how it is developed, tested, and maintained.

Instead of relying on Word documents, Excel sheets, or proprietary requirements tools, TSF integrates **trust metadata directly into the project's Git repository** (alongside code, documentation, and tests).

TSF focuses on **critical systems**—where **security, performance, availability, and reliability** are essential—and allows you to:
- **Track expectations and evidence** (who guarantees what and based on which proofs);
- **Quantify trust** (via scores);
- **Maintain consistency** between what the software claims to do and what it actually does.

### Current Status

- TSF is **incubated at the Eclipse Foundation**, with **active development by Codethink**.  
- It is **open source**, licensed under **EPL 2.0** and **CC BY-SA 4.0**.  
- Main development occurs on Codethink's GitLab:  
  👉 [https://gitlab.com/CodethinkLabs/trustable/trustable](https://gitlab.com/CodethinkLabs/trustable/trustable)
- The **official tooling** is **TruDAG** (Trustable Directed Acyclic Graph tool), implemented in Python.  
- The model is based on **Directed Acyclic Graphs (DAGs)** composed of **Statements** linked by logical relationships.

### 🧩 Trustable Software Framework (TSF) — Overview and Technical Context

📚 Official source (GitLab Project)

“Trustable Software Framework (TSF) builds on top of Doorstop to manage requirements, tests, and assurance cases in a traceable, verifiable way.”
👉 Source: https://gitlab.com/trustable/trustable

In short:

- TSF extends Doorstop to provide full traceability and certification evidence management.

### ⚙️ What is Doorstop

- Doorstop is the foundation of TSF.
- It’s a Python-based tool that uses YAML files to represent requirements, tests, and documentation in a version-controlled and traceable way.

📘 Official source (Doorstop README):

“Doorstop is a requirements management tool that stores data in version control alongside source code.”
👉 Source: https://github.com/doorstop-dev/doorstop

💡 Instead of using Word or Excel, each requirement is a small .yaml file stored next to your source code in Git.
This makes it possible to link requirements, implementation, and verification directly.

### 🧠 What is trudag

- trudag is the command-line interface (CLI) included with the trustable package.
- It’s used to generate traceability diagrams and documents from TSF repositories.

- It converts relationships between requirements and tests into .dot files (Graphviz format), which can later be exported as PDF or PNG to visualize traceability.

📗 Official source (Trustable CLI docs):

“The trudag CLI builds traceability diagrams (.dot files) from TSF repositories.”
👉 Source: https://gitlab.com/trustable/trustable/-/tree/main/frontends/cli

### 🔷 What are .dot files

.dot files use the Graphviz DOT language, which is a plain text graph description format used to represent nodes and links.

📚 Official source (Graphviz):

“The DOT language is a plain text graph description language.”
👉 https://graphviz.org/doc/info/lang.html

💡 In TSF, DOT files are used to visualize relationships like:

[REQ-001] --> [TEST-001]

### What is a Graph (in TSF context)

A graph is a set of nodes connected by edges:

- Each node is a Statement (a claim about the software).

- Each edge is a logical link, meaning “this leads to that” or “this depends on that”.

----

### Directed Acyclic Graph - TSF uses a DAG (Directed Acyclic Graph):

- Directed → edges have direction (A supports B).

- Acyclic → no cycles allowed (A cannot depend on itself indirectly).

💡 Simple analogy:
Imagine a family tree: each person (Statement) is linked to parents/children. No one can be their own ancestor → no cycles.

### Types of Statements

#### 🔹 Conceptual Structure

Like it was said, each project is described by a **trust graph**, composed of:
- **Expectations** → requirements or goals defined by stakeholders.  
- **Assertions** → statements connecting expectations and evidence.  
- **Premises / Evidence** → concrete proofs (documents, code, test results, audits, etc.).  
- **Assumptions** → external conditions necessary for the project (e.g., OS dependency).

From these elements, TSF builds a **traceable model**, allowing you to:
- Justify each requirement with evidence.
- Link test results and automated analyses.
- Automatically evaluate **Confidence Scores** via CI/CD.

#### 🔹 Resuming Table
| Type                | Description (What it is)                                          | Function (What it does)                                | Example                               |
|---------------------|-------------------------------------------------------------------|----------------------------------------------------------|----------------------------------------|
| **Expectation**     | What the software should achieve (goal defined by stakeholders)   | Top of the graph — what we want to prove                | “System responds under 200ms”          |
| **Assertion**       | Logical justification (reason why something is true)              | Connects Expectations to Evidence                        | “Performance tests are automated”      |
| **Evidence (Premise)** | Concrete proof                                                   | Shows that the Assertion is true                         | “Automated tests show avg 180ms”       |
| **Assumption**      | External factor presumed true                                      | Condition required for Assertions/Expectations to hold   | “System runs on Linux”                 |

#### Links in the graph:

- Expectation → supported by Assertions

- Assertions → supported by Evidence

- Assumptions → linked as external conditions

#### Simple Visual Representation
Expectation: Software XYZ is safe
        |
     Assertion: Automated security tests passed
        |
     Evidence: CI/CD logs show 0 failures
        |
   Assumption: Runs on Linux

Each level is a layer of the graph.

If something changes (e.g., a test fails), TruDAG marks the Statement as Suspect, signaling a review is needed.

#### This enables automatic generation of traceability diagrams for:

- Requirements

- Tests

- Verification activities

- Certification evidence

#### ✅ Difference Between Expectation and Assertion (simple and direct explanation)

Think about it like this:

##### 🔹 Expectation = What we want to be true

An **Expectation** is a **goal, requirement, or intention** of the project.

It is **high-level**, something the stakeholders want.

###### Examples of Expectations:
- “The system is secure.”
- “The response time is below 200ms.”
- “The software behaves reliably under load.”

These are **macro-level statements** that need justification.

---

##### 🔹 Assertion = How we justify that Expectation

An **Assertion** is a **logical statement** that connects an Expectation to the Evidence.

It answers the question: **“Why do we believe this is true?”**

Assertions act as **bridges** in the graph.

###### Examples of Assertions:
- “Security tests are executed automatically in CI.”
- “All commits pass static code analysis.”
- “Load tests run nightly on a production-like environment.”

Assertions are **not proofs** — they are **reasons** that support the Expectation.

---

#### 🧩 Simple visual example

##### Expectation
“The system is secure.”

⬇️ *Why?* (Assertion)

##### Assertion
“Security tests are executed automatically in CI.”

⬇️ *What proof do we have?* (Evidence)

##### Evidence
“CI logs show 0 failed security tests in the last 50 runs.”

---

##### Visual summary
Expectation
↓
Assertion
↓
Evidence

---

#### 💡 Analogy

Imagine you are in an oral exam and the professor asks:

**❓ “Why do you believe the system is secure?”**  
(Expectation)

You answer:

**💬 “Because the security tests run automatically and never fail.”**  
(Assertion)

Then you show:

**📄 “Here are the logs from the last 6 months.”**  
(Evidence)

---

#### ✔️ Golden rule

- **Expectations** = what we want to prove  
- **Assertions** = how we justify it  
- **Evidence** = proof supporting the justification

### 🔗 How the Components Fit Together

📘 Source (Trustable GitLab):

“TSF supports traceability and assurance evidence in compliance with certification standards.”
👉 https://gitlab.com/trustable/trustable

#### 🔧 TSF Component Overview (Unified Table)

| Component / Tool    | Role / Purpose                                                   | Key Feature                                      | File Types Produced / Used |
|---------------------|------------------------------------------------------------------|--------------------------------------------------|-----------------------------|
| **TSF (Trustable)** | End-to-end traceability + assurance framework (uses Doorstop + trudag + other utilities) | Integrates requirements, tests, evidence, and confidence scoring | `.yaml`, `.dot` |
| **Doorstop**        | Requirements management foundation                               | YAML-based version-controlled requirements        | `.yaml` |
| **trudag**          | Diagram + report generator for TSF                                | Generates Graphviz diagrams and PDFs with DAGs    | `.dot`, `.pdf`, `.png` |

---

## 2. Tools: TruDAG ... installation

**TruDAG** is the **official tool** for implementing TSF.  
It allows you to **create, manage, and evaluate Trustable Graphs** directly in your Git repository.

### 🔹 Installation

```bash
pipx install trustable --index-url https://gitlab.com/api/v4/projects/66600816/packages/pypi/simple
```

OR 

```bash
pip install trustable --index-url https://gitlab.com/api/v4/projects/66600816/packages/pypi/simple
```

### 🔹 Basic Usage

After installation, you can:

- Create Statements (Expectations, Assertions, Evidence, Assumptions);

- Define links between them;

- Associate artifacts (files, tests, outputs);

- Generate Confidence Scores and reports.

🧩 Each Statement and its links are recorded in the Git repository, ensuring native traceability.

### 🪜 Why We Use This Installation Method

✔️ Technical reason: Using pipx keeps the TSF environment isolated, avoiding version conflicts with system-wide Python packages.
✔️ Security reason: TSF is used in safety-critical domains where dependency control is essential.
✔️ Traceability reason: All requirements and evidence are version-controlled in GitHub as YAML files, supporting certification standards like DO-178C, ISO 26262, and EN 50128.

## ⚙️ 3. Simple Lab

This is an example of a workflow for TSF.

1. Create virtual env  
    ```bash
    python3.12 -m venv .venv
    source .venv/bin/activate
    ```

2. Install trudag (official TSF tool)  
    ```bash
    git clone https://gitlab.com/CodethinkLabs/trustable/trustable.git /tmp/trustable
    cd /tmp/trustable
    git checkout v2025.09.16
    pip install .
    cd -
    ```

3. trudag --init & Create requirement  
    ```bash
    trudag --init
    trudag manage create-item UR 001 reqs/
    # Edit UR-001.md - change the template
    # -> do this for every requirement (UR, SYSR, SOFR, TC)
    trudag manage lint
    ```

4. Link requirements  
    ```bash
    trudag manage create-link UR-001 SYSR-001
    # -> do this for every requirement
    trudag manage lint
    ```

5. Review requirements  
    ```bash
    trudag manage set-item XX-001
    # Review every requirement needed
    ```

6. Add references/evidence & configure scoring  
    - Add reference files to the requirements/tests  
    - The files must exist in the correct path  

    ```bash
    trudag manage lint
    trudag score
    ```

---

# Requirement template:

```yaml
---
id: UR-001
normative: true
level: 1.1

references:
  - type: "file"
    path: "ref1.md"
---

---
id: UR-001
normative: true
level: 1.2

references:
  - type: "file"
    path: "ref2.md"

reviewers:
  - name: David
    email: david.fernandes@seame.pt
    reviewed: "2025-11-12 - Approved by David david.fernandes@seame.pt"
    review_status: true

score:
  David: 0.2
  Joao: 0.4
---
```

## ⚙️ 4. Detailed Implementation Example

. Practical Implementation with TruDAG

Assume a project XYZ aims to prove it is safe and trustworthy.

### 🪄 Step 1: Initialize Repository
```bash
git init XYZ
cd XYZ
```

### 📦 Step 2: Install TruDAG / Trustable
```bash
pipx install trustable --index-url https://gitlab.com/api/v4/projects/66600816/packages/pypi/simple
```

OR

```bash
pip install trustable --index-url https://gitlab.com/api/v4/projects/66600816/packages/pypi/simple
```

### 🧱 Step 3: Init
```bash
trudag --init 
```

### Step 4: Create Statements
```bash
trudag manage create-item "Software XYZ is safe" ./XYZ/ --type Expectation
trudag manage create-item "Automated security tests passed" ./XYZ/ --type Assertion
trudag manage create-item "CI/CD logs show 0 failures" ./XYZ/ --type Evidence
trudag manage create-item "Runs on Linux" ./XYZ/ --type Assumption
```
. Check for errors in the graph connections
```bash
trudag manage lint 
```
### 🔗 Step 5: Link Statements
```bash
trudag manage create-link "Automated security tests passed" "Software XYZ is safe"
trudag manage create-link "CI/CD logs show 0 failures" "Automated security tests passed"
trudag manage create-link "Runs on Linux" "Automated security tests passed"
```

### 🧩 Step 6: Validate Evidente
```bash 
trudag manage set-item "Software XYZ is safe" ./XYZ/ --type Expectation
trudag manage set-item "Automated security tests passed" ./XYZ/ --type Assertion
trudag manage set-item "CI/CD logs show 0 failures" ./XYZ/ --type Evidence
trudag manage set-item "Runs on Linux" ./XYZ/ --type Assumption
```
. Check for errors in the graph connections
```bash
trudag manage lint 
```

### Step 7: Evaluate Confidence
```bash
trudag score
```

### Step 8: Publish
```bash
trudag publish
```

## Other commands
. inspect specific items or links 
```bash
trugad manage show-item 
trudag manage show-link
```
.reviewing items
```bash
trudag manage set-item #name of the item#
trudag manage set item #name of the item1# #name of the item2#
```
.Clearing suspect items
```bash
trudag manage set link #name of the item#
```
. Linking a trustable graph
```bash
trudag manage lint 
```

TruDAG traverses the graph, checks all links and evidence, and calculates how much we can trust the Expectation.


## 🧮 4. Relation to Traceability Matrix and Other Approaches

TSF replaces (or generalizes) traditional traceability matrices, but in a live and automated form rather than static documents.

| Method                  | Traditional Form                     | TSF / TruDAG                                     |
| ----------------------- | ------------------------------------ | ------------------------------------------------ |
| **Traceability Matrix** | Manually in Excel or dedicated tools | Represented as a **graph (DAG)** in Git          |
| **Evidence**            | Linked via documents                 | Linked via **verifiable artifacts and metadata** |
| **Updates**             | Manual, error-prone                  | **Automated** via CI/CD                          |
| **Assessment**          | Subjective                           | **Quantifiable** with “Confidence Scores”        |
| **Scalability**         | Limited                              | **High**, with composition across projects       |

Thus, no traditional matrix is needed—the TSF graph replaces it, and TruDAG is the practical management tool.


## 🧭 5. Conclusion

TSF provides a modern, formal approach to assess software trustability, replacing manual documents and matrices with a declarative, traceable structure integrated into the development workflow.

### 🔑 Key Takeaways

- Not a static Excel matrix — it is a graph of Statements in Git.

- Each Statement is traceable and linked to concrete evidence.

- TruDAG automates creation, linking, and analysis.

- Confidence is calculated automatically, but human review remains essential.

➡️ Outcome: a report with confidence scores, evidence, and traceable logical dependencies.

### ✅ Practical steps:

- Use TruDAG (official Python tool)

- Model the project as a graph of Statements

- Link concrete evidence and artifacts

- Automate analysis via CI/CD

- Discard traditional spreadsheets and traceability tools


## 📚 References

- Eclipse TSF Project Page: https://projects.eclipse.org/projects/technology.tsf

- Codethink TSF GitLab: https://gitlab.com/CodethinkLabs/trustable/trustable

- Structure: https://gitlab.com/CodethinkLabs/safety-monitor/safety-monitor/-/tree/main/trustable

- TruDAG Methodology & Documentation: https://codethinklabs.gitlab.io/trustable/trustable/index.html

- TruDAG Installation Guide: https://codethinklabs.gitlab.io/trustable/trustable/trudag/install.html#user
. https://gitlab.com/CodethinkLabs/trustable/trustable/-/tree/main/docs/trudag 

- TSF Methodology Overview: https://codethinklabs.gitlab.io/trustable/trustable/methodology.html

- https://score-json.github.io/json/main/generated/trustable_graph.html

- Meetings: https://pad.codethink.co.uk/Bz7i6JRyRBSLC_Y7ID4lYQ

- Question: https://gitlab.com/CodethinkLabs/trustable/trustable/-/issues/22
. https://gitlab.com/CodethinkLabs/trustable/trustable/-/issues?sort=created_date&state=opened&first_page_size=100&page_after=eyJjcmVhdGVkX2F0IjoiMjAyNS0wNi0xMSAxMTo1MDoxNy4wNjQxNDMwMDAgKzAwMDAiLCJpZCI6IjE2ODczNjk4OCJ9

## 💡 “Any consideration of trust must be based on evidence.”
— Trustable Software Framework
