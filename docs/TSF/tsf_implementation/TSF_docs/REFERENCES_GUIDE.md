# TSF References Guide

This guide defines how to model and maintain `references` in TSF items for the SEA:ME Team 6 repository.

**Last Updated:** April 2026

---

## Table of Contents

1. [Scope](#1-scope)
2. [Why references matter](#2-why-references-matter)
3. [From theory to practice (current implementation)](#3-from-theory-to-practice-current-implementation)
4. [Supported reference types](#4-supported-reference-types)
5. [Per-item rules (EXPECT/ASSERT/EVID/ASSUMP)](#5-per-item-rules-expectassertevidassump)
6. [Placeholder policy](#6-placeholder-policy)
7. [Paths and URL conventions](#7-paths-and-url-conventions)
8. [Common anti-patterns](#8-common-anti-patterns)
9. [Validation checklist](#9-validation-checklist)
10. [Quick examples](#10-quick-examples)

---

## 1. Scope

This guide covers references inside TSF item frontmatter files under:

- `docs/TSF/tsf_implementation/items/expectations/`
- `docs/TSF/tsf_implementation/items/assertions/`
- `docs/TSF/tsf_implementation/items/evidences/`
- `docs/TSF/tsf_implementation/items/assumptions/`

It complements:

- `docs/TSF/tsf_implementation/TSF_docs/VALIDATORS_GUIDE.md`

---

## 2. Why references matter

References are used for traceability and for TruDAG content hashing/scoring context.

Good references are:

- explicit,
- stable,
- verifiable,
- consistent in format.

Poor references create noisy logs, broken links, and score instability.

### Why Validators and References Exist

In TSF, validators and references solve different but complementary problems:
- `references` provide traceability links to artifacts (files/URLs) and parent/child statements.
- `validators` provide automated confidence scoring when assumptions claim preconditions (hardware/software/environment) are true.

Why we created them in this project:
- to avoid manual-only review of 100+ items,
- to make score changes reproducible across runs,
- to surface missing evidence early in `--check`/`--validate`,
- to keep requirement chains auditable from EXPECT -> ASSERT -> EVID/ASSUMP.

---

## 3. From theory to practice (current implementation)

This guide is the practical policy for how references are written in TSF items and how they are interpreted at runtime.

References are documented in:
- `docs/TSF/tsf_implementation/TSF_docs/REFERENCES_GUIDE.md`

### Plugin Layout (Why You See 3 Paths)

Current runtime paths:

1. `.dotstop_extensions/` - real source of custom validators/references.
- Runtime reference implementation source: `.dotstop_extensions/references.py`.

2. `localplugins/` - symlink to `.dotstop_extensions` used by TruDAG import conventions.
- Runtime import alias used by TruDAG: `localplugins/references.py`.
- `localplugins` is a symlink to `.dotstop_extensions` (same code, different import path).

3. `docs/TSF/tsf_implementation/.dotstop_extensions` - symlink used when running from `tsf_implementation`.
- `docs/TSF/tsf_implementation/.dotstop_extensions` is a symlink to the same root folder to support runs from inside `tsf_implementation`.
- Runtime execution symlink in TSF working dir: `docs/TSF/tsf_implementation/.dotstop_extensions`.

These are not three independent implementations; they point to the same plugin code.
- So there are three visible paths, but one practical codebase.

Why this exists:

- TruDAG/plugin loading expects `localplugins` import conventions in this workflow.
- TSF scripts run from `docs/TSF/tsf_implementation`, where a local `.dotstop_extensions` path is expected.
- Symlinks keep compatibility without duplicating plugin code.

How theory becomes practice:

1. You write `references:` blocks in item frontmatter (`items/expectations`, `items/assertions`, `items/evidences`, `items/assumptions`).
2. TruDAG loads reference types (`file`, `url`) and resolves them through built-in and plugin classes.
3. Custom URL handling in `.dotstop_extensions/references.py` applies repository-specific safeguards.
4. Resolved reference content feeds hashing/scoring and appears in generated trustable artifacts.

Design decision (April 2026):

- Custom `FileReference` was removed to avoid symbol collision with built-in TruDAG `FileReference`.
- Built-in file behavior remains active, and custom behavior is kept focused on URL reference handling.


---

## 4. Supported reference types

### A. File reference

Use for repository files:

```yaml
references:
- type: file
  path: docs/guides/Power Consumption Analysis.md
```

### B. URL reference

Use for external resources or temporary placeholders:

```yaml
references:
- type: url
  url: https://example.org/report
  description: External benchmark report
```

---

## 5. Per-item rules (EXPECT/ASSERT/EVID/ASSUMP)

### EXPECT

- `references` optional.
- If present, should point to supporting context/docs.
- Should not carry EVID placeholder marker.

### ASSERT

- `references` optional in current workflow.
- Commonly links to related EXPECT and EVID files.
- Should not carry EVID placeholder marker.

### EVID

- `references` required in practice (empty list is invalid for current TruDAG pipeline).
- Can contain temporary placeholder while real evidence is missing.
- Must be replaced by real evidence during sync.

### ASSUMP

- `references` optional (usually context/parent links).
- Should not carry EVID placeholder marker.
- Do not use placeholder marker here; ASSUMP quality is tracked via validator configuration.

---

## 6. Placeholder policy

Official placeholder marker:

- `description: TSF_PLACEHOLDER_EVIDENCE`

Current allowed usage:

- only in EVID references.

Typical placeholder entry:

```yaml
references:
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md
  description: TSF_PLACEHOLDER_EVIDENCE
```

When real evidence is available, sync must replace this placeholder.

---

## 7. Paths and URL conventions

### File paths

- Prefer repository-relative paths for stability.
- Avoid fragile deep relative chains where possible.
- Path must resolve to an existing file.

### URLs

- Use full HTTPS URLs.
- Add `description` when meaning is not obvious.
- Avoid truncated URLs.

---

## 8. Common anti-patterns

1. Duplicate references in the same item.
2. Placeholder marker outside EVID.
3. Empty `references` in EVID.
4. Broken file paths.
5. Mixed inconsistent formatting of entries.
6. Using TODO/TBD in reference entries.

---

## 9. Validation checklist

Before running full validate:

1. Run check:

```bash
source .venv/bin/activate
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check
```

2. Confirm no unintended placeholder references in EXPECT/ASSERT/ASSUMP.

3. Confirm EVID placeholders are only where evidence is genuinely pending.

4. Run sync if needed:

```bash
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --sync
```

---

## 10. Quick examples

### Good EVID reference block

```yaml
references:
- type: file
  path: docs/demos/ota/ota_benchmark_report.md
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/actions/runs/123456
  description: CI run evidence
```

### Good temporary EVID placeholder

```yaml
references:
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md
  description: TSF_PLACEHOLDER_EVIDENCE
score: 0.0
```

### Bad usage (do not do)

```yaml
# Placeholder marker outside EVID item
references:
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md
  description: TSF_PLACEHOLDER_EVIDENCE
```

---

## Owner

SEA:ME Team 6 - TSF maintainers
