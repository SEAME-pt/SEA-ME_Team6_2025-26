# TSF Implementation Guide — SEA:ME Team 6

This document describes the TSF (Trustable Software Framework) implementation for the PiRacer Warm-Up project, including the unified automation script, TruDAG integration, and validation workflows.

**Last Updated:** February 2026  
**Authors:** SEA-ME Team 6

---

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Installation Instructions by Operating System](#installation-instructions-by-operating-system)
3. [Installation Troubleshoot](#installation-troubleshoot)
4. [Project Structure](#project-structure)
5. [Quick Start Commands](#quick-start-commands)
6. [Main Automation Script](#main-automation-script)
7. [Workflow Stages](#workflow-stages)
8. [TruDAG Integration](#trudag-integration)
9. [Validators](#validators)
10. [Symlinks Explained](#symlinks-explained)
11. [CI/CD Integration](#cicd-integration)
12. [Published Reports](#published-reports)
13. [Troubleshooting](#troubleshooting)

---

## Prerequisites

- **Python 3.9+** available on the system PATH
- **Virtual environment** support (`python -m venv` or `python3 -m venv`)
- **trustable** package for TSF generation and validation, which provides the `trudag` CLI
- **graphviz** (`dot`) for rendering graph.dot
- **gh CLI** with Copilot extension (optional, for AI generation)

## Installation Instructions by Operating System

#### **1. Python 3.9+**

##### Windows
```powershell
# Option A: Using winget (recommended)
winget install Python.Python.3.11

# If Python is installed but still not found, disable the Windows app execution aliases:
# Settings > Apps > Advanced app settings > App execution aliases
# Turn OFF python.exe and python3.exe, then open a new PowerShell window.

# If the launcher is available, you can also verify with:
py --version

# Option B: Manual download from https://www.python.org/downloads/
# (Make sure to check "Add Python to PATH" during installation)

# Verify installation
python --version

# If python still does not resolve, use the launcher for the venv step:
# py -m venv .venv
```

##### macOS
```bash
# Using Homebrew
brew install python@3.11

# Verify installation
python3 --version
```

##### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install python3.11 python3.11-venv

# Verify installation
python3 --version
```

#### **2. Virtual Environment (.venv)**

##### Windows
```powershell
# Navigate to project directory
cd C:\Users\LG\Downloads\SEA-ME_Team6_2025-26

# Create virtual environment
python -m venv .venv

# Activate it
.\.venv\Scripts\Activate.ps1

# If you get execution policy error, run:
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

### Windows: `Activate.ps1 cannot be loaded because running scripts is disabled on this system`

PowerShell blocks script execution by default on some Windows setups. Use one of these fixes and then try activating the virtual environment again.

```powershell
# Option A: Allow scripts for the current user (persistent)
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser

# Option B: Bypass only for the current PowerShell session
Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process

# Retry activation
.\.venv\Scripts\Activate.ps1
```

If the policy change is applied successfully, the prompt should change to show `(.venv)` at the beginning.

##### macOS / Linux
```bash
cd ~/path/to/SEA-ME_Team6_2025-26

# Create virtual environment
python3 -m venv .venv

# Activate it
source .venv/bin/activate
```

#### **3. TruDAG Package** (`trustable` package)

```bash
# With virtual environment activated:
python -m pip install --upgrade pip
python -m pip install trustable --index-url https://gitlab.eclipse.org/api/v4/projects/12202/packages/pypi/simple

# Verify installation and CLI availability
trudag --help
```

##### If `python -m pip install trustable --index-url ...` fails

Run the install from the active virtual environment and use the Python interpreter directly. The `trustable` package is the official distribution source for the `trudag` CLI:

```powershell
# Windows PowerShell
cd C:\Users\LG\Downloads\SEA-ME_Team6_2025-26
python -m venv .venv
.\.venv\Scripts\Activate.ps1
python -m pip install --upgrade pip
python -m pip install trustable --index-url https://gitlab.eclipse.org/api/v4/projects/12202/packages/pypi/simple
```

If the error still appears after that, the package is not being resolved from the Eclipse TSF package index in your environment. In that case, follow the project-approved TruDAG installation source referenced in the TSF docs before retrying.

#### **4. GraphViz** (`dot` tool)

##### Windows
```powershell
# Option A: Using Chocolatey (if installed)
choco install graphviz -y

# Option B: Using winget
winget install graphviz.graphviz

# Option C: Manual download from https://graphviz.org/download/

# Verify installation
dot -V
```


NOTE: O trustable/trudag comes froma a Specific Python Index that comes from Eclipse TSF. That´s the reason why we use python -m pip install ... --index-url .... On the other hand Graphviz its a system tool, not a Python package, and thats the reason why we install it with winget or choco or manual installor on Windows.

A virtual environment only isolates Python packages. It does not install or manage system-level executables.

Graphviz is mainly needed here for the dot command-line tool. That tool is a native system binary, so it has to be installed at the OS level, not inside .venv.

The distinction is:

trustable is a Python package, so installing it in the virtual environment makes sense.
graphviz here means the Graphviz engine and the dot executable, which are external system tools.
There is also a Python package called graphviz, but that is only a Python wrapper. It does not replace the real Graphviz installation. In other words, you can install the Python wrapper in .venv, but if dot is missing from the system, the actual rendering still will not work.

So the correct setup is:

Install Python packages like trustable inside .venv.
Install Graphviz (dot) system-wide with winget, choco, or the official installer.
That is why Graphviz is not installed through the virtual environment in this case. If you want, I can also help reword that explanation directly in the documentation.


##### macOS
```bash
# Using Homebrew
brew install graphviz

# Verify installation
dot -V
```
NOTE: O trustable/trudag comes froma a Specific Python Index that comes from Eclipse TSF. That´s the reason why we use python -m pip install ... --index-url .... On the other hand Graphviz its a system tool, not a Python package, and thats the reason why we install it with winget or choco or manual installor on Windows.

A virtual environment only isolates Python packages. It does not install or manage system-level executables.

Graphviz is mainly needed here for the dot command-line tool. That tool is a native system binary, so it has to be installed at the OS level, not inside .venv.

The distinction is:

trustable is a Python package, so installing it in the virtual environment makes sense.
graphviz here means the Graphviz engine and the dot executable, which are external system tools.
There is also a Python package called graphviz, but that is only a Python wrapper. It does not replace the real Graphviz installation. In other words, you can install the Python wrapper in .venv, but if dot is missing from the system, the actual rendering still will not work.

So the correct setup is:

Install Python packages like trustable inside .venv.
Install Graphviz (dot) system-wide with winget, choco, or the official installer.
That is why Graphviz is not installed through the virtual environment in this case. If you want, I can also help reword that explanation directly in the documentation.


##### Linux (Ubuntu/Debian)
```bash
sudo apt-get install graphviz

# Verify installation
dot -V
```
NOTE: O trustable/trudag comes froma a Specific Python Index that comes from Eclipse TSF. That´s the reason why we use python -m pip install ... --index-url .... On the other hand Graphviz its a system tool, not a Python package, and thats the reason why we install it with winget or choco or manual installor on Windows.

A virtual environment only isolates Python packages. It does not install or manage system-level executables.

Graphviz is mainly needed here for the dot command-line tool. That tool is a native system binary, so it has to be installed at the OS level, not inside .venv.

The distinction is:

trustable is a Python package, so installing it in the virtual environment makes sense.
graphviz here means the Graphviz engine and the dot executable, which are external system tools.
There is also a Python package called graphviz, but that is only a Python wrapper. It does not replace the real Graphviz installation. In other words, you can install the Python wrapper in .venv, but if dot is missing from the system, the actual rendering still will not work.

So the correct setup is:

Install Python packages like trustable inside .venv.
Install Graphviz (dot) system-wide with winget, choco, or the official installer.
That is why Graphviz is not installed through the virtual environment in this case. If you want, I can also help reword that explanation directly in the documentation.



#### **5. GitHub CLI (Optional - for AI generation)**

##### Windows
```powershell
# Option A: Using Chocolatey
choco install gh -y

# Option B: Using winget
winget install GitHub.cli

# Verify installation
gh --version
```

##### macOS
```bash
# Using Homebrew
brew install gh

# Verify installation
gh --version
```

##### Linux (Ubuntu/Debian)
```bash
sudo apt-get install gh

# Verify installation
gh --version
```

### Virtual Environment Setup & Activation

##### Windows (PowerShell)
```powershell
# Activate the project virtual environment
.\.venv\Scripts\Activate.ps1

# Verify trudag is available
trudag --help
```

##### macOS / Linux (Bash)
```bash
# Activate the project virtual environment
source .venv/bin/activate

# Verify trudag is available
trudag --help
```

### Verify All Prerequisites Installed

```bash
# With virtual environment activated, check all tools:
python --version     # Should be 3.9+
pip --version       # Should work
trudag --version    # Should show version
dot -V              # Should show Graphviz version
gh --version        # (Optional) Should show GitHub CLI version
```

---

## Project Structure

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

## Quick Start Commands

### ⚠️ IMPORTANT: Always activate the virtual environment first!

#### Windows (PowerShell)
```powershell
# Activate virtual environment (REQUIRED)
.\.venv\Scripts\Activate.ps1
```

#### macOS / Linux (Bash)
```bash
# Activate virtual environment (REQUIRED)
source .venv/bin/activate
```

### Main Commands

#### Windows (PowerShell)
```powershell
# Check current state (read-only, shows status of all requirements)
python docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check

# Full validation + TruDAG run + publish reports
python docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate

# Sync changes (update items from table + AI generation if needed)
python docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --sync

# Run all stages
python docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --all
```

#### macOS / Linux (Bash)
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

## Main Automation Script

**File:** `open_check_sync_update_validate_run_publish_tsfrequirements.py`

**Config file:** `docs/TSF/tsf_implementation/scripts/config.yaml`

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

### Important Runtime Notes

- Use `--check`, `--sync`, `--validate`, `--all` (double hyphen). Using `-check` or `–check` (en-dash) can fail.
- If `repo_root` in `config.yaml` points to an old machine path (e.g. `/Volumes/...`), table parsing will fail with "Requirements table not found".
- During `--sync`, if VSCode CLI (`code`) is not available in PATH, the script now continues without crashing and asks you to open files manually.

---

## Workflow Stages

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
│    - If `code` CLI is missing, continue without auto-open   │
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

## TruDAG Integration

### setup_trudag_clean.sh

The canonical script for TruDAG operations.
It resolves repository paths dynamically from script location (no hardcoded machine paths):

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

## Validators

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

## Symlinks Explained

The project uses symlinks for trudag compatibility:

| Symlink | Target | Purpose |
|---------|--------|---------|
| `.dotstop.dot` (repo root) | `docs/TSF/tsf_implementation/.dotstop.dot` | TruDAG finds DB from repo root |
| `localplugins` (repo root) | `.dotstop_extensions` | TruDAG imports custom references |
| `.dotstop_extensions` (tsf_impl) | `../../../.dotstop_extensions` | Validators accessible from tsf_implementation |

---

## CI/CD Integration

### GitHub Actions Workflow

**File:** `.github/workflows/validate_items_formatation.yml`

Validates TSF items on PRs to `development`:
- Runs front-matter validation
- Checks YAML structure
- Validates reference paths

---

## Published Reports

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

## Troubleshooting

### Script argument error: `unrecognized arguments: –check`

This happens when using an en-dash (`–`) instead of hyphen-minus (`-`), or when using `-check` instead of `--check`.

```powershell
# CORRECT
python docs\TSF\tsf_implementation\scripts\open_check_sync_update_validate_run_publish_tsfrequirements.py --check

# WRONG
python ... –check
python ... -check
```

### Script startup error: `Requirements table not found: /Volumes/...`

This means `config.yaml` still points to an old path from another machine.

```yaml
paths:
  repo_root: "C:/Users/LG/Downloads/SEA-ME_Team6_2025-26"
```

After updating `repo_root`, run `--check` again.

### Sync error: `FileNotFoundError [WinError 2]` when opening files in VSCode

This happens when VSCode CLI command `code` is not in PATH.

- The script now continues without crashing and asks for manual file opening.
- To enable auto-open, install the VSCode shell command and restart terminal.

### Windows: `python --version` says Python was not found after installing with winget

This usually means the Windows app execution alias is still active or the terminal has not refreshed the PATH.

```powershell
# 1. Close every PowerShell window
# 2. Open Settings > Apps > Advanced app settings > App execution aliases
# 3. Turn OFF python.exe and python3.exe
# 4. Open a new PowerShell window
python --version
```

If it still fails, use the Python launcher if installed:

```powershell
py --version
py -m pip --version
```

### Windows: `pip` is not recognized

This usually means Python is not correctly on PATH or the virtual environment is not active.

```powershell
# Confirm Python works first
python --version

# Then use pip through Python
python -m pip install --upgrade pip

# Create and activate the virtual environment before installing trudag
python -m venv .venv
.\.venv\Scripts\Activate.ps1
```

### Windows: `python -m pip install trustable --index-url ...` returns `No matching distribution found`

This usually means one of the following:
- the virtual environment is not active yet
- pip is outdated
- the machine cannot reach the Python package index
- the Eclipse TSF package index is not reachable from your machine

Use this sequence first:

```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
python -m pip install --upgrade pip
python -m pip install trustable --index-url https://gitlab.eclipse.org/api/v4/projects/12202/packages/pypi/simple
```

If the error persists, check network access and verify that the GitLab package index is reachable from your environment before trying again.

### Windows: `sudo` is not needed and may be disabled

On Windows, do not use `sudo`. The command is not available by default and is not required for removing aliases or configuring Python.

```powershell
# Correct
Remove-Item -Path "HKCU:\Software\Microsoft\Windows\CurrentVersion\App Paths\python.exe" -Force -ErrorAction SilentlyContinue
```

### "source: no such file or directory: venv/bin/activate"

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

## Config.yaml and Machine-Specific Paths

`config.yaml` is committed to the repository.  
To avoid conflicts between machines (Windows path vs macOS path), set `repo_root` to the special value `"auto"`:

```yaml
paths:
  repo_root: "auto"
```

When set to `"auto"`, the Python script detects the repository root automatically from its own location (`scripts/ → tsf_implementation/ → TSF/ → docs/ → repo_root`).  
This works on all machines without any manual change.

If auto-detection fails on a specific machine (e.g., unusual folder structure), override it locally:

```yaml
paths:
  repo_root: "C:/Users/YourUser/path/to/SEA-ME_Team6_2025-26"
```

**Important:** If you override `repo_root` with a machine-specific absolute path, **do not commit that change** — or other developers will need to revert it on their machines.

---

## Installation Troubleshoot

## Troubleshooting — Windows-Specific Issues (May 2026)

These issues were encountered and resolved during Windows setup in May 2026.

### `UnicodeEncodeError: 'charmap' codec can't encode character '\u2713'`

**Cause:** Python's default encoding on Windows is `cp1252`, which cannot encode Unicode symbols like `✓` or `✔` that are printed by the TruDAG tooling.

**Solution:** `setup_trudag_clean.sh` now sets UTF-8 environment variables before running any Python-based tooling:

```bash
export PYTHONUTF8=1
export PYTHONIOENCODING="utf-8"
export LANG="C.UTF-8"
export LC_ALL="C.UTF-8"
```

These are set at the top of the script and apply to all subsequent Python calls.  
Additionally, the inline Python block in the script was updated to avoid printing Unicode checkmark characters directly, using plain ASCII text instead.

---

### `[WinError 1314] A required privilege is not held by the client` (symlink)

**Cause:** On Windows, creating symlinks requires either Administrator privileges or Developer Mode enabled.

**Solution:** `setup_trudag_clean.sh` now falls back to a directory copy if symlink creation fails:

```bash
if ln -s "..." ".dotstop_extensions" 2>/dev/null; then
    echo "Symlink created"
elif [ -d "$REPO_ROOT/.dotstop_extensions" ]; then
    cp -R "$REPO_ROOT/.dotstop_extensions" ".dotstop_extensions"
    echo "Copied directory (symlink unavailable)"
fi
```

The Python automation script (`open_check_...py`) does the same for `.dotstop_extensions` and `localplugins`.

---

### `[WinError 2] The system cannot find the file specified` (TruDAG or validator)

**Cause:** This occurs when the script tries to call `bash` or `python3`, which are not available by default on Windows.

**Solutions applied:**

1. `resolve_bash_command()` in the Python script searches for `bash.exe` in common Git for Windows locations:
   - `C:\Program Files\Git\bin\bash.exe`
   - `C:\Program Files\Git\usr\bin\bash.exe`
   - `%LocalAppData%\Programs\Git\bin\bash.exe`

2. `setup_trudag_clean.sh` now detects the correct Python binary:
   ```bash
   if command -v python3 >/dev/null 2>&1; then PYTHON_BIN="python3"
   elif command -v python >/dev/null 2>&1; then PYTHON_BIN="python"
   fi
   ```

---

### Score verification shows `0/128 at 1.0` after successful TruDAG run

**Cause:** The Python script parsed the `trudag score` output incorrectly. The output format is:

```
ASSERTIONS-ASSERT_L0_1 = 1.0; Validator with References
```

The original code tried `float(parts[1])` which failed on `"1.0; Validator with References"`.

**Solution:** Changed to `float(parts[1].split(';')[0].strip())` to extract only the numeric part before the semicolon.

---

### `ERROR: No dependencies specified in configuration` (repeated)

**Cause:** Several ASSUMP items use the `validate_software_dependencies` validator but have no `dependencies` key in their `configuration` block.

**Impact:** These items score `0.0`. This is not a script error but a content issue in the item files (`ASSUMP-L0-23` through `ASSUMP-L0-31`).

**Fix:** Add a `dependencies` list to the `evidence.configuration` block in those ASSUMP items:

```yaml
evidence:
  type: validate_software_dependencies
  configuration:
    dependencies:
      - "python3"
      - "trudag"
```

---

### `EXPECT` items always score `0.0; Missing`

**This is expected behaviour.** EXPECT items do not have a `score:` field or evidence validator — their score comes from the chain: `EXPECT → ASSERT → EVID`. If the EVID and ASSERT items score 1.0, the overall trustable report is correct even with EXPECT showing 0.0 in `trudag score`.

---

### `ASSERT_L0_31 = 0.0; Missing` — RAUC URL returns HTTP 403

**Cause:** The URL `https://rauc.readthedocs.io/` returns HTTP 403 (Forbidden) when accessed programmatically. TruDAG marks this reference as unresolvable.

**Options:**
1. Replace the URL with a specific versioned page that allows access.
2. Change the reference type from `url` to `file` pointing to a local copy.
3. Accept the 0.0 score for L0-31 (OTA requirement) until evidence is available.

---

## Current Status (May 2026)

- **Total Requirements:** 31 (L0-1 to L0-31)
- **Total TSF Items:** 124 (31 × 4 types)
- **Items scoring 1.0:** 74/124
- **Items scoring 0.0:** 50/124
  - All 31 EXPECT items (expected — no score field)
  - ASSERT-L0-31 (RAUC URL returns 403)
  - ASSUMP-L0-23 to L0-31 (missing `dependencies` in validator config)
  - EVID-L0-22 to L0-31 (evidence not yet linked)
- **TruDAG publish:** ✅ Successful
- **Platform:** Fully working on Windows (PowerShell + Git Bash) as of May 2026

---

## Quick Reference

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
