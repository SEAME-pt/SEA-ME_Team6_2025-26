# Traceability matrix (TSF implementation)

This is a lightweight traceability matrix used by TSF validators. It lists requirement IDs, linked code modules, tests and evidence references.

| Requirement | Code | Tests | Evidence |
|-------------|------|-------|----------|
| L0-001 | src/cluster/main.cpp, src/cluster/* | tests/manual_drive_tests | refs: `references/evidences/EVID-L0-001.md` |
| L0-002 | src/cluster/* | tests/traceability | refs: `references/evidences/EVID-L0-002.md` |
| L0-003 | src/* | tests/traceability | refs: `references/evidences/EVID-L0-003.md` |
| L0-004 | src/* | tests/ | refs: `references/evidences/EVID-L0-004.md` |
| L0-005 | src/* | tests/ | refs: `references/evidences/EVID-L0-005.md` |

Update this file as real links/artifacts are created. CI validators should verify the table format and presence of expected entries.
