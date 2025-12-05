"""
Fix remaining broken references by applying safe heuristics:
- If a reference has `type: file` and `path` starts with 'http' or 'https', change `type` to 'url'.
- If a reference has `type: file` and `path` resolves to an existing directory, and that directory contains `README.md`, replace the path with `<dir>/README.md`.

This is conservative and only modifies files where the fix is obvious.

Usage:
  .venv/bin/python docs/TSF/tsf_implementation/tools/fix_url_and_dir_refs.py --root . --apply
"""

import argparse
from pathlib import Path
import yaml
import os


def process_file(md_path: Path, root: Path, apply: bool):
    text = md_path.read_text(encoding='utf-8', errors='ignore')
    if not text.startswith('---'):
        return False
    parts = text.split('---')
    if len(parts) < 3:
        return False
    yaml_text = parts[1]
    try:
        data = yaml.safe_load(yaml_text) or {}
    except Exception:
        return False
    refs = data.get('references') or []
    changed = False
    for ref in refs:
        if not isinstance(ref, dict):
            continue
        p = ref.get('path')
        t = ref.get('type')
        if p is None:
            continue
        if isinstance(p, str) and p.startswith(('http://','https://')):
            # if type is file, convert to url
            if t == 'file' or t is None:
                print('Converting to url:', md_path, p)
                ref['type'] = 'url'
                changed = True
            continue
        # resolve path
        resolved = (md_path.parent / p).resolve()
        if resolved.exists() and resolved.is_dir():
            # try README.md
            readme = resolved / 'README.md'
            if readme.exists() and readme.is_file():
                new_rel = os.path.relpath(readme, start=md_path.parent)
                print('Replacing dir ref with README:', md_path, p, '->', new_rel)
                ref['path'] = new_rel
                changed = True
            else:
                # skip - can't safely fix
                continue
    if changed and apply:
        bak = md_path.with_suffix(md_path.suffix + '.bak')
        bak.write_text(text, encoding='utf-8')
        new_yaml = yaml.safe_dump(data, sort_keys=False)
        rest = '---'.join(parts[2:])
        new_text = '---\n' + new_yaml + '---\n' + rest.lstrip('\n')
        md_path.write_text(new_text, encoding='utf-8')
    return changed


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--root', default='.')
    ap.add_argument('--apply', action='store_true')
    args = ap.parse_args()
    root = Path(args.root).resolve()
    modified = []
    for md in root.rglob('*.md'):
        if 'tools' in md.parts:
            continue
        if process_file(md, root, args.apply):
            modified.append(md)
    print('Modified files:', len(modified))
    for m in modified[:200]:
        print(' -', m)
    if args.apply and modified:
        os.system('git add -A')
        os.system("git commit -m 'Fix URL and directory references (automated)' || echo 'No changes to commit'")

if __name__ == '__main__':
    main()
