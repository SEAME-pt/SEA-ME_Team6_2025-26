#!/usr/bin/env python3
"""Simple front-matter validator that ensures each item has required fields.

Checks for: id/header/text/level/normative
Exits with non-zero code when issues found (suitable for CI).
"""
import sys
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[2] / 'items'
REQUIRED = ['id', 'header', 'text', 'level', 'normative']


def read_frontmatter(p: Path):
    text = p.read_text(encoding='utf-8')
    if not text.strip().startswith('---'):
        return None
    parts = text.split('---')
    if len(parts) < 3:
        return None
    fm = parts[1]
    return fm


def parse_keys(fm: str):
    keys = []
    for line in fm.splitlines():
        m = re.match(r'^\s*([a-zA-Z0-9_-]+)\s*:', line)
        if m:
            keys.append(m.group(1))
    return keys


def main():
    issues = []
    for p in ROOT.rglob('*.md'):
        fm = read_frontmatter(p)
        if fm is None:
            issues.append(f"MISSING_FRONTMATTER: {p}")
            continue
        keys = parse_keys(fm)
        for r in REQUIRED:
            if r not in keys:
                issues.append(f"MISSING_FIELD {r}: {p}")
    if issues:
        print('\n'.join(issues))
        sys.exit(2)
    print('OK: front-matter check passed')

if __name__ == '__main__':
    main()
