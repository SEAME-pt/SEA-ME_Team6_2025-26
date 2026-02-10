# TSF Validators — Complete Guide

This guide covers the validator architecture and implementation for the Trustable Software Framework (TSF) in the SEA:ME Team 6 project.

**Last Updated:** February 2026

---

## Table of Contents

1. [Overview](#overview)
2. [Two Types of Validators](#two-types-of-validators)
3. [Structure Validators (CI)](#structure-validators-ci)
4. [Content Validators (TruDAG)](#content-validators-trudag)
5. [Validator Configuration](#validator-configuration)
6. [Score Calculation](#score-calculation)
7. [Creating Custom Validators](#creating-custom-validators)
8. [Troubleshooting](#troubleshooting)

---

## Overview

The TSF implementation uses **two types of validators** for different purposes:

| Type | When | Purpose | Location |
|------|------|---------|----------|
| **Structure Validators** | CI (GitHub Actions) | Validate YAML format, file structure | `scripts/modules/validate_items_formatation.py` |
| **Content Validators** | TruDAG scoring | Validate claims, check evidence | `.dotstop_extensions/validators.py` |

This separation ensures:
- ✅ CI validators catch format errors early (before merge)
- ✅ TruDAG validators assess quality and completeness (for scoring)

---

## Two Types of Validators

### Structure Validators (CI)

**Purpose:** Ensure files are properly formatted before merging to main branches.

**Executed by:** GitHub Actions on PRs

**Checks:**
- YAML front-matter syntax
- Required fields presence
- Reference path validity
- File naming conventions

### Content Validators (TruDAG)

**Purpose:** Automatically score assumptions by searching for evidence in the repository.

**Executed by:** `trudag score --validate`

**Checks:**
- Hardware component documentation
- Linux environment documentation
- Software dependency documentation

---

## Structure Validators (CI)

### File Location

`docs/TSF/tsf_implementation/scripts/modules/validate_items_formatation.py`

### Validation Rules

#### Front-matter Validation

```yaml
# Required fields for all items
---
id: EXPECT-L0-1          # Must match filename
header: "Title"           # Non-empty string
text: |                   # Non-empty text
  Description here
level: 1.1                # Numeric level
normative: true           # Boolean
references: []            # List (can be empty)
reviewers: []             # List with name/email
review_status: accepted   # accepted/pending/rejected
---
```

#### Reference Validation

```yaml
references:
  - type: file
    path: ../assertions/ASSERT-L0-1.md  # Must exist
  - type: url
    url: https://github.com/...          # Valid URL format
```

### Running Structure Validation

```bash
# Via the main script
source .venv/bin/activate && python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check

# Directly
python3 docs/TSF/tsf_implementation/scripts/modules/validate_items_formatation.py docs/TSF/tsf_implementation/items
```

### CI Integration

**File:** `.github/workflows/validate_items_formatation.yml`

```yaml
name: Validate TSF Items

on:
  pull_request:
    branches: [development, main]

jobs:
  validate:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-python@v5
        with:
          python-version: '3.11'
      - run: pip install pyyaml
      - run: python docs/TSF/tsf_implementation/scripts/modules/validate_items_formatation.py docs/TSF/tsf_implementation/items
```

---

## Content Validators (TruDAG)

### File Location

`.dotstop_extensions/validators.py`

### Available Validators

| Validator | Purpose | Used By |
|-----------|---------|---------|
| `validate_hardware_availability` | Checks hardware component documentation | ASSUMP items |
| `validate_linux_environment` | Validates Linux tools/packages documentation | ASSUMP items |
| `validate_software_dependencies` | Checks software dependency documentation | ASSUMP items |

### How They Work

Validators search the repository for evidence of the claimed assumptions:

```python
def validate_hardware_availability(configuration):
    """
    Searches repository for documentation mentioning hardware components.
    
    Args:
        configuration: Dict with 'components' list
        
    Returns:
        Tuple[float, List[str]]: (score, warnings)
        - score: 0.0 to 1.0 based on components found
        - warnings: List of missing components
    """
    components = configuration.get("components", [])
    found = 0
    warnings = []
    
    for component in components:
        # Search in docs/, README.md, etc.
        if evidence_found(component):
            found += 1
        else:
            warnings.append(f"No evidence for: {component}")
    
    score = found / len(components) if components else 0.0
    return (score, warnings)
```

### Running Content Validators

```bash
# Via TruDAG
cd docs/TSF/tsf_implementation
trudag score --validate

# Via the main script
source .venv/bin/activate && python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate
```

---

## Validator Configuration

### In ASSUMP Items

Use the `evidence:` field to configure validators:

```yaml
---
id: ASSUMP-L0-1
header: "Hardware components available"
text: |
  The following hardware components are assumed to be available...
level: 1.1
normative: true
references:
  - type: file
    path: ../assertions/ASSERT-L0-1.md
reviewers:
  - name: Joao Jesus Silva
    email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "Raspberry Pi 5"
      - "STM32"
      - "CAN transceiver"
      - "DC motors"
      - "Servo motor"
---
```

### Configuration Options

#### validate_hardware_availability

```yaml
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "Component Name 1"
      - "Component Name 2"
```

#### validate_linux_environment

```yaml
evidence:
  type: validate_linux_environment
  configuration:
    required_tools:
      - "gcc"
      - "cmake"
      - "python3"
    packages:
      - "qt6-base"
      - "can-utils"
```

#### validate_software_dependencies

```yaml
evidence:
  type: validate_software_dependencies
  configuration:
    dependencies:
      - "Qt 6"
      - "ThreadX"
      - "AGL"
```

---

## Score Calculation

### Scoring Formula

```
score = found_items / total_items
```

Where:
- `found_items` = Number of components/dependencies with evidence
- `total_items` = Total components/dependencies in configuration

### Score Meanings

| Score | Meaning |
|-------|---------|
| 1.0 | All items have evidence |
| 0.5 | Half of items have evidence |
| 0.0 | No evidence found |

### Example

```yaml
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "Raspberry Pi 5"    # Found in docs → +1
      - "STM32"             # Found in docs → +1
      - "Hailo AI Hat"      # NOT found → +0
```

Result: `score = 2/3 = 0.67`

---

## Creating Custom Validators

### Step 1: Add Function to validators.py

```python
# .dotstop_extensions/validators.py

def validate_custom_requirement(configuration):
    """
    Custom validator for specific requirements.
    
    Args:
        configuration: Dict from YAML evidence.configuration
        
    Returns:
        Tuple[float, List[str]]: (score, warnings)
    """
    items = configuration.get("items", [])
    found = 0
    warnings = []
    
    for item in items:
        if check_item_exists(item):
            found += 1
        else:
            warnings.append(f"Missing: {item}")
    
    score = found / len(items) if items else 0.0
    return (score, warnings)
```

### Step 2: Register in __init__.py

```python
# .dotstop_extensions/__init__.py

from .validators import (
    validate_hardware_availability,
    validate_linux_environment,
    validate_software_dependencies,
    validate_custom_requirement,  # Add new validator
)
```

### Step 3: Use in ASSUMP Items

```yaml
evidence:
  type: validate_custom_requirement
  configuration:
    items:
      - "item1"
      - "item2"
```

---

## Troubleshooting

### "Cannot find a validator function for type X"

**Cause:** Validator not found in `.dotstop_extensions/validators.py`

**Solution:**
1. Check validator name matches exactly
2. Ensure function is exported in `__init__.py`
3. Verify symlinks exist:
   ```bash
   ls -la .dotstop_extensions
   ls -la localplugins
   ls -la docs/TSF/tsf_implementation/.dotstop_extensions
   ```

### Score always 0.0

**Cause:** Validator not finding evidence

**Solution:**
1. Check component names match documentation exactly
2. Verify docs exist in searched paths
3. Review validator search patterns

### "No dependencies specified in configuration"

**Cause:** Empty or missing `configuration` in YAML

**Solution:**
```yaml
# Wrong
evidence:
  type: validate_software_dependencies

# Correct
evidence:
  type: validate_software_dependencies
  configuration:
    dependencies:
      - "Qt 6"
      - "ThreadX"
```

### Validator not running

**Cause:** TruDAG not finding validators

**Solution:**
1. Run from correct directory:
   ```bash
   cd docs/TSF/tsf_implementation
   trudag score --validate
   ```
2. Ensure `.dotstop_extensions` symlink exists in tsf_implementation

---

## Best Practices

### 1. Be Specific

```yaml
# Too vague
components:
  - "hardware"
  - "software"

# Better
components:
  - "Raspberry Pi 5"
  - "STM32F4 microcontroller"
  - "MCP2515 CAN transceiver"
```

### 2. Document Evidence

Ensure components/dependencies are mentioned in:
- `docs/` markdown files
- `README.md`
- Sprint reports

### 3. Use Appropriate Validator

| Assumption Type | Validator |
|-----------------|-----------|
| Hardware available | `validate_hardware_availability` |
| Linux tools installed | `validate_linux_environment` |
| Software/libraries | `validate_software_dependencies` |

### 4. Test Before Committing

```bash
# Quick validation check
source .venv/bin/activate && python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check
```
