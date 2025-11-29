#!/usr/bin/env python3
"""Normalize `score` types in frontmatter: ensure floats or mapping of str->float.

Converts integer scalar scores (e.g. 1) to float (1.0) and mapping values to floats.
"""
from pathlib import Path
import yaml

ROOT = Path(__file__).resolve().parents[2] / 'tsf_implementation'

def split_frontmatter(text: str):
    parts = text.split('---')
    if len(parts) >= 3:
        fm = parts[1]
        body = '---'.join(parts[2:]).lstrip('\n')
        return fm, body
    return None, text

def process_file(p: Path):
    text = p.read_text(encoding='utf-8')
    fm_text, body = split_frontmatter(text)
    if fm_text is None:
        return False
    try:
        fm = yaml.safe_load(fm_text) or {}
    except Exception:
        return False
    changed = False
    if 'score' in fm:
        s = fm['score']
        if isinstance(s, int):
            fm['score'] = float(s)
            changed = True
        elif isinstance(s, dict):
            newm = {}
            for k, v in s.items():
                if isinstance(v, int):
                    newm[k] = float(v)
                    changed = True
                else:
                    newm[k] = v
            fm['score'] = newm
    if changed:
        content = '---\n' + yaml.safe_dump(fm, sort_keys=False) + '---\n\n' + (body or '')
        p.write_text(content, encoding='utf-8')
    return changed

def walk_and_fix(root: Path):
    count = 0
    for sub in ['items', '.trudag_items']:
        base = root / sub
        if not base.exists():
            continue
        for p in base.rglob('*.md'):
            if sub == '.trudag_items' and p.name.lower() == 'item.md':
                continue
            try:
                if process_file(p):
                    print('fixed', p)
                    count += 1
            except Exception as e:
                print('error', p, e)
    print('fixed total', count)

if __name__ == '__main__':
    walk_and_fix(ROOT)
