#!/usr/bin/env python3
"""Generate ASSUMP items for each EXPECT and normalize level fields.

Usage: python3 generate_assumptions_and_normalize_levels.py --items <items_root>

Behavior:
- For each EXPECT-L0-N, create items/assumptions/ASSUMP-L0-N.md if missing.
- Normalize front-matter `level` values according to mapping:
    EXPECT -> '1.1'
    ASSERT -> '1.2'
    EVID   -> '1.3'
    ASSUMP -> '1.4'

This script is conservative: it will not overwrite existing ASSUMP files.
It will update `level` in-place for all existing items of the four kinds.
"""
from pathlib import Path
import sys
import argparse
import re
import yaml
import textwrap


def split_front_matter(text: str):
    if text.startswith('---\n'):
        parts = text.split('---', 2)
        fm = parts[1]
        body = parts[2].lstrip('\n')
        return fm, body
    return None, text


def load_fm(fm_text):
    if not fm_text:
        return {}
    return yaml.safe_load(fm_text) or {}


def dump_fm(fm):
    return yaml.safe_dump(fm, sort_keys=False).strip() + '\n'


def ensure_assumptions(items_root: Path):
    expect_dir = items_root / 'expectations'
    assump_dir = items_root / 'assumptions'
    assump_dir.mkdir(parents=True, exist_ok=True)
    created = []
    for exp in sorted(expect_dir.glob('EXPECT-L0-*.md')):
        m = re.search(r'L0-(\d+)', exp.name)
        if not m:
            continue
        n = m.group(1)
        assump_name = f'ASSUMP-L0-{n}.md'
        assump_path = assump_dir / assump_name
        if assump_path.exists():
            continue
        # read expectation to seed assumption
        txt = exp.read_text(encoding='utf-8')
        fm_text, body = split_front_matter(txt)
        fm = load_fm(fm_text) if fm_text else {}
        header = fm.get('header', f'Assumption for L0-{n}')
        # create conservative assumption text
        text = textwrap.dedent(f"""
        Assumption related to {exp.name}.

        Seeded from the expectation header: {header}

        Please refine this assumption with project-specific details (hardware availability, toolchain, datasets, timelines, etc.).
        """)
        assump_fm = {
            'id': f'ASSUMP-L0-{n}',
            'header': f'Assumption: {header}',
            'level': '1.4',
            'normative': False,
            'references': [{'id': f'EXPECT-L0-{n}', 'path': f'../expectations/EXPECT-L0-{n}.md'}],
            'reviewers': [{'name': 'Joao Jesus Silva', 'email': 'joao.silva@seame.pt'}]
        }
        new_text = '---\n' + dump_fm(assump_fm) + '---\n\n' + text.strip() + '\n'
        assump_path.write_text(new_text, encoding='utf-8')
        created.append(str(assump_path))
    return created


def normalize_levels(items_root: Path):
    mapping = {
        'expectations': '1.1',
        'assertions': '1.2',
        'evidences': '1.3',
        'assumptions': '1.4',
    }
    changed = []
    for kind, level in mapping.items():
        d = items_root / kind
        if not d.exists():
            continue
        for p in sorted(d.glob('*.md')):
            txt = p.read_text(encoding='utf-8')
            fm_text, body = split_front_matter(txt)
            if not fm_text:
                fm = {}
            else:
                fm = load_fm(fm_text)
            old = fm.get('level')
            if str(old) != level:
                fm['level'] = level
                new_txt = '---\n' + dump_fm(fm) + '---\n\n' + (body or '')
                p.write_text(new_txt, encoding='utf-8')
                changed.append(str(p))
    return changed


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--items', required=True)
    args = p.parse_args()
    items_root = Path(args.items)
    if not items_root.exists():
        print('items root missing:', items_root)
        sys.exit(2)

    created = ensure_assumptions(items_root)
    changed = normalize_levels(items_root)
    if created:
        print('Created assumptions:')
        for c in created:
            print(' -', c)
    if changed:
        print('Normalized levels in:')
        for c in changed:
            print(' -', c)
    if not created and not changed:
        print('No changes necessary.')


if __name__ == '__main__':
    main()
