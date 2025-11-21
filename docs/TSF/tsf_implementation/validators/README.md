Validator stubs for TSF implementation

This folder contains small validator scripts that help verify evidence references and basic constraints.

Files:
- check_evidence_paths.py: scans reference markdown files and checks that referenced file paths exist (simple, naive search).

How to run:
  python validators/check_evidence_paths.py

Notes:
- These are lightweight helpers. For production-grade validation, integrate with unit tests or CI steps that run validators and report results to Trustable/TruDAG.
