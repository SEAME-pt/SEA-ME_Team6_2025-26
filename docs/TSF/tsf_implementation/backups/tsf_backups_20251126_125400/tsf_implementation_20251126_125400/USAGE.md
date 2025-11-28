TSF Implementation - Usage

This document covers how to work locally with the `tsf_implementation` folder.

1) Create a virtual environment and install Trustable (trudag):

   python -m venv .venv
   . .venv/bin/activate
   pip install --upgrade pip
   pip install trustable

2) Initialize Trudag database (local):

   trudag --init

3) Dry-run create items (prints commands):

   docs/TSF/tsf_implementation/scripts/create_tsf_items.sh.disabled

4) To actually run the create commands, set DO_TRUDAG=1 (only after reviewing):

   DO_TRUDAG=1 docs/TSF/tsf_implementation/scripts/create_tsf_items.sh.disabled

5) Run validators:

   python docs/TSF/tsf_implementation/validators/check_evidence_paths.py

6) CI: See .github/workflows/tsf-ci.yml — it runs `trudag manage lint` and `trudag score`.

Notes:
- I will not run `trudag publish` without your explicit confirmation (it may require secrets/access).
- Files in `docs/TSF/requirements/` were read but not modified.
