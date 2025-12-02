#!/usr/bin/env python3
"""Cleanup scripts:

- Remove the literal prefix 'Assertion: ' from `header` in all assertion files.
- Remove EVID boilerplate paragraphs from the `text` YAML scalar or body.

Run: python3 clean_asserts_and_evids.py --items <items_root>
"""
from pathlib import Path
import re
import sys
import argparse
import yaml


BOILERPLATE = [
    "The following artifacts are included as references and should be reviewed as evidence:",
    "- Sprint reports describing related activities (sprint1..3).",
    "- Demos and diagrams related to the requirement.",
    "- Guides and analyses related to the decision.",
    "Additional sprint-linked artifacts included as evidence:",
]


def split_front_matter(text: str):
    if text.startswith('---\n'):
        parts = text.split('---', 2)
        fm = parts[1]
        body = parts[2].lstrip('\n')
        return fm, body
    return None, text


def load_fm(fm_text: str):
    if not fm_text:
        return {}
    try:
        return yaml.safe_load(fm_text) or {}
    except Exception as e:
        print('WARN parse fm:', e)
        return {}


def dump_fm(fm: dict):
    return yaml.safe_dump(fm, sort_keys=False).strip() + '\n'


def clean_assert_headers(assert_dir: Path):
    changed = []
    for p in sorted(assert_dir.glob('ASSERT-*.md')):
        txt = p.read_text(encoding='utf-8')
        fm_text, body = split_front_matter(txt)
        fm = load_fm(fm_text) if fm_text else {}
        header = fm.get('header')
        if isinstance(header, str) and header.startswith('Assertion:'):
            new_header = header[len('Assertion:'):].strip()
            fm['header'] = new_header
            new_txt = '---\n' + dump_fm(fm) + '---\n\n' + (body or '')
            p.write_text(new_txt, encoding='utf-8')
            changed.append(str(p))
    return changed


def remove_boilerplate_from_evid(evid_dir: Path):
    changed = []
    # create a regex to remove the boilerplate block where present
    for p in sorted(evid_dir.glob('EVID-*.md')):
        txt = p.read_text(encoding='utf-8')
        fm_text, body = split_front_matter(txt)
        fm = load_fm(fm_text) if fm_text else {}
        modified = False

        # clean YAML `text` scalar if present
        text_field = fm.get('text')
        if isinstance(text_field, str):
            original = text_field
            for marker in BOILERPLATE:
                original = original.replace(marker, '')
            # also remove blank bullet lines and trailing markers
            original = re.sub(r"\n{3,}", '\n\n', original).strip() + '\n'
            if original != text_field:
                fm['text'] = original
                modified = True

        # clean body as fallback
        if body:
            original_body = body
            for marker in BOILERPLATE:
                original_body = original_body.replace(marker, '')
            original_body = re.sub(r"\n{3,}", '\n\n', original_body).strip() + '\n'
            if original_body != body:
                body = original_body
                modified = True

        if modified:
            new_txt = '---\n' + dump_fm(fm) + '---\n\n' + (body or fm.get('text',''))
            p.write_text(new_txt, encoding='utf-8')
            changed.append(str(p))

    return changed


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--items', required=True)
    args = p.parse_args()
    items_root = Path(args.items)
    ass = items_root / 'assertions'
    evid = items_root / 'evidences'
    changed = []
    if ass.exists():
        changed += clean_assert_headers(ass)
    if evid.exists():
        changed += remove_boilerplate_from_evid(evid)
    if changed:
        print('Updated files:')
        for c in changed:
            print(' -', c)
    else:
        print('No changes necessary.')


if __name__ == '__main__':
    main()
