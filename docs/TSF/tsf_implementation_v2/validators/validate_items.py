#!/usr/bin/env python3
"""Simple validator: checks that each item markdown file has YAML front-matter with uid and type."""
import sys
from pathlib import Path
import yaml

ROOT = Path(__file__).resolve().parents[1] / "items"

def check_file(p: Path):
    text = p.read_text(encoding='utf-8')
    if not text.startswith('---'):
        return f"MISSING_FRONTMATTER: {p.relative_to(ROOT)}"
    try:
        parts = text.split('---')
        fm = parts[1]
        meta = yaml.safe_load(fm)
        if not meta or 'uid' not in meta or 'type' not in meta:
            return f"INVALID_FRONTMATTER: {p.relative_to(ROOT)}"
    except Exception as e:
        return f"PARSE_ERROR: {p.relative_to(ROOT)} -> {e}"
    return None


def main():
    errors = []
    for p in ROOT.rglob('*.md'):
        err = check_file(p)
        if err:
            errors.append(err)
    if errors:
        print('\n'.join(errors))
        sys.exit(2)
    print('OK: all items have front-matter (uid,type)')

if __name__ == '__main__':
    main()
