# TSF Automation Scripts — Technical Documentation

This document explains the automation scripts used for TSF management in the SEA:ME Team 6 project.

**Last Updated:** February 2026

---

## Table of Contents

1. [Overview](#overview)
2. [Main Automation Script](#main-automation-script)
3. [TruDAG Setup Script](#trudag-setup-script)
4. [Configuration](#configuration)
5. [Module Reference](#module-reference)
6. [Workflow Diagram](#workflow-diagram)

---

## Overview

### Directory Structure

```
docs/TSF/tsf_implementation/
├── scripts/
│   ├── open_check_sync_update_validate_run_publish_tsfrequirements.py  # Main script
│   ├── setup_trudag_clean.sh          # TruDAG operations
│   ├── config.yaml                     # Configuration
│   └── modules/
│       ├── __init__.py
│       ├── validate_items_formatation.py  # YAML validation
│       └── ...
├── items/                              # Source TSF items (editable)
│   ├── assertions/
│   ├── assumptions/
│   ├── evidences/
│   └── expectations/
├── .trudag_items/                      # Generated items (do not edit)
├── graph/
│   └── graph.dot                       # Dependency graph
└── tools/
    └── generate_graph_from_heuristics.py
```

---

## Main Automation Script

**File:** `open_check_sync_update_validate_run_publish_tsfrequirements.py`

### Purpose

Unified script that handles the complete TSF workflow:
- Parsing requirements table
- Detecting new/removed requirements
- Syncing evidence from sprints
- Generating TSF items
- Running TruDAG validation
- Publishing reports

### Usage

```bash
# Always activate virtual environment first
source .venv/bin/activate

# Check current state (read-only)
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check

# Sync and update items
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --sync

# Validate and publish
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate

# Run all stages
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --all
```

### Command-Line Options

| Option | Description |
|--------|-------------|
| `--check` | Verify table completeness, detect orphans, validate content |
| `--sync` | Generate missing items, update content with AI |
| `--validate` | Run TruDAG setup, scoring, and publish |
| `--all` | Run all stages in sequence |

### Key Classes

#### `Config`
Loads and manages configuration from `config.yaml`:
- Path resolution
- AI method settings
- Evidence patterns

#### `TableRow`
Represents a row in the requirements table:
- ID, requirement, acceptance criteria, verification method, evidence
- Methods for completeness checking

#### `EvidenceLink`
Represents an evidence link from sprint files:
- EXPECT ID, description, URL, source file

#### `EvidenceParser`
Parses sprint files to extract evidence:
- Handles various markdown formats
- Extracts URLs and file paths

#### `TableParser`
Parses the requirements table:
- Markdown table parsing
- Row extraction

#### `ItemFileManager`
Manages TSF item files:
- Finds existing items
- Creates new items
- Detects orphans
- Removes orphan files (with confirmation)

### Three Stages

#### Stage 1: open_check()
```python
def open_check(config: Config) -> Dict[str, Any]:
    """
    1. Parse requirements table
    2. Check for incomplete fields
    3. Parse sprint files for evidence
    4. Check item files existence
    5. Fix structural issues
    6. Validate content
    7. Detect orphan files
    8. Identify sync needs
    """
```

#### Stage 2: sync_update()
```python
def sync_update(config: Config, check_results: Dict) -> Dict[str, Any]:
    """
    1. Generate missing item files
    2. Update EXPECT headers from table
    3. Update EVID references from sprints
    4. AI content generation (if enabled)
    5. Remove orphan files (with confirmation)
    """
```

#### Stage 3: validate_run_publish()
```python
def validate_run_publish(config: Config) -> Dict[str, Any]:
    """
    1. Check required symlinks
    2. Run item validation
    3. Execute TruDAG (setup_trudag_clean.sh)
    4. Verify scores
    """
```

---

## TruDAG Setup Script

**File:** `setup_trudag_clean.sh`

### Purpose

The canonical script for all TruDAG operations. Called by the main Python script during `--validate`.

### Steps Performed

```bash
# Step 0: Clean generated files (preserve items/ source)
rm -f graph/graph.dot
rm -f .dotstop.dot
rm -rf .trudag_items/

# Step 1: Generate graph.dot
python3 tools/generate_graph_from_heuristics.py --items items/ --out graph/graph.dot

# Step 2: Initialize TruDAG database
trudag init

# Step 3: Create items from source
# (copies items/ to .trudag_items/ with transformations)

# Step 4: Fix file reference paths and IDs

# Step 5: Create logical links from graph.dot
trudag manage link ITEM1 ITEM2

# Step 6: Mark items as reviewed
trudag manage set-item ITEM_ID --links

# Step 7: Run lint
trudag lint

# Step 8: Calculate scores
trudag score --validate

# Step 9: Publish reports
trudag publish --output-dir ../../../docs/doorstop --validate --all-bodies
```

### Usage

```bash
# Run directly (from tsf_implementation directory)
cd docs/TSF/tsf_implementation
bash scripts/setup_trudag_clean.sh

# Or via the main script
source .venv/bin/activate && python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate
```

---

## Configuration

**File:** `config.yaml`

### Structure

```yaml
# Paths Configuration
paths:
  repo_root: "/Volumes/Important_Docs/42/SEA-ME_Team6_2025-26"
  tsf_implementation: "${paths.repo_root}/docs/TSF/tsf_implementation"
  items_dir: "${paths.tsf_implementation}/items"
  scripts_dir: "${paths.tsf_implementation}/scripts"
  sprints_dir: "${paths.repo_root}/docs/sprints"
  requirements_table: "${paths.repo_root}/docs/TSF/requirements/tsf-requirements-table.md"

# AI Configuration
ai:
  primary_method: "manual"  # Option G: VSCode/Claude
  fallbacks: ["copilot_cli"]  # Option C: gh copilot CLI
  
  manual:
    open_in_vscode: true
    show_prompt_suggestion: true
    wait_for_user_confirmation: true
    
  copilot:
    timeout: 30

# Evidence Sync Configuration
evidence_sync:
  sprint_files:
    - "sprint1.md"
    - "sprint2.md"
    - "sprint3.md"
    - "sprint4.md"
    - "sprint5.md"
    - "sprint6.md"
    - "sprint7.md"
  evidence_patterns:
    image: '!\[([^\]]*)\]\(([^)]+)\)'
    link: '\[([^\]]+)\]\(([^)]+)\)'
```

---

## Module Reference

### validate_items_formatation.py

Validates YAML structure in TSF item files:

```python
def validate_front_matter(file_path: Path) -> List[str]:
    """Validate YAML front-matter structure."""
    
def validate_references(file_path: Path) -> List[str]:
    """Validate reference paths exist."""
    
def run_validation(items_dir: Path) -> Tuple[bool, List[str]]:
    """Run complete validation on all items."""
```

### generate_graph_from_heuristics.py

Generates the dependency graph:

```python
def generate_graph(items_dir: Path) -> str:
    """Generate DOT graph from item relationships."""
    
def write_graph(graph: str, output_path: Path):
    """Write graph to file."""
```

---

## Workflow Diagram

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    TSF AUTOMATION WORKFLOW                              │
└─────────────────────────────────────────────────────────────────────────┘

┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   SPRINTS       │    │  REQUIREMENTS   │    │    ITEMS        │
│   (source)      │    │     TABLE       │    │   (source)      │
│                 │    │   (source)      │    │                 │
│ sprint1.md      │    │ tsf-require-    │    │ EXPECT-L0-X.md  │
│ sprint2.md      │    │ ments-table.md  │    │ ASSERT-L0-X.md  │
│ ...             │    │                 │    │ EVID-L0-X.md    │
│ sprint7.md      │    │                 │    │ ASSUMP-L0-X.md  │
└────────┬────────┘    └────────┬────────┘    └────────┬────────┘
         │                      │                      │
         │                      ▼                      │
         │            ┌─────────────────┐              │
         └──────────► │   --check       │ ◄────────────┘
                      │                 │
                      │ Parse table     │
                      │ Extract evidence│
                      │ Validate items  │
                      │ Detect orphans  │
                      └────────┬────────┘
                               │
                               ▼
                      ┌─────────────────┐
                      │   --sync        │
                      │                 │
                      │ Generate items  │
                      │ Update content  │
                      │ AI generation   │
                      │ Remove orphans  │
                      └────────┬────────┘
                               │
                               ▼
                      ┌─────────────────┐
                      │  --validate     │
                      │                 │
                      │ setup_trudag_   │
                      │ clean.sh        │
                      │                 │
                      │ • Generate graph│
                      │ • Init DB       │
                      │ • Create items  │
                      │ • Apply links   │
                      │ • Run lint      │
                      │ • Score         │
                      │ • Publish       │
                      └────────┬────────┘
                               │
                               ▼
                      ┌─────────────────┐
                      │    OUTPUTS      │
                      │                 │
                      │ .trudag_items/  │
                      │ graph/graph.dot │
                      │ .dotstop.dot    │
                      │ docs/doorstop/  │
                      │  ├── ASSERTIONS │
                      │  ├── ASSUMPTIONS│
                      │  ├── EVIDENCES  │
                      │  ├── EXPECTATIONS│
                      │  └── dashboard  │
                      └─────────────────┘
```

---

## Best Practices

### Before Making Changes

1. Always run `--check` first to see current state
2. Backup items if making significant changes
3. Review orphan files before deletion

### Adding New Requirements

1. Add row to `tsf-requirements-table.md`
2. Run `--sync` to generate item files
3. Edit generated items to add content
4. Run `--validate` to verify

### Removing Requirements

1. Remove row from `tsf-requirements-table.md`
2. Run `--check` to detect orphan files
3. Run `--sync` to remove orphans (with confirmation)

### Updating Evidence

1. Add evidence links to sprint files (use `<!-- EXPECT-L0-X -->` comments)
2. Run `--check` to see extracted evidence
3. Run `--sync` to update EVID files

---

## Deprecated Scripts

The following scripts are deprecated and kept only for historical reference:

- `sync_tsf_manager.py` - Replaced by unified script
- `sync_tsf_requirements_table.py` - Replaced by unified script
- `trudag_runner.py` - Replaced by `setup_trudag_clean.sh`

**Always use the unified script:**
```bash
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py
```
