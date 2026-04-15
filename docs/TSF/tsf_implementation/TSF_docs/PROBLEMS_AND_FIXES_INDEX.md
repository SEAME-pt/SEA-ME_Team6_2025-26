# TSF Problems and Fixes Index

Unified index for TSF troubleshooting history.

This document is the practical junction of:

- `docs/TSF/tsf_implementation/TSF_docs/TSF_SCORE_RECOVERY_MAR2026.md`
- `docs/TSF/tsf_implementation/TSF_docs/TSF_STRUCTURAL_ANALYSIS_REPORT.md`

It provides one entry point to understand what failed, why it failed, and what was done.

**Last Updated:** April 2026

---

## Table of Contents

1. [Purpose](#1-purpose)
2. [How to use this index](#2-how-to-use-this-index)
3. [Problem timeline](#3-problem-timeline)
4. [Issue-to-fix map](#4-issue-to-fix-map)
5. [Current known pending items](#5-current-known-pending-items)
6. [Related guides](#6-related-guides)

---

## 1. Purpose

Use this file as the first stop for TSF incident analysis.

- For score recovery chronology, go to `TSF_SCORE_RECOVERY_MAR2026.md`.
- For deep structural/content analysis and broader issue catalog, go to `TSF_STRUCTURAL_ANALYSIS_REPORT.md`.

---

## 2. How to use this index

1. Start here and identify the issue category.
2. Follow the link to the detailed source document.
3. Apply or verify the documented fix.
4. Confirm with:

```bash
source .venv/bin/activate
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check
```

---

## 3. Problem timeline

### March 2026 - Score Recovery Wave

Primary source:
- `docs/TSF/tsf_implementation/TSF_docs/TSF_SCORE_RECOVERY_MAR2026.md`

Key outcomes:
- Restored broken graph links.
- Stabilized URL-reference SHA behavior.
- Recovered affected L0_22-L0_31 chains.
- Reached full score state for that cycle.

### April 2026 - Structural and Validation Hardening

Primary source:
- `docs/TSF/tsf_implementation/TSF_docs/TSF_STRUCTURAL_ANALYSIS_REPORT.md`

Key outcomes:
- Removed plugin reference-class collision noise (`FileReference` shadowing).
- Clarified interrupted runs (`130/143`) vs true validation failures.
- Improved semantic placeholder detection in `--check`.

---

## 4. Issue-to-fix map

| Issue | Main Fix | Detailed Source |
|---|---|---|
| Missing or weak score propagation | Graph link restoration and SHA rebuild | `TSF_SCORE_RECOVERY_MAR2026.md` |
| URL-reference hash instability | Deterministic URL SHA strategy | `TSF_SCORE_RECOVERY_MAR2026.md` |
| Plugin warning spam (`FileReference` collision) | Removed custom `FileReference`; kept focused URL customization | `TSF_STRUCTURAL_ANALYSIS_REPORT.md` |
| Interrupted run confusion (`130/143`) | Treat as signal interruption, not model failure | `TSF_STRUCTURAL_ANALYSIS_REPORT.md` |
| Placeholder detection asymmetry | Added semantic ASSUMP checks and cross-type placeholder guardrails | `TSF_STRUCTURAL_ANALYSIS_REPORT.md` |

---

## 5. Current known pending items

Expected placeholders still pending replacement:

- `EVID-L0-32`
- `ASSUMP-L0-32`

These are currently visible by design in `--check` output.

---

## 6. Related guides

- `docs/TSF/tsf_implementation/TSF_docs/VALIDATORS_GUIDE.md`
- `docs/TSF/tsf_implementation/TSF_docs/REFERENCES_GUIDE.md`
- `docs/TSF/tsf_implementation/TSF_docs/README.md`
