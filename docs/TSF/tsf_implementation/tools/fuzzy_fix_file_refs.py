#!/usr/bin/env python3
"""Fuzzy-fix unresolved `type: file` references under TSF implementation.

This script searches for close basename matches in the repository using
difflib.get_close_matches and applies replacements when there's a single
high-confidence candidate. It is conservative: only applies when a single
close match with cutoff is found.

Usage: fuzzy_fix_file_refs.py --root <root> [--apply] [--cutoff 0.8]
"""
from pathlib import Path
import argparse
import yaml
import difflib
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


def build_basename_index(repo_root: Path):
    files = [p for p in repo_root.rglob('*') if p.is_file()]
    index = {}
    for p in files:
        name = p.name
        index.setdefault(name, []).append(p)
    return index


def normalize_name(name: str):
    # Lowercase, replace %20, spaces -> '-', collapse underscores
    n = name.replace('%20', ' ')
    n = n.replace(' ', '-').replace('_', '-').lower()
    return n


def all_basenames(index):
    return list(index.keys())


def find_best_match(basename: str, index, cutoff=0.8):
    # Try exact normalized match first
    norm = normalize_name(basename)
    # Build normalized map once
    norm_map = {}
    for name in index:
        norm_map.setdefault(normalize_name(name), []).append(name)
    if norm in norm_map:
        # If norm maps to exactly one file basename, return that
        candidates = []
        for nm in norm_map[norm]:
            candidates.extend(index.get(nm, []))
        if len(candidates) == 1:
            return candidates[0]
        # if multiple, prefer exact original name
        if basename in index and len(index[basename]) == 1:
            return index[basename][0]
    # Use difflib on actual basenames
    names = all_basenames(index)
    matches = difflib.get_close_matches(basename, names, n=5, cutoff=cutoff)
    # If no matches, try on normalized names
    if not matches:
        matches = difflib.get_close_matches(norm, names, n=5, cutoff=cutoff)
    if len(matches) == 1:
        name = matches[0]
        # if there are multiple paths for same basename, don't choose
        if len(index.get(name, [])) == 1:
            return index[name][0]
    return None


def process_file(md: Path, repo_root: Path, index, cutoff=0.8, apply=False):
    text = md.read_text(encoding='utf-8', errors='ignore')
    fm_raw, fm_obj, rest = load_frontmatter(text)
    if fm_obj is None:
        return []
    refs = fm_obj.get('references') or []
    if not isinstance(refs, list):
        return []
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
        match = find_best_match(basename, index, cutoff=cutoff)
        if match is None:
            # Try slight variant: remove trailing digits before extension
            stem = Path(basename).stem
            import re
            m = re.match(r'^(.*?)(\d+)$', stem)
            if m:
                alt = m.group(1) + Path(basename).suffix
                match = find_best_match(alt, index, cutoff=cutoff)
        if match is not None:
            rel = os.path.relpath(match, start=md.parent)
            rel = rel.replace('\\', '/')
            old = r.get('path')
            r['path'] = rel
            changes.append((str(md), old, rel, str(match)))
    if changes and apply:
        new_fm = dump_frontmatter(fm_obj)
        new_text = f"---\n{new_fm}---{rest}"
        md.write_text(new_text, encoding='utf-8')
    return changes


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--root', required=True)
    ap.add_argument('--apply', action='store_true')
    ap.add_argument('--cutoff', type=float, default=0.8)
    args = ap.parse_args()
    root = Path(args.root)
    repo_root = Path('.').resolve()
    index = build_basename_index(repo_root)
    md_files = sorted(root.rglob('*.md'))
    total = []
    for md in md_files:
        if '.trudag_items' in str(md) and md.suffix == '.md':
            try:
                changes = process_file(md, repo_root, index, cutoff=args.cutoff, apply=args.apply)
            except Exception as e:
                print('ERROR', md, e, file=sys.stderr)
                continue
            total.extend(changes)
    print('Files scanned:', len(md_files))
    print('Fuzzy changes applied/proposed:', len(total))
    for src, old, new, target in total:
        print(f'{src}\n  - {old} -> {new}\n    target: {target}')


if __name__ == '__main__':
    main()
