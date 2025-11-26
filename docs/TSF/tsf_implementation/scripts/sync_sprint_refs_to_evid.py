#!/usr/bin/env python3
"""Sync sprint file references into EVID items only when relevant.

Behavior:
- Remove any `docs/sprints/...` references from EXPECT and ASSERT items.
- For each EVID item, detect artifact paths referenced in its body (docs/, src/, demos, guides, images).
- For each sprint file under `docs/sprints/*.md`, if the sprint contains any of those artifact paths or basenames,
  include that sprint file as a `{'type':'file','path': relpath}` reference in the EVID front-matter.
- Remove sprint references from an EVID if the sprint does not mention any artifacts used by that EVID.

Usage: python3 sync_sprint_refs_to_evid.py --items <items_root> --sprints <sprints_dir>
"""
from pathlib import Path
import re
import sys
import argparse
import yaml
import os


ARTIFACT_PATH_RE = re.compile(r"(?P<path>(?:docs|src)[^\s)\]]+)")


def split_front_matter(text: str):
    if text.startswith('---\n'):
        parts = text.split('---', 2)
        fm = parts[1]
        body = parts[2].lstrip('\n')
        return fm, body
    return None, text


def load_fm(fm_text):
    if not fm_text:
        return {}
    try:
        return yaml.safe_load(fm_text) or {}
    except Exception as e:
        print('WARN: failed to parse front matter:', e)
        return {}


def dump_fm(fm):
    return yaml.safe_dump(fm, sort_keys=False).strip() + '\n'


def find_artifacts(body: str):
    paths = set()
    for m in ARTIFACT_PATH_RE.finditer(body):
        p = m.group('path').strip()
        # ignore references to TSF items themselves
        if p.startswith('docs/TSF/tsf_implementation'):
            continue
        paths.add(p)
    # also include basenames for looser matching
    basenames = {Path(p).name for p in paths}
    return paths, basenames


def sprint_mentions_artifacts(sprint_text: str, artifacts_paths, artifacts_basenames):
    # Match exact artifact path or basename occurrence
    for p in artifacts_paths:
        if p in sprint_text:
            return True
    for b in artifacts_basenames:
        if b and (b in sprint_text):
            return True
    return False


def update_evid_sprints(evid_path: Path, sprint_files, repo_root: Path):
    text = evid_path.read_text(encoding='utf-8')
    fm_text, body = split_front_matter(text)
    fm = load_fm(fm_text) if fm_text else {}

    artifacts_paths, artifacts_basenames = find_artifacts(body)

    desired_sprints = []
    for s in sprint_files:
        sprint_text = s.read_text(encoding='utf-8')
        if sprint_mentions_artifacts(sprint_text, artifacts_paths, artifacts_basenames):
            rel = os.path.relpath(s, start=evid_path.parent).replace('\\', '/')
            desired_sprints.append({'type': 'file', 'path': rel})

    # Collect existing non-sprint refs
    existing_refs = fm.get('references') or []
    non_sprint_refs = [r for r in existing_refs if not (isinstance(r, str) and 'docs/sprints/' in r) and not (isinstance(r, dict) and r.get('path') and 'docs/sprints/' in r.get('path'))]

    # Merge non-sprint refs + desired sprint refs (avoid duplicates)
    merged = list(non_sprint_refs)
    for ds in desired_sprints:
        if not any((isinstance(r, dict) and r.get('path') == ds['path']) for r in merged):
            merged.append(ds)

    # If changed, write back
    if merged != existing_refs:
        fm['references'] = merged
        new_text = '---\n' + dump_fm(fm) + '---\n\n' + body
        evid_path.write_text(new_text, encoding='utf-8')
        return True
    return False


def remove_sprint_refs_from_item(path: Path):
    text = path.read_text(encoding='utf-8')
    fm_text, body = split_front_matter(text)
    if not fm_text:
        return False
    fm = load_fm(fm_text)
    refs = fm.get('references')
    if not refs:
        return False
    new_refs = [r for r in refs if not (isinstance(r, str) and 'docs/sprints/' in r) and not (isinstance(r, dict) and r.get('path') and 'docs/sprints/' in r.get('path'))]
    if new_refs != refs:
        if new_refs:
            fm['references'] = new_refs
        else:
            fm.pop('references', None)
        new_text = '---\n' + dump_fm(fm) + '---\n\n' + body
        path.write_text(new_text, encoding='utf-8')
        return True
    return False


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--items', required=True)
    p.add_argument('--sprints', required=False, default='docs/sprints')
    args = p.parse_args()

    items_root = Path(args.items)
    repo_root = Path(items_root).parents[3]
    sprints_dir = Path(args.sprints)
    if not sprints_dir.is_absolute():
        sprints_dir = repo_root / args.sprints

    sprint_files = sorted(sprints_dir.glob('*.md'))
    changed = []

    # Remove sprint refs from EXPECT and ASSERT
    for kind in ('expectations', 'assertions'):
        for path in sorted((items_root / kind).glob('*.md')):
            if remove_sprint_refs_from_item(path):
                changed.append(str(path))

    # Update EVIDs to include only relevant sprints
    for evid in sorted((items_root / 'evidences').glob('EVID-L0-*.md')):
        if update_evid_sprints(evid, sprint_files, repo_root):
            changed.append(str(evid))

    if changed:
        print('Updated files:')
        for c in changed:
            print(' -', c)
    else:
        print('No changes necessary.')


if __name__ == '__main__':
    main()
