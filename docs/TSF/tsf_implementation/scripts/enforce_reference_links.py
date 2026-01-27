#!/usr/bin/env python3
"""Ensure EXPECT -> ASSERT and ASSERT -> EVID references exist for L0 items.

This script does not move artifacts; it only enforces the YAML front-matter
references pattern described by the project policy:

- EXPECT-L0-N references ASSERT-L0-N (path: ../assertions/ASSERT-L0-N.md)
- ASSERT-L0-N references EVID-L0-N (path: ../evidences/EVID-L0-N.md)

It preserves existing non-file references and avoids duplicates.
"""
import sys
from pathlib import Path
import re
import yaml


def split_front_matter(text):
    if text.startswith('---\n'):
        parts = text.split('---', 2)
        fm = parts[1]
        body = parts[2].lstrip('\n')
        return fm, body
    return None, text


def load_fm(fm_text):
    if not fm_text:
        return {}
    return yaml.safe_load(fm_text) or {}


def dump_fm(fm):
    return yaml.safe_dump(fm, sort_keys=False).strip() + '\n'


def ensure_ref_list(fm):
    refs = fm.get('references')
    if refs is None:
        refs = []
    return refs


def has_ref(refs, kind, target_id=None, target_path=None):
    for r in refs:
        if isinstance(r, dict):
            if target_id and r.get('id') == target_id:
                return True
            if target_path and r.get('path') and r.get('path').endswith(target_path):
                return True
        elif isinstance(r, str):
            if target_path and target_path in r:
                return True
    return False


def process_items(items_root: Path):
    changes = []
    exp_dir = items_root / 'expectations'
    ast_dir = items_root / 'assertions'
    evid_dir = items_root / 'evidences'

    for exp_file in sorted(exp_dir.glob('EXPECT-L0-*.md')):
        m = re.search(r'L0-(\d+)', exp_file.name)
        if not m:
            continue
        n = m.group(1)
        assert_name = f'ASSERT-L0-{n}'
        assert_path = f'../assertions/{assert_name}.md'

        # load EXPECT
        txt = exp_file.read_text(encoding='utf-8')
        fm_text, body = split_front_matter(txt)
        fm = load_fm(fm_text) if fm_text else {}
        refs = ensure_ref_list(fm)

        if not has_ref(refs, 'assert', target_id=assert_name, target_path=assert_path):
            refs.append({'id': assert_name, 'path': assert_path})
            fm['references'] = refs
            new_text = '---\n' + dump_fm(fm) + '---\n\n' + body
            exp_file.write_text(new_text, encoding='utf-8')
            changes.append(str(exp_file))

        # ensure ASSERT references EVID
        ast_file = ast_dir / f'{assert_name}.md'
        evid_name = f'EVID-L0-{n}'
        evid_path_rel = f'../evidences/{evid_name}.md'
        if ast_file.exists():
            a_txt = ast_file.read_text(encoding='utf-8')
            a_fm_text, a_body = split_front_matter(a_txt)
            a_fm = load_fm(a_fm_text) if a_fm_text else {}
            a_refs = ensure_ref_list(a_fm)
            if not has_ref(a_refs, 'evid', target_id=evid_name, target_path=evid_path_rel):
                a_refs.append({'type': 'file', 'path': evid_path_rel, 'id': evid_name})
                a_fm['references'] = a_refs
                new_a_text = '---\n' + dump_fm(a_fm) + '---\n\n' + a_body
                ast_file.write_text(new_a_text, encoding='utf-8')
                changes.append(str(ast_file))
        else:
            # create minimal assertion if missing
            new_a_fm = {
                'id': assert_name,
                'header': f'Assertion auto-created for {assert_name}',
                'level': '1.2',
                'normative': True,
                'references': [{'type': 'file', 'path': evid_path_rel, 'id': evid_name}],
            }
            new_a_text = '---\n' + dump_fm(new_a_fm) + '---\n\n' + 'Auto-created assertion. Please review and expand.' + '\n'
            ast_file.write_text(new_a_text, encoding='utf-8')
            changes.append(str(ast_file))

    return changes


def main():
    if len(sys.argv) != 2 and not ('--items' in sys.argv):
        print('Usage: enforce_reference_links.py --items <items_root>')
        sys.exit(2)
    import argparse
    p = argparse.ArgumentParser()
    p.add_argument('--items', required=True)
    args = p.parse_args()
    items_root = Path(args.items)
    if not items_root.exists():
        print('items root missing:', items_root)
        sys.exit(2)
    changes = process_items(items_root)
    if changes:
        print('Updated files:')
        for c in changes:
            print(' -', c)
    else:
        print('No changes necessary.')


if __name__ == '__main__':
    main()
