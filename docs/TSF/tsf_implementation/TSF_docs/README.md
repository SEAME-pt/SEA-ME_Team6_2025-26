# 📚 TSF Documentation Index

Welcome to the TSF (Trustable Software Framework) documentation for SEA:ME Team 6.

**Last Updated:** April 2026

## April 2026 Update

- Fixed TruDAG plugin warning noise caused by `localplugins.FileReference` name collision with built-in references.
- Clarified that `exit 130/143` seen in some runs were interruption signals (`SIGINT`/`SIGTERM`), not TSF structural failures.
- Improved `--check` semantic placeholder detection for ASSUMP validator configuration.
- Current expected pending placeholders are cleared for the manual retest.

---

## 📖 Reading Order

Follow this order to understand the TSF implementation from concepts to practice:

| # | Document | Description | Time |
|---|----------|-------------|------|
| 1 - | [**WhatsTSF.md**](WhatsTSF.md) | Introduction to TSF concepts, TruDAG, and our current status | ~10 min |
| 2 - | [**WhatsTSF_detailed.md**](WhatsTSF_detailed.md) | Deep dive into TSF architecture, methodology, and implementation patterns | ~25 min |
| 3 - | [**TSF_IMPLEMENTATION.md**](TSF_IMPLEMENTATION.md) | Complete implementation guide and quick reference | ~15 min |
| 4 - | [**automatization_scripts_explanation.md**](automatization_scripts_explanation.md) | Detailed explanation of the automation scripts | ~12 min |
| 5 - | [**how_we_are_using_genAI_on_tsf.md**](how_we_are_using_genAI_on_tsf.md) | How we are using GenAI in TSF automation workflows | ~8 min |
| 6 - | [**VALIDATORS_GUIDE.md**](VALIDATORS_GUIDE.md) | CI validators vs TruDAG validators guide | ~8 min |
| 7 - | [**REFERENCES_GUIDE.md**](REFERENCES_GUIDE.md) | Reference modeling rules, placeholder policy, and anti-patterns | ~8 min |
| 8 - | [**Still_TO_DO**](Still_TO_DO) | Open follow-up tasks and next cleanup actions | ~4 min |
| 9 - | [**problems_and_fixes/**](problems_and_fixes/) | Dedicated folder for incident history and fix reports, including the TruDAG link-review order report | ~20 min |
| 10 - | [**old/**](old/) | Archived legacy documentation | ~2 min |

---

## 📑 Index

1. [Reading Order](#-reading-order)
2. [Script Execution Commands](#-script-execution-commands)
3. [Current Status](#-current-status)
4. [Document Summaries](#-document-summaries)
5. [Problem and Resolution Trail](#-problem-and-resolution-trail)

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

Note: historical counters below may differ from latest branch state when new L0 items are added.

- **Requirements:** 30 (L0-1 to L0-30)
- **Total Items:** 116 (4 types × 30 - 4 orphans removed)
- **TruDAG Score:** 86/116 items at 1.0

---

## 🗂️ Document Summaries

### 1 - WhatsTSF.md
> **Start here!** Learn what TSF is, the four statement types (Expectations, Assertions, Evidences, Assumptions), how TruDAG works, and the current state of our implementation.

### 2 - WhatsTSF_detailed.md
> **Deep dive into TSF architecture.** Comprehensive guide covering TSF scaffolding, statement types, the DAG structure, validators, artifacts, remote graphs, implementation patterns, and CI/CD integration with detailed examples and workflows.

### 3 - TSF_IMPLEMENTATION.md
> The complete reference guide. Quick commands, project structure, symlinks system, troubleshooting, and everything you need to work with TSF day-to-day.

### 4 - automatization_scripts_explanation.md
> Deep dive into the main script `open_check_sync_update_validate_run_publish_tsfrequirements.py`. Covers all modules, workflow, configuration, and the evidence synchronization system.

### 5 - how_we_are_using_genAI_on_tsf.md
> Understand how we integrated GenAI into TSF workflows. Covers the options we evaluated (API, CLI, local LLM) and why we chose VSCode/Claude as primary with gh copilot CLI as fallback.

### 6 - VALIDATORS_GUIDE.md
> Explains the difference between CI validators (GitHub Actions) and TruDAG validators. Covers the universal validators (hardware, linux, software) and how to create new ones.

### 7 - REFERENCES_GUIDE.md
> Defines how `references` should be written for EXPECT/ASSERT/EVID/ASSUMP, including placeholder policy and validation checklist.

### 8 - Still_TO_DO
> Short action list for the next cleanup pass, including evidence handling and the ASSUMP validator/model decision.

### 9 - problems_and_fixes/
> Folder containing the incident-resolution reading order:
> - TRUDAG_LINK_REVIEW_ORDER_FIX_REPORT.md
> - PROBLEMS_AND_FIXES_INDEX.md
> - TSF_SCORE_RECOVERY_MAR2026.md
> - TSF_STRUCTURAL_ANALYSIS_REPORT.md
> - L0-32_REFERENCES_FIX_REPORT.md

### 10 - old/
> Archived legacy documentation, including the older GenAI-on-TSF note.

---

## Problem and Resolution Trail

If you are debugging regressions or trying to understand historical failures, read in this order:

1. [problems_and_fixes/README.md](problems_and_fixes/README.md) - unified incident index and fast triage path.
2. [VALIDATORS_GUIDE.md](VALIDATORS_GUIDE.md) - validator behavior expected after fixes.
3. [REFERENCES_GUIDE.md](REFERENCES_GUIDE.md) - reference policy and practical implementation mapping.

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
- [problems_and_fixes/TSF_SCORE_RECOVERY_MAR2026.md](problems_and_fixes/TSF_SCORE_RECOVERY_MAR2026.md)
