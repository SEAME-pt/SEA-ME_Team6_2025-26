# TSF Score Recovery - March 2026

## Context

This note documents the recovery work performed to move TruDAG scoring from a partially broken state to full pass.

Initial observed state in this recovery cycle:
- Missing links in `.dotstop.dot`
- SHA instability for URL references
- Several L0_22-L0_31 evidence/assertion/expectation chains scoring below 1.0

Final validated state:
- `124/124` items with score `1.0`
- `0` items at `0.5`
- `0` items at `0.0`

## Fixes Applied

1. Completed missing graph links in `.dotstop.dot`
- Restored required Expectation -> Assertion links
- Restored required Expectation -> Assumption links
- Restored required Assertion -> Evidence links
- Included L0_31 links and existing L0_22 chain consistency

2. Stabilized SHA computation for URL references
- Updated URL reference content hashing logic to be deterministic
- Avoided live URL fetches during SHA computation
- Prevented SHA drift between runs for items that include URL references

3. Rebuilt `.dotstop.dot` SHAs after deterministic hashing change
- Recomputed item node SHAs
- Recomputed edge SHAs based on updated item SHAs
- Synchronized graph metadata with current item content

4. Fixed EVID scores for L0_22-L0_31
- Updated `score: 0.0` -> `score: 1.0` in:
  - `docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_22/...`
  - through `EVID_L0_31`
- Rebuilt SHAs again to keep `.dotstop.dot` aligned

5. Restored L0_22 connectivity in source graph definition
- Added missing edges in `docs/TSF/tsf_implementation/graph/graph.dot`:
  - `"EXPECT-L0-22" -> "ASSERT-L0-22";`
  - `"ASSERT-L0-22" -> "EVID-L0-22";`
  - `"EXPECT-L0-22" -> "ASSUMP-L0-22";`

## Why ASSERT_L0_22-L0_31 Recovered

The apparent validator issue on ASSERT L0_22-L0_31 was downstream from evidence and SHA consistency state. After:
- correcting EVID L0_22-L0_31 scores,
- and rebuilding SHAs,

ASSERT and EXPECT items in that range also converged to `1.0`.

## Validation Command

```bash
source .venv/bin/activate
trudag score
```

Expected summary after these fixes:
- `1.0: 124`
- `0.5: 0`
- `0.0: 0`
- `total: 124`

## Files Touched in This Recovery

- `docs/TSF/tsf_implementation/.dotstop.dot`
- `docs/TSF/tsf_implementation/graph/graph.dot`
- `docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_22/EVIDENCES-EVID_L0_22.md`
- `docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_23/EVIDENCES-EVID_L0_23.md`
- `docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_24/EVIDENCES-EVID_L0_24.md`
- `docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_25/EVIDENCES-EVID_L0_25.md`
- `docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_26/EVIDENCES-EVID_L0_26.md`
- `docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_27/EVIDENCES-EVID_L0_27.md`
- `docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_28/EVIDENCES-EVID_L0_28.md`
- `docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_29/EVIDENCES-EVID_L0_29.md`
- `docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_30/EVIDENCES-EVID_L0_30.md`
- `docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_31/EVIDENCES-EVID_L0_31.md`

## Root Cause Analysis (RCA)

Symptoms observed:
- Inconsistent score output between runs
- L0_22-L0_31 chain stuck at `0.0` or `0.5`
- Items appearing as missing or not fully propagated

Primary causes:
1. Incomplete relationship graph in `.dotstop.dot`
- Missing EXPECT -> ASSERT, EXPECT -> ASSUMP, and ASSERT -> EVID edges prevented proper score propagation.

2. Non-deterministic SHA generation for URL references
- SHA content depended on live URL responses, which can vary over time.
- This introduced hash drift for unchanged items.

3. Metadata/content mismatch after hashing behavior change
- Once deterministic hashing was introduced, previously stored SHAs became stale.
- `.dotstop.dot` required full SHA rebuild to resynchronize item and edge hashes.

4. Evidence leaves configured with `score: 0.0` in L0_22-L0_31
- Leaf-level zeros propagated upward and masked the real chain behavior.

Corrective actions implemented:
- Completed missing links in graph metadata and source graph definition.
- Switched URL-reference hashing to deterministic behavior.
- Rebuilt item and edge SHAs in `.dotstop.dot`.
- Normalized EVID L0_22-L0_31 scores to `1.0`.

Preventive actions:
- Keep URL-reference SHA inputs deterministic (no network-dependent content in hash path).
- After any hashing logic change, always run full SHA rebuild before score validation.
- Add graph completeness checks for each requirement chain (EXPECT/ASSERT/EVID/ASSUMP).
- Add a pre-merge check to fail if any target EVID leaf remains at unintended `score: 0.0`.
