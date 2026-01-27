#!/usr/bin/env python3
"""Compare `docs/TSF/requirements/LO_requirements.md` Evidence column with EVID-L0-*.md files.

Outputs a concise report:
- items present in EVID but missing in table
- items present in table but missing in EVID

Run from repo root:
  python3 docs/TSF/tsf_implementation/scripts/compare_table_vs_evids.py
"""
import re
from pathlib import Path
import os
from urllib.parse import unquote

LO_REQ = Path('docs/TSF/requirements/LO_requirements.md')
EVID_DIR = Path('docs/TSF/tsf_implementation/items/evidences')


def extract_table_evidence():
    text = LO_REQ.read_text(encoding='utf-8')
    # split rows starting with | **L0-
    rows = re.split(r"\n(?=\| \*\*L0-)", text)
    table = {}
    for r in rows:
        m = re.match(r"\| \*\*(L0-(\d+))\*\*\s*\|.*\|.*\|.*\|(.+?)\|\n", r, re.S)
        if m:
            lid = m.group(2)
            evidence_cell = m.group(3)
            # find backtick-enclosed paths and also urls
            paths = set()
            for bt in re.findall(r'`([^`]+)`', evidence_cell):
                bn = os.path.basename(bt)
                paths.add(unquote(bn))
            for url in re.findall(r'https?://[^\s<]+', evidence_cell):
                bn = os.path.basename(url.split('?',1)[0].rstrip('/'))
                paths.add(unquote(bn))
            table[lid] = paths
    return table


def extract_evid_refs():
    evid_map = {}
    for p in sorted(EVID_DIR.glob('EVID-L0-*.md')):
        lid = p.stem.split('-')[-1]
        txt = p.read_text(encoding='utf-8')
        refs = set()
        # find path: lines
        for m in re.findall(r'path:\s*(.+)', txt):
            val = m.strip().strip('"').strip("'")
            # if it's a url, get basename after blob/main or last segment
            if val.startswith('http'):
                bn = os.path.basename(val.split('?',1)[0].rstrip('/'))
            else:
                bn = os.path.basename(val)
            if bn:
                refs.add(unquote(bn))
        evid_map[lid] = refs
    return evid_map


def main():
    table = extract_table_evidence()
    evid = extract_evid_refs()

    all_lids = set(list(table.keys()) + list(evid.keys()))
    any_diff = False
    for lid in sorted(all_lids, key=lambda x:int(x)):
        t = table.get(lid, set())
        e = evid.get(lid, set())
        only_in_e = sorted(e - t)
        only_in_t = sorted(t - e)
        if only_in_e or only_in_t:
            any_diff = True
            print(f'L0-{lid}:')
            if only_in_e:
                print('  - present in EVID but missing in table:')
                for x in only_in_e:
                    print('    -', x)
            if only_in_t:
                print('  - present in table but missing in EVID:')
                for x in only_in_t:
                    print('    -', x)
    if not any_diff:
        print('Table evidence and EVID-L0-* references match for all L0 items.')

if __name__ == "__main__":
    main()
