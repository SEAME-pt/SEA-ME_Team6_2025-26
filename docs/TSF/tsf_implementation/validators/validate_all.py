#!/usr/bin/env python3
"""Run the suite of TSF validators.

This wrapper runs the lightweight front-matter validator and the extended validator
in sequence and aggregates results. Use this as the single entry point for CI.
"""
import sys
import subprocess
from pathlib import Path

BASE = Path(__file__).resolve().parents[1]


def run(script_path):
    print(f'Running {script_path}...')
    res = subprocess.run([sys.executable, str(script_path)])
    return res.returncode


def main():
    validators = [
        BASE / 'validators' / 'validate_front_matter.py',
        BASE / 'validators' / 'validate_extended.py',
    ]
    exit_codes = []
    for v in validators:
        if not v.exists():
            print('Missing validator:', v)
            exit_codes.append(2)
            continue
        rc = run(v)
        exit_codes.append(rc)
        if rc != 0:
            print(f'Validator {v.name} failed with code {rc}')
    final = 0 if all(c == 0 for c in exit_codes) else 2
    if final == 0:
        print('All validators passed')
    else:
        print('One or more validators failed')
    sys.exit(final)


if __name__ == '__main__':
    main()
