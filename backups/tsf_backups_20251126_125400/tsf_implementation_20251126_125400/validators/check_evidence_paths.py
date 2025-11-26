#!/usr/bin/env python3
"""
Simple validator: scan markdown files under references/ and check referenced file paths exist.
It looks for YAML front-matter `path:` entries under `references:` items and checks them.
Run: python validators/check_evidence_paths.py
"""

import re
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
REFS = ROOT / "references"

md_files = list(REFS.rglob("*.md"))
if not md_files:
    print("No markdown reference files found.")
    sys.exit(0)

path_re = re.compile(r"path:\s*\"?([^\"\n]+)\"?")

errors = 0
for md in md_files:
    text = md.read_text(encoding="utf-8")
    # naive scan for path: "..." occurrences
    for m in path_re.finditer(text):
        rel = m.group(1).strip()
        # normalize relative paths: treat paths starting with .. as relative to md parent
        candidate = (md.parent / rel).resolve()
        if not candidate.exists():
            print(f"MISSING: {md.relative_to(ROOT.parent)} -> {rel} -> resolved {candidate}")
            errors += 1

if errors:
    print(f"Found {errors} missing referenced files.")
    sys.exit(2)
else:
    print("All referenced paths exist (by simple check).")
    sys.exit(0)
