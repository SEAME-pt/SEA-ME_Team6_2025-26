"""
Apply safe reference path fixes from a proposals file produced by
`propose_reference_fixes.py`.

Usage:
  .venv/bin/python docs/TSF/tsf_implementation/tools/apply_reference_fixes.py \
      --proposals /tmp/ref_proposals.txt --root . --apply

Behavior:
- Parses lines of the form: "<md_path>: <old_path> -> <new_path>"
- Skips lines where new_path == "[NO MATCH FOUND]"
- Resolves <md_path> to an existing file (tries as-is, then searches workspace by basename)
- Loads YAML frontmatter, updates `references[*].path` entries that exactly match old_path
- Writes a backup `.bak` for each modified file and updates the file in-place
- Prints a summary of modified files
"""

import argparse
import os
import re
import sys
import yaml
from pathlib import Path


def parse_line(line):
    # Try to parse: <path>: <old> -> <new>
    if '->' not in line:
        return None
    try:
        left, rest = line.split(':', 1)
        if '->' not in rest:
            return None
        old, new = rest.split('->', 1)
        return left.strip(), old.strip(), new.strip()
    except Exception:
        return None


def find_file(root: Path, maybe_path: str):
    p = Path(maybe_path)
    # If it's absolute or relative and exists, return it
    if p.exists():
        return p
    # Try relative to root
    cand = root / maybe_path
    if cand.exists():
        return cand
    # Try basename search across repo (first match)
    base = Path(maybe_path).name
    for f in root.rglob(base):
        # prefer exact name match
        if f.is_file():
            return f
    return None


def load_frontmatter(text: str):
    # Return (yaml_obj, start_idx, end_idx, indent) or (None, None, None, None)
    m = re.match(r"^(---\s*\n)(.*?)(\n---\s*\n)(.*)$", text, re.S)
    if not m:
        return None, None, None, None
    pre = m.group(1)
    yaml_block = m.group(2)
    post_delim = m.group(3)
    rest = m.group(4)
    return yaml.safe_load(yaml_block), 0, len(pre) + len(yaml_block) + len(post_delim), yaml_block


def dump_frontmatter(yaml_obj):
    # Dump YAML with safe formatting
    return yaml.safe_dump(yaml_obj, sort_keys=False)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--proposals', required=True)
    ap.add_argument('--root', default='.')
    ap.add_argument('--apply', action='store_true', help='Write changes; otherwise dry-run')
    args = ap.parse_args()

    root = Path(args.root).resolve()
    proposals = Path(args.proposals)
    if not proposals.exists():
        print('Proposals file not found:', proposals)
        sys.exit(2)

    modified = []
    skipped = 0
    lines = proposals.read_text(encoding='utf-8', errors='replace').splitlines()
    for ln in lines:
        parsed = parse_line(ln)
        if not parsed:
            continue
        md_path_raw, old_path, new_path = parsed
        if new_path == '[NO MATCH FOUND]':
            skipped += 1
            continue
        # Clean potential trailing absolute path added by previous tee (drop after whitespace + /Volumes/...)
        # Many proposal lines had an appended absolute path; remove anything after two or more spaces then '/'
        md_path = md_path_raw.split('\t')[0].split('  ')[0].strip()
        target = find_file(root, md_path)
        if target is None:
            # try basename only
            target = find_file(root, Path(md_path).name)
        if target is None:
            print('Could not resolve file for proposal, skipping:', md_path_raw)
            skipped += 1
            continue

        text = target.read_text(encoding='utf-8')
        # Find YAML frontmatter
        if not text.startswith('---'):
            print('No YAML frontmatter in', target)
            skipped += 1
            continue
        # Split frontmatter
        parts = text.split('---')
        if len(parts) < 3:
            print('Unexpected frontmatter structure in', target)
            skipped += 1
            continue
        # parts[0] is empty before first '---', parts[1] is yaml, parts[2:] is rest (may contain additional '---')
        yaml_text = parts[1]
        try:
            data = yaml.safe_load(yaml_text) or {}
        except Exception as e:
            print('YAML parse error in', target, e)
            skipped += 1
            continue

        changed = False
        refs = data.get('references') or []
        if isinstance(refs, list):
            for r in refs:
                if not isinstance(r, dict):
                    continue
                v = r.get('path')
                if v is None:
                    continue
                # Match exact equality or trailing match
                if v == old_path or v.endswith(old_path):
                    print(f'Will update {target}: {v} -> {new_path}')
                    r['path'] = new_path
                    changed = True
        if not changed:
            # No change to this file
            continue

        if args.apply:
            bak = target.with_suffix(target.suffix + '.bak')
            bak.write_text(text, encoding='utf-8')
            # Reconstruct file: '---' + dumped yaml + '---' + rest
            new_yaml = dump_frontmatter(data)
            rest = '---'.join(parts[2:])
            # Ensure single newline separation
            new_text = '---\n' + new_yaml + '---\n' + rest.lstrip('\n')
            target.write_text(new_text, encoding='utf-8')
            modified.append(str(target))
        else:
            modified.append(str(target) + ' (dry-run)')

    print('\nSummary:')
    print('  proposals processed:', len([l for l in lines if parse_line(l)]))
    print('  skipped (no-match or unresolved):', skipped)
    print('  files modified/affected:', len(modified))
    for m in modified[:200]:
        print('   -', m)

    if args.apply and modified:
        # create a git commit
        print('\nStaging changes and creating a commit...')
        os.system('git add -A')
        os.system("git commit -m 'Apply automated safe reference fixes (script)' || echo 'No changes to commit'")


if __name__ == '__main__':
    main()
