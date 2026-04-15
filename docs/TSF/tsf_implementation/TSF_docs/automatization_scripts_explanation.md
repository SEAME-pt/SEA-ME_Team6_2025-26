# TSF Automation Scripts — Updated Technical Documentation

This document describes the current TSF automation process in the SEA:ME Team 6 repository, including recent changes to evidence sync and placeholder handling.

**Last Updated:** April 2026

## April 2026 Update

- Removed local plugin reference-name collision that generated repeated TruDAG warning lines.
- Added ASSUMP semantic placeholder/default detection in content validation.
- Current expected pending `--check` items are cleared for the manual retest.

## Table of Contents

1. [Main script and commands](#1-main-script-and-commands)
2. [Recent process changes](#2-recent-process-changes)
3. [Detailed evidence extraction and sync flow](#3-detailed-evidence-extraction-and-sync-flow)
4. [Source-of-truth order](#4-source-of-truth-order)
5. [Placeholder behavior (new)](#5-placeholder-behavior-new)
6. [Current known limitation](#6-current-known-limitation)
7. [Updated best practices](#7-updated-best-practices)
8. [Legacy scripts](#8-legacy-scripts)
9. [Interpreting `trudag score` output and command variants](#9-interpreting-trudag-score-output-and-command-variants)

---

## 1. Main script and commands

**Unified script:** `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py`

Run from the repository root:

```bash
source .venv/bin/activate

# Check (analysis and sync detection)
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check

# Sync (generation/update + evidence sync)
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --sync

# Validate (TruDAG pipeline)
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate

# Full pipeline
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --all
```

---

## 2. Recent process changes

The following changes were introduced and are now part of the current flow:

1. An explicit placeholder marker for evidence references was introduced:
   - `PLACEHOLDER_EVIDENCE_MARKER = "TSF_PLACEHOLDER_EVIDENCE"` in `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:119`.
2. Dedicated placeholder detection was added:
   - `is_placeholder_evidence_reference(...)` in `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:122`.
3. Content validation now marks EVID files with placeholders as invalid:
   - check added in `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:1138`.
4. `open_check()` now flags `sync_needed` also when an EVID has a placeholder and real evidence already exists:
   - logic added in `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:1864`, `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:1883`, `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:1886`.
5. Automatic structure fixing for EVID files without references now creates a placeholder with marker:
   - insertion of `description: TSF_PLACEHOLDER_EVIDENCE` in `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:1553`.
6. EVID sync removes placeholder references when real evidence is found:
   - filtering in `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:2096` and `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:2097`.

Important note: using a `README.md` URL is no longer a placeholder criterion. The official criterion is the marker in `description`.

---

## 3. Detailed evidence extraction and sync flow

This section includes the requested explanation, with current code references.

### 3.1 Load configuration and define the sprints folder

1. `Config._resolve_paths()` defines `self.sprints_dir = repo_root/docs/sprints` in `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:212`.
2. The sprint list is read from `evidence_sync.sprint_files` in `docs/TSF/tsf_implementation/scripts/config.yaml:134`.
3. This list is exposed by `Config.sprint_files` in `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:256`.

### 3.2 Enter `open_check()`

1. Main check function: `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:1696`.

### 3.3 Extract evidence from sprints (`docs/sprints/*.md`)

1. It creates `EvidenceParser` (`class EvidenceParser`) in `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:270`.
2. It reads each sprint from the configured list with `parse_all_sprints()` in `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:405`.
3. Each file is processed by `parse_sprint_file()` in `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:302`.
4. Actual link extraction happens in `_extract_evidence_from_content()` in `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:313`.

### 3.4 How extraction works inside sprint files (regex/patterns)

1. It identifies EXPECT blocks via `expect_header` in `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:286`.
2. It captures markdown images `![...](...)` with `markdown_image` in `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:293`.
3. It captures markdown links `[...] (...)` with `markdown_link` in `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:290`.
4. It captures raw URLs in indented lines with `raw_url` in `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:296`.
5. Pattern usage in the parsing loop is at `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:324`, `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:339`, `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:351`, `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:370`.

### 3.5 It also scans evidence folders (not only sprints)

1. `scan_evidence_folders()` in `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:420`.
2. Scanned folders:
   - `demos` at `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:429`
   - `guides` at `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:430`
   - `images` at `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:431`
   - `presentations` at `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:432`
   - `src` at `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:433`

### 3.6 Merge sprint evidence + folder evidence

1. Call to `parse_all_sprints()` at `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:1742`.
2. Call to `scan_evidence_folders()` at `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:1753`.
3. Explicit merge at `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:1764`.
4. It then marks `sync_needed` when applicable at `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:1878` and `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:1886`.

### 3.7 In `--sync`, it writes both Table and EVID

1. Flow enters at `if args.sync or args.all` in `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:2606`.
2. It calls `sync_evidence_from_sprints(...)` at `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:2610` (after `sync_update` at `:2608`).
3. Inside `sync_evidence_from_sprints()` (`...py:1902`):
   - updates table via `_sync_evidence_to_table()` at `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:1988`
   - updates EVID files via `_sync_evidence_to_evid_files()` at `docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py:2049`

### 3.8 What each sub-step does

1. `_sync_evidence_to_table()` writes evidence in the table evidence column when needed (empty/TODO), using extracted links.
2. `_sync_evidence_to_evid_files()` updates `frontmatter.references` in EVID files with discovered URLs, removes placeholder refs, and sets score back to `1.0` when real evidence exists.

---

## 4. Source-of-truth order

The current operational order is:

1. `docs/sprints/*.md` + folder scan (`docs/demos`, `docs/guides`, `docs/images`, `docs/presentations`, `src`)
2. `docs/TSF/requirements/tsf-requirements-table.md`
3. `docs/TSF/tsf_implementation/items/evidences/EVID-L0-*.md`

This means final EVID references come from the extraction/sync process, not from isolated manual edits in EVID files.

---

## 5. Placeholder behavior (new)

### Official rule

A placeholder reference is identified by:

```yaml
description: TSF_PLACEHOLDER_EVIDENCE
```

### Process effect

1. In `--check`: an EVID with this marker is considered invalid content and enters `sync_needed` when real evidence is available.
2. In `--sync`: placeholder refs are removed and replaced with real extracted references.

### Valid placeholder example (temporary state)

```yaml
references:
  - type: url
    url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md
    description: TSF_PLACEHOLDER_EVIDENCE
score: 0.0
```

---

## 6. Current known limitation

`--sync` updates `items/evidences/*`, but does not automatically replicate changes to `.trudag_items/`.

When you need immediate TruDAG score reflection without recreating the whole structure, you must sync files and recalculate SHAs.

Practical example:

```bash
# Copy EVID source -> .trudag_items (batch example)
for n in $(seq 0 124); do
  src="docs/TSF/tsf_implementation/items/evidences/EVID-L0-${n}.md"
  dst="docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_${n}/EVIDENCES-EVID_L0_${n}.md"
  [ -f "$src" ] && [ -f "$dst" ] && cp "$src" "$dst"
done

# Rebuild SHAs in .dotstop.dot
source .venv/bin/activate
python3 rebuild_dotstop_shas.py
```

---

## 7. Updated best practices

1. Always run `--check` before `--sync`.
2. Do not use `README.md` as an implicit placeholder signal; always use `description: TSF_PLACEHOLDER_EVIDENCE`.
3. After structural EVID changes in an active TruDAG environment, ensure consistency between `items/` and `.trudag_items/` and rebuild SHAs when needed.
4. Validate with `--validate` and confirm final score with `trudag score`.

---

## 8. Legacy scripts

These remain for historical reference, but the recommended flow is the unified script:

- `sync_tsf_manager.py`
- `sync_tsf_requirements_table.py`
- `trudag_runner.py`

Recommended command:

```bash
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py
```

---

## 9. Interpreting `trudag score` Output and Command Variants

### 9.1 Are all those warnings normal?

In the current project setup, most of the `trudag score` console output is expected:

1. `INFO: Executing validator ...` and `finished execution ...`
- Normal verbose runtime logs.
- They indicate validators are being executed across many items.

2. `Reference object <class 'localplugins.FileReference'> shadows an existing Reference ...`
- A real warning, but usually non-blocking.
- It means the local plugin defines a `FileReference` class name that already exists in TruDAG, so the shadowed one is not imported.
- Scoring can still complete successfully.

3. `EVIDENCES-EVID_L0_X = 1.0; Missing`
- The word `Missing` here is the reason label shown by TruDAG in this setup, not necessarily a failed score.
- If the item line shows `= 1.0`, then the computed score is still `1.0`.
- In this repository, EVID items can explicitly carry `score: 1.0`, so they remain at `1.0` even when the reason text is `Missing`.

### 9.2 Which command form should be used?

Preferred when you are already at repository root:

```bash
source .venv/bin/activate && python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check
```

Equivalent full form when you are not at repository root:

```bash
cd /absolute/path/to/SEA-ME_Team6_2025-26 && source .venv/bin/activate && python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check
```

### 9.3 Important differences between command variants

1. `cd ... &&` vs no `cd`
- `cd` is required only when you are not already in repo root.

2. `python3` vs `python`
- Use `python3` for consistency and to avoid environments where `python` may point to a different interpreter.

3. With `--check` vs without `--check`
- `--check` runs the check stage explicitly.
- If omitted, the script behavior depends on default argparse flow and may not run the intended stage.

4. A bare quoted path like `'/Users/.../SEA-ME_Team6_2025-26' && ...`
- This is not a valid directory change command by itself.
- It must be `cd '/Users/.../SEA-ME_Team6_2025-26' && ...`.
