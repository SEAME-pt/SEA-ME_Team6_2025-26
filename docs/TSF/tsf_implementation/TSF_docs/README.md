# 📚 TSF Documentation Index

Welcome to the TSF (Trustable Software Framework) documentation for SEA:ME Team 6.

**Last Updated:** February 2026

---

## 📖 Reading Order

Follow this order to understand the TSF implementation from concepts to practice:

| # | Document | Description | Time |
|---|----------|-------------|------|
| 1️⃣ | [**WhatsTSF.md**](WhatsTSF.md) | Introduction to TSF concepts, TruDAG, and our current status | ~10 min |
| 2️⃣ | [**genAI_on_tsf.md**](genAI_on_tsf.md) | How we use GenAI (Claude/Copilot) in TSF automation | ~8 min |
| 3️⃣ | [**automatization_scripts_explanation.md**](automatization_scripts_explanation.md) | Detailed explanation of the automation scripts | ~12 min |
| 4️⃣ | [**VALIDATORS_GUIDE.md**](VALIDATORS_GUIDE.md) | CI validators vs TruDAG validators guide | ~8 min |
| 5️⃣ | [**TSF_IMPLEMENTATION.md**](TSF_IMPLEMENTATION.md) | Complete implementation guide and quick reference | ~15 min |

---

## 🎯 Quick Start

If you just want to run the TSF system:

```bash
# Activate environment
source .venv/bin/activate

# Check status
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check

# Full validation with TruDAG
./docs/TSF/tsf_implementation/scripts/setup_trudag_clean.sh
```

---

## 📊 Current Status

- **Requirements:** 30 (L0-1 to L0-30)
- **Total Items:** 116 (4 types × 30 - 4 orphans removed)
- **TruDAG Score:** 86/116 items at 1.0

---

## 🗂️ Document Summaries

### 1️⃣ WhatsTSF.md
> **Start here!** Learn what TSF is, the four statement types (Expectations, Assertions, Evidences, Assumptions), how TruDAG works, and the current state of our implementation.

### 2️⃣ genAI_on_tsf.md
> Understand how we integrated GenAI into TSF workflows. Covers the options we evaluated (API, CLI, local LLM) and why we chose VSCode/Claude as primary with gh copilot CLI as fallback.

### 3️⃣ automatization_scripts_explanation.md
> Deep dive into the main script `open_check_sync_update_validate_run_publish_tsfrequirements.py`. Covers all modules, workflow, configuration, and the evidence synchronization system.

### 4️⃣ VALIDATORS_GUIDE.md
> Explains the difference between CI validators (GitHub Actions) and TruDAG validators. Covers the universal validators (hardware, linux, software) and how to create new ones.

### 5️⃣ TSF_IMPLEMENTATION.md
> The complete reference guide. Quick commands, project structure, symlinks system, troubleshooting, and everything you need to work with TSF day-to-day.

---

## 🔗 Related Resources

- **Scripts:** `docs/TSF/tsf_implementation/scripts/`
- **Items:** `docs/TSF/tsf_implementation/items/`
- **TruDAG Output:** `docs/doorstop/`
- **Config:** `docs/TSF/tsf_implementation/scripts/config.yaml`

---

## ❓ Need Help?

1. Check [TSF_IMPLEMENTATION.md](TSF_IMPLEMENTATION.md) troubleshooting section
2. Run `--check` to see current status
3. Review TruDAG logs in `docs/TSF/tsf_implementation/logs/`
