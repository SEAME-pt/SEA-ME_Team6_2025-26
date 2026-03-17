# 📚 TSF Documentation Index

Welcome to the TSF (Trustable Software Framework) documentation for SEA:ME Team 6.

**Last Updated:** February 2026

---

## 📖 Reading Order

Follow this order to understand the TSF implementation from concepts to practice:

| # | Document | Description | Time |
|---|----------|-------------|------|
| 1 - | [**WhatsTSF.md**](WhatsTSF.md) | Introduction to TSF concepts, TruDAG, and our current status | ~10 min |
| 2 - | [**TSF_IMPLEMENTATION.md**](TSF_IMPLEMENTATION.md) | Complete implementation guide and quick reference | ~15 min |
| 3 - | [**automatization_scripts_explanation.md**](automatization_scripts_explanation.md) | Detailed explanation of the automation scripts | ~12 min |
| 4 - | [**how_we_are_using_genAI_on_tsf.md**](how_we_are_using_genAI_on_tsf.md) | How we are using GenAI in TSF automation workflows | ~8 min |
| 5 - | [**VALIDATORS_GUIDE.md**](VALIDATORS_GUIDE.md) | CI validators vs TruDAG validators guide | ~8 min |
| 6 - | [**TSF_SCORE_RECOVERY_MAR2026.md**](TSF_SCORE_RECOVERY_MAR2026.md) | March 2026 score recovery and technical fixes log | ~6 min |

---

## 📑 Index

1. [Reading Order](#-reading-order)
2. [Script Execution Commands](#-script-execution-commands)
3. [Current Status](#-current-status)
4. [Document Summaries](#-document-summaries)

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

## 🔧 Script Execution Commands

Short form (from repo root, after activating venv):

```bash
source .venv/bin/activate

# Check item formatting and structure
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check

# Sync evidence extracted from sprint markers
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --sync

# Run TruDAG validate/score/publish pipeline
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate

# End-to-end execution
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --all
```

Full form (from anywhere, includes cd + venv activation):

```bash
# Check item formatting and structure
cd /home/seame/Documents/SEA-ME_Team6_2025-26 && source /home/seame/Documents/SEA-ME_Team6_2025-26/.venv/bin/activate && python3 /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check

# Sync evidence extracted from sprint markers
cd /home/seame/Documents/SEA-ME_Team6_2025-26 && source /home/seame/Documents/SEA-ME_Team6_2025-26/.venv/bin/activate && python3 /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --sync

# Run TruDAG validate/score/publish pipeline
cd /home/seame/Documents/SEA-ME_Team6_2025-26 && source /home/seame/Documents/SEA-ME_Team6_2025-26/.venv/bin/activate && python3 /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate

# End-to-end execution
cd /home/seame/Documents/SEA-ME_Team6_2025-26 && source /home/seame/Documents/SEA-ME_Team6_2025-26/.venv/bin/activate && python3 /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --all
```

---

## 📊 Current Status

- **Requirements:** 30 (L0-1 to L0-30)
- **Total Items:** 116 (4 types × 30 - 4 orphans removed)
- **TruDAG Score:** 86/116 items at 1.0

---

## 🗂️ Document Summaries

### 1 - WhatsTSF.md
> **Start here!** Learn what TSF is, the four statement types (Expectations, Assertions, Evidences, Assumptions), how TruDAG works, and the current state of our implementation.

### 2 - TSF_IMPLEMENTATION.md
> The complete reference guide. Quick commands, project structure, symlinks system, troubleshooting, and everything you need to work with TSF day-to-day.

### 3 - automatization_scripts_explanation.md
> Deep dive into the main script `open_check_sync_update_validate_run_publish_tsfrequirements.py`. Covers all modules, workflow, configuration, and the evidence synchronization system.

### 4 - how_we_are_using_genAI_on_tsf.md
> Understand how we integrated GenAI into TSF workflows. Covers the options we evaluated (API, CLI, local LLM) and why we chose VSCode/Claude as primary with gh copilot CLI as fallback.

### 5 - VALIDATORS_GUIDE.md
> Explains the difference between CI validators (GitHub Actions) and TruDAG validators. Covers the universal validators (hardware, linux, software) and how to create new ones.

### 6 - TSF_SCORE_RECOVERY_MAR2026.md
> Technical record of the March 2026 recovery work, including validator/path fixes, graph/link corrections, evidence normalization, and score restoration steps.

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

---

## Latest Technical Update (Mar 2026)

Post-debug fixes documented in this folder include:

- symlink cleanup robustness (`-e` and `-L`) in setup script
- dynamic `TSF_IMPL` path usage (removed hardcoded `/Volumes/...` path)
- validator signature alignment with TruDAG (`yaml` imported from core validator)
- `validate_software_dependencies` made retrocompatible with `components` key
- score recovery fixes for graph links, deterministic URL-reference SHA, and L0_22-L0_31 evidence normalization

Current documented outcome: score recovered to `124/124`.

Detailed recovery log:
- [TSF_SCORE_RECOVERY_MAR2026.md](TSF_SCORE_RECOVERY_MAR2026.md)
