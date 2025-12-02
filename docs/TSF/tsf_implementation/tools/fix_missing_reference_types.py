#!/usr/bin/env python3
"""Fix missing `type` in references across items and .trudag_items.

Sets `type: url` for paths starting with http(s), otherwise `type: file`.
"""
import sys
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
    if 'references' in fm and isinstance(fm['references'], list):
        new_refs = []
        for ref in fm['references']:
            if isinstance(ref, dict):
                if 'type' not in ref:
                    pth = str(ref.get('path',''))
                    if pth.lower().startswith('http'):
                        ref['type'] = 'url'
                    else:
                        ref['type'] = 'file'
                    changed = True
            new_refs.append(ref)
        fm['references'] = new_refs
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
            # skip item.md inside .trudag_items
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
