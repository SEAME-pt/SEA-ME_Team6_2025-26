# TSF Implementation Guide — SEA:ME Team 6

This document describes the TSF (Trustable Software Framework) implementation for the PiRacer Warm-Up project, including the unified automation script, TruDAG integration, and validation workflows.

**Last Updated:** March 2026  
**Authors:** SEA-ME Team 6

---

## Table of Contents

1. [Script execution commands](#1-script-execution-commands)
2. [Prerequisites](#2-prerequisites)
3. [Installation guide](#3-installation-guide)
4. [Post-installation setup](#4-post-installation-setup)
5. [Project structure](#5-project-structure)
6. [Quick start commands](#6-quick-start-commands)
7. [Main automation script](#7-main-automation-script)
8. [Workflow stages](#8-workflow-stages)
9. [TruDAG integration](#9-trudag-integration)
10. [Validators](#10-validators)
11. [Symlinks explained](#11-symlinks-explained)
12. [CI/CD integration](#12-cicd-integration)
13. [Published reports](#13-published-reports)
14. [Troubleshooting](#14-troubleshooting)
15. [Post 0/124 fixes (Mar 2026)](#15-post-0124-fixes-mar-2026)
16. [Current status (March 2026)](#16-current-status-march-2026)
17. [Quick reference](#17-quick-reference)

---

## 1. Script Execution Commands

From repository root.

Short form (from repo root, after activating venv):

```bash
source .venv/bin/activate

# 1) Lint/structure check only
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check

# 2) Evidence synchronization only
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --sync

# 3) TruDAG validate/score/publish
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate

# 4) Full pipeline in sequence
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --all
```

Full form (from anywhere, includes cd + venv activation):

```bash
# 1) Lint/structure check only
cd /home/seame/Documents/SEA-ME_Team6_2025-26 && source /home/seame/Documents/SEA-ME_Team6_2025-26/.venv/bin/activate && python3 /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check

# 2) Evidence synchronization only
cd /home/seame/Documents/SEA-ME_Team6_2025-26 && source /home/seame/Documents/SEA-ME_Team6_2025-26/.venv/bin/activate && python3 /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --sync

# 3) TruDAG validate/score/publish
cd /home/seame/Documents/SEA-ME_Team6_2025-26 && source /home/seame/Documents/SEA-ME_Team6_2025-26/.venv/bin/activate && python3 /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate

# 4) Full pipeline in sequence
cd /home/seame/Documents/SEA-ME_Team6_2025-26 && source /home/seame/Documents/SEA-ME_Team6_2025-26/.venv/bin/activate && python3 /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --all
```

---

## 2. Prerequisites

### Required Software

| Software | Version | Purpose |
|----------|---------|---------|
| **Python** | 3.11+ | Runtime for trudag and scripts |
| **pip** | Latest | Python package manager |
| **git** | Latest | Version control |
| **trudag** | Latest | TSF validation and scoring tool |

### Optional Software

| Software | Purpose |
|----------|---------|
| **graphviz** | Render graph.dot diagrams visually (PNG/SVG) - only needed for `trudag plot` |
| **gh CLI** | GitHub CLI with Copilot extension for AI generation |
| **pipx** | Isolated Python package installation |

---

## 3. Installation Guide

### 🍎 macOS Installation

#### Step 1: Install Homebrew (if not installed)

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

After installation, add Homebrew to PATH:

```bash
# For Apple Silicon (M1/M2/M3)
echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
eval "$(/opt/homebrew/bin/brew shellenv)"

# For Intel Macs
echo 'eval "$(/usr/local/bin/brew shellenv)"' >> ~/.zprofile
eval "$(/usr/local/bin/brew shellenv)"
```

#### Step 2: Install Python 3.11+

```bash
brew install python@3.11
```

Verify installation:

```bash
python3.11 --version
# Should output: Python 3.11.x
```

#### Step 3: Create Virtual Environment

```bash
cd /path/to/SEA-ME_Team6_2025-26

# Remove old venv if exists
rm -rf .venv

# Create new venv with Python 3.11
python3.11 -m venv .venv

# Activate venv
source .venv/bin/activate
```

#### Step 4: Install Python Dependencies

```bash
# Upgrade pip
pip install --upgrade pip

# Install trudag from Eclipse GitLab
pip install trustable --index-url https://gitlab.eclipse.org/api/v4/projects/12202/packages/pypi/simple

# Install other dependencies
pip install pyyaml
```

#### Step 5: Verify Installation

```bash
trudag --help
# Should display trudag usage information
```

---

### 🐧 Linux Installation (Ubuntu/Debian)

#### Step 1: Install System Dependencies

```bash
sudo apt update
sudo apt install -y software-properties-common git curl
```

#### Step 2: Install Python 3.11+

```bash
# Add deadsnakes PPA for newer Python versions
sudo add-apt-repository ppa:deadsnakes/ppa
sudo apt update

# Install Python 3.11
sudo apt install -y python3.11 python3.11-venv python3.11-dev python3-pip
```

Verify installation:

```bash
python3.11 --version
# Should output: Python 3.11.x
```

#### Step 3: Create Virtual Environment

```bash
cd /path/to/SEA-ME_Team6_2025-26

# Remove old venv if exists
rm -rf .venv

# Create new venv with Python 3.11
python3.11 -m venv .venv

# Activate venv
source .venv/bin/activate
```

#### Step 4: Install Python Dependencies

```bash
# Upgrade pip
pip install --upgrade pip

# Install trudag from Eclipse GitLab
pip install trustable --index-url https://gitlab.eclipse.org/api/v4/projects/12202/packages/pypi/simple

# Install other dependencies
pip install pyyaml
```

#### Step 5: Verify Installation

```bash
trudag --help
# Should display trudag usage information
```

---

### 🪟 Windows Installation

#### Step 1: Install Python 3.11+

1. Download Python 3.11+ from [python.org](https://www.python.org/downloads/)
2. Run the installer
3. ✅ **IMPORTANT:** Check "Add Python to PATH" during installation
4. Click "Install Now"

Verify in PowerShell or Command Prompt:

```powershell
python --version
# Should output: Python 3.11.x
```

#### Step 2: Install Git

1. Download Git from [git-scm.com](https://git-scm.com/download/win)
2. Run installer with default options
3. Verify:

```powershell
git --version
```

#### Step 3: Create Virtual Environment

```powershell
cd C:\path\to\SEA-ME_Team6_2025-26

# Remove old venv if exists
Remove-Item -Recurse -Force .venv -ErrorAction SilentlyContinue

# Create new venv
python -m venv .venv

# Activate venv
.\.venv\Scripts\Activate.ps1
```

> **Note:** If you get an execution policy error, run:
> ```powershell
> Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
> ```

#### Step 4: Install Python Dependencies

```powershell
# Upgrade pip
pip install --upgrade pip

# Install trudag from Eclipse GitLab
pip install trustable --index-url https://gitlab.eclipse.org/api/v4/projects/12202/packages/pypi/simple

# Install other dependencies
pip install pyyaml
```

#### Step 5: Verify Installation

```powershell
trudag --help
# Should display trudag usage information
```

---

### 📊 Optional: Install Graphviz (for graph visualization)

> **Note:** Graphviz is an **optional** dependency. It is only needed if you want to use the `trudag plot` command to generate visual images of the dependency graph (PNG/SVG).
>
> For the normal workflow (`--check`, `--sync`, `--validate`), **graphviz is NOT required** because:
> - The `graph.dot` file is generated as a text file by the Python script
> - `trudag` reads and processes the `.dot` file without needing to render it visually

#### macOS

```bash
brew install graphviz
```

#### Linux (Ubuntu/Debian)

```bash
sudo apt install -y graphviz
```

#### Windows

1. Download Graphviz from [graphviz.org](https://graphviz.org/download/)
2. Run the installer
3. ✅ **IMPORTANT:** Check "Add Graphviz to PATH" during installation

#### Verify graphviz installation

```bash
dot -V
# Expected: dot - graphviz version X.X.X
```

---

## 4. Post-Installation Setup

### Activate Virtual Environment

Always activate the virtual environment before running TSF commands:

```bash
# macOS/Linux
source .venv/bin/activate

# Windows (PowerShell)
.\.venv\Scripts\Activate.ps1

# Windows (Command Prompt)
.\.venv\Scripts\activate.bat
```

### Verify Complete Setup

```bash
# Check Python version
python --version
# Expected: Python 3.11.x or higher

# Check trudag
trudag --help
# Expected: trudag usage information

# Run TSF check
python docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check
# Expected: Successful analysis of requirements
```

---

## 5. Project Structure

```
SEA-ME_Team6_2025-26/
├── .venv/                         # Python virtual environment (ONLY ONE)
├── .dotstop.dot                   # Symlink → docs/TSF/tsf_implementation/.dotstop.dot
├── .dotstop_extensions/           # Custom trudag extensions
│   ├── __init__.py                # Exports all references and validators
│   ├── references.py              # FileReference, UrlReference classes
│   └── validators.py              # Hardware, Linux, Software validators
├── localplugins                   # Symlink → .dotstop_extensions
└── docs/
    ├── doorstop/                  # Published reports (generated by TruDAG)
    │   ├── ASSERTIONS.md
    │   ├── ASSUMPTIONS.md
    │   ├── EVIDENCES.md
    │   ├── EXPECTATIONS.md
    │   ├── dashboard.md
    │   └── trustable_report_for_Software.md
    ├── sprints/                   # Sprint files (evidence source)
    │   ├── sprint1.md ... sprint7.md
    └── TSF/
        ├── requirements/
        │   └── tsf-requirements-table.md   # Source of truth for requirements
        └── tsf_implementation/
            ├── .dotstop.dot               # TruDAG database (generated)
            ├── .dotstop_extensions        # Symlink → ../../../.dotstop_extensions
            ├── .trudag_items/             # Generated trudag items
            ├── items/                     # SOURCE TSF items (editable)
            │   ├── assertions/            # ASSERT-L0-X.md
            │   ├── assumptions/           # ASSUMP-L0-X.md
            │   ├── evidences/             # EVID-L0-X.md
            │   └── expectations/          # EXPECT-L0-X.md
            ├── graph/
            │   └── graph.dot              # Dependency graph (generated)
            └── scripts/
                ├── open_check_sync_update_validate_run_publish_tsfrequirements.py
                ├── setup_trudag_clean.sh
                ├── config.yaml
                └── modules/
```

---

## 6. Quick Start Commands

### ⚠️ IMPORTANT: Always activate the virtual environment first!

```bash
# Activate virtual environment (REQUIRED)
source .venv/bin/activate
```

### Main Commands

```bash
# Check current state (read-only, shows status of all requirements)
source .venv/bin/activate && python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check

# Full validation + TruDAG run + publish reports
source .venv/bin/activate && python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate

# Sync changes (update items from table + AI generation if needed)
source .venv/bin/activate && python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --sync

# Run all stages
source .venv/bin/activate && python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --all
```

---

## 7. Main Automation Script

**File:** `open_check_sync_update_validate_run_publish_tsfrequirements.py`

This unified script handles the complete TSF workflow:

### Script Options

| Flag | Description |
|------|-------------|
| `--check` | Verify table completeness, detect orphan files, validate content |
| `--sync` | Generate missing items, update content with AI |
| `--validate` | Run full validation, TruDAG setup, scoring, and publish |
| `--all` | Run all stages in sequence |

### What It Does

1. **Parses requirements table** (`tsf-requirements-table.md`)
2. **Detects new/removed requirements** (creates or flags orphan files)
3. **Syncs evidence from sprints** (extracts links from sprint files)
4. **Validates item content** (YAML structure, references)
5. **Runs TruDAG** (setup, score, publish)
6. **Generates reports** (in `docs/doorstop/`)

---

## 8. Workflow Stages

### Stage 1: OPEN & CHECK (`--check`)

```
┌─────────────────────────────────────────────────────────────┐
│ 📋 STEP 1: OPEN & CHECK                                     │
├─────────────────────────────────────────────────────────────┤
│ 1. Parse requirements table (tsf-requirements-table.md)    │
│ 2. Check for incomplete fields                             │
│ 3. Parse sprint files for evidence links                   │
│ 4. Check item files existence (EXPECT/ASSERT/EVID/ASSUMP)  │
│ 5. Validate content of existing items                      │
│ 6. Detect orphan files (items without table entry)         │
│ 7. Identify sync needs                                     │
└─────────────────────────────────────────────────────────────┘
```

**Orphan Detection:** If a requirement is removed from the table, the script detects orphan files and prompts for confirmation before deletion.

### Stage 2: SYNC & UPDATE (`--sync`)

```
┌─────────────────────────────────────────────────────────────┐
│ 🔄 STEP 2: SYNC & UPDATE                                    │
├─────────────────────────────────────────────────────────────┤
│ 1. Generate missing item files (skeleton structure)        │
│ 2. Update EXPECT headers from table                        │
│ 3. Update EVID references from sprint evidence             │
│ 4. AI content generation (if enabled):                     │
│    - Option G: VSCode/Claude (semi-automated)              │
│    - Option C: gh copilot CLI (fallback)                   │
└─────────────────────────────────────────────────────────────┘
```

### Stage 3: VALIDATE, RUN & PUBLISH (`--validate`)

```
┌─────────────────────────────────────────────────────────────┐
│ ✅ STEP 3: VALIDATE, RUN & PUBLISH                          │
├─────────────────────────────────────────────────────────────┤
│ 1. Check required symlinks                                 │
│ 2. Run item validation (YAML structure)                    │
│ 3. Execute TruDAG (setup_trudag_clean.sh):                 │
│    - Clean generated files                                 │
│    - Generate graph.dot                                    │
│    - Initialize DB                                         │
│    - Create items in .trudag_items/                        │
│    - Apply logical links                                   │
│    - Mark items as reviewed                                │
│    - Run lint                                              │
│    - Calculate scores                                      │
│    - Publish reports                                       │
│ 4. Verify scores (report items below 1.0)                  │
└─────────────────────────────────────────────────────────────┘
```

---

## 9. TruDAG Integration

### setup_trudag_clean.sh

The canonical script for TruDAG operations:

```bash
bash docs/TSF/tsf_implementation/scripts/setup_trudag_clean.sh
```

**Steps performed:**
1. Clean all generated files (preserves `items/` source)
2. Generate `graph/graph.dot` from source items
3. Initialize trudag DB (`.dotstop.dot`)
4. Create items in `.trudag_items/` from source
5. Fix file reference paths and IDs
6. Create logical links from graph.dot
7. Mark all items as reviewed
8. Run `trudag lint`
9. Calculate scores (`trudag score --validate`)
10. Publish reports (`trudag publish`)

### Direct TruDAG Commands

```bash
# Run from tsf_implementation directory
cd docs/TSF/tsf_implementation

# Initialize database
trudag init

# Score with validators
trudag score --validate

# Publish reports
trudag publish --output-dir ../../../docs/doorstop --validate --all-bodies

# Lint validation
trudag lint
```

---

## 10. Validators

### Structure Validators (CI)

**File:** `scripts/modules/validate_items_formatation.py`

Validates YAML structure in item files:
- Front-matter format
- Required fields presence
- Reference path validity

### Content Validators (TruDAG)

**File:** `.dotstop_extensions/validators.py`

| Validator | Purpose | Used By |
|-----------|---------|---------|
| `validate_hardware_availability` | Checks hardware components in repo | ASSUMP items |
| `validate_linux_environment` | Validates Linux tools/packages | ASSUMP items |
| `validate_software_dependencies` | Checks software dependencies | ASSUMP items |

### Validator Configuration Example

In ASSUMP items:

```yaml
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "Raspberry Pi 5"
      - "STM32"
      - "CAN"
```

---

## 11. Symlinks Explained

The project uses symlinks for trudag compatibility:

| Symlink | Target | Purpose |
|---------|--------|---------|
| `.dotstop.dot` (repo root) | `docs/TSF/tsf_implementation/.dotstop.dot` | TruDAG finds DB from repo root |
| `localplugins` (repo root) | `.dotstop_extensions` | TruDAG imports custom references |
| `.dotstop_extensions` (tsf_impl) | `../../../.dotstop_extensions` | Validators accessible from tsf_implementation |

---

## 12. CI/CD Integration

### GitHub Actions Workflow

**File:** `.github/workflows/validate_items_formatation.yml`

Validates TSF items on PRs to `development`:
- Runs front-matter validation
- Checks YAML structure
- Validates reference paths

---

## 13. Published Reports

Reports are generated in `docs/doorstop/`:

| File | Description |
|------|-------------|
| `ASSERTIONS.md` | All assertion items with scores |
| `ASSUMPTIONS.md` | All assumption items with scores |
| `EVIDENCES.md` | All evidence items with links |
| `EXPECTATIONS.md` | All expectation items |
| `dashboard.md` | Score distribution summary |
| `trustable_report_for_Software.md` | Full trustable report |

---

## 14. Troubleshooting

### Installation Issues

#### "ERROR: No matching distribution found for trudag"

The package is called `trustable`, not `trudag`. Use the correct command:

```bash
pip install trustable --index-url https://gitlab.eclipse.org/api/v4/projects/12202/packages/pypi/simple
```

#### "trudag requires Python 3.11+"

Your Python version is too old. Install Python 3.11 or higher:

```bash
# macOS
brew install python@3.11

# Linux
sudo add-apt-repository ppa:deadsnakes/ppa
sudo apt install python3.11 python3.11-venv

# Then recreate venv
rm -rf .venv
python3.11 -m venv .venv
source .venv/bin/activate
pip install trustable --index-url https://gitlab.eclipse.org/api/v4/projects/12202/packages/pypi/simple
```

#### "zsh: command not found: brew" (macOS)

Homebrew is not installed or not in PATH:

```bash
# Install Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Add to PATH (Apple Silicon)
echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
eval "$(/opt/homebrew/bin/brew shellenv)"
```

#### Windows: "Execution of scripts is disabled"

PowerShell execution policy is blocking the venv activation:

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

---

### Runtime Issues

#### "source: no such file or directory: venv/bin/activate"

The project uses `.venv` (with dot), not `venv`:

```bash
# WRONG
source venv/bin/activate

# CORRECT
source .venv/bin/activate
```

### "Cannot find a validator function for type X"

Ensure symlinks exist:
```bash
# Check symlinks
ls -la .dotstop_extensions
ls -la localplugins
ls -la docs/TSF/tsf_implementation/.dotstop_extensions
```

### "ln: failed to create symbolic link '.dotstop.dot': File exists"

This usually means a stale or broken symlink already exists.

Fix manually:

```bash
rm -f .dotstop.dot
rm -f docs/TSF/tsf_implementation/.dotstop_extensions
source .venv/bin/activate
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate
```

---

## 15. Post 0/124 Fixes (Mar 2026)

After a full debug cycle, the following fixes were applied to stabilize validation/scoring:

1. `setup_trudag_clean.sh`: fixed symlink cleanup for broken links by checking `-e` or `-L` before `rm`.
2. `setup_trudag_clean.sh`: removed hardcoded macOS path (`/Volumes/...`) and switched to dynamic `TSF_IMPL` in embedded Python.
3. `.dotstop_extensions/validators.py`: imported `yaml` from `trudag.dotstop.core.validator` so validator signatures match TruDAG strict discovery.
4. `.dotstop_extensions/validators.py`: updated `validate_software_dependencies` to accept `components` in addition to `dependencies/packages` (retrocompatible).

Validation impact after fixes:

- `validator_not_found`: resolved
- `missing file` path rewrite issue: resolved
- score improved from `0/124` to `82/124` (with remaining items currently failing as `Missing` evidence)

Note: `setup_trudag_clean.sh` now removes both regular files and broken symlinks before recreating links.

### "Cannot get non-existent or non-regular file"

File path in references is incorrect. Paths should be:
- Relative to `items/` folder: `../assertions/ASSERT-L0-X.md`
- OR relative to repo root for actual files: `docs/demos/image.png`

### Items have score 0.0

Check:
1. Evidence files exist at referenced paths
2. Validators have proper configuration
3. No broken URL references

### TruDAG returns exit code 1

Usually caused by:
- Invalid references in items
- Missing files in references
- Validation errors

Run check first:
```bash
source .venv/bin/activate && python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check
```

---

## 16. Current Status (March 2026)

- **Total Requirements:** 31 (L0-1 to L0-31)
- **Total TSF Items:** 124 (31 × 4 types)
- **Python Version:** 3.11+ (required for trudag)
- **TruDAG Source:** Eclipse GitLab Package Registry

---

## 17. Quick Reference

```bash
# Full workflow
source .venv/bin/activate
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --all

# Just check status
source .venv/bin/activate
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check

# Just validate and publish
source .venv/bin/activate
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate
```
