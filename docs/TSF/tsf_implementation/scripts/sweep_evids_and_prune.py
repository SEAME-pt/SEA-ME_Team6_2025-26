#!/usr/bin/env python3
"""Sweep EVID items to add missing url references for file entries
and prune unwanted cluster/AGL links from EXPECT/ASSERT/ASSUMP items.

Usage: run from repository root: python3 docs/TSF/tsf_implementation/scripts/sweep_evids_and_prune.py
"""
import os
import re
from pathlib import Path

REPO_URL_BASE = "https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main"


def read_file(path):
    with open(path, 'r', encoding='utf-8') as f:
        return f.read()


def write_file(path, content):
    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)


def ensure_url_for_file_refs(md_path):
    text = read_file(md_path)
    if 'references:' not in text:
        return False
    # find front-matter
    m = re.match(r"^(---\n.*?\n---\n)", text, re.S)
    if not m:
        return False
    fm = m.group(1)
    rest = text[len(fm):]

    lines = fm.splitlines()
    out_lines = []
    i = 0
    changed = False
    while i < len(lines):
        line = lines[i]
        out_lines.append(line)
        if line.strip().startswith('references:'):
            i += 1
            # process reference block
            refs_block = []
            while i < len(lines) and (lines[i].startswith(' ') or lines[i].startswith('-')):
                refs_block.append(lines[i])
                i += 1
            # parse refs_block into entries
            entries = []
            cur = None
            for l in refs_block:
                if l.lstrip().startswith('- type:'):
                    if cur:
                        entries.append(cur)
                    cur = {'type': l.split(':',1)[1].strip(), 'raw': [l]}
                elif cur is not None:
                    cur['raw'].append(l)
            if cur:
                entries.append(cur)

            # for each file-type entry, ensure corresponding url exists
            new_entries_raw = []
            md_dir = os.path.dirname(md_path)
            for e in entries:
                new_entries_raw.extend(e['raw'])
                if e['type'] == 'file':
                    # find path value in raw
                    path_line = next((l for l in e['raw'] if 'path:' in l), None)
                    if path_line:
                        rel_path = path_line.split(':',1)[1].strip().strip('"').strip("'")
                        abs_path = os.path.normpath(os.path.join(md_dir, rel_path))
                        if not os.path.exists(abs_path):
                            # file reference points to non-existent file; still create URL using relpath
                            rel_to_repo = os.path.relpath(os.path.normpath(os.path.join(md_dir, rel_path)), os.getcwd())
                        else:
                            rel_to_repo = os.path.relpath(abs_path, os.getcwd())
                        url = REPO_URL_BASE + '/' + rel_to_repo.replace('\\', '/')
                        # check if url entry already present
                        url_exists = any(('type: url' in ''.join(x['raw']) and url in ''.join(x['raw'])) for x in entries)
                        if not url_exists:
                            new_entries_raw.append('  - type: url')
                            new_entries_raw.append(f'    path: {url}')
                            changed = True
            # append new entries lines
            out_lines.extend(new_entries_raw)
            continue
        i += 1

    if changed:
        new_fm = '\n'.join(out_lines) + '\n'
        new_text = new_fm + rest
        write_file(md_path, new_text)
    return changed


def prune_unwanted_links_in_items(items_dir):
    removed_any = False
    patterns = [r'src/cluster', r'AGL and ThreadX benefits Guide']
    for root, _, files in os.walk(items_dir):
        for fn in files:
            if not fn.endswith('.md'):
                continue
            p = os.path.join(root, fn)
            txt = read_file(p)
            new_txt = txt
            for pat in patterns:
                new_txt = re.sub(r'.*' + re.escape(pat) + r'.*\n', '', new_txt)
            if new_txt != txt:
                write_file(p, new_txt)
                removed_any = True
    return removed_any


def main():
    repo_root = os.getcwd()
    evid_dir = os.path.join('docs', 'TSF', 'tsf_implementation', 'items', 'evidences')
    items_dir = os.path.join('docs', 'TSF', 'tsf_implementation', 'items')

    changed_files = []
    for fn in sorted(os.listdir(evid_dir)):
        if not fn.endswith('.md'):
            continue
        path = os.path.join(evid_dir, fn)
        if ensure_url_for_file_refs(path):
            changed_files.append(path)

    pruned = prune_unwanted_links_in_items(items_dir)

    print('Sweep complete.')
    if changed_files:
        print('Modified EVID files:')
        for c in changed_files:
            print(' -', c)
    else:
        print('No EVID file changes needed.')
    print('Pruned items:' , pruned)


if __name__ == '__main__':
    main()
