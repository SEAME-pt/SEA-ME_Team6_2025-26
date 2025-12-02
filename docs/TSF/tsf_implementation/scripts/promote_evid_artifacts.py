#!/usr/bin/env python3
"""Promote artifact bullet lists in EVID bodies into front-matter references.

For each EVID-L0-*.md file:
- Find bullet lines in the body that reference repository artifacts (docs/, src/, docs/demos/, docs/guides/, docs/images/, images/).
- Remove those bullets from the body.
- Add them to the front-matter `references` list as dicts: {'type':'file','path': '<relpath>'} where <relpath> is relative to the EVID file.

Usage: python3 promote_evid_artifacts.py --items <items_root>
"""
from pathlib import Path
import re
import os
import sys
import argparse
import yaml


ART_BULLET_RE = re.compile(r"(?m)^\s*-\s*(?P<path>(?:docs|src|images)[^\n\r]*)$")
MD_LINK_RE = re.compile(r"\[([^\]]+)\]\((?P<path>(?:docs|src|images)[^)]+)\)")


def split_front_matter(text: str):
    if text.startswith('---\n'):
        parts = text.split('---', 2)
        fm = parts[1]
        body = parts[2].lstrip('\n')
        return fm, body
    return None, text


def load_fm(fm_text: str):
    if not fm_text:
        return {}
    try:
        return yaml.safe_load(fm_text) or {}
    except Exception as e:
        print('WARN: failed to parse front matter:', e)
        return {}


def dump_fm(fm: dict):
    return yaml.safe_dump(fm, sort_keys=False).strip() + '\n'


def find_artifact_paths(body: str):
    paths = []
    # bullets containing paths
    for m in ART_BULLET_RE.finditer(body):
        p = m.group('path').strip()
        paths.append(p)
    # markdown links
    for m in MD_LINK_RE.finditer(body):
        p = m.group('path').strip()
        paths.append(p)
    return [p.replace('\\', '/').strip() for p in paths]


def promote_for_evid(evid_path: Path, repo_root: Path):
    text = evid_path.read_text(encoding='utf-8')
    fm_text, body = split_front_matter(text)
    fm = load_fm(fm_text) if fm_text else {}

    # gather artifacts from both the body and the YAML `text` scalar (if present)
    artifact_paths = find_artifact_paths(body)
    text_field = fm.get('text')
    if isinstance(text_field, str):
        # extract artifact-like paths from the text field as well
        text_artifacts = find_artifact_paths(text_field)
        for a in text_artifacts:
            if a not in artifact_paths:
                artifact_paths.append(a)

    if not artifact_paths:
        return False

    # remove bullet lines and markdown links for these artifacts from body
    new_body = body
    for p in artifact_paths:
        new_body = re.sub(rf"(?m)^\s*-\s*{re.escape(p)}\s*$", '', new_body)
        new_body = re.sub(rf"\[([^\]]+)\]\({re.escape(p)}\)", r"\1", new_body)

    # also remove the artifact lines from the YAML `text` field if present
    if isinstance(text_field, str):
        new_text_field = text_field
        for p in artifact_paths:
            new_text_field = re.sub(rf"(?m)^\s*-\s*{re.escape(p)}\s*$", '', new_text_field)
            new_text_field = re.sub(rf"\[([^\]]+)\]\({re.escape(p)}\)", r"\1", new_text_field)
        # normalize whitespace
        new_text_field = re.sub(r"\n{3,}", '\n\n', new_text_field).strip() + '\n'
        # update or remove the YAML `text` scalar
        if new_text_field.strip():
            fm['text'] = new_text_field
        else:
            fm.pop('text', None)

    # normalize whitespace
    new_body = re.sub(r"\n{3,}", '\n\n', new_body).strip() + '\n'

    # prepare references list
    existing = fm.get('references') or []
    # non-file refs preserved
    non_file_refs = [r for r in existing if not (isinstance(r, dict) and r.get('path') and ('docs/' in r.get('path') or 'src/' in r.get('path') or 'images/' in r.get('path')))]

    # build file refs
    file_refs = []
    for p in artifact_paths:
        abs_p = (repo_root / p).resolve()
        rel = os.path.relpath(abs_p, start=evid_path.parent).replace('\\', '/')
        entry = {'type': 'file', 'path': rel}
        if not any(isinstance(r, dict) and r.get('path') == entry['path'] for r in existing):
            file_refs.append(entry)

    merged = non_file_refs + file_refs
    fm['references'] = merged

    new_text = '---\n' + dump_fm(fm) + '---\n\n' + new_body
    evid_path.write_text(new_text, encoding='utf-8')
    return True


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--items', required=True)
    args = p.parse_args()

    items_root = Path(args.items)
    if not items_root.exists():
        print('items root missing:', items_root)
        sys.exit(2)

    repo_root = Path(items_root).parents[3]
    changed = []
    for evid in sorted((items_root / 'evidences').glob('EVID-L0-*.md')):
        try:
            if promote_for_evid(evid, repo_root):
                changed.append(str(evid))
        except Exception as e:
            print('ERROR processing', evid, e)

    if changed:
        print('Updated evid files:')
        for c in changed:
            print(' -', c)
    else:
        print('No evid artifact bullets found to promote.')


if __name__ == '__main__':
    main()
