#!/usr/bin/env python3
"""Normalize item `level` fields to doc-number-based: 1.<n> for L0-n items.

For each file under expectations/, assertions/, evidences/, assumptions/ named *L0-<n>*.md,
set `level: 1.<n>` in the front-matter.
"""
from pathlib import Path
import re
import sys
import argparse
import yaml


def split_fm(text: str):
    if text.startswith('---\n'):
        parts = text.split('---', 2)
        return parts[1], parts[2].lstrip('\n')
    return None, text


def load_fm_text(fm_text: str):
    if not fm_text:
        return {}
    return yaml.safe_load(fm_text) or {}


def dump_fm(fm: dict):
    return yaml.safe_dump(fm, sort_keys=False).strip() + '\n'


def normalize_dir(d: Path):
    changed = []
    if not d.exists():
        return changed
    for p in sorted(d.glob('*.md')):
        m = re.search(r'L0-(\d+)', p.name)
        if not m:
            continue
        n = m.group(1)
        target_level = f'1.{n}'
        txt = p.read_text(encoding='utf-8')
        fm_text, body = split_fm(txt)
        fm = load_fm_text(fm_text) if fm_text else {}
        old = fm.get('level')
        if str(old) != target_level:
            fm['level'] = target_level
            new = '---\n' + dump_fm(fm) + '---\n\n' + (body or '')
            p.write_text(new, encoding='utf-8')
            changed.append(str(p))
    return changed


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--items', required=True)
    args = p.parse_args()
    items = Path(args.items)
    if not items.exists():
        print('items root missing:', items)
        sys.exit(2)
    all_changed = []
    for sub in ('expectations', 'assertions', 'evidences', 'assumptions'):
        all_changed += normalize_dir(items / sub)
    if all_changed:
        print('Updated levels in:')
        for c in all_changed:
            print(' -', c)
    else:
        print('No changes necessary.')


if __name__ == '__main__':
    main()
