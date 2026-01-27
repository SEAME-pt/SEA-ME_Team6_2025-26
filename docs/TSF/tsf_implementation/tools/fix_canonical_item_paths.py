#!/usr/bin/env python3
"""Fix canonical `.trudag_items` reference paths that incorrectly point into
`.trudag_items/items/...` by rewriting them to point to the real `items/...` files.

Behavior:
 - Scan markdown files under `<root>/.trudag_items`.
 - For each `references` entry with `type: file` whose resolved path either
   a) does not exist as a regular file, or
   b) resolves inside a path containing `/.trudag_items/`
   try to find a unique matching file by basename under `<root>/items`.
 - If exactly one match is found, replace the `path` with a relative path from
   the referencing file to that `items/...` target and update the file.

This script writes changes in-place (NO backups) per user preference.
"""
from pathlib import Path
import argparse
import yaml
import os
import sys


def load_frontmatter(text: str):
    if not text.startswith('---'):
        return None, None, text
    parts = text.split('---', 2)
    if len(parts) < 3:
        return None, None, text
    return parts[1], yaml.safe_load(parts[1]) or {}, parts[2]


def dump_frontmatter(obj) -> str:
    return yaml.safe_dump(obj, sort_keys=False, allow_unicode=True)


def compute_relpath(from_path: Path, to_path: Path) -> str:
    rel = os.path.relpath(to_path, start=from_path)
    return rel.replace('\\', '/')


def find_matches_by_basename(basename: str, items_root: Path):
    return [p for p in items_root.rglob(basename) if p.is_file()]


def process_file(md: Path, root: Path, apply: bool = False):
    text = md.read_text(encoding='utf-8', errors='ignore')
    fm_raw, fm_obj, rest = load_frontmatter(text)
    if fm_obj is None:
        return []
    refs = fm_obj.get('references') or []
    if not isinstance(refs, list):
        # if it's not a list, skip (avoid making assumptions)
        return []
    changed = []
    items_root = root / 'items'
    for r in refs:
        if not isinstance(r, dict):
            continue
        typ = r.get('type')
        path = r.get('path')
        if typ != 'file' or not isinstance(path, str):
            continue
        resolved = (md.parent / path).resolve()
        # Condition: target doesn't exist or resolves under .trudag_items
        if (resolved.exists() and resolved.is_file()) and ('.trudag_items' not in str(resolved)):
            continue
        # Try to find match under items_root
        basename = Path(path).name
        matches = find_matches_by_basename(basename, items_root)
        if len(matches) == 1:
            target = matches[0]
            rel = compute_relpath(md.parent, target)
            # ensure relative path uses posix separators
            old = r.get('path')
            if old != rel:
                r['path'] = rel
                changed.append((str(md), old, rel))
    if changed and apply:
        new_fm = dump_frontmatter(fm_obj)
        new_text = f"---\n{new_fm}---{rest}"
        md.write_text(new_text, encoding='utf-8')
    return changed


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--root', default='docs/TSF/tsf_implementation', help='TSF implementation root')
    ap.add_argument('--apply', action='store_true', help='Apply changes in-place')
    args = ap.parse_args()
    root = Path(args.root)
    if not root.exists():
        print('root does not exist:', root, file=sys.stderr)
        sys.exit(2)
    md_files = sorted((root / '.trudag_items').rglob('*.md'))
    total_changes = []
    for md in md_files:
        if md.name.endswith('.bak'):
            continue
        try:
            changes = process_file(md, root, apply=args.apply)
        except Exception as e:
            print('ERROR', md, e, file=sys.stderr)
            continue
        total_changes.extend(changes)
    print('Files scanned (canonical):', len(md_files))
    print('Changes proposed/applied:', len(total_changes))
    for src, old, new in total_changes:
        print(f'{src}\n  - {old} -> {new}')


if __name__ == '__main__':
    main()
