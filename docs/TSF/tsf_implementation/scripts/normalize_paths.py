#!/usr/bin/env python3
"""
Normalize path entries in TSF item markdown files.

Rules applied:
- Decode '%20' to space in local `path:` entries.
- Collapse repeated 'docs/docs/...'/multiple 'docs/' sequences to a single 'docs/'.
- For urls: collapse repeated '/blob/main/docs/.../docs/' sequences to single '/blob/main/docs/'.
- For any local path that contains '/src/' ensure it uses 5 ups from items ("../../../../../src/...") to reach repo root.

Usage:
  python3 normalize_paths.py --items-dir docs/TSF/tsf_implementation/items --dry-run

This script can run in dry-run mode to print proposed diffs.
"""
import argparse
import re
from pathlib import Path
import difflib


def normalize_line(line: str) -> str:
    orig = line
    # operate only on lines that contain 'path:'
    if 'path:' not in line:
        return line
    # split into prefix and value
    parts = line.split('path:', 1)
    if len(parts) < 2:
        return line
    prefix, value = parts[0], parts[1]
    val = value.strip()
    # handle URLs separately
    if val.startswith('http://') or val.startswith('https://'):
        # collapse repeated /blob/main/docs/docs/... to single
        val = re.sub(r'(/blob/main/)(?:docs/)+', r'\1docs/', val)
        # also collapse trailing repeated docs/ in any path segment
        val = re.sub(r'(/(?:docs/){2,})', '/docs/', val)
        new = f"{prefix}path: {val}\n"
        if new != orig:
            return new
        return orig

    # local paths: decode %20
    val = val.replace('%20', ' ')
    # collapse repeated docs/ sequences
    val = re.sub(r'(?:docs/){2,}', 'docs/', val)
    # if path contains '/src/' ensure it starts with 5 ups to reach repo root
    if '/src/' in val and not val.startswith('/'):
        # keep trailing after src/
        idx = val.find('/src/')
        trailing = val[idx+1:]
        val = '../../../../../' + trailing

    # if path contains '/TSF/' with repeated docs, collapse (already handled)

    new = f"{prefix}path: {val}\n"
    if new != orig:
        return new
    return orig


def process_file(path: Path, dry_run=True):
    text = path.read_text(encoding='utf-8')
    lines = text.splitlines(keepends=True)
    new_lines = []
    changed = False
    for ln in lines:
        new_ln = normalize_line(ln)
        if new_ln != ln:
            changed = True
        new_lines.append(new_ln)

    if not changed:
        return False, ''

    new_text = ''.join(new_lines)
    if dry_run:
        diff = ''.join(difflib.unified_diff(text.splitlines(True), new_text.splitlines(True), fromfile=str(path), tofile=str(path) + '.normalized'))
        return True, diff
    else:
        path.write_text(new_text, encoding='utf-8')
        return True, ''


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--items-dir', default='docs/TSF/tsf_implementation/items')
    parser.add_argument('--dry-run', action='store_true')
    args = parser.parse_args()

    items_dir = Path(args.items_dir)
    if not items_dir.exists():
        print(f"Items dir not found: {items_dir}")
        raise SystemExit(2)

    changed_any = False
    for md in sorted(items_dir.rglob('*.md')):
        changed, payload = process_file(md, dry_run=args.dry_run)
        if changed:
            changed_any = True
            if args.dry_run:
                print(payload)
            else:
                print(f"Patched: {md}")

    if not changed_any:
        print("No changes proposed.")


if __name__ == '__main__':
    main()
