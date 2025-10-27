# 🧩 Trustable Software Framework (TSF) — Overview and Implementation

## 1. What is TSF (Trustable Software Framework)

The **Eclipse Trustable Software Framework (TSF)** is a **model and methodology to assess the "trustability" of software**, based on **verifiable evidence** of how it is developed, tested, and maintained.

Instead of relying on Word documents, Excel sheets, or proprietary requirements tools, TSF integrates **trust metadata directly into the project's Git repository** (alongside code, documentation, and tests).

TSF focuses on **critical systems**—where **security, performance, availability, and reliability** are essential—and allows you to:
- **Track expectations and evidence** (who guarantees what and based on which proofs);
- **Quantify trust** (via scores);
- **Maintain consistency** between what the software claims to do and what it actually does.

---

## 2. Current Status

- TSF is **incubated at the Eclipse Foundation**, with **active development by Codethink**.  
- It is **open source**, licensed under **EPL 2.0** and **CC BY-SA 4.0**.  
- Main development occurs on Codethink's GitLab:  
  👉 [https://gitlab.com/CodethinkLabs/trustable/trustable](https://gitlab.com/CodethinkLabs/trustable/trustable)
- The **official tooling** is **TruDAG** (Trustable Directed Acyclic Graph tool), implemented in Python.  
- The model is based on **Directed Acyclic Graphs (DAGs)** composed of **Statements** linked by logical relationships.

### 🔹 Conceptual Structure

Each project is described by a **trust graph**, composed of:
- **Expectations** → requirements or goals defined by stakeholders.  
- **Assertions** → statements connecting expectations and evidence.  
- **Premises / Evidence** → concrete proofs (documents, code, test results, audits, etc.).  
- **Assumptions** → external conditions necessary for the project (e.g., OS dependency).

From these elements, TSF builds a **traceable model**, allowing you to:
- Justify each requirement with evidence.
- Link test results and automated analyses.
- Automatically evaluate **Confidence Scores** via CI/CD.

---

## 3. How to Implement TSF in Practice

### 🧾 Methodology Steps

1. **Define Expectations**  
   → What the software should achieve (functional and non-functional).  
   Example: “The system must respond in under 200ms for 95% of requests.”

2. **Identify Evidence**  
   → Which artifacts prove these claims?  
   Example: automated tests, performance logs, code reviews, security documentation.

3. **Document Assumptions**  
   → External factors the project depends on (e.g., client infrastructure).

4. **Record Logic (Assertions)**  
   → Create links between Expectations and Evidence, forming a **DAG**.  
   Each link represents a logical deduction (“this evidence supports this expectation”).

5. **Assess Confidence**  
   → Automate metric collection and generate scores.  
   (This can be integrated into CI/CD pipelines.)

---

## 4. Tools: TruDAG

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

🔹 Basic Usage

After installation, you can:

Create Statements (Expectations, Assertions, Evidence, Assumptions);

Define links between them;

Associate artifacts (files, tests, outputs);

Generate Confidence Scores and reports.

🧩 Each Statement and its links are recorded in the Git repository, ensuring native traceability.

## 🧮 5. Relation to Traceability Matrix and Other Approaches

TSF replaces (or generalizes) traditional traceability matrices, but in a live and automated form rather than static documents.

Method	Traditional Form	TSF / TruDAG
Traceability Matrix	Manually in Excel or dedicated tools	Represented as a graph (DAG) in Git
Evidence	Linked via documents	Linked via verifiable artifacts and metadata
Updates	Manual, error-prone	Automated via CI/CD
Assessment	Subjective	Quantifiable with “Confidence Scores”
Scalability	Limited	High, with composition across projects

Thus, no traditional matrix is needed—the TSF graph replaces it, and TruDAG is the practical management tool.

| Method                  | Traditional Form                     | TSF / TruDAG                                     |
| ----------------------- | ------------------------------------ | ------------------------------------------------ |
| **Traceability Matrix** | Manually in Excel or dedicated tools | Represented as a **graph (DAG)** in Git          |
| **Evidence**            | Linked via documents                 | Linked via **verifiable artifacts and metadata** |
| **Updates**             | Manual, error-prone                  | **Automated** via CI/CD                          |
| **Assessment**          | Subjective                           | **Quantifiable** with “Confidence Scores”        |
| **Scalability**         | Limited                              | **High**, with composition across projects       |



## ⚙️ 6. Simplified Implementation Example
### 1️⃣ What is a Graph (in TSF context)

A graph is a set of nodes connected by edges:

Each node is a Statement (a claim about the software).

Each edge is a logical link, meaning “this leads to that” or “this depends on that”.

TSF uses a DAG – Directed Acyclic Graph:

Directed → edges have direction (A supports B).

Acyclic → no cycles allowed (A cannot depend on itself indirectly).

💡 Simple analogy:
Imagine a family tree: each person (Statement) is linked to parents/children. No one can be their own ancestor → no cycles.

### 2️⃣ Types of Statements
Type	Description	Example
Expectation	What the software should achieve (defined by stakeholders)	“System responds under 200ms”
Assertion	Logical link between Expectations and Evidence	“Performance tests are automated”
Evidence (Premise)	Concrete proof supporting an Assertion	“Automated tests show avg 180ms”
Assumption	External factor presumed true	“System runs on Linux”

Links in the graph:

Expectation → supported by Assertions

Assertions → supported by Evidence

Assumptions → linked as external conditions

### 3️⃣ Building a TSF Graph Practically

Define what you want to prove (Expectation).
Example: “Software XYZ is safe”

Create Statements to explain the path:

Assertion: “Code passed automated security tests”

Evidence: “Test logs show 0 failures”

Link Statements in the graph:

Assertion → linked to Expectation

Evidence → linked to Assertion

TruDAG manages this process.
Each Statement and link is recorded in Git.
TruDAG can then calculate a confidence score: how trustworthy is this Expectation based on available Evidence.

### 4️⃣ Simple Visual Representation
Expectation: Software XYZ is safe
        |
     Assertion: Automated security tests passed
        |
     Evidence: CI/CD logs show 0 failures
        |
   Assumption: Runs on Linux


Each level is a layer of the graph.

If something changes (e.g., a test fails), TruDAG marks the Statement as Suspect, signaling a review is needed.

### 5️⃣ Practical Implementation with TruDAG

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

### 🧱 Step 3: Create Statements
```bash
trudag manage create-item "Software XYZ is safe" ./XYZ/ --type Expectation
trudag manage create-item "Automated security tests passed" ./XYZ/ --type Assertion
trudag manage create-item "CI/CD logs show 0 failures" ./XYZ/ --type Evidence
trudag manage create-item "Runs on Linux" ./XYZ/ --type Assumption
```

### 🔗 Step 4: Link Statements
```bash
trudag manage create-link "Automated security tests passed" "Software XYZ is safe"
trudag manage create-link "CI/CD logs show 0 failures" "Automated security tests passed"
trudag manage create-link "Runs on Linux" "Automated security tests passed"
```

### 🧩 Step 5: Evaluate Confidence
```bash
trudag score
```

TruDAG traverses the graph, checks all links and evidence, and calculates how much we can trust the Expectation.

### 🔑 Step 6: Key Takeaways

Not a static Excel matrix — it is a graph of Statements in Git.

Each Statement is traceable and linked to concrete evidence.

TruDAG automates creation, linking, and analysis.

Confidence is calculated automatically, but human review remains essential.

➡️ Outcome: a report with confidence scores, evidence, and traceable logical dependencies.

## 🧭 7. Conclusion

TSF provides a modern, formal approach to assess software trustability, replacing manual documents and matrices with a declarative, traceable structure integrated into the development workflow.

### ✅ Practical steps:

- Use TruDAG (official Python tool)

- Model the project as a graph of Statements

- Link concrete evidence and artifacts

- Automate analysis via CI/CD

- Discard traditional spreadsheets and traceability tools

## 📚 References

- Eclipse TSF Project Page: https://projects.eclipse.org/projects/technology.tsf

- Codethink TSF GitLab: https://gitlab.com/CodethinkLabs/trustable/trustable

- TruDAG Methodology & Documentation: https://codethinklabs.gitlab.io/trustable/trustable/index.html

- TruDAG Installation Guide: https://codethinklabs.gitlab.io/trustable/trustable/trudag/install.html#user

- TSF Methodology Overview: https://codethinklabs.gitlab.io/trustable/trustable/methodology.html



## 💡 “Any consideration of trust must be based on evidence.”
— Trustable Software Framework
