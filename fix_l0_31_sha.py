#!/usr/bin/env python3
"""Fix all L0_31 items in .dotstop.dot: update stale SHAs and re-add missing ones."""
import sys
import os
import re

# Set working directory
os.chdir('/home/seame/Documents/SEA-ME_Team6_2025-26')

# Add venv to path
sys.path.insert(0, '.venv/lib/python3.11/site-packages')

from pathlib import Path
from trudag.dotstop.core.item import item_from_markdown

ITEMS_DIR = Path('docs/TSF/tsf_implementation/.trudag_items')
DOTFILE = Path('.dotstop.dot')

items_to_fix = [
    ('ASSERTIONS-ASSERT_L0_31',   ITEMS_DIR / 'ASSERTIONS/ASSERT_L0_31/ASSERTIONS-ASSERT_L0_31.md'),
    ('ASSUMPTIONS-ASSUMP_L0_31',  ITEMS_DIR / 'ASSUMPTIONS/ASSUMP_L0_31/ASSUMPTIONS-ASSUMP_L0_31.md'),
    ('EVIDENCES-EVID_L0_31',      ITEMS_DIR / 'EVIDENCES/EVID_L0_31/EVIDENCES-EVID_L0_31.md'),
    ('EXPECTATIONS-EXPECT_L0_31', ITEMS_DIR / 'EXPECTATIONS/EXPECT_L0_31/EXPECTATIONS-EXPECT_L0_31.md'),
]

dot_content = DOTFILE.read_text()

for item_name, item_path in items_to_fix:
    md_content = item_path.read_text()
    item = item_from_markdown(item_name, md_content)
    new_sha = item.sha

    # Check if item already exists in .dotstop.dot
    search_pattern = rf'"{re.escape(item_name)}"'
    match = re.search(search_pattern, dot_content)

    if match:
        # Item exists - check if SHA needs updating
        old_pattern = rf'"{re.escape(item_name)}" \[sha="[^"]+"\]'
        sha_match = re.search(old_pattern, dot_content)
        if sha_match:
            old_sha = re.search(r'sha="([^"]+)"', sha_match.group()).group(1)
            if old_sha != new_sha:
                dot_content = re.sub(old_pattern, f'"{item_name}" [sha="{new_sha}"]', dot_content)
                print(f'UPDATED SHA: {item_name}')
                print(f'  old: {old_sha}')
                print(f'  new: {new_sha}')
            else:
                print(f'OK (SHA correct): {item_name}')
    else:
        # Item is MISSING - add it before the closing }
        new_node_line = f'"{item_name}" [sha="{new_sha}"];\n'
        dot_content = dot_content.rstrip('\n')
        if dot_content.endswith('}'):
            last_brace = dot_content.rfind('}')
            dot_content = dot_content[:last_brace] + new_node_line + '}\n'
        else:
            dot_content = dot_content + new_node_line
        print(f'ADDED: {item_name} [sha={new_sha}]')

DOTFILE.write_text(dot_content)
print(f'\nDone. Lines: {len(dot_content.splitlines())}')

print('\nVerification:')
for item_name, _ in items_to_fix:
    found = item_name in dot_content
    print(f'  {item_name}: {"PRESENT" if found else "MISSING"}')
