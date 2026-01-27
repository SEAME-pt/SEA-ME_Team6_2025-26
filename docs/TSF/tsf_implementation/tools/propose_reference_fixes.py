#!/usr/bin/env python3
"""Propose fixes for missing reference `path` entries in TSF items.

This script performs a dry-run only: it scans `items/` and `.trudag_items/`
markdown frontmatter for `references` entries that include a `path` value,
checks whether the referenced file exists (resolved relative to the repository
root), and when missing, searches the repository for files with the same
basename and proposes a relative path replacement.

Usage:
  ./propose_reference_fixes.py --root PATH

Output: prints one proposal per line as: <file>: <old_path> -> <proposed_new_path>
"""
import argparse
import os
from pathlib import Path
import yaml


def split_frontmatter(text: str):
    parts = text.split('---')
    if len(parts) >= 3:
        fm = parts[1]
        body = '---'.join(parts[2:]).lstrip('\n')
        return fm, body
    return None, text


def load_fm(path: Path):
    text = path.read_text(encoding='utf-8')
    fm_text, body = split_frontmatter(text)
    if fm_text is None:
        return {}
    try:
        return yaml.safe_load(fm_text) or {}
    except Exception:
        return {}


def find_candidates(repo_root: Path, basename: str):
    # search under repo root for files matching basename (case-sensitive)
    results = []
    for p in repo_root.rglob(basename):
        if p.is_file():
            results.append(p)
    return results


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--root', default='.')
    args = ap.parse_args()

    repo_root = Path(args.root).resolve()
    impl_root = repo_root / 'docs' / 'TSF' / 'tsf_implementation'
    if not impl_root.exists():
        print('Implementation root not found:', impl_root)
        return

    search_dirs = [impl_root / 'items', impl_root / '.trudag_items']
    proposals = []

    for base in search_dirs:
        if not base.exists():
            continue
        for md in base.rglob('*.md'):
            try:
                fm = load_fm(md)
            except Exception:
                continue
            refs = fm.get('references') if isinstance(fm.get('references'), list) else []
            for ref in refs:
                if not isinstance(ref, dict):
                    continue
                if 'path' not in ref:
                    continue
                old = str(ref['path']).strip()
                # resolve relative to repo root if relative
                cand = (repo_root / old).resolve() if not Path(old).is_absolute() else Path(old)
                if cand.exists():
                    continue
                # file missing: search repo for basename
                basename = Path(old).name
                candidates = find_candidates(repo_root, basename)
                if not candidates:
                    proposals.append((md, old, None))
                else:
                    # choose shortest path candidate
                    candidates = sorted(candidates, key=lambda p: len(str(p)))
                    best = candidates[0]
                    # compute relative path from the md file's directory to the candidate
                    rel = os.path.relpath(best, start=md.parent)
                    proposals.append((md, old, rel))

    # Print proposals
    if not proposals:
        print('No missing reference paths detected.')
        return
    print('Proposed reference path fixes (dry-run):')
    for md, old, new in proposals:
        if new is None:
            print(f"{md}: {old} -> [NO MATCH FOUND]")
        else:
            print(f"{md}: {old} -> {new}")


if __name__ == '__main__':
    main()
