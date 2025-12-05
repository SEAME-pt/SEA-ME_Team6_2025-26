#!/usr/bin/env python3
"""Deduplicate references in EVID markdown front-matter.

This script normalizes each EVID file's `references` list so there is at most
one `file` entry and one `url` entry per basename/path. It writes files in-place.
"""
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
EVID_DIR = ROOT / "docs" / "TSF" / "tsf_implementation" / "items" / "evidences"


def parse_front_matter(text):
    m = re.match(r"^---\n(.*?)\n---\n", text, re.S)
    if not m:
        return None, text
    return m.group(1), text[m.end():]


def build_front_matter(fm_dict):
    lines = ["---"]
    for k, v in fm_dict.items():
        if k != 'references':
            lines.append(f"{k}: {v}")
        else:
            lines.append("references:")
            for ref in v:
                lines.append("  - type: %s" % ref['type'])
                lines.append("    path: %s" % ref['path'])
    lines.append("---")
    return "\n".join(lines) + "\n\n"


def normalize_path(p: str) -> str:
    # Normalize common URL-encoding and multiple ../ prefixes
    return p.replace('%20', ' ').replace('////', '/').replace('///', '/').strip()


def dedupe_refs(refs):
    seen = set()
    out = []
    for r in refs:
        key = (r.get('type'), normalize_path(r.get('path', '')))
        if key in seen:
            continue
        seen.add(key)
        out.append({'type': r['type'], 'path': normalize_path(r['path'])})
    return out


def extract_refs(fm_text):
    refs = []
    # crude parse lines under references:
    lines = fm_text.splitlines()
    in_refs = False
    cur = {}
    for ln in lines:
        if ln.strip().startswith('references:'):
            in_refs = True
            continue
        if not in_refs:
            continue
        if re.match(r"\s*-\s*type:\s*(\S+)", ln):
            m = re.search(r"type:\s*(\S+)", ln)
            if m:
                if cur:
                    refs.append(cur)
                cur = {'type': m.group(1)}
        elif re.search(r"path:\s*(.+)", ln):
            m = re.search(r"path:\s*(.+)", ln)
            if m and cur is not None:
                cur['path'] = m.group(1).strip()
    if cur:
        refs.append(cur)
    return refs


def process_file(p: Path):
    txt = p.read_text(encoding='utf-8')
    fm_text, body = parse_front_matter(txt)
    if fm_text is None:
        return False
    refs = extract_refs(fm_text)
    if not refs:
        return False
    deduped = dedupe_refs(refs)
    # Rebuild fm dict preserving other fields roughly (simple parse)
    fm_lines = fm_text.splitlines()
    fm_dict = {}
    for ln in fm_lines:
        if ':' in ln:
            k, v = ln.split(':', 1)
            k = k.strip()
            v = v.strip()
            if k == 'references':
                break
            fm_dict[k] = v
    fm_dict['references'] = deduped
    new = build_front_matter(fm_dict) + body
    if new != txt:
        p.write_text(new, encoding='utf-8')
        return True
    return False


def main():
    modified = []
    for f in sorted(EVID_DIR.glob('EVID-L0-*.md')):
        if process_file(f):
            modified.append(str(f.relative_to(ROOT)))
    print('Deduplicate completed. Modified files:')
    for m in modified:
        print(' -', m)


if __name__ == '__main__':
    main()
