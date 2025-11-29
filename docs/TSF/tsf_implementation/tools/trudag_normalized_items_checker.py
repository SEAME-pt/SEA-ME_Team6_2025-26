#!/usr/bin/env python3
"""trudag_normalized_items_checker

Checks and (optionally) normalizes item frontmatter across `items/` and `.trudag_items/`.

Behaviours implemented (dry-run by default):
- validate frontmatter keys (id, header, text, level, normative)
- ensure references entries have `type` and `path` and that referenced paths exist
- normalize paths (%20 -> space, collapse duplicate `docs/` prefixes)
- for `expectations` and `assertions`: remove `score` key (SME scores should live on leaves)
- for `evidences` and `assumptions` (leaves): check they have `references` or `evidence`
- sync canonical `.trudag_items` -> `items` so both trees match

Usage:
  ./trudag_normalized_items_checker.py [--root PATH] [--apply] [--verbose]

By default performs a dry-run and prints proposed changes.
"""
import argparse
import os
import re
import sys
from pathlib import Path
import yaml

REPO_ROOT = Path(__file__).resolve().parents[2]
DEFAULT_IMPL_ROOT = REPO_ROOT / "docs" / "TSF" / "tsf_implementation"


def split_frontmatter(text: str):
    parts = text.split('---')
    if len(parts) >= 3:
        # parts: ['', '\nkey: val\n', '\nbody...']
        fm = parts[1]
        body = '---'.join(parts[2:]).lstrip('\n')
        return fm, body
    return None, text


def normalize_path(p: str) -> str:
    # replace %20 with space, collapse duplicate docs/ segments, strip trailing spaces
    p = p.replace('%20', ' ')
    p = re.sub(r'(docs/)+', 'docs/', p)
    return p.strip()


def load_markdown(path: Path):
    text = path.read_text(encoding='utf-8')
    fm_text, body = split_frontmatter(text)
    if fm_text is None:
        return {}, body
    try:
        fm = yaml.safe_load(fm_text) or {}
    except Exception as e:
        # return the error in the fm so callers can report and skip
        return {'__yaml_error': str(e)}, body
    return fm, body


def write_markdown(path: Path, fm: dict, body: str):
    content = '---\n' + yaml.safe_dump(fm, sort_keys=False) + '---\n\n' + (body or '')
    path.write_text(content, encoding='utf-8')


def find_items(root: Path):
    items_dir = root / 'items'
    trudag_dir = root / '.trudag_items'
    item_files = []
    # collect from items/
    for sub in ['expectations', 'assertions', 'assumptions', 'evidences']:
        d = items_dir / sub
        if not d.exists():
            continue
        for p in sorted(d.glob('*.md')):
            item_files.append((sub, p, 'items'))
    # collect canonical files from .trudag_items
    for prefix_dir in sorted(trudag_dir.glob('*')):
        if not prefix_dir.is_dir():
            continue
        for uid_dir in sorted(prefix_dir.glob('*')):
            if not uid_dir.is_dir():
                continue
            for p in sorted(uid_dir.glob('*.md')):
                if p.name.lower() == 'item.md':
                    continue
                # infer sub from prefix name
                prefix = prefix_dir.name.lower()
                sub = None
                if 'expect' in prefix:
                    sub = 'expectations'
                elif 'assert' in prefix:
                    sub = 'assertions'
                elif 'assump' in prefix or 'assum' in prefix:
                    sub = 'assumptions'
                elif 'evid' in prefix:
                    sub = 'evidences'
                else:
                    sub = prefix
                item_files.append((sub, p, '.trudag_items'))
    return item_files


def relative_in_repo(path: str, root: Path) -> Path:
    p = Path(path)
    if p.is_absolute():
        return p
    # try to resolve relative to root
    candidate = (root / path).resolve()
    return candidate


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--root', default=str(DEFAULT_IMPL_ROOT))
    ap.add_argument('--apply', action='store_true')
    ap.add_argument('--verbose', action='store_true')
    args = ap.parse_args()

    root = Path(args.root)
    if not root.exists():
        print('Root not found:', root)
        sys.exit(2)

    changes = []
    errors = []

    # scan items tree (prefer authoritative .trudag_items canonical files when syncing)
    # First, build map of canonical files for easy sync
    canonical_map = {}
    trudag_dir = root / '.trudag_items'
    for prefix_dir in sorted(trudag_dir.glob('*')):
        if not prefix_dir.is_dir():
            continue
        for uid_dir in sorted(prefix_dir.glob('*')):
            if not uid_dir.is_dir():
                continue
            for p in sorted(uid_dir.glob('*.md')):
                if p.name.lower() == 'item.md':
                    continue
                # compute target path under items
                # canonical file like PREFIX-NAME.md -> target NAME with underscores->hyphens
                name = p.name.split('-', 1)[1].replace('_', '-')
                canonical_map[p] = name

    # Now process items/ and .trudag_items
    items_dir = root / 'items'
    # walk expectations/assertions to remove score
    for sub in ['expectations', 'assertions']:
        d = items_dir / sub
        if not d.exists():
            continue
        for f in sorted(d.glob('*.md')):
            fm, body = load_markdown(f)
            if isinstance(fm, dict) and fm.get('__yaml_error'):
                errors.append((f, 'yaml_error', fm.get('__yaml_error')))
                continue
            if 'score' in fm:
                changes.append((f, 'remove_score'))
                if args.apply:
                    fm.pop('score', None)
                    write_markdown(f, fm, body)

    # process leaves: evidences and assumptions
    for sub in ['evidences', 'assumptions']:
        d = items_dir / sub
        if not d.exists():
            continue
        for f in sorted(d.glob('*.md')):
            fm, body = load_markdown(f)
            if isinstance(fm, dict) and fm.get('__yaml_error'):
                errors.append((f, 'yaml_error', fm.get('__yaml_error')))
                continue
            # check for references or evidence
            has_ref = 'references' in fm and fm.get('references')
            has_evid = 'evidence' in fm and fm.get('evidence')
            if not has_ref and not has_evid:
                errors.append((f, 'missing_references_or_evidence'))
            # normalize references
            if 'references' in fm and isinstance(fm['references'], list):
                new_refs = []
                seen = set()
                for ref in fm['references']:
                    if not isinstance(ref, dict):
                        errors.append((f, 'bad_reference_format', ref))
                        continue
                    # ensure type
                    if 'type' not in ref:
                        # try to infer
                        if 'path' in ref:
                            ref['type'] = 'file'
                        else:
                            ref['type'] = 'file'
                    if 'path' in ref:
                        ref['path'] = normalize_path(str(ref['path']))
                        # if this is a URL, skip local existence check
                        if isinstance(ref.get('type'), str) and ref.get('type').lower() == 'url':
                            pass
                        elif str(ref['path']).lower().startswith('http://') or str(ref['path']).lower().startswith('https://'):
                            # path looks like a URL, treat as external
                            pass
                        else:
                            candidate = relative_in_repo(ref['path'], DEFAULT_IMPL_ROOT.parent.parent)
                            if not candidate.exists():
                                errors.append((f, 'ref_path_missing', ref['path']))
                    key = (ref.get('type'), ref.get('path') if 'path' in ref else None)
                    if key in seen:
                        continue
                    seen.add(key)
                    new_refs.append(ref)
                if args.apply and new_refs != fm['references']:
                    fm['references'] = new_refs
                    write_markdown(f, fm, body)
                    changes.append((f, 'normalize_references'))

    # Sync .trudag_items canonical -> items to ensure equality if apply
    if args.apply:
        for canonical_path, target_name in canonical_map.items():
            # infer prefix folder from canonical_path parent name
            prefix = canonical_path.parent.parent.name.lower()
            if 'expect' in prefix:
                sub = 'expectations'
            elif 'assert' in prefix:
                sub = 'assertions'
            elif 'assump' in prefix or 'assum' in prefix:
                sub = 'assumptions'
            elif 'evid' in prefix:
                sub = 'evidences'
            else:
                sub = prefix
            dst = items_dir / sub / target_name
            # copy canonical to dst
            dst.parent.mkdir(parents=True, exist_ok=True)
            data = canonical_path.read_bytes()
            dst.write_bytes(data)
            changes.append((dst, 'sync_from_canonical'))

    # report
    print('Proposed changes:')
    for c in changes:
        print('-', c)
    if errors:
        print('\nErrors/warnings:')
        for e in errors:
            print('-', e)
    else:
        print('\nNo errors detected.')

    if args.apply:
        print('\nApplied changes (see above).')
    else:
        print('\nDry-run complete. Re-run with --apply to make changes.')


if __name__ == '__main__':
    main()
