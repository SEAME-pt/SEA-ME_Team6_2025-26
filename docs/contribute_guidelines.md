# Contributing Guide – SEA:ME Team 6 (2025/26)

This document explains **how we work** in this repo: branches, commits, PRs, reviews, CI, documentation, and our sprint strategy.

---

## 📚 Index

- [Branching](#branching)
- [Working Flow](#working-flow)
- [TSF Traceability (always in PR)](#tsf-traceability-always-in-pr)
- [Commit Style (Conventional Commits)](#commit-style-conventional-commits)
- [Issues](#issues)
- [Pull Requests](#pull-requests)
- [CI & Automation](#ci--automation)
- [Sprints](#sprints)

---

<a id="branching"></a>
## 🔀 Branching

- **main** → protected and release-ready.
- **dev** → integration branch; all features merge here first.
- **branch number**-<epic>/\<name> → working branches off **dev**.
  - Example:
    - `#12-QtApp-mockup`

---
<a id="working-flow"></a>
## 🧭 Working Flow

```mermaid 
  sequenceDiagram
  participant Dev as Developer
  participant FeatureBranch as feat branch
  participant Deve as dev branch
  participant Main as main
  participant Rev as reviewer

  Dev->>FeatureBranch: Create #number-Epic-name from dev
  Dev->>FeatureBranch: Push commits to feature branch
  FeatureBranch->>Deve: Open PR feature -> dev
  Dev-->>Deve: Run checks (build, test, lint)
  Deve->>Main: Open PR dev -> main
  Dev-->>Deve: Report checks status (pass)
  Rev-->>Main: Approve PR
  Deve->>Main: Merge PR into main
```
<a id="tsf-traceability-always-in-pr"></a>
## 🧱 TSF Traceability (always in PR)

** Still working on this part (will update when me and @jpjpcs finish this part) **

Keep the TSF table updated in the PR body:

| Requirement ID | Description | Status |
|---|---|---|
| REQ-001 | Display speed on screen | ✅ Implemented |
| REQ-002 | Remote control capability | ⚙️ In Progress |

---
<a id="commit-style-conventional-commits"></a>
## 🧠 Commit Style (Conventional Commits)

We should write clear commits, nothing to long.
Keep it simple for clarity.

- `feat(ui): add speedometer widget`
- `fix(rt): correct timer overflow`
- `docs(workflow): add daily meeting explainer`

Small commits are easier to review.

---
<a id="issues"></a>
## 🔃 Issues

->The issues are normally open in the begginning of each sprint -- planning.

->Branches are created from the issue.

->When the task of the issue is done -> go to the issue -> comment /taskly -> fill the comment -> close the issue ✅

<a id="pull-requests"></a>
## 🔃 Pull Requests

Open PRs from `feature/*` → **dev**. Later, open **dev → main** for releases.

**PR requirements**
- Use the repo **PR Template**.
- Clear **Summary**, **Testing steps**, **TSF table**.
- Link Issues/Epics (`Relates to Epic: ...`).

**Reviews**
- 1 approval required for **main**.

**Merging**
- **Merge** into **dev** and **main**.
- Delete merged branches.

---
<a id="ci--automation"></a>
## 🔧 CI & Automation

**(Need to work more on this part, later I will create a document to explain GitHub actions)**

- **GitHub Actions** run on PRs/Issues (build/test/lint, docs helpers).
- **Daily Standup**: Issue form creates an artifact with `YYYY-MM-DD-daily.md` and comments instructions to commit it via normal PR.

---
<a id="sprints"></a>
## 🗺️ Sprints

- Sprint lives in the **Project board** (Sprint view).  
- Use labels to filter and track (examples):  
  `Sprint 1`, `Type: Epic`, `Daily Meeting`, `Blocked`.
- Link tasks to Epics for traceability.
