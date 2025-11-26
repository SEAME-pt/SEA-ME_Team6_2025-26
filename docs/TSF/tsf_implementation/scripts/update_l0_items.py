#!/usr/bin/env python3
"""Update L0 EXPECT/ASSERT/EVID items from the LO_requirements table.

Usage: python3 update_l0_items.py
"""
import re
from pathlib import Path

ROOT = Path('.').resolve()
REQ = ROOT / 'docs' / 'TSF' / 'requirements' / 'LO_requirements.md'
ITEMS = ROOT / 'docs' / 'TSF' / 'tsf_implementation' / 'items'
SPRINT_REFS = [
    'docs/sprints/sprint1.md',
    'docs/sprints/sprint2.md',
    'docs/sprints/sprint3.md',
]
COMMON_DEMOS = [
    'docs/demos/system-architecture-cross.png',
    'docs/demos/first_qt_app.jpeg',
    'docs/demos/eletric-scheme.png',
]
COMMON_GUIDES = [
    'docs/guides/Power Consumption Analysis.md',
    'docs/guides/AGL_Installation_Guide.md',
]
REVIEWER = {'name':'Joao Jesus Silva','email':'joao.silva@seame.pt'}

def short_summary(text, maxlen=80):
    text = text.strip()
    # prefer first sentence
    for sep in ('.', ':'):
        if sep in text:
            s = text.split(sep,1)[0].strip()
            if len(s) <= maxlen:
                return s
    # fallback: truncate
    return (text[:maxlen].rsplit(' ',1)[0] + '...') if len(text) > maxlen else text

def parse_requirements(md_text):
    # match rows like: | **L0-1** | Requirement | Acceptance Criteria | Verification Method |
    pattern = re.compile(r"\|\s*\*\*(L0-\d+)\*\*\s*\|\s*(.*?)\s*\|\s*(.*?)\s*\|\s*(.*?)\s*\|", re.S)
    items = {}
    for m in pattern.finditer(md_text):
        rid = m.group(1)
        req = ' '.join(m.group(2).split())
        acc = ' '.join(m.group(3).split())
        ver = ' '.join(m.group(4).split())
        items[rid] = {'requirement': req, 'acceptance': acc, 'verification': ver}
    return items

def write_expectation(rid, data):
    path = ITEMS / 'expectations' / f'EXPECT-{rid}.md'
    header = short_summary(data['requirement'], maxlen=60)
    # normalize whitespace and wrap into readable paragraphs
    import textwrap
    req_text = re.sub(r'\s+', ' ', data['requirement']).strip()
    wrapped = textwrap.wrap(req_text, width=80)
    content = (
        f"---\n"
        f"id: EXPECT-{rid}\n"
        f"header: \"{header}\"\n"
        f"text: |\n"
    )
    for line in wrapped:
        content += f"  {line}\n"
    content += "\nlevel: 1.1\nnormative: true\nreferences:\n"
    for r in SPRINT_REFS:
        content += f"  - type: \"file\"\n    path: \"../../../../{r}\"\n"
    content += f"reviewers:\n  - name: \"{REVIEWER['name']}\"\n    email: \"{REVIEWER['email']}\"\n---\n\n"
    path.write_text(content)

def write_assertion(rid, data):
    path = ITEMS / 'assertions' / f'ASSERT-{rid}.md'
    header = short_summary(data['acceptance'] or 'Assertion for ' + rid, maxlen=80)
    text = f"{data['acceptance']}\n\n" if data['acceptance'] else ''
    text += (
        "System architecture documentation maps components to implemented modules: The system architecture documentation "
        "(diagrams and descriptive documents) shall provide a clear mapping between the listed hardware and software components "
        "and their implementation (e.g. Expansion Board, Raspberry Pi 5, Hailo AI Hat, DC motors, servo motor, display interface)."
    )
    content = f"---\n" \
        + f"id: ASSERT-{rid}\n" \
        + f"header: \"Assertion: {header}\"\n" \
        + "text: |\n"
    for line in re.sub(r'\s+', ' ', text).split('. '):
        if line.strip():
            content += f"  {line.strip()}\n"
    content += "level: 1.2\nnormative: true\nreferences:\n"
    content += f"  - type: \"file\"\n    path: \"../expectations/EXPECT-{rid}.md\"\n"
    content += f"  - type: \"file\"\n    path: \"../evidences/EVID-{rid}.md\"\n"
    content += f"reviewers:\n  - name: \"{REVIEWER['name']}\"\n    email: \"{REVIEWER['email']}\"\n---\n\n"
    path.write_text(content)

def write_evidence(rid, data):
    path = ITEMS / 'evidences' / f'EVID-{rid}.md'
    header = f"Evidence: {data['verification']} - {short_summary(data['requirement'], maxlen=80)}"
    content = f"---\n" \
        + f"id: EVID-{rid}\n" \
        + f"header: \"{header}\"\n" \
        + "text: |\n"
    content += "  This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.\n\n"
    content += "  The following artifacts are included as references and should be reviewed as evidence:\n\n"
    content += "  - Sprint reports describing related activities (sprint1..3).\n"
    content += "  - Demos and diagrams related to the requirement.\n"
    content += "  - Guides and analyses related to the decision.\n\n"
    content += "level: 1.1\nnormative: false\nreferences:\n"
    for r in SPRINT_REFS:
        content += f"  - type: \"file\"\n    path: \"../../../../{r}\"\n"
    for r in COMMON_DEMOS:
        content += f"  - type: \"file\"\n    path: \"../../../../{r}\"\n"
    for r in COMMON_GUIDES:
        content += f"  - type: \"file\"\n    path: \"../../../../{r}\"\n"
    content += f"reviewers:\n  - name: \"{REVIEWER['name']}\"\n    email: \"{REVIEWER['email']}\"\n---\n\n"
    path.write_text(content)

def main():
    md = REQ.read_text(encoding='utf-8')
    items = parse_requirements(md)
    # operate on all parsed L0 items (including newly added ones)
    # sort by numeric id
    def keyfn(rid):
        m = re.match(r'L0-(\d+)', rid)
        return int(m.group(1)) if m else 9999

    for rid in sorted(items.keys(), key=keyfn):
        data = items[rid]
        write_expectation(rid, data)
        write_assertion(rid, data)
        write_evidence(rid, data)
        print(f"Updated {rid}")

if __name__ == '__main__':
    main()
