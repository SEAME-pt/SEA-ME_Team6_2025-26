# TSF Validators - Complete Guide

This comprehensive guide covers the validator architecture, implementation details, and technical specifications for the Trustable Software Framework (TSF) validators.

## Table of Contents

### Part I: Architecture & Implementation
1. [Overview](#overview)
2. [Validator Architecture](#validator-architecture)
3. [Implementation Approaches](#implementation-approaches)
4. [CI vs TruDAG Validators](#ci-vs-trudag-validators)
5. [Execution Workflows](#execution-workflows)

### Part II: Technical Reference
6. [Validator Functions Reference](#validator-functions-reference)
7. [Universal Assumption Validators](#universal-assumption-validators)
8. [Validator Execution Flow](#validator-execution-flow)
9. [Usage Statistics](#usage-statistics)

### Part III: Practical Guide
10. [Best Practices](#best-practices)
11. [Score Calculation Examples](#score-calculation-examples)
12. [Troubleshooting](#troubleshooting)
13. [Next Steps](#next-steps)

---

# Part I: Architecture & Implementation

## Overview

The `validators.py` file contains **3 universal validator functions** that automate the scoring of TSF assumptions by searching for evidence in the repository. These validators are executed by TruDAG during the `trudag score` command.

**Key Features:**
- Evidence-based validation (searches repository for actual proof)
- Proportional scoring (0.0 to 1.0 based on evidence found)
- Informative warnings (identifies missing evidence)
- Flexible configuration (YAML-based parameters)
- Fail-safe behavior (returns 0.0 on errors, doesn't crash)

---

## Validator Architecture

We maintain **separation of concerns** between two types of validators:

- **Structure Validators** (CI): Validate YAML format, frontmatter structure, and file integrity
- **Content Validators** (TruDAG): Validate claims, evidence, and assumption requirements

This separation ensures:
- CI validators catch format errors early (before merge)
- TruDAG validators assess the quality and completeness of content (for scoring)

---

## Implementation Approaches

Two main approaches exist for implementing TruDAG validators:

### APPROACH A: Universal Validator (RECOMMENDED) ⭐

**Characteristics:**
- ✅ **Single script** validates all assumptions
- ✅ **Easier to maintain** (centralized logic)
- ✅ **Configurable** via YAML metadata
- ✅ **Scalable** for new assumptions
- ✅ **Code reuse** and shared utilities

**Current Implementation:**

The project uses this approach with `.dotstop_extensions/validators.py` containing:
- `validate_hardware_availability` - for hardware component assumptions
- `validate_linux_environment` - for Linux platform assumptions
- `validate_software_dependencies` - for software/library assumptions

**Structure:**

```python
#!/usr/bin/env python3
"""Universal validators for TSF assumptions."""

def validate_hardware_availability(configuration):
    """Validates hardware component assumptions."""
    components = configuration.get("components", [])
    # Search for evidence of each component in repository
    # Return (score, warnings)
    pass

def validate_linux_environment(configuration):
    """Validates Linux environment assumptions."""
    required_tools = configuration.get("required_tools", [])
    # Search for evidence of Linux tools/platform
    # Return (score, warnings)
    pass

def validate_software_dependencies(configuration):
    """Validates software dependency assumptions."""
    dependencies = configuration.get("dependencies", [])
    # Search for evidence in requirements files and docs
    # Return (score, warnings)
    pass
```

**Usage in YAML (e.g., ASSUMP-L0-1.md):**

```yaml
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "Raspberry Pi 5"
      - "Hailo AI Hat"
      - "DSI display"
```

**Advantages:**
- Centralized maintenance
- Easy to add new validator functions
- Shared utilities (file search, pattern matching)
- Consistent error handling

---

### APPROACH B: Individual Validators

**Characteristics:**
- ✅ **Complete isolation** between validations
- ✅ **Simplicity** (each script is independent)
- ❌ **Harder to maintain** (code duplication)
- ❌ **File proliferation** (17 scripts for 17 assumptions)

**Structure:**

```
validators/
├── validate_items_formatation.py      # CI validator (integrated)
├── validate_assump_l0_1_hardware.py   # TruDAG validator
├── validate_assump_l0_2_assembly.py   # TruDAG validator
├── validate_assump_l0_3_linux.py      # TruDAG validator
└── ... (14 more scripts)
```

**Disadvantages:**
- Code duplication (each script repeats file search logic)
- Difficult to update common functionality
- More files to manage

---

### Comparison Matrix

| Criterion | Universal (A) | Individual (B) |
|-----------|---------------|----------------|
| Maintenance | ⭐⭐⭐⭐⭐ | ⭐⭐ |
| Clarity | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Scalability | ⭐⭐⭐⭐⭐ | ⭐⭐ |
| Isolation | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| Code Reuse | ⭐⭐⭐⭐⭐ | ⭐ |

**Recommendation**: Use **Approach A (Universal)** for better scalability and maintainability.

---

## CI vs TruDAG Validators

Understanding the difference between CI validators and TruDAG validators:

```
┌─────────────────────────────────────────────────────────────┐
│  CI VALIDATORS (validate_items_formatation.py)              │
│  ├─ When: Pull requests, pushes to development/main         │
│  ├─ What: Validates STRUCTURE and FORMAT                    │
│  │   • YAML syntax                                          │
│  │   • Required frontmatter fields                          │
│  │   • File naming conventions                              │
│  │   • Reference integrity                                  │
│  └─ Effect: Blocks PR if validation fails                   │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│  TRUDAG VALIDATORS (validators.py)                          │
│  ├─ When: trudag score command                              │
│  ├─ What: Validates CONTENT and CLAIMS                      │
│  │   • Hardware component availability                      │
│  │   • Software dependency documentation                    │
│  │   • Linux environment evidence                           │
│  │   • Artifact presence                                    │
│  └─ Effect: Calculates item score (0.0 to 1.0)              │
└─────────────────────────────────────────────────────────────┘
```

**Key Differences:**

| Aspect | CI Validators | TruDAG Validators |
|--------|---------------|-------------------|
| **Trigger** | Git events (PR, push) | Manual command |
| **Purpose** | Prevent broken files | Assess quality |
| **Scope** | File structure | Content claims |
| **Output** | Pass/Fail | Score (0.0-1.0) |
| **Blocking** | Yes (PR reviews) | No (informational) |

---

## Execution Workflows

### CI Validator Workflow

**Trigger Events:**
1. **Pull Request** to `development` or `main`
2. **Direct push** to `development`
3. **Manual execution** by developers

**GitHub Actions Configuration** (`.github/workflows/validate_items_formatation.yml`):

```yaml
name: 'TSF Front-matter validation'

on:
  pull_request:
    branches: [ development, main ]
  push:
    branches: [ development ]

jobs:
  validate-tsf:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Set up Python
        uses: actions/setup-python@v4
        with:
          python-version: '3.11'
      
      - name: Run TSF validators
        run: |
          python3 docs/TSF/tsf_implementation/validators/validate_items_formatation.py
```

**Execution Flow:**

```
Developer creates PR
    ↓
GitHub Actions triggered
    ↓
validate_items_formatation.py executes
    ├─ Front-matter validation
    ├─ Extended validation
    └─ Structure checks
    ↓
All validations pass?
    ├─ YES → PR can be merged ✓
    └─ NO → PR blocked ✗
```

---

### TruDAG Validator Workflow

**Trigger**: Manual execution of `trudag score`

**Execution Flow:**

```
Developer runs: trudag score
    ↓
TruDAG reads .dotstop.dot graph
    ↓
For each item with evidence:
    ↓
    Load validator from validators.py
    ↓
    Execute validator(configuration)
    ↓
    Receive (score, warnings)
    ↓
    Assign score to item
    ↓
Output scores for all items
```

**Example**:

```bash
$ trudag score

INFO: Executing validator: validate_hardware_availability at 2025-12-12 19:06:26.341281
INFO: Validator: validate_hardware_availability finished execution in 0.01s

ASSUMPTIONS-ASSUMP_L0_1 = 1.0
ASSUMPTIONS-ASSUMP_L0_2 = 1.0
ASSUMPTIONS-ASSUMP_L0_3 = 1.0
...
EXPECTATIONS-EXPECT_L0_1 = 1.0
```

---

# Part II: Technical Reference

## Universal Assumption Validators

These three validators automatically validate assumptions by searching for evidence in the repository.

### 1. `validate_hardware_availability`

**Purpose**: Validates that hardware components mentioned in assumptions are documented in the repository.

**What it Tests**:
1. Searches for component names in documentation files (`.md`, `.txt`)
2. Searches in source code files (`.py`, `.sh`)
3. Checks multiple directories:
   - `docs/demos/`
   - `docs/guides/`
   - `docs/sprints/`
   - `docs/dailys/`
   - `docs/TSF/`
   - `README.md`
   - `src/`

**Configuration Requirements**:
- `components`: list of hardware component names to validate
- `search_paths` (optional): custom search paths

**Returns**: 
- **Score**: Percentage of components found (validated_count / total_count)
- **Warnings**: List of components not found in documentation

**Use Case**: 
- ASSUMP-L0-1: Raspberry Pi 5, Hailo AI Hat, DSI display, etc.
- ASSUMP-L0-2: DC motors, servo motors, CAN transceivers
- ASSUMP-L0-4: Jetson Nano hardware
- ASSUMP-L0-8, L0-9, L0-10, L0-12, L0-17: Various hardware components

**Example**:
```yaml
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "Raspberry Pi 5"
      - "Hailo AI Hat"
      - "DC motors"
      - "DSI display"
```

**Test Logic**:
```
For each component:
  1. Create case-insensitive regex pattern
  2. Search in all specified directories
  3. Check file extensions: *.md, *.txt, *.py, *.sh
  4. If found → mark as validated
  5. If not found → add warning

Score = (validated components) / (total components)
```

---

### 2. `validate_linux_environment`

**Purpose**: Validates that Linux-specific tools and environment are documented in the repository.

**What it Tests**:
1. Searches for Linux tool names in documentation
2. Searches in installation guides and scripts
3. Verifies Ubuntu version mentions (default: 22.04)

**Configuration Requirements**:
- `required_tools`: list of Linux tools/packages to verify
- `ubuntu_version` (optional): Ubuntu version to check (default: "22.04")

**Returns**:
- **Score**: Percentage of tools documented (documented_tools / total_tools)
- **Warnings**: List of tools without documentation

**Use Case**:
- ASSUMP-L0-3: Ubuntu 22.04, apt, systemd
- ASSUMP-L0-11: Linux environment dependencies

**Example**:
```yaml
evidence:
  type: validate_linux_environment
  configuration:
    required_tools:
      - "Ubuntu"
      - "apt"
      - "systemd"
    ubuntu_version: "22.04"
```

**Test Logic**:
```
For each tool:
  1. Create case-insensitive regex pattern
  2. Search in docs/ and src/ directories
  3. Check file extensions: *.md, *.txt, *.sh, *.py
  4. If found → mark as documented
  5. If not found → add warning

Score = (documented tools) / (total tools)
```

---

### 3. `validate_software_dependencies`

**Purpose**: Validates that software dependencies and libraries are documented in dependency files or documentation.

**What it Tests**:
1. Searches for dependencies in standard dependency files:
   - `requirements.txt` (Python)
   - `package.json` (Node.js)
   - `CMakeLists.txt` (C/C++)
   - `Dockerfile`
   - `*.cmake` files
2. Falls back to searching in documentation if not found in dependency files

**Configuration Requirements**:
- `dependencies` OR `packages`: list of dependencies to validate
- `dependency_files` (optional): custom list of files to check

**Returns**:
- **Score**: Percentage of dependencies found (found_deps / total_deps)
- **Warnings**: List of dependencies without evidence

**Use Case**:
- ASSUMP-L0-5: TSF, TruDAG, Trustable framework
- ASSUMP-L0-6: Qt6, QML
- ASSUMP-L0-7: Python libraries
- ASSUMP-L0-13, L0-14, L0-15, L0-16: Various software dependencies

**Example**:
```yaml
evidence:
  type: validate_software_dependencies
  configuration:
    packages:
      - "Qt6"
      - "OpenCV"
      - "Python"
    dependency_files:
      - "requirements.txt"
      - "CMakeLists.txt"
```

**Test Logic**:
```
For each dependency:
  1. Search in dependency files (requirements.txt, package.json, etc.)
  2. If not found → search in docs/ markdown files
  3. Use case-insensitive regex matching
  4. If found → mark as validated
  5. If not found → add warning

Score = (validated dependencies) / (total dependencies)
```

---

## Validator Execution Flow

```
┌─────────────────────────────────────────────────────────────┐
│  1. TruDAG reads item YAML frontmatter                      │
└────────────────────────────┬────────────────────────────────┘
                             │
┌────────────────────────────▼────────────────────────────────┐
│  2. Finds "evidence: type: <validator_name>"                │
└────────────────────────────┬────────────────────────────────┘
                             │
┌────────────────────────────▼────────────────────────────────┐
│  3. Loads validator function from validators.py             │
└────────────────────────────┬────────────────────────────────┘
                             │
┌────────────────────────────▼────────────────────────────────┐
│  4. Calls validator(configuration)                          │
└────────────────────────────┬────────────────────────────────┘
                             │
┌────────────────────────────▼────────────────────────────────┐
│  5. Validator returns (score, warnings)                     │
└────────────────────────────┬────────────────────────────────┘
                             │
┌────────────────────────────▼────────────────────────────────┐
│  6. TruDAG assigns score to item                            │
└─────────────────────────────────────────────────────────────┘
```

---

## Usage Statistics

Based on the current TSF implementation:

| Validator | Usage Count | Items |
|-----------|-------------|-------|
| `validate_hardware_availability` | 8 | ASSUMP-L0-1, 2, 4, 8, 9, 10, 12, 17 |
| `validate_linux_environment` | 2 | ASSUMP-L0-3, 11 |
| `validate_software_dependencies` | 7 | ASSUMP-L0-5, 6, 7, 13, 14, 15, 16 |

**Total**: 17 assumptions with automated validators (100% coverage)

**Current Results** (as of December 12, 2025):
- ✅ All 17 ASSUMPTIONS = 1.0 (100%)
- ✅ All 17 ASSERTIONS = 1.0 (100%)
- ✅ All 17 EVIDENCES = 1.0 (100%)
- ✅ All 17 EXPECTATIONS = 1.0 (100%)

---

## Validator Design Principles

### 1. **Evidence-Based Validation**
All validators search for actual evidence in the repository rather than relying solely on manual assertions.

### 2. **Proportional Scoring**
Validators return scores proportional to the amount of evidence found:
- 1.0 = All components/dependencies validated
- 0.5 = Half of requirements validated
- 0.0 = No evidence found

### 3. **Informative Warnings**
When validation fails, validators provide specific warnings indicating what evidence is missing.

### 4. **Flexible Configuration**
Validators accept configuration parameters allowing customization without code changes.

### 5. **Fail-Safe Behavior**
If a validator encounters an error (missing configuration, invalid parameters), it returns 0.0 with an error message rather than crashing.

---

# Part III: Practical Guide

## Best Practices

### For Validator Development

1. **Return Type Consistency**
   ```python
   def validator(config: dict) -> tuple[float, list[Exception | Warning]]:
       # Always return (score, warnings/errors)
       return (1.0, [])
   ```

2. **Error Handling**
   ```python
   def validator(config: dict):
       try:
           # Validation logic
           return (score, warnings)
       except Exception as e:
           return (0.0, [Exception(f"Validator failed: {e}")])
   ```

3. **Configuration Validation**
   ```python
   def validator(config: dict):
       required_field = config.get("required_field")
       if not required_field:
           return (0.0, [ValueError("Missing required_field in config")])
   ```

4. **Informative Warnings**
   ```python
   warnings = []
   for component in missing_components:
       warnings.append(Warning(f"No evidence found for: {component}"))
   return (score, warnings)
   ```

---

### For Assumption Authors

1. **Use Specific Names**
   - ✅ `"Raspberry Pi 5"` (exact model)
   - ❌ `"RasPi"` (ambiguous abbreviation)

2. **Match Documentation**
   - Ensure component names match those in documentation
   - Use consistent terminology across files

3. **Provide Adequate Configuration**
   ```yaml
   # Good: Specific and complete
   evidence:
     type: validate_hardware_availability
     configuration:
       components:
         - "Raspberry Pi 5"
         - "Hailo-8L AI Accelerator"
   
   # Bad: Too vague
   evidence:
     type: validate_hardware_availability
     configuration:
       components:
         - "computer"
   ```

---

### For Maintenance

1. **Update Search Paths**
   - When repository structure changes, update validator search paths
   - Keep `search_paths` configuration up to date

2. **Monitor Warnings**
   - Review validator warnings regularly
   - Add missing documentation or update configurations

3. **Version Control**
   - Document validator changes in commit messages
   - Update this guide when adding new validators

4. **Testing**
   ```bash
   # Test validators locally before pushing
   trudag score
   
   # Check for errors/warnings
   trudag manage lint
   
   # Rebuild database after changes
   bash docs/TSF/tsf_implementation/scripts/setup_trudag_clean.sh
   ```

---

## Score Calculation Examples

### Example 1: Hardware Validator - Partial Success
```yaml
configuration:
  components:
    - "Raspberry Pi 5"   # Found in docs/guides/
    - "Hailo AI Hat"     # Found in README.md
    - "DSI display"      # Found in docs/demos/
    - "Unknown Sensor"   # NOT FOUND

Result: 
  Score = 3/4 = 0.75
  Warnings: ["No evidence found for component: Unknown Sensor"]
```

### Example 2: Software Validator - Complete Success
```yaml
configuration:
  packages:
    - "Qt6"        # Found in CMakeLists.txt
    - "Python"     # Found in requirements.txt
    - "OpenCV"     # Found in docs/guides/

Result:
  Score = 3/3 = 1.0
  Warnings: []
```

### Example 3: Linux Validator - Complete Failure
```yaml
configuration:
  required_tools:
    - "RedHat"     # NOT FOUND
    - "yum"        # NOT FOUND

Result:
  Score = 0/2 = 0.0
  Warnings: [
    "No documentation found for: RedHat",
    "No documentation found for: yum"
  ]
```

---

## Troubleshooting

### Common Issues

**Issue**: Validator returns 0.0 but evidence exists

**Solutions**:
1. Check component name spelling matches documentation
2. Verify search paths include the documentation location
3. Add debug logging to see what files are being searched
4. Check file encoding (validators expect UTF-8)

**Issue**: Validator runs very slowly

**Solutions**:
1. Reduce number of search paths
2. Limit file extensions being searched
3. Use more specific glob patterns
4. Consider caching search results

**Issue**: Warnings about missing components

**Solutions**:
1. Add missing documentation
2. Update component names in assumption configuration
3. Extend search paths if documentation exists elsewhere
4. Use more generic patterns if component names vary

**Issue**: Score is 0.0 but components exist

**Cause**: Component name mismatch  
**Solution**: Check exact spelling in documentation vs assumption configuration

**Issue**: Warnings about missing components after adding documentation

**Cause**: TruDAG database not rebuilt  
**Solution**: Run `bash docs/TSF/tsf_implementation/scripts/setup_trudag_clean.sh`

---

## Next Steps

### Implementing a New Validator

**Step 1**: Add function to `validators.py`

```python
def validate_new_requirement(configuration: dict[str, yaml]) -> tuple[float, list[Exception | Warning]]:
    """Validator for new requirement type.
    
    Configuration should contain:
        requirement_items: list of items to validate
    
    Returns:
        (score, warnings) where score = percentage validated
    """
    items = configuration.get("requirement_items", [])
    if not items:
        return (0.0, [ValueError("No items specified")])
    
    # Validation logic here
    validated_count = 0
    warnings = []
    
    # ... search for evidence ...
    
    score = validated_count / len(items) if items else 0.0
    return (score, warnings)
```

**Step 2**: Update assumption YAML

```yaml
---
id: ASSUMP-L0-X
header: New assumption
evidence:
  type: validate_new_requirement
  configuration:
    requirement_items:
      - "Item 1"
      - "Item 2"
---
```

**Step 3**: Test the validator

```bash
# Rebuild TruDAG database
bash docs/TSF/tsf_implementation/scripts/setup_trudag_clean.sh

# Run scoring
trudag score

# Check result
trudag score | grep ASSUMP_L0_X
```

**Step 4**: Document the validator

- Add entry to this guide's validator reference section
- Update usage statistics
- Commit changes with descriptive message

---

### Adding CI Validation

If you want TruDAG validators to also run in CI (to block PRs with low scores):

**Option 1**: Add score check to existing CI workflow

```yaml
# .github/workflows/validate_items_formatation.yml
- name: Validate TSF scores
  run: |
    python3 docs/TSF/tsf_implementation/validators/validate_items_formatation.py
    
    # Run trudag score and check for failures
    trudag score > scores.txt
    if grep -q "= 0.0" scores.txt; then
      echo "ERROR: Some items have score 0.0"
      exit 1
    fi
```

**Option 2**: Create separate workflow for TruDAG validation

```yaml
# .github/workflows/trudag-validate.yml
name: 'TruDAG Score Validation'

on:
  pull_request:
    branches: [ development, main ]

jobs:
  validate-scores:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Install TruDAG
        run: pip install trustable
      
      - name: Calculate scores
        run: trudag score
      
      - name: Check minimum scores
        run: |
          # Fail if any score < 0.5
          trudag score | grep "= 0\.[0-4]" && exit 1 || exit 0
```

---

## Future Enhancements

Potential improvements to the validator system:

1. **Caching**: Cache search results to speed up subsequent validations
2. **Parallel Processing**: Search files in parallel for large repositories
3. **Version Checking**: Validate specific versions of dependencies
4. **Link Validation**: Verify that referenced URLs are accessible
5. **Image Analysis**: OCR for component names in images
6. **Git History**: Check if components were present in previous commits
7. **Performance Metrics**: Track validator execution times
8. **Custom Search Filters**: Allow regex patterns in configuration

---

## Additional Resources

- [validators.py](../../.dotstop_extensions/validators.py) - Validator source code
- [TruDAG Documentation](https://gitlab.com/trustable/trustable) - Official TruDAG docs
- [TSF Overview](../../WhatsTSF.md) - TSF methodology guide
- [Setup Script](../scripts/setup_trudag_clean.sh) - Database rebuild script

---

## Questions & Support

For questions about validators:

1. **Check existing documentation**:
   - This guide (complete reference)
   - WhatsTSF.md (TSF methodology)
   - validators.py source code

2. **Test locally**:
   ```bash
   trudag score
   trudag manage lint
   bash docs/TSF/tsf_implementation/scripts/setup_trudag_clean.sh
   ```

3. **Contact team**:
   - Create GitHub issue
   - Team discussion in daily standups
   - Review validator warnings in CI

---

**Last Updated**: December 12, 2025  
**Maintainer**: SEA-ME Team 6  
**Version**: 3.0 (Unified Guide)  
**Repository**: [SEA-ME_Team6_2025-26](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26)
