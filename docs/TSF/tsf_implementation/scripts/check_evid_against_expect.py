#!/usr/bin/env python3
"""Check EVID-L0-*.md files for expected evidence items per EXPECT-L0-* list.

Run from repo root:
  python3 docs/TSF/tsf_implementation/scripts/check_evid_against_expect.py

Prints a concise report of missing artifacts per EVID.
"""
import os
import re
from pathlib import Path

EXPECTED = {
    '1': {
        'id': 'EXPECT-L0-1',
        'items': [
            'assembled-car1.jpeg', 'assembled-car2.jpeg', 'eletric-scheme.png',
            'Validation-of-energy-layout-sprint3.jpeg',
            'Power Consumption Analysis.md', 'WhatsTSF.md'
        ]
    },
    '2': {
        'id': 'EXPECT-L0-2',
        'items': ['3d-car.jpg', '3d-car2.jpg', '3d-car3.jpg']
    },
    '3': {
        'id': 'EXPECT-L0-3',
        'items': ['AGL_Installation_Guide.md', 'AGL and ThreadX benefits Guide.md', 'src/cluster']
    },
    '4': {
        'id': 'EXPECT-L0-4',
        'items': ['Install and Test Joystick in AGL.md']
    },
    '5': {
        'id': 'EXPECT-L0-5',
        'items': ['first_qt_app.jpeg', 'src/cluster', 'QML-guide.md', 'README.md']
    },
    '6': {
        'id': 'EXPECT-L0-6',
        'items': ['mockup1.jpeg','mockup2.jpeg','mockup3.jpeg','mockup4.jpeg','mockup5.jpeg','mockup6.jpeg',
                  'Cluster-display-sprint3.jpeg','Cluster-mockup-sprint3.jpeg','Cluster-study.jpeg','QML-guide.md']
    },
    '7': {
        'id': 'EXPECT-L0-7',
        'items': ['system-architecture-cross.png','src/cross-compiler']
    },
    '8': {
        'id': 'EXPECT-L0-8',
        'items': ['CANbus Scheme Updated.png','CAN-layout-sprint3.jpeg','CAN-overview.md','CAN-test-guide.md','CAN-explanation.md',
                  'CANFrame.png','CANsignal.png','CANtopology.png','CANtopology2.png','Half-duplex-communication.png']
    },
    '9': {
        'id': 'EXPECT-L0-9',
        'items': ['ThreadX_Installation_Guide.md','AGL and ThreadX benefits Guide.md']
    },
    '10': {
        'id': 'EXPECT-L0-10',
        'items': ['Validation-of-energy-layout-sprint3.jpeg','eletric-scheme.png']
    },
    '11': {
        'id': 'EXPECT-L0-11',
        'items': ['src/cluster']
    },
    '12': {
        'id': 'EXPECT-L0-12',
        'items': []
    },
    '13': {
        'id': 'EXPECT-L0-13',
        'items': ['github-guidelines.md','github-actions-guide.md']
    },
    '14': {
        'id': 'EXPECT-L0-14',
        'items': ['WhatsTSF.md']
    },
    '15': {
        'id': 'EXPECT-L0-15',
        'items': ['dailys_warmups.md','docs/sprints','projects']
    },
    '16': {
        'id': 'EXPECT-L0-16',
        'items': ['github-actions-guide.md','.github/workflows']
    },
    '17': {
        'id': 'EXPECT-L0-17',
        'items': ['genAI-pair-programming-guidelines.md','Generative-AI-and-LLMs-for-Dummies.pdf','generative-ai-for-beginners']
    }
}

EVID_DIR = Path('docs/TSF/tsf_implementation/items/evidences')

def read_evid(evid_path):
    return evid_path.read_text(encoding='utf-8')

def check_presence(text, term):
    # check by exact basename or path segment
    bn = os.path.basename(term)
    # try match basename
    if re.search(re.escape(bn), text, re.I):
        return True
    # also check full term
    if re.search(re.escape(term), text, re.I):
        return True
    return False

def main():
    report = {}
    for n, spec in EXPECTED.items():
        evid_file = EVID_DIR / f'EVID-L0-{n}.md'
        if not evid_file.exists():
            report[n] = {'missing_evid_file': True}
            continue
        txt = read_evid(evid_file)
        missing = []
        for item in spec['items']:
            if not check_presence(txt, item):
                missing.append(item)
        report[n] = {'missing': missing}

    # print concise report
    any_missing = False
    for n in sorted(report.keys(), key=lambda x:int(x)):
        entry = report[n]
        if entry.get('missing_evid_file'):
            print(f'EVID-L0-{n}: missing evid file')
            any_missing = True
            continue
        if entry['missing']:
            any_missing = True
            print(f'EVID-L0-{n} missing {len(entry['missing'])} items:')
            for m in entry['missing']:
                print(' -', m)

    if not any_missing:
        print('All EXPECT-listed artifacts are present in their corresponding EVID files.')

if __name__ == '__main__':
    main()
