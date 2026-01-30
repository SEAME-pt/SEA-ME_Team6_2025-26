# PLANNING WITH GROK: TSF Automation for SEA-ME Team6 2025-26

## Project Context

This document outlines the work done with Grok (GitHub Copilot) to automate the Trustable Software Framework (TSF) for the SEA-ME Team6 2025-26 project, specifically the PiRacer Warm-Up module.

### Project Overview
- **Team**: SEA-ME Team6 2025-26
- **Module**: PiRacer Warm-Up (2025-2026)
- **Goal**: Familiarize students with PiRacer hardware (Raspberry Pi 5, Hailo AI Hat, motors, display), Qt development, Git/GitHub, Agile/Scrum, and TSF integration.
- **TSF Purpose**: Ensure traceability between requirements, design, and testing using automated item generation.

### Key Components
- **Requirements Table**: `docs/TSF/requirements/tsf-requirements-table.md` - Markdown table with L0 requirements, including manual evidence links.
- **Automation Script**: `docs/TSF/tsf_implementation/scripts/generate_from_requirements.py` - Parses table, generates EXPECT/ASSERT/EVID items, auto-discovers evidence via keyword matching.
- **TruDAG Tool**: Validates TSF items, performs linting, scoring, and publishing.
- **Generated Items**: Stored in `docs/TSF/tsf_implementation/items/` (expectations, assertions, evidences).

## What We Accomplished

### Initial Issues Identified
1. **TruDAG Lint Failures**: Due to malformed YAML frontmatter in generated evidence files.
2. **404 URL Errors**: Incorrect URL in requirements table (`src/cluste` instead of `src/cluster`).
3. **Duplicate/Inconsistent References**: Script generated `references: []` (empty lists) or duplicated sections.
4. **Path Resolution Bugs**: Incorrect ROOT calculation in script, causing extra "docs/" prefixes.

### Fixes Implemented
1. **URL Correction**: Fixed typo in `tsf-requirements-table.md` (line ~32: `/main/docs/demos/eletric-scheme.p` → corrected path).
2. **Path Resolution Fix**: Updated `generate_from_requirements.py` to use `ROOT = Path(__file__).resolve().parents[4]` for correct workspace root.
3. **References Formatting**: Modified templates to omit `references` section when empty (instead of `references: []`), as TruDAG rejects empty lists but accepts omission.
4. **Duplicate Cleanup**: Removed old `.trudag_items` and `.dotstop.dot` databases to force regeneration.
5. **Item Regeneration**: Re-ran script with `--overwrite --run-trudag` to create clean items.

### Current State
- ✅ TruDAG lint passes without errors.
- ✅ All YAML frontmatter is valid.
- ✅ Items are generated and linked correctly.
- ⚠️ Automatic evidence discovery doesn't match manual evidences in the table (see below).

## Errors Encountered and Resolutions

### 1. TruDAG Lint Errors: "list([]) should evaluate to True"
**Cause**: Script always included `references:` in templates, even when no evidence was found, resulting in `references: []` (empty YAML list).
**Why**: TruDAG's YAML parser rejects empty lists for optional fields; omission is preferred.
**Fix**: Changed templates to use `{references_section}` placeholder, and `format_references_section()` returns empty string when no paths. Regenerated items.

### 2. 404 URL Errors in Lint
**Cause**: Malformed URL in requirements table: `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/eletric-scheme.p` (incomplete filename).
**Why**: Typo in manual entry; script didn't validate URLs.
**Fix**: Corrected to proper path in table.

### 3. Duplicate References Sections
**Cause**: Script had bugs in path calculation, adding extra "docs/" and duplicating entries.
**Why**: ROOT was set to `parents[3]`, pointing to wrong directory.
**Fix**: Changed to `parents[4]` to reach workspace root.

### 4. Trudag Create-Item Failures
**Cause**: Old database files (`.dotstop.dot`, `.trudag_items`) contained invalid items with empty references.
**Why**: Previous runs left corrupted state.
**Fix**: Deleted old DB files, reinitialized Trudag, regenerated items.

### 5. Evidence Mismatch: Auto vs Manual
**Cause**: Script uses keyword-based search (grep for words from requirement text), while table has manually curated evidences.
**Why**: Automation prioritizes discovery over manual curation; e.g., for L0-1, auto-found files like `systemstatus.cpp` vs manual images like `assembled-car1.jpeg`.
**Fix**: Not yet addressed; requires script modification to parse table's "Evidence" column instead of keyword search.

## Project Structure and Files

```
/Volumes/Important_Docs/42/SEA-ME_Team6_2025-26/
├── docs/
│   ├── TSF/
│   │   ├── requirements/tsf-requirements-table.md  # Manual requirements table
│   │   └── tsf_implementation/
│   │       ├── scripts/generate_from_requirements.py  # Automation script
│   │       ├── items/  # Generated EXPECT/ASSERT/EVID files
│   │       └── .dotstop.dot  # Trudag database
├── src/  # Code (cluster, CAN, etc.)
└── docs/demos/  # Evidence files
```

## Next Steps and Recommendations

1. **Integrate Manual Evidences**: Modify `generate_from_requirements.py` to parse the "Evidence" column from the table and use those paths directly, instead of keyword search.
2. **Improve Keyword Matching**: Enhance `find_evidence_files()` to better match requirement intent (e.g., prioritize images for architecture requirements).
3. **Validation Workflow**: Add GitHub Actions to auto-run lint on PRs.
4. **Documentation**: Update TSF docs to reflect automation process.
5. **Testing**: Run full Trudag score and publish to verify end-to-end.

## Lessons Learned
- TruDAG is strict on YAML: omit optional fields instead of empty lists.
- Path calculations must account for script location in nested directories.
- Manual curation vs automation: balance between accuracy and efficiency.
- Clean DB state is crucial for Trudag operations.

**Date**: 22 December 2025  
**Agent**: Grok (GitHub Copilot)  
**Status**: Automation functional, lint passing, ready for integration.