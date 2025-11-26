#!/usr/bin/env python3
"""Canonicalize evidence references:

- Remove concrete file references (docs/, src/, images, demos, guides) from EXPECT and ASSERT files.
- Ensure ASSERT front-matter references the corresponding EVID item.
- Ensure EXPECT front-matter references the corresponding ASSERT item.
- Add any removed concrete references into the corresponding EVID file body (under a small marker list).

Usage: python3 canonicalize_references.py --items <items_root>
"""
import argparse
import glob
import os
import re
import sys
from pathlib import Path

try:
    import yaml
except Exception:
    print("ERROR: pyyaml is required. Install with: pip3 install pyyaml")
    sys.exit(1)


ARTIFACT_RE = re.compile(r"(?m)^\s*-\s*(?P<path>(?:docs|src|images|docs/demos|docs/guides|docs/images)[^\s\)]*)")
MARKDOWN_LINK_RE = re.compile(r"\[.*?\]\((?P<path>(?:docs|src|images)[^)]+)\)")


def split_front_matter(text):
    if text.startswith("---\n"):
        parts = text.split("---", 2)
        # parts[0] == '' , parts[1] == frontmatter body (with leading newline), parts[2] == rest
        fm = parts[1]
        body = parts[2].lstrip('\n')
        return fm, body
    return None, text


def load_front_matter(fm_text):
    if not fm_text:
        return {}
    try:
        return yaml.safe_load(fm_text) or {}
    except Exception as e:
        print("WARN: failed to parse front matter:\n", e)
        return {}


def dump_front_matter(fm):
    return yaml.safe_dump(fm, sort_keys=False).strip() + "\n"


def find_artifacts(body):
    paths = set()
    for m in ARTIFACT_RE.finditer(body):
        paths.add(m.group('path').strip())
    for m in MARKDOWN_LINK_RE.finditer(body):
        paths.add(m.group('path').strip())
    return sorted(paths)


def ensure_evid_has_artifacts(evid_path: Path, artifacts: list):
    if not evid_path.exists():
        print(f"WARN: missing evid file: {evid_path}")
        return False
    text = evid_path.read_text(encoding='utf-8')
    fm_text, body = split_front_matter(text)
    if fm_text is None:
        fm = {}
    else:
        fm = load_front_matter(fm_text)

    # Place artifacts into body under marker 'Canonicalized evidence artifacts:'
    marker = '\nCanonicalized evidence artifacts:\n'
    lines = []
    for a in artifacts:
        # Avoid duplicates
        if re.search(rf"(?m)^\s*-\s*{re.escape(a)}\s*$", body):
            continue
        lines.append(f"- {a}\n")

    if not lines:
        return True

    if 'Canonicalized evidence artifacts:' in body:
        body = body.replace('Canonicalized evidence artifacts:\n', 'Canonicalized evidence artifacts:\n')
        # append after marker
        body = body + '\n' + ''.join(lines)
    else:
        body = body + '\n\nCanonicalized evidence artifacts:\n' + ''.join(lines)

    # write back
    new_text = '---\n' + (dump_front_matter(fm) if fm else '') + '---\n\n' + body
    evid_path.write_text(new_text, encoding='utf-8')
    return True


def normalize_items(items_root: Path):
    changed = []
    # Process EXPECT and ASSERT files
    for kind in ('expectations', 'assertions'):
        for path in sorted((items_root / kind).glob('*.md')):
            text = path.read_text(encoding='utf-8')
            fm_text, body = split_front_matter(text)
            fm = load_front_matter(fm_text) if fm_text else {}

            artifacts = find_artifacts(body)
            # also inspect front-matter references that are strings containing docs/ or src/
            fm_refs = fm.get('references')
            removed_from_fm = []
            if isinstance(fm_refs, list):
                kept = []
                for r in fm_refs:
                    if isinstance(r, str) and (r.startswith('docs/') or r.startswith('src/') or 'docs/' in r or 'src/' in r):
                        removed_from_fm.append(r)
                    else:
                        kept.append(r)
                if removed_from_fm:
                    fm['references'] = kept
                    artifacts.extend(removed_from_fm)

            if not artifacts:
                continue

            # remove artifact lines from body (matching bullet lines and markdown links)
            new_body = body
            for a in artifacts:
                # remove bullet lines
                new_body = re.sub(rf"(?m)^\s*-\s*{re.escape(a)}\s*$", '', new_body)
                # remove markdown links that point to the artifact
                new_body = re.sub(rf"\[([^\]]+)\]\({re.escape(a)}\)", r"\1", new_body)

            # normalize whitespace (remove blank lines that resulted)
            new_body = re.sub(r"\n{3,}", '\n\n', new_body).strip() + '\n'

            # determine numbers from filename, e.g. EXPECT-L0-1.md or ASSERT-L0-1.md
            m = re.search(r"L0-(\d+)", path.name)
            if not m:
                print(f"WARN: cannot determine id from {path}")
                continue
            n = m.group(1)
            evid_name = f"EVID-L0-{n}"
            evid_path = items_root / 'evidences' / f"{evid_name}.md"

            # ensure ASSERT references the EVID
            if kind == 'assertions':
                refs = fm.get('references') or []
                # if not already referencing evid entry, add dict
                already = False
                for r in refs:
                    if isinstance(r, dict) and (r.get('id') == evid_name or r.get('path','').endswith(f"{evid_name}.md")):
                        already = True
                        break
                if not already:
                    refs.append({'id': evid_name, 'path': f'../evidences/{evid_name}.md'})
                    fm['references'] = refs

            # ensure EXPECT references ASSERT
            if kind == 'expectations':
                assert_name = f"ASSERT-L0-{n}"
                refs = fm.get('references') or []
                already = False
                for r in refs:
                    if isinstance(r, dict) and (r.get('id') == assert_name or r.get('path','').endswith(f"{assert_name}.md")):
                        already = True
                        break
                if not already:
                    refs.append({'id': assert_name, 'path': f'../assertions/{assert_name}.md'})
                    fm['references'] = refs

            # write back file
            new_text = '---\n' + (dump_front_matter(fm) if fm else '') + '---\n\n' + new_body
            path.write_text(new_text, encoding='utf-8')
            changed.append(str(path))

            # add artifacts to evid file
            added_ok = ensure_evid_has_artifacts(evid_path, sorted(set(artifacts)))
            if not added_ok:
                print(f"WARN: failed to add artifacts to {evid_path}")

    return changed


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--items', required=True, help='path to items root (contains expectations/assertions/evidences)')
    args = p.parse_args()

    items_root = Path(args.items)
    if not items_root.exists():
        print('ERROR: items root does not exist:', items_root)
        sys.exit(2)

    changed = normalize_items(items_root)
    if changed:
        print('UPDATED FILES:')
        for c in changed:
            print(' -', c)
    else:
        print('No changes were necessary.')


if __name__ == '__main__':
    main()
