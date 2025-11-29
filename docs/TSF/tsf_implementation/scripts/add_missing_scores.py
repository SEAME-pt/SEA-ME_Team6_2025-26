#!/usr/bin/env python3
"""
Add missing `score:` front-matter to TSF items according to type.

Rules:
- expectations -> 0.2
- assertions  -> 0.4
- evidences   -> 1
- assumptions -> 0.6

Only adds `score:` when it is missing in the YAML front-matter. Does not overwrite existing scores.
"""
from pathlib import Path
import re
import sys
import argparse


SCORES = {
    'expectations': '0.2',
    'assertions': '0.4',
    'evidences': '1',
    'assumptions': '0.6',
}


def process_file(path: Path, score_val: str):
    txt = path.read_text(encoding='utf-8')
    # detect YAML front-matter
    if txt.startswith('---'):
        # find closing '---'
        m = re.search(r'^---\s*$\n(.*?)\n^---\s*$\n', txt, re.M | re.S)
        if m:
            yaml_block = m.group(1)
            if re.search(r'^\s*score\s*:\s*', yaml_block, re.M):
                return False
            # insert score before closing ---
            new_yaml = yaml_block + f"\nscore: {score_val}"
            new_txt = txt[:m.start(1)] + new_yaml + txt[m.end(1):]
            path.write_text(new_txt, encoding='utf-8')
            return True
        else:
            # malformed front-matter; append a score line after first ---
            parts = txt.split('\n', 1)
            if len(parts) == 2:
                new = '---\nscore: ' + score_val + '\n---\n' + parts[1]
                path.write_text(new, encoding='utf-8')
                return True
            return False
    else:
        # no front-matter: add one
        new = f"---\nscore: {score_val}\n---\n\n" + txt
        path.write_text(new, encoding='utf-8')
        return True


def main():
    p = argparse.ArgumentParser(description='Add missing score front-matter to TSF items')
    p.add_argument('--force', action='store_true', help='Overwrite existing score values')
    p.add_argument('--targets', nargs='+', choices=['items', 'staged', 'both'], default=['items'],
                   help='Which locations to update: "items" (source), "staged" (.trudag_items), or "both"')
    args = p.parse_args()

    roots = []
    tsf_impl = Path(__file__).resolve().parents[1]
    if 'items' in args.targets or 'both' in args.targets:
        roots.append(tsf_impl / 'items')
    if 'staged' in args.targets or 'both' in args.targets:
        roots.append(tsf_impl / '.trudag_items')

    changed = []
    for base in roots:
        if not base.exists():
            print('Skipping missing directory:', base, file=sys.stderr)
            continue
        for kind, score in SCORES.items():
            d = base / kind
            if not d.exists():
                continue
            for md in sorted(d.glob('*.md')):
                try:
                    txt = md.read_text(encoding='utf-8')
                    has_yaml = txt.startswith('---') and re.search(r'^---\s*$\n(.*?)\n^---\s*$', txt, re.M | re.S)
                    if not args.force:
                        # use existing logic: only add when missing
                        updated = process_file(md, score)
                    else:
                        # force: ensure front-matter exists and set/replace score
                        if has_yaml:
                            # replace existing score line if present, else append before closing ---
                            def repl_score(match):
                                block = match.group(1)
                                if re.search(r'^\s*score\s*:\s*', block, re.M):
                                    new_block = re.sub(r'(^\s*score\s*:\s*).*$', f'score: {score}', block, flags=re.M)
                                else:
                                    new_block = block + f"\nscore: {score}"
                                return '---\n' + new_block + '\n---\n'

                            new_txt = re.sub(r'^---\s*$\n(.*?)\n^---\s*$', repl_score, txt, flags=re.M | re.S)
                            md.write_text(new_txt, encoding='utf-8')
                            updated = True
                        else:
                            # no front-matter: add one
                            new = f"---\nscore: {score}\n---\n\n" + txt
                            md.write_text(new, encoding='utf-8')
                            updated = True
                except Exception as e:
                    print('Failed processing', md, e)
                    updated = False
                if updated:
                    changed.append(str(md))

    print('Updated', len(changed), 'files')
    for pth in changed:
        print('  ', pth)


if __name__ == '__main__':
    main()
