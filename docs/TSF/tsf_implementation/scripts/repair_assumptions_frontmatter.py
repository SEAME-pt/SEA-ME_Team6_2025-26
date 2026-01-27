#!/usr/bin/env python3
"""Repair malformed ASSUMP-L0-*.md files where front-matter got duplicated.

Strategy:
- For each assumptions file, locate the last closing '---' marker and treat text after it as body.
- Extract id, the last header occurrence, references to EXPECT if present, and reviewers.
- Rebuild a clean YAML front-matter and write the file.
"""
from pathlib import Path
import re
import yaml
import sys


def extract_last(match_iter):
    last = None
    for m in match_iter:
        last = m.group(1)
    return last


def repair(path: Path):
    txt = path.read_text(encoding='utf-8')
    # find all '---' markers at line starts
    markers = [m.start() for m in re.finditer(r"^---\s*$", txt, flags=re.M)]
    if len(markers) < 2:
        return False
    # find position of last '---' line
    last_marker_pos = markers[-1]
    # body is text after that marker line
    # find index of newline after that marker
    body = txt[last_marker_pos+4:].lstrip('\n')

    # extract id
    id_m = re.search(r"^id:\s*(?P<id>\S+)", txt, flags=re.M)
    idv = id_m.group('id') if id_m else path.stem

    # extract last header occurrence (quoted or unquoted)
    hdr_m = re.finditer(r"^header:\s*(?:'(?P<h1>[^']*)'|\"(?P<h2>[^\"]*)\"|(?P<h3>.*))$", txt, flags=re.M)
    header = None
    for m in hdr_m:
        header = (m.group('h1') or m.group('h2') or m.group('h3')).strip()

    if not header:
        header = f'Assumption {idv}'

    # find reference to EXPECT if present
    expect_ref = None
    ex_m = re.search(r"-\s*id:\s*(EXPECT-L0-\d+)", txt)
    if ex_m:
        expect_ref = {'id': ex_m.group(1), 'path': f'../expectations/{ex_m.group(1)}.md'}

    # find reviewers
    reviewers = []
    for m in re.finditer(r"^-\s*name:\s*\"?(?P<name>[^\n\"]+)\"?\s*\n\s*\s*email:\s*\"?(?P<email>[^\n\"]+)\"?", txt, flags=re.M):
        reviewers.append({'name': m.group('name').strip(), 'email': m.group('email').strip()})

    fm = {'id': idv, 'header': header, 'level': None, 'normative': False}
    if expect_ref:
        fm['references'] = [expect_ref]
    if reviewers:
        fm['reviewers'] = reviewers

    new_text = '---\n' + yaml.safe_dump(fm, sort_keys=False).strip() + '\n---\n\n' + body.strip() + '\n'
    path.write_text(new_text, encoding='utf-8')
    return True


def main():
    base = Path(sys.argv[1]) if len(sys.argv) > 1 else Path('.')
    items = Path(base)
    ass_dir = items / 'assumptions'
    if not ass_dir.exists():
        print('assumptions dir missing:', ass_dir)
        return
    changed = []
    for p in sorted(ass_dir.glob('ASSUMP-L0-*.md')):
        try:
            if repair(p):
                changed.append(str(p))
        except Exception as e:
            print('error', p, e)
    if changed:
        print('Repaired assumptions:')
        for c in changed:
            print(' -', c)
    else:
        print('No repairs needed')


if __name__ == '__main__':
    main()
