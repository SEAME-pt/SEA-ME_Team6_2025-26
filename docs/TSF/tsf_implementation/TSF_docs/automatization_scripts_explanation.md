docs/TSF/tsf_implementation/

# TSF Automation Scripts — Technical Documentation

## 🚦 Automated Process (2025/26)

**Central script:** `sync_tsf_requirements_table.py` (Layer 1 Manager)

docs/TSF/tsf_implementation/
├── scripts/
│   ├── sync_tsf_manager.py          # 🎯 Main orchestrator script
│   ├── modules/
│   │   ├── __init__.py
│   │   ├── detectors.py             # Detects changes (LO_requirements + sprints)
│   │   ├── generators.py            # Creates EXPECT/ASSERT/EVID/ASSUMP
│   │   ├── sync_evidence.py         # Syncs sprint → EVID
│   │   ├── ai_generator.py          # 3 AI methods (manual/ollama/api)
│   │   ├── validators.py            # Wrapper for validate_items_formatation.py
│   │   └── trudag_runner.py         # DEPRECATED: Use setup_trudag_clean.sh instead
│   └── config.yaml                  # Configuration
├── backups/
│   └── items_backup1.tar.gz         # Numbered backups
└── validators/
    └── validate_items_formatation.py  # Already exists ✅

    PHASE 1 (first 40 min):

✅ detectors.py - Detects changes
✅ ai_generator.py - 3 AI methods
✅ generators.py - Creates all 4 files (EXPECT/ASSERT/EVID/ASSUMP)
✅ sync_tsf_manager.py - Main orchestrator
✅ config.yaml - Configuration

```
┌─────────────────────────────────────────────────────────────┐
│ sync_tsf_requirements_table.py (Layer 1 Manager)           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│ 1. DETECT changes in the table:                            │
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

**Note:** AI generation only occurs if an external API (OpenAI, etc.) or manual Copilot Chat input is configured. Otherwise, Acceptance/Verification fields remain empty and the script generates a pending report.

**Sync is unidirectional:** from the table to items/ and evidences. Never the other way around.

---

## 🚦 Official Pipeline for Layer 3 (.trudag_items/)

**The only supported and canonical pipeline for generating and validating `.trudag_items/` is:**

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

---


**Project:** SEA-ME Team 6 2025-26  
**Purpose:** Automated generation and synchronization of TSF items  
**Created:** December 15, 2025  
**Last Updated:** December 17, 2025

---

## Overview

This directory contains a modular automation system that detects new requirements in `tsf-requirements-table.md`, generates corresponding TSF items (EXPECT/ASSERT/EVID/ASSUMP), synchronizes evidence from sprint files, and validates all items using TruDAG.

---

## 🏗️ Three-Layer Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│ LAYER 1: tsf-requirements-table.md (PRIMARY SOURCE OF TRUTH)   │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ├─── Column "ID" + "Requirement"                              │
│  │    └─→ MANUAL (human writes)                                 │
│  │                                                             │
│  ├─── Column "Acceptance Criteria" + "Verification Method"     │
│  │    └─→ AI GENERATED (automatically when new Requirement)     │
│  │                                                             │
│  └─── Column "Evidence"                                        │
│       └─→ AUTOMATIC (scan of sprints/*.md)                      │
│           • detectors.py::scan_sprint_evidence()                │
│           • Extracts links from EXPECT-L0-X                      │
│           • Adds to the table automatically                      │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────────┐
│ LAYER 2: items/ (SECONDARY SOURCE OF TRUTH)                    │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  • generator_items_second_source_of_truth.py generates/updates  │
│  • Structure: expectations/ assertions/ evidences/ assumptions/ │
│  • Format: EXPECT-L0-X.md, ASSERT-L0-X.md, etc.                │
│  • ⚠️  NEVER edit manually!                                    │
│  • ✅ CRUD operations: Create, Update, Delete, Sync             │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────────┐
│ LAYER 3: .trudag_items/ (TRUDAG DATABASE)                      │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  • Regenerable with setup_trudag_clean.sh                       │
│  • Internal TruDAG format (EXPECTATIONS/EXPECT_L0_X/)           │
│  • Auto-generated: graph.dot, .dotstop.dot                      │
│  • Commands: trudag manage, lint, score, publish, plot          │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

**Key Principles:**
1. **Single Source of Truth:** `tsf-requirements-table.md` is the PRIMARY source
2. **Automatic Propagation:** Changes flow down automatically (Layer 1 → 2 → 3)
3. **Never Edit Manually:** `items/` and `.trudag_items/` are auto-generated
4. **Unidirectional Evidence Sync:** Evidence flows from the table to items/ and evidences (never the other way around)

---

## Workflow

1. **Detect** → New requirements or empty Acceptance/Verification (sync_tsf_requirements_table.py)
2. **Generate** → Generates Acceptance/Verification with AI (if configured)
3. **Sync** → Updates Evidence column by reading evidences from items/evidences/
4. **Validate** → Format validation + TruDAG score
5. **Backup** → Sequential backups before modifying

---

## Scripts Documentation

### 1. `modules/detectors.py`

**Status:** ✅ Complete (400 lines)  
**Purpose:** Detection and evidence extraction engine

## Scripts

### `sync_tsf_requirements_table.py` (new)
Unified script that:
1. Detects changes in the table (new requirements, empty Acceptance/Verification)
2. Generates Acceptance/Verification with AI (if configured)
3. Synchronizes evidences by reading items/evidences/
4. Updates the table preserving formatting

### Deprecated scripts
- `generator_items_second_source_of_truth.py` (removed)
- `detectors.py` (removed)
- `validate_items_formatation.py` (removed)

### How to use

```bash
# Run full synchronization:
python3 docs/TSF/tsf_implementation/scripts/sync_tsf_requirements_table.py
```

---

## Frequently Asked Questions

**How does AI generation work?**
It only works if you configure an external API (OpenAI, etc.) or use Copilot Chat/manual. Otherwise, Acceptance/Verification fields remain empty and the script generates a pending report.

**Is the sync bidirectional?**
No. Sync is always from the table to items/ and evidences.

---

**Last update:** December 17, 2025

---

**Branch:** feature/TSF/automatize-tsf-in-github  
**Related PR:** feature/TSF/integrate-tsf-in-github → development
