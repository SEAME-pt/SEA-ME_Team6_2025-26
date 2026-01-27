#!/usr/bin/env python3
"""Fix reference types in TSF implementation markdown files.

Rule: if a reference `path` starts with 'https' it is a URL (`type: url`).
Otherwise it is a file (`type: file`).

Usage:
  ./fix_ref_types_https.py --root docs/TSF/tsf_implementation [--apply]

When `--apply` is passed the script will edit files in-place and write
backups with `.bak` extension. Without `--apply` it only reports what
would change (dry-run).
"""
import argparse
from pathlib import Path
import yaml
import sys
import shutil


def load_frontmatter(text: str):
    if not text.startswith('---'):
        return None, None, text
    parts = text.split('---', 2)
    if len(parts) < 3:
        return None, None, text
    # parts: ['', frontmatter, rest]
    return parts[1], yaml.safe_load(parts[1]) or {}, parts[2]


def dump_frontmatter(obj) -> str:
    # Use safe_dump with consistent formatting
    return yaml.safe_dump(obj, sort_keys=False, allow_unicode=True)


def process_file(p: Path, apply_changes: bool = False):
    text = p.read_text(encoding='utf-8', errors='ignore')
    fm_raw, fm_obj, rest = load_frontmatter(text)
    if fm_obj is None:
        return False, None
    refs = fm_obj.get('references')
    if not refs or not isinstance(refs, list):
        return False, None
    changed = False
    for i, r in enumerate(refs):
        if not isinstance(r, dict):
            continue
        path = r.get('path')
        if not isinstance(path, str):
            continue
        # Decide type: 'url' only when it starts with exactly 'https'
        new_type = 'url' if path.startswith('https') else 'file'
        if r.get('type') != new_type:
            r['type'] = new_type
            changed = True
    if not changed:
        return False, None
    # Write back with backup if apply
    if apply_changes:
        bak = p.with_suffix(p.suffix + '.bak')
        try:
            shutil.copy2(p, bak)
        except Exception:
            # if backup fails, abort to avoid data loss
            raise
        # Rebuild frontmatter text
        new_fm = dump_frontmatter(fm_obj)
        new_text = f"---\n{new_fm}---{rest}"
        p.write_text(new_text, encoding='utf-8')
    # Return a compact summary of changes (we'll output filepath and count)
    return True, fm_obj


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--root', required=True, help='Root path to tsf implementation')
    ap.add_argument('--apply', action='store_true', help='Apply changes in-place (creates .bak backups)')
    args = ap.parse_args()
    root = Path(args.root)
    if not root.exists():
        print('Root does not exist:', root, file=sys.stderr)
        sys.exit(2)
    md_files = sorted(root.rglob('*.md'))
    changed_files = []
    for md in md_files:
        try:
            changed, fm = process_file(md, apply_changes=args.apply)
        except Exception as e:
            print(f'ERROR processing {md}: {e}', file=sys.stderr)
            continue
        if changed:
            changed_files.append(str(md))
    print('Files scanned:', len(md_files))
    print('Files changed :', len(changed_files))
    if changed_files:
        print('\nFirst 20 changed files:')
        for f in changed_files[:20]:
            print(f)
    # Exit code 0 even if no changes; non-zero on unexpected fatal earlier


if __name__ == '__main__':
    main()
