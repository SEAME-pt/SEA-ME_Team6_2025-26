

# TSF Implementation — Sea:ME (official format)

This folder contains the implementation of the Trustable Software Framework (TSF) for the Sea:ME project.
All automation, generation, and synchronization processes are documented in:

👉 [`TSF_docs/automatization_scripts_explanation.md`](TSF_docs/automatization_scripts_explanation.md)

---

## 🏗️ Architecture and Process

```
┌─────────────────────────────────────────────────────────────┐
│ sync_tsf_requirements_table.py (Layer 1 Manager)           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│ 1. DETECT changes in the requirements table:               │
│    - New requirements (ID + Requirement filled)            │
│    - Acceptance/Verification empty or incomplete           │
│                                                             │
│ 2. GENERATE with AI (if configured):                       │
│    - Acceptance Criteria (based on Requirement)            │
│    - Verification Method (based on category)               │
│    ⚠️  If LLM not available: leaves empty                  │
│                                                             │
│ 3. SYNC Evidence (automatic):                              │
│    - Reads EVID-L0-X.md from items/evidences/              │
│    - Extracts all references (type: url)                   │
│    - Updates Evidence column in the table                  │
│                                                             │
│ 4. WRITE updated table:                                    │
│    - Preserves markdown formatting                         │
│    - Keeps other columns intact                            │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**Sync is unidirectional:** from the table to items/ and evidences. Never the other way around.

For details, see the main document above.

---

## 🚦 Official Pipeline for Automation

**The canonical and only supported pipeline for TSF item generation, synchronization, and validation is:**

	./scripts/setup_trudag_clean.sh

Use this script for all local and CI/CD (GitHub Actions) automation. It:
- Cleans generated files (preserves items/ as source of truth)
- Generates the dependency graph
- Initializes the trudag database
- Copies and transforms all items from items/ to .trudag_items/ (with deduplication, reference correction, and ID/path fixes)
- Applies logical links
- Marks all items as reviewed
- Runs trudag lint

**Do not use `trudag_runner.py` or any other pipeline script.**
`trudag_runner.py` is deprecated and kept only for historical reference.

Update all documentation and automation to reference only `setup_trudag_clean.sh`.
