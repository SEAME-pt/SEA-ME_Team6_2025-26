#!/usr/bin/env python3
"""
Simple validators for TSF implementation.

This script provides a couple of small checks used by CI before running trudag:
- verify referenced files exist
- compute SHA256 for referenced files (helpful for reproducibility)

Run: python3 validators.py --dotstop ./.dotstop.dot --items-dir docs/TSF/tsf_implementation/items
"""
import argparse
import hashlib
import json
import os
from pathlib import Path


def sha256_of_file(path: Path) -> str:
    h = hashlib.sha256()
    with path.open('rb') as f:
        for chunk in iter(lambda: f.read(8192), b''):
            h.update(chunk)
    return h.hexdigest()


def find_referenced_files(items_dir: Path):
    files = []
    for md in items_dir.rglob('*.md'):
        text = md.read_text(encoding='utf-8')
        # naive parse: look for lines like 'path: ../../../../docs/...'
        for line in text.splitlines():
            line = line.strip()
            if line.startswith('path:'):
                parts = line.split('path:', 1)
                if len(parts) > 1:
                    p = parts[1].strip()
                    # ignore urls
                    if p.startswith('http://') or p.startswith('https://') or p.startswith('git'):
                        continue
                    files.append((md, p))
    return files


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--items-dir', default='docs/TSF/tsf_implementation/items')
    parser.add_argument('--dotstop', default='.dotstop.dot')
    parser.add_argument('--dry-run', action='store_true')
    args = parser.parse_args()

    items_dir = Path(args.items_dir)
    if not items_dir.exists():
        print(f"Items dir not found: {items_dir}")
        raise SystemExit(2)

    refs = find_referenced_files(items_dir)
    report = []
    for md, relpath in refs:
        # normalize relative paths: join from the md parent
        candidate = (md.parent / relpath).resolve()
        exists = candidate.exists()
        sha = sha256_of_file(candidate) if exists and candidate.is_file() else None
        report.append({
            'item': str(md),
            'ref_path': relpath,
            'resolved': str(candidate),
            'exists': exists,
            'sha256': sha,
        })

    out = {'dotstop': args.dotstop, 'checks': report}
    print(json.dumps(out, indent=2))
    # exit non-zero if missing files found
    missing = [r for r in report if not r['exists']]
    if missing:
        print(f"Missing referenced files: {len(missing)}")
        raise SystemExit(3)


if __name__ == '__main__':
    main()
