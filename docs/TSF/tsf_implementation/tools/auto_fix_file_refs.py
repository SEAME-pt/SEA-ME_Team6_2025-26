#!/usr/bin/env python3
"""Auto-fix unresolved file references by basename matches.

For each markdown under the provided `--root`, find references declared as
`type: file` whose `path` does not resolve to an existing regular file.
For each such reference, search the repository for files with the same
basename. If exactly one match is found, replace the `path` with a
relative path from the markdown file and leave `type: file`.

This script will NOT create backups (per user request). Use with care.

Usage: auto_fix_file_refs.py --root <root> [--apply]
Without `--apply` it runs in dry-run mode and prints proposed changes.
"""
from pathlib import Path
import argparse
import yaml
import sys
import os


def load_frontmatter(text: str):
    if not text.startswith('---'):
        return None, None, text
    parts = text.split('---', 2)
    if len(parts) < 3:
        return None, None, text
    return parts[1], yaml.safe_load(parts[1]) or {}, parts[2]


def dump_frontmatter(obj) -> str:
    return yaml.safe_dump(obj, sort_keys=False, allow_unicode=True)


def find_unique_match(basename: str, repo_root: Path):
    matches = []
    # Walk repository, skip .git and typical virtualenvs
    for p in repo_root.rglob(basename):
        if '.git' in p.parts:
            continue
        if p.is_file():
            matches.append(p)
    if len(matches) == 1:
        return matches[0]
    return None


def compute_relpath(from_path: Path, to_path: Path) -> str:
    # Return POSIX-style relative path
    return os.path.relpath(to_path, start=from_path)


def process_file(md: Path, repo_root: Path, apply: bool = False):
    text = md.read_text(encoding='utf-8', errors='ignore')
    fm_raw, fm_obj, rest = load_frontmatter(text)
    if fm_obj is None:
        return []
    refs = fm_obj.get('references') or []
    changes = []
    for r in refs:
        if not isinstance(r, dict):
            continue
        typ = r.get('type')
        path = r.get('path')
        if typ != 'file' or not isinstance(path, str):
            continue
        resolved = (md.parent / path).resolve()
        if resolved.exists() and resolved.is_file():
            continue
        basename = Path(path).name
        match = find_unique_match(basename, repo_root)
        if match is None:
            continue
        # Compute relative path from md.parent to match
        rel = compute_relpath(md.parent, match)
        # Normalize to use posix separators in frontmatter
        rel_norm = rel.replace('\\', '/')
        changes.append((str(md), path, rel_norm))
        if apply:
            r['path'] = rel_norm
    if apply and changes:
        new_fm = dump_frontmatter(fm_obj)
        new_text = f"---\n{new_fm}---{rest}"
        md.write_text(new_text, encoding='utf-8')
    return changes


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--root', required=True, help='Root path to tsf implementation')
    ap.add_argument('--apply', action='store_true', help='Apply changes in-place (NO backups)')
    args = ap.parse_args()
    root = Path(args.root)
    if not root.exists():
        print('root does not exist', root, file=sys.stderr)
        sys.exit(2)
    repo_root = Path('.').resolve()
    md_files = sorted(root.rglob('*.md'))
    all_changes = []
    for md in md_files:
        try:
            changes = process_file(md, repo_root, apply=args.apply)
        except Exception as e:
            print('ERROR', md, e, file=sys.stderr)
            continue
        all_changes.extend(changes)
    print('Files scanned:', len(md_files))
    print('Proposed changes:', len(all_changes))
    for src, old, new in all_changes[:200]:
        print(f'{src}\n  - {old} -> {new}')


if __name__ == '__main__':
    main()
