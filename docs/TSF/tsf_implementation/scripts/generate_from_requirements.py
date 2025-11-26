#!/usr/bin/env python3
"""
Generate Trustable-format items (EXPECT / ASSERT / EVID) from LO_requirements.md

Usage:
  python3 generate_from_requirements.py --input ../../requirements/LO_requirements.md

Features:
- Parses a simple markdown table of L0 requirements and creates EXPECT-L0-XXX.md items
  in `items/expectations/` (upstream format: id, header, text, level, normative, references, reviewers).
- Creates skeleton ASSERT and EVID items for each expectation.
- Attempts to auto-discover evidence files by scanning project folders for keyword matches.
- Optionally runs `trudag` publish flow if `--run-trudag` is provided and `TRUDAG_AUTO_PUBLISH` env var is set.

This script is conservative: it will not overwrite existing items unless `--overwrite` is passed.
"""

import argparse
import re
from pathlib import Path
import shutil
import hashlib
import os

ROOT = Path(__file__).resolve().parents[3]
REQ = ROOT / 'docs' / 'TSF' / 'requirements' / 'LO_requirements.md'
OUT = ROOT / 'docs' / 'TSF' / 'tsf_implementation' / 'items'
SCANNED_DIRS = [ROOT / 'docs' / 'sprints', ROOT / 'docs' / 'demos', ROOT / 'docs' / 'guides', ROOT / 'docs' / 'images', ROOT / 'src']

RE_L0 = re.compile(r"\|\s*\*\*(L0-\d+)\*\*\s*\|\s*(.+?)\s*\|")

TEMPLATE_EXPECT = """---
id: {id}
header: "{header}"
text: |
  {text}
level: 1.1
normative: true
references:
{references}
reviewers:
  - name: "TODO"
    email: "TODO@example.com"
---

"""

TEMPLATE_ASSERT = """---
id: ASSERT-{l0}
header: "{header}"
text: |
  Assertion supporting {id}.
level: 1.2
normative: true
references:
  - type: "file"
    path: "../expectations/{id}.md"
reviewers:
  - name: "TODO"
    email: "TODO@example.com"
---

"""

TEMPLATE_EVID = """---
id: EVID-{l0}
header: "L0 Evidence: {short}"
text: |
  Evidence files for {id}.
level: 1.1
normative: false
references:
{references}
reviewers:
  - name: "TODO"
    email: "TODO@example.com"
---

"""


def sha256_of_file(p: Path):
    h = hashlib.sha256()
    with p.open('rb') as f:
        while True:
            data = f.read(8192)
            if not data:
                break
            h.update(data)
    return h.hexdigest()


def find_evidence_files(keywords):
    results = []
    for d in SCANNED_DIRS:
        if not d.exists():
            continue
        for p in d.rglob('*'):
            if p.is_file():
                name = p.name.lower()
                # simple heuristic: match any keyword in filename or parent folder
                if any(kw.lower() in name for kw in keywords[:5]):
                    rel = p.relative_to(ROOT)
                    results.append(rel.as_posix())
    return results[:5]


def format_refs(paths):
    if not paths:
        return "  # no references found"
    s = ''
    for p in paths:
        s += f"  - type: \"file\"\n    path: \"{p}\"\n"
    return s


def parse_requirements(mdpath: Path):
    text = mdpath.read_text(encoding='utf-8')
    items = []
    for m in RE_L0.finditer(text):
        rid = m.group(1)
        desc = m.group(2).strip()
        items.append((rid, desc))
    return items


def write_if_missing(path: Path, content: str, overwrite=False):
    if path.exists() and not overwrite:
        print(f"SKIP (exists): {path}")
        return False
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding='utf-8')
    print(f"WROTE: {path}")
    return True


def make_items(requirements, overwrite=False, run_trudag=False):
    created = []
    for idx, (rid, desc) in enumerate(requirements, start=1):
        num = rid.split('-')[1]
        uid = f"EXPECT-{rid}"
        header = f"{rid} {desc[:50]}"
        # find evidence candidates
        keywords = re.findall(r"\w{4,}", desc)
        evidence_files = find_evidence_files(keywords)
        refs = format_refs(evidence_files)
        expect_path = OUT / 'expectations' / f"{uid}.md"
        expect_content = TEMPLATE_EXPECT.format(id=uid, header=header.replace('"','\"'), text=desc, references=refs)
        wrote_e = write_if_missing(expect_path, expect_content, overwrite=overwrite)

        # assertion
        assert_path = OUT / 'assertions' / f"ASSERT-{rid}.md"
        assert_content = TEMPLATE_ASSERT.format(l0=rid, header=f"Assertion for {rid}", id=uid)
        wrote_a = write_if_missing(assert_path, assert_content, overwrite=overwrite)

        # evidence item
        evid_path = OUT / 'evidences' / f"EVID-{rid}.md"
        evid_refs = refs if evidence_files else '  # no references found'
        evid_content = TEMPLATE_EVID.format(l0=rid, short=desc[:60].replace('\n',' '), id=uid, references=evid_refs)
        wrote_ev = write_if_missing(evid_path, evid_content, overwrite=overwrite)

        created.append((uid, expect_path, assert_path, evid_path, evidence_files))
    return created


def run_trudag_flow():
    # runs a basic trudag flow — requires trudag in PATH
    print('Running trudag init/manage/publish flow...')
    os.system('trudag init')
    # create items from files
    for p in (OUT / 'expectations').glob('*.md'):
        os.system(f'trudag manage create-item "{p}" || true')
    for p in (OUT / 'assertions').glob('*.md'):
        os.system(f'trudag manage create-item "{p}" || true')
    for p in (OUT / 'evidences').glob('*.md'):
        os.system(f'trudag manage create-item "{p}" || true')
    # TODO: create links based on graph/graph.dot or simple EX->ASSERT->EVID
    os.system('trudag manage lint || true')
    os.system('trudag score || true')
    os.system('trudag publish || true')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--input', '-i', type=str, default=str(REQ))
    parser.add_argument('--overwrite', action='store_true')
    parser.add_argument('--run-trudag', action='store_true')
    args = parser.parse_args()

    md = Path(args.input)
    if not md.exists():
        print(f"Requirements file not found: {md}")
        raise SystemExit(2)

    reqs = parse_requirements(md)
    if not reqs:
        print('No L0 requirements found in table format. Look for lines like | **L0-1** | ... |')
        raise SystemExit(1)

    created = make_items(reqs, overwrite=args.overwrite, run_trudag=args.run_trudag)
    print('\nCreated items:')
    for c in created:
        uid, expect_path, assert_path, evid_path, evidence_files = c
        print(f"- {uid}: expectation={expect_path}, assertion={assert_path}, evidence={evid_path}")
        if evidence_files:
            print(f"  matched evidence: {evidence_files}")

    if args.run_trudag or os.getenv('TRUDAG_AUTO_PUBLISH') == 'true':
        run_trudag_flow()
