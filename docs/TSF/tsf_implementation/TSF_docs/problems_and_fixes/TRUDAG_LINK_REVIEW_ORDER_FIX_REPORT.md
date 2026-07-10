# TruDAG Link Review Order Fix Report

**Date:** April 16, 2026  
**Status:** Resolved

## 1. Problem Summary

Validation was passing, but score propagation remained inconsistent (many `EXPECTATIONS-*` and `ASSERTIONS-*` at `0.0; Missing`).

Root behavior observed:
- `EVID`/`ASSUMP` with explicit score/validator reached `1.0`
- `EXPECT`/`ASSERT` did not inherit derived score as expected

## 2. Root Cause

The setup pipeline created links before all items were set as reviewed.

In TruDAG, creating links between unreviewed items is blocked by design. This is aligned with the official CLI behavior in:
- `docs/TSF/Howtoimplement/tsf-official/frontends/cli/__init__.py`

So the practical failure mode was:
1. Items created
2. Link creation attempted too early
3. Part of the graph reasoning links not applied
4. Recursive score derivation had missing paths

## 3. Fix Applied

File changed:
- `docs/TSF/tsf_implementation/scripts/setup_trudag_clean.sh`

Change:
- Added **Step 3.5** to mark all items reviewed **before** Step 4 link creation.
- Kept per-item logs (`✓ ITEM` / `✗ ITEM`) for visibility.

This restores the intended order:
1. Create items
2. Set reviewed status
3. Create links
4. Run lint/score/publish

## 4. Methodology Alignment (TSF Official)

This fix aligns with the official TSF model and tooling behavior:
- Score should be derived recursively through argument links, not forced manually on non-leaf statements.
- Expectations and Assertions should gain confidence via supporting chains from Evidence.

References:
- `docs/TSF/Howtoimplement/tsf-official/docs/model/methodology.md`
- `docs/TSF/Howtoimplement/tsf-official/docs/model/scoring/implementation.md`

## 5. Clarification About `sync_evidence.py`

`docs/TSF/tsf_implementation/scripts/modules/sync_evidence.py` exists, but it is **not** the primary path used in the current `open_check_sync_update_validate_run_publish_tsfrequirements.py` flow.

Current flow uses internal functions in the main script:
- `sync_evidence_from_sprints(...)`
- `_sync_evidence_to_table(...)`
- `_sync_evidence_to_evid_files(...)`

This clarification avoids confusion between legacy/standalone helper scripts and the active pipeline.

## 6. Documentation Placement

The wider incident history has been reorganized into `TSF_docs/problems_and_fixes/`, with a dedicated folder README that defines the reading order for the recovery reports.

## 7. Verification Checklist

Use:

```bash
source .venv/bin/activate
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate
```

Confirm in output:
- Step 3.5 appears before link creation
- Per-item review logs are printed
- Step 4 creates links from `graph.dot`
- Score summary reflects derived propagation behavior
