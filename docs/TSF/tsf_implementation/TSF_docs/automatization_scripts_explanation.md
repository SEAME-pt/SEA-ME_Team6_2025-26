# TSF Automation Scripts - Technical Documentation

**Project:** SEA-ME Team 6 2025-26  
**Purpose:** Automated generation and synchronization of TSF items  
**Created:** December 15, 2025  
**Last Updated:** December 15, 2025

---

## Overview

This directory contains a modular automation system that detects new requirements in `LO_requirements.md`, generates corresponding TSF items (EXPECT/ASSERT/EVID/ASSUMP), synchronizes evidence from sprint files, and validates all items using TruDAG.

**Workflow:**
1. **Detect** → New requirements without TSF items
2. **Generate** → Create missing TSF items (template-based)
3. **Sync** → Update evidence from sprints and table
4. **Validate** → Run formatation validator + TruDAG score
5. **Backup** → Sequential backups before modifications

---

## Scripts Documentation

### 1. `modules/detectors.py`

**Status:** ✅ Complete (400 lines)  
**Purpose:** Detection and evidence extraction engine

#### Implemented Functions:

| Function | Description | Returns |
|----------|-------------|---------|
| `parse_lo_requirements_table()` | Parse markdown table from LO_requirements.md | List of requirement dicts (id, requirement, acceptance, verification, evidence) |
| `get_existing_tsf_items()` | Scan TSF directories for existing items | Dict mapping item type → list of existing IDs |
| `detect_new_items()` | Detect requirements without TSF items | Dict mapping item type → list of missing requirement dicts |
| `extract_evidence_from_table()` | Extract evidence from Evidence column | List of evidence refs (type, path) for specific req_id |
| `scan_sprint_evidence()` | Scan sprint*.md for EXPECT references | Dict mapping req_id → list of evidence refs (type, path, sprint) |
| `get_complete_evidence()` | Combine table + sprint evidence | Tuple of (table_evidence, sprint_evidence) |
| `main()` | Test all detector functions | Prints comprehensive test results |

#### Configuration:

```python
PROJECT_ROOT = Path(__file__).resolve().parents[5]
LO_REQUIREMENTS_PATH = docs/TSF/requirements/LO_requirements.md
TSF_ITEMS_BASE = docs/TSF/tsf_implementation/
SPRINTS_DIR = docs/sprints/

ITEM_TYPES = ['expectations', 'assertions', 'evidences', 'assumptions']
ITEM_PREFIXES = {'expectations': 'EXPECT', 'assertions': 'ASSERT', ...}
```

#### Evidence Extraction Patterns:

**Pattern 1:** `path` — URL  
Example: `` `docs/demos/car.jpeg` — https://github.com/... ``
- Extracts both file path and URL as separate references

**Pattern 2:** Standalone URLs  
Example: `https://github.com/SEAME-pt/...`
- Extracts any HTTP/HTTPS link

**Pattern 3:** File paths in backticks  
Example: `` `docs/guides/github-guidelines.md` ``
- Extracts paths to .md, .jpg, .jpeg, .png, .pdf, .py, .sh files

#### Sprint Evidence Scanning:

- Searches for `EXPECT-L0-X` references in sprint*.md files
- Extracts ±200 characters context around each reference
- Finds URLs and file paths within context
- Tags evidence with source sprint name

#### Test Command:

```bash
python3 docs/TSF/tsf_implementation/scripts/modules/detectors.py
```

**Test Output:**
1. ✅ Parse requirements table (count + first/last items)
2. ✅ Scan existing TSF items (count per type)
3. ✅ Detect missing items (count + examples)
4. ✅ Extract table evidence for L0-1
5. ✅ Scan sprint evidence (all requirements)
6. ✅ Complete evidence for L0-1 (table + sprint)

---

### 2. `modules/generators.py`

**Status:** 🔄 In Progress  
**Purpose:** Template-based TSF item generation

#### Planned Functions:

- `generate_expectation()` - Create EXPECT-L0-X.md files
- `generate_assertion()` - Create ASSERT-L0-X.md files
- `generate_evidence()` - Create EVID-L0-X.md files
- `generate_assumption()` - Create ASSUMP-L0-X.md files

#### Template Strategy:

- Intelligent keyword detection from requirement text
- Rule-based content generation (not Gen AI)
- Proper YAML front-matter formatting
- Reference linking between items

---

### 3. `sync_tsf_manager.py`

**Status:** ⏸️ Pending  
**Purpose:** Main orchestrator script

#### Planned Features:

- Coordinate detectors → generators → validators → TruDAG
- Sequential backup system (items_backup1.tar.gz, items_backup2.tar.gz, ...)
- Git safety branch creation
- Automated workflow execution
- Error handling and rollback

---

### 4. Additional Modules (Future)

#### `modules/sync_evidence.py`
- Bidirectional evidence synchronization
- Update Evidence column in LO_requirements.md
- Update references in EVID-L0-X.md files

#### `modules/validators.py`
- Wrapper for validate_items_formatation.py
- YAML syntax validation
- Reference integrity checks

#### `modules/trudag_runner.py`
- Wrapper for setup_trudag_clean.sh
- TruDAG score execution
- Results parsing and reporting

---

## Usage Workflow

### Manual Testing (Current):

```bash
# Test detectors
python3 docs/TSF/tsf_implementation/scripts/modules/detectors.py

# Test generators (when implemented)
python3 docs/TSF/tsf_implementation/scripts/modules/generators.py --test

# Run full automation (when orchestrator ready)
python3 docs/TSF/tsf_implementation/scripts/sync_tsf_manager.py
```

### Automated CI/CD (Future):

Triggered by:
- Push to development/main (if LO_requirements.md or sprints/*.md changed)
- Manual workflow dispatch

Action:
- **On push:** Auto-generate and commit
- **On PR:** Validation only (no commits)

---

## Development Status

| Component | Status | Lines | Completion |
|-----------|--------|-------|------------|
| `detectors.py` | ✅ Complete | 400 | 100% |
| `generators.py` | 🔄 In Progress | - | 0% |
| `sync_tsf_manager.py` | ⏸️ Pending | - | 0% |
| `sync_evidence.py` | ⏸️ Pending | - | 0% |
| `validators.py` | ⏸️ Pending | - | 0% |
| `trudag_runner.py` | ⏸️ Pending | - | 0% |
| GitHub Actions workflow | ⏸️ Pending | - | 0% |

**Overall Progress:** 1/7 components (14%)

---

## Notes

- All scripts use **template-based generation** (not Gen AI API calls)
- Full automation: NO interactive prompts during execution
- Review happens AFTER generation completes
- Backup system ensures safety (rollback capability)
- Step-by-step development with validation between phases

---

**Last Updated:** December 15, 2025  
**Branch:** feature/TSF/automatize-tsf-in-github  
**Related PR:** feature/TSF/integrate-tsf-in-github → development
