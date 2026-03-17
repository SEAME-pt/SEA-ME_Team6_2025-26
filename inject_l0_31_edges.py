#!/usr/bin/env python3
"""Directly inject L0_31 edges into .dotstop.dot, bypassing trudag CLI SHA fragility."""
import sys
import os
import hashlib
import re

os.chdir('/home/seame/Documents/SEA-ME_Team6_2025-26')
sys.path.insert(0, '.venv/lib/python3.11/site-packages')

from pathlib import Path
from trudag.dotstop.core.item import item_from_markdown

ITEMS_DIR = Path('docs/TSF/tsf_implementation/.trudag_items')
DOTFILE = Path('.dotstop.dot')

def get_item_sha(name, path):
    md = path.read_text()
    item = item_from_markdown(name, md)
    return item.sha, item.sha256

def compute_link_sha(parent_sha256: bytes, child_sha256: bytes) -> str:
    h = hashlib.sha256()
    h.update(parent_sha256 + child_sha256)
    return h.hexdigest()

# Define the 3 edges to add for L0_31
# (ASSERT is already in .dotstop.dot, needing edges)
edges = [
    ('ASSERTIONS-ASSERT_L0_31',   ITEMS_DIR/'ASSERTIONS/ASSERT_L0_31/ASSERTIONS-ASSERT_L0_31.md',
     'EVIDENCES-EVID_L0_31',      ITEMS_DIR/'EVIDENCES/EVID_L0_31/EVIDENCES-EVID_L0_31.md'),
    ('EXPECTATIONS-EXPECT_L0_31', ITEMS_DIR/'EXPECTATIONS/EXPECT_L0_31/EXPECTATIONS-EXPECT_L0_31.md',
     'ASSERTIONS-ASSERT_L0_31',   ITEMS_DIR/'ASSERTIONS/ASSERT_L0_31/ASSERTIONS-ASSERT_L0_31.md'),
    ('EXPECTATIONS-EXPECT_L0_31', ITEMS_DIR/'EXPECTATIONS/EXPECT_L0_31/EXPECTATIONS-EXPECT_L0_31.md',
     'ASSUMPTIONS-ASSUMP_L0_31',  ITEMS_DIR/'ASSUMPTIONS/ASSUMP_L0_31/ASSUMPTIONS-ASSUMP_L0_31.md'),
]

dot_content = DOTFILE.read_text()
edge_count_before = len(re.findall(r'->', dot_content))

lines_to_add = []
for parent_name, parent_path, child_name, child_path in edges:
    parent_sha_hex, parent_sha256 = get_item_sha(parent_name, parent_path)
    child_sha_hex, child_sha256 = get_item_sha(child_name, child_path)
    link_sha = compute_link_sha(parent_sha256, child_sha256)
    
    edge_line = f'"{parent_name}" -> "{child_name}" [sha="{link_sha}"];\n'
    
    # Check if edge already exists
    if f'"{parent_name}" -> "{child_name}"' in dot_content:
        print(f'SKIP (exists): {parent_name} -> {child_name}')
    else:
        lines_to_add.append(edge_line)
        print(f'ADD: {parent_name} -> {child_name} [sha={link_sha[:16]}...]')

if lines_to_add:
    # Insert before the closing }
    dot_content = dot_content.rstrip('\n')
    if dot_content.endswith('}'):
        last_brace = dot_content.rfind('}')
        insertion = ''.join(lines_to_add)
        dot_content = dot_content[:last_brace] + insertion + '}\n'
    DOTFILE.write_text(dot_content)

edge_count_after = len(re.findall(r'->', DOTFILE.read_text()))
print(f'\nEdges: {edge_count_before} -> {edge_count_after}')

# Also ensure item node SHAs are correct (update if needed)
print('\n--- Item SHA verification ---')
items_to_verify = [
    ('ASSERTIONS-ASSERT_L0_31',   ITEMS_DIR/'ASSERTIONS/ASSERT_L0_31/ASSERTIONS-ASSERT_L0_31.md'),
    ('ASSUMPTIONS-ASSUMP_L0_31',  ITEMS_DIR/'ASSUMPTIONS/ASSUMP_L0_31/ASSUMPTIONS-ASSUMP_L0_31.md'),
    ('EVIDENCES-EVID_L0_31',      ITEMS_DIR/'EVIDENCES/EVID_L0_31/EVIDENCES-EVID_L0_31.md'),
    ('EXPECTATIONS-EXPECT_L0_31', ITEMS_DIR/'EXPECTATIONS/EXPECT_L0_31/EXPECTATIONS-EXPECT_L0_31.md'),
]
dot_content = DOTFILE.read_text()
for item_name, item_path in items_to_verify:
    sha_hex, _ = get_item_sha(item_name, item_path)
    stored_match = re.search(rf'"{re.escape(item_name)}" \[sha="([^"]+)"\]', dot_content)
    if stored_match:
        stored = stored_match.group(1)
        if stored == sha_hex:
            print(f'OK: {item_name}')
        else:
            # Update
            dot_content = re.sub(rf'"{re.escape(item_name)}" \[sha="[^"]+"\]',
                                  f'"{item_name}" [sha="{sha_hex}"]', dot_content)
            print(f'FIXED SHA: {item_name}')
    else:
        # Add item node
        new_line = f'"{item_name}" [sha="{sha_hex}"];\n'
        dot_content2 = dot_content.rstrip('\n')
        last_brace = dot_content2.rfind('}')
        dot_content = dot_content2[:last_brace] + new_line + '}\n'
        print(f'ADDED node: {item_name}')

DOTFILE.write_text(dot_content)
print(f'\nFinal lines: {len(DOTFILE.read_text().splitlines())}')
print(f'Final edges: {DOTFILE.read_text().count("->")}')
