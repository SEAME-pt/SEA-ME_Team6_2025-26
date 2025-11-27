#!/usr/bin/env python3
"""Ensure every EVID has both `type: file` and `type: url` for each referenced artifact.

Algorithm:
- Parse front-matter of each EVID-L0-*.md file.
- Collect existing `file` and `url` entries.
- For each `file` entry missing a matching `url`, add a url using the repo base + relative path.
- For each `url` entry missing a matching `file`, derive the repo-relative path from the URL (strip '/blob/main/') and add a file entry.
- Write updates in-place and print a concise report.
"""
import os
import re
from pathlib import Path

REPO_URL_BASE = "https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main"


def read(path):
    return Path(path).read_text(encoding='utf-8')


def write(path, text):
    Path(path).write_text(text, encoding='utf-8')


def parse_front_matter(text):
    m = re.match(r'(---\n.*?\n---\n)', text, re.S)
    if not m:
        return None, text
    fm = m.group(1)
    rest = text[len(fm):]
    return fm, rest


def extract_references(fm_text):
    refs = []
    lines = fm_text.splitlines()
    in_refs = False
    cur = None
    for l in lines:
        if l.strip().startswith('references:'):
            in_refs = True
            continue
        if not in_refs:
            continue
        if l.lstrip().startswith('- type:'):
            if cur:
                refs.append(cur)
            typ = l.split(':',1)[1].strip()
            cur = {'type': typ, 'path': None, 'raw': [l]}
        elif cur is not None and 'path:' in l:
            cur['path'] = l.split(':',1)[1].strip().strip('"').strip("'")
            cur['raw'].append(l)
        elif cur is not None:
            cur['raw'].append(l)
    if cur:
        refs.append(cur)
    return refs


def build_front_matter_with_refs(fm_text, refs):
    # reconstruct fm but replace references block
    before_refs = re.split(r'(references:\n)', fm_text, maxsplit=1)
    if len(before_refs) < 3:
        return fm_text
    prefix = before_refs[0] + before_refs[1]
    new_refs_lines = []
    for r in refs:
        new_refs_lines.append('  - type: ' + r['type'])
        new_refs_lines.append('    path: ' + r['path'])
    return prefix + '\n'.join(new_refs_lines) + '\n---\n'


def ensure_pairs_in_file(path):
    text = read(path)
    fm, rest = parse_front_matter(text)
    if not fm:
        return False, 'no front-matter'
    refs = extract_references(fm)
    files = [r for r in refs if r['type'] == 'file' and r['path']]
    urls = [r for r in refs if r['type'] == 'url' and r['path']]

    # map base names to entries
    file_map = {os.path.normpath(r['path']): r for r in files}
    url_map = {r['path']: r for r in urls}

    changed = False

    # For each file entry, ensure matching url
    for fpath, r in list(file_map.items()):
        # derive repo-relative path
        rel = os.path.normpath(os.path.join(os.path.dirname(path), fpath))
        # make relative to repo root
        repo_rel = os.path.relpath(rel, os.getcwd()).replace('\\', '/')
        url = REPO_URL_BASE + '/' + repo_rel
        # check if url exists in any url_map
        if not any(url == u for u in url_map):
            urls.append({'type': 'url', 'path': url})
            url_map[url] = urls[-1]
            changed = True

    # For each url entry, ensure matching file
    for u in list(urls):
        p = u['path']
        m = re.search(r'/blob/main/(.*)$', p)
        if m:
            repo_rel = m.group(1)
            file_path = os.path.normpath(repo_rel)
            # prefer path relative to EVID file as earlier convention: ../../../../<repo_path>
            rel_from_evid = os.path.join('../../../../', file_path)
            rel_from_evid = rel_from_evid.replace('\\', '/')
            if not any(os.path.normpath(r['path']) == os.path.normpath(rel_from_evid) for r in files):
                files.append({'type': 'file', 'path': rel_from_evid})
                changed = True

    # Reconstruct refs preserving order: alternate file/url pairs grouped by basename
    merged = []
    # build set of basenames
    basenames = []
    for r in files + urls:
        name = os.path.basename(r['path'])
        if name not in basenames:
            basenames.append(name)

    for name in basenames:
        # add file entries matching name
        for f in files:
            if os.path.basename(f['path']) == name:
                merged.append({'type': 'file', 'path': f['path']})
        for u in urls:
            if os.path.basename(u['path']) == name:
                merged.append({'type': 'url', 'path': u['path']})

    if changed:
        new_fm = build_front_matter_with_refs(fm, merged)
        new_text = new_fm + rest
        write(path, new_text)

    # return whether file now has matching pairs for all items
    # compute mismatches
    final_refs = extract_references(new_fm if changed else fm)
    names = {}
    for r in final_refs:
        bn = os.path.basename(r['path'])
        names.setdefault(bn, set()).add(r['type'])
    mismatches = {k: v for k, v in names.items() if v != {'file', 'url'}}
    return changed, mismatches


def main():
    evid_dir = Path('docs/TSF/tsf_implementation/items/evidences')
    report = {}
    modified = []
    for p in sorted(evid_dir.glob('EVID-L0-*.md')):
        changed, mismatches = ensure_pairs_in_file(str(p))
        if changed:
            modified.append(str(p))
        report[str(p)] = mismatches

    print('Ensure pairs completed.')
    if modified:
        print('Modified files:')
        for m in modified:
            print(' -', m)
    # print concise report of mismatches
    print('\nMismatches (basename -> missing types):')
    any_mismatch = False
    for p, mism in report.items():
        if not mism:
            continue
        # mism may be a dict or an error string
        if isinstance(mism, dict):
            any_mismatch = True
            print(p)
            for name, missing in mism.items():
                print('   -', name, '->', missing)
        else:
            any_mismatch = True
            print(p)
            print('   - ERROR:', mism)
    if not any_mismatch:
        print('All EVIDs have both file and url entries for each referenced artifact.')


if __name__ == '__main__':
    main()
