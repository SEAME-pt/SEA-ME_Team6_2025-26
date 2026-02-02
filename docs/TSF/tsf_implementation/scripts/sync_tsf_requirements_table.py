#!/usr/bin/env python3
"""
sync_tsf_requirements_table.py

Unified script for:
1. Detecting changes in the tsf-requirements-table.md table
2. Generating Acceptance/Verification with AI (if configured)
3. Synchronizing evidences from items/evidences/
4. Updating the table while preserving formatting

Note: AI generation only works if there is integration with an external API or manual input via Copilot Chat.
"""
import os
import re
from pathlib import Path

# Caminhos principais
ROOT = Path(__file__).resolve().parents[4]
TSF_TABLE = ROOT / 'docs/TSF/requirements/tsf-requirements-table.md'
EVID_DIR = ROOT / 'docs/TSF/tsf_implementation/items/evidences/'

 # 1. Detect changes in the table (new requirements, empty Acceptance/Verification fields)
def detect_table_changes():
    with open(TSF_TABLE, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    # Example: find requirement lines with empty Acceptance/Verification fields
    reqs = []
    for i, line in enumerate(lines):
        if re.match(r'\| *L0-\d+ *\|', line):
            cols = [c.strip() for c in line.split('|') if c.strip()]
            if len(cols) >= 4:
                req_id, requirement, acceptance, verification = cols[:4]
                if not acceptance or acceptance.lower() in ('-', 'todo', 'tbd'):
                    reqs.append((i, req_id, requirement, 'acceptance'))
                if not verification or verification.lower() in ('-', 'todo', 'tbd'):
                    reqs.append((i, req_id, requirement, 'verification'))
    return reqs, lines

 # New function: generates Acceptance Criteria or Verification Method by category
def detect_category(requirement_text):
    text = requirement_text.lower()
    if any(kw in text for kw in ['hardware', 'motor', 'servo', 'raspberry', 'hailo', 'stm32', 'board', 'sensor']):
        return 'hardware'
    if any(kw in text for kw in ['qt', 'software', 'application', 'gui', 'program', 'code', 'crosscompile', 'python', 'c++', 'linux', 'toolchain']):
        return 'software'
    if any(kw in text for kw in ['can', 'communication', 'message', 'protocol', 'signal', 'bidirectional', 'bus']):
        return 'communication'
    if any(kw in text for kw in ['document', 'documentation', 'guide', 'readme', 'specification', 'manual']):
        return 'documentation'
    if any(kw in text for kw in ['agile', 'scrum', 'github', 'ci/cd', 'workflow', 'tsf', 'methodology']):
        return 'process'
    if any(kw in text for kw in ['display', 'ui', 'interface', 'screen', 'visual', 'mockup']):
        return 'display'
    if any(kw in text for kw in ['threadx', 'rtos', 'thread', 'real-time', 'scheduler', 'timing', 'latency']):
        return 'realtime'
    return 'generic'

def generate_field_from_requirement(requirement, field):
    categoria = detect_category(requirement)
    if field == 'acceptance':
        if categoria == 'hardware':
            return 'All described hardware components are correctly installed, connected, and functional.'
        if categoria == 'software':
            return 'The system runs the described software without errors and fulfills the specified functionalities.'
        if categoria == 'comunicacao':
            return 'Communication between modules occurs bidirectionally and without failures.'
        if categoria == 'documentacao':
            return 'Documentation is present and covers all aspects of the requirement.'
        if categoria == 'processo':
            return 'The process/methodology is followed and evidenced by artifacts (boards, reports, etc.).'
        if categoria == 'display':
            return 'The graphical interface presents all required visual elements and responds to interactions.'
        if categoria == 'realtime':
            return 'The system meets real-time and task prioritization requirements.'
        return 'The requirement is considered fulfilled when the described functionality is implemented and validated.'
    elif field == 'verification':
        if categoria == 'hardware':
            return 'Physical inspection and functional testing of the components.'
        if categoria == 'software':
            return 'Demonstration of software startup and functional verification.'
        if categoria == 'comunicacao':
            return 'Testing of message transmission and reception between modules.'
        if categoria == 'documentacao':
            return 'Inspection of the documentation in the repository.'
        if categoria == 'processo':
            return 'Review of process artifacts and meetings.'
        if categoria == 'display':
            return 'Visual inspection of the interface in operation.'
        if categoria == 'realtime':
            return 'Performance testing and analysis of scheduling logs.'
        return 'Verification by test, demonstration, or inspection as applicable.'
    return ''

# 3. Sincronizar evidências
def sync_evidence():
    evid_map = {}
    for evid_file in EVID_DIR.glob('EVID-*.md'):
        content = evid_file.read_text(encoding='utf-8')
        urls = re.findall(r'- type: url\s*\n\s*url: (.+)', content)
        evid_map[evid_file.stem] = urls
    return evid_map

# 4. Atualizar tabela
def update_table(lines, reqs, evid_map):
    new_lines = lines[:]
    for i, req_id, requirement, field in reqs:
        value = generate_field_from_requirement(requirement, field)
        if value:
            cols = [c.strip() for c in new_lines[i].split('|')]
            if field == 'acceptance':
                cols[2] = value
            elif field == 'verification':
                cols[3] = value
            new_lines[i] = ' | '.join([''] + cols[1:-1] + ['']) + '\n'
    # Update Evidence column
    for i, line in enumerate(new_lines):
        if re.match(r'\| *L0-\d+ *\|', line):
            cols = [c.strip() for c in line.split('|') if c.strip()]
            if len(cols) >= 5:
                req_id = cols[0]
                evids = [e for e in evid_map if req_id in e]
                evid_str = ', '.join(evids)
                cols[4] = evid_str
                new_lines[i] = ' | '.join([''] + cols[1:-1] + ['']) + '\n'
    return new_lines

def main():
    reqs, lines = detect_table_changes()
    evid_map = sync_evidence()
    new_lines = update_table(lines, reqs, evid_map)
    with open(TSF_TABLE, 'w', encoding='utf-8') as f:
        f.writelines(new_lines)
    print('Table synchronized. (AI only if configured)')

if __name__ == '__main__':
    main()
