#!/usr/bin/env python3
"""Sync evidence references from sprint files into EVID-L0-*.md items.

This script scans `docs/sprints/*.md` for lines mentioning `EXPECT-L0-<n>`
and collects nearby markdown links/images. It then updates the corresponding
`docs/TSF/tsf_implementation/items/evidences/EVID-L0-<n>.md` files by adding
these artifacts to the `references:` section and appending a short note in
the `text:` body.

Usage: python3 sync_evidence_from_sprints.py
"""
import re
from pathlib import Path

ROOT = Path('.').resolve()
SPRINTS = sorted((ROOT / 'docs' / 'sprints').glob('*.md'))
EVID_DIR = ROOT / 'docs' / 'TSF' / 'tsf_implementation' / 'items' / 'evidences'

link_re = re.compile(r"!\[[^\]]*\]\((https?://[^)]+)\)|\[[^\]]*\]\((https?://[^)]+)\)|\((https?://github.com/.+?/blob/.+?/([^\)]+))\)")
md_link_re = re.compile(r"!\[[^\]]*\]\(([^)]+)\)|\[[^\]]*\]\(([^)]+)\)")

def normalize_path(url):
    # If it's a GitHub blob link, extract the path after /blob/<branch>/
    m = re.search(r"github.com/.+?/blob/[^/]+/(.+)$", url)
    if m:
        return m.group(1)
    # If it's a relative docs path (starting docs/...), return as-is
    if url.startswith('docs/'):
        return url
    # If it's an absolute GitHub raw or other, return URL
    return None

def collect_evidence_from_sprint(path):
    text = path.read_text(encoding='utf-8')
    lines = text.splitlines()
    mapping = {}
    for i, line in enumerate(lines):
        # find EXPECT mentions
        m = re.search(r"EXPECT-L0-(\d+)", line)
        if m:
            rid = f'L0-{m.group(1)}'
            # collect following up to 8 lines for links/images
            artifacts = []
            for j in range(i+1, min(i+9, len(lines))):
                l = lines[j].strip()
                if not l:
                    break
                # find markdown links or images
                for mm in md_link_re.finditer(l):
                    url = mm.group(1) or mm.group(2)
                    if not url:
                        continue
                    # clean url from surrounding <> or extra parentheses
                    url = url.strip().strip('<>')
                    # ignore anchors
                    if url.startswith('#'):
                        continue
                    artifacts.append(url)
            if artifacts:
                mapping.setdefault(rid, []).extend(artifacts)
    return mapping

def update_evid_file(rid, artifacts):
    evid_path = EVID_DIR / f'EVID-{rid}.md'
    if not evid_path.exists():
        print(f"EVID file missing: {evid_path}, skipping")
        return
    content = evid_path.read_text(encoding='utf-8')
    # find references section
    # we'll append missing references to the `references:` list
    ref_block = ''
    new_refs = []
    for a in artifacts:
        p = normalize_path(a)
        if p:
            new_refs.append(('file', p))
        else:
            new_refs.append(('url', a))

    # Insert new refs just before reviewers: or end of references
    if 'references:' in content:
        parts = content.split('references:')
        before = parts[0]
        rest = 'references:' + parts[1]
        # find insertion point (just before reviewers:)
        if '\nreviewers:' in rest:
            refs_part, after = rest.split('\nreviewers:', 1)
            refs_lines = refs_part.strip().splitlines()[1:] if refs_part.strip()!='references:' else []
            existing = []
            for rl in refs_lines:
                m = re.search(r"path: \"?([^"]+)\"?", rl)
                if m:
                    existing.append(m.group(1))
            # build added refs avoiding duplicates
            add_lines = ''
            for typ, path in new_refs:
                if typ=='file':
                    if path in existing:
                        continue
                    add_lines += f"  - type: \"file\"\n    path: \"../../../../{path}\"\n"
                else:
                    if path in existing:
                        continue
                    add_lines += f"  - type: \"url\"\n    path: \"{path}\"\n"
            new_content = before + 'references:' + after.replace('\nreviewers:', '\n' + add_lines + '\nreviewers:', 1)
            content = new_content
        else:
            # append refs at end of file before reviewers
            # fallback: naive append
            append = '\nreferences:\n'
            for typ, path in new_refs:
                if typ=='file':
                    append += f"  - type: \"file\"\n    path: \"../../../../{path}\"\n"
                else:
                    append += f"  - type: \"url\"\n    path: \"{path}\"\n"
            content = content + append
    else:
        # no references section; append one
        append = '\nreferences:\n'
        for typ, path in new_refs:
            if typ=='file':
                append += f"  - type: \"file\"\n    path: \"../../../../{path}\"\n"
            else:
                append += f"  - type: \"url\"\n    path: \"{path}\"\n"
        content = content + append

    # also append brief list to text body (before level)
    if '\nlevel:' in content:
        before_level, after_level = content.split('\nlevel:', 1)
        add_text = '\n  Additional sprint-linked artifacts included as evidence:\n'
        for typ, path in new_refs:
            if typ=='file':
                add_text += f"  - {path}\n"
            else:
                add_text += f"  - {path}\n"
        before_level = before_level + add_text
        content = before_level + '\nlevel:' + after_level

    evid_path.write_text(content, encoding='utf-8')
    print(f"Updated references in {evid_path.name}")

def main():
    overall = {}
    for sp in SPRINTS:
        m = collect_evidence_from_sprint(sp)
        for k,v in m.items():
            overall.setdefault(k, []).extend(v)

    for rid, arts in overall.items():
        # deduplicate
        uniq = []
        for a in arts:
            if a not in uniq:
                uniq.append(a)
        update_evid_file(rid, uniq)

if __name__ == '__main__':
    main()
