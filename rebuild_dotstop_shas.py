#!/usr/bin/env python3
"""Recompute and update ALL item SHAs and edge SHAs in .dotstop.dot.

Required after changing localplugins/references.py which affects hash computation.
"""
import sys
import os
import re
import hashlib
from pathlib import Path

os.chdir('/home/seame/Documents/SEA-ME_Team6_2025-26')
sys.path.insert(0, '.venv/lib/python3.11/site-packages')

from trudag.dotstop.core.item import item_from_markdown

ITEMS_DIR = Path('docs/TSF/tsf_implementation/.trudag_items')
DOTFILE = Path('.dotstop.dot')

# Step 1: Collect all .trudag_items files and compute their SHAs
print("=== Step 1: Computing SHAs for all items ===")
item_shas = {}   # name -> (sha_hex, sha256_bytes)

categories = ['ASSERTIONS', 'ASSUMPTIONS', 'EVIDENCES', 'EXPECTATIONS']
for category in categories:
    cat_dir = ITEMS_DIR / category
    if not cat_dir.exists():
        continue
    for item_dir in sorted(cat_dir.iterdir()):
        if not item_dir.is_dir():
            continue
        # Find the .md file
        md_files = list(item_dir.glob('*.md'))
        if not md_files:
            continue
        md_file = md_files[0]
        item_name = md_file.stem  # e.g., "ASSERTIONS-ASSERT_L0_1"
        try:
            md_content = md_file.read_text()
            item = item_from_markdown(item_name, md_content)
            item_shas[item_name] = (item.sha, item.sha256)
        except Exception as e:
            print(f'  ERROR loading {item_name}: {e}')

print(f"  Loaded {len(item_shas)} items")

# Step 2: Read current .dotstop.dot
dot_content = DOTFILE.read_text()

# Step 3: Update all item node SHAs
print("\n=== Step 2: Updating item node SHAs in .dotstop.dot ===")
updated_items = 0
not_found_items = 0

for item_name, (sha_hex, sha256_bytes) in item_shas.items():
    old_pattern = rf'"{re.escape(item_name)}" \[sha="([^"]*)"\]'
    match = re.search(old_pattern, dot_content)
    if match:
        old_sha = match.group(1)
        if old_sha != sha_hex:
            dot_content = re.sub(old_pattern, f'"{item_name}" [sha="{sha_hex}"]', dot_content)
            updated_items += 1
    else:
        not_found_items += 1

print(f"  Updated: {updated_items} item SHAs")
print(f"  Not found in .dotstop.dot: {not_found_items}")

# Step 4: Update all edge SHAs
print("\n=== Step 3: Updating edge SHAs in .dotstop.dot ===")
updated_edges = 0
broken_edges = 0

# Find all edge patterns
edge_pattern = r'"([^"]+)" -> "([^"]+)" \[sha="([^"]*)"\]'
for match in re.finditer(edge_pattern, dot_content):
    parent_name = match.group(1)
    child_name = match.group(2)
    old_sha = match.group(3)
    
    if parent_name in item_shas and child_name in item_shas:
        parent_sha256 = item_shas[parent_name][1]
        child_sha256 = item_shas[child_name][1]
        
        # Recompute link SHA
        h = hashlib.sha256()
        h.update(parent_sha256 + child_sha256)
        new_sha = h.hexdigest()
        
        if old_sha != new_sha:
            old_edge = match.group(0)
            new_edge = f'"{parent_name}" -> "{child_name}" [sha="{new_sha}"]'
            dot_content = dot_content.replace(old_edge, new_edge, 1)
            updated_edges += 1
    else:
        broken_edges += 1
        missing = []
        if parent_name not in item_shas:
            missing.append(f'parent={parent_name}')
        if child_name not in item_shas:
            missing.append(f'child={child_name}')
        print(f'  BROKEN edge (missing SHA for): {", ".join(missing)}')

print(f"  Updated: {updated_edges} edge SHAs")
print(f"  Broken edges (unknown items): {broken_edges}")

# Step 5: Write updated .dotstop.dot
DOTFILE.write_text(dot_content)
print(f"\n=== Done ===")
print(f"Final lines: {len(dot_content.splitlines())}")
node_count = len(re.findall(r'\[sha=', dot_content))
print(f"Final item nodes: {node_count}")
print(f"Final edges: {dot_content.count(' -> ')}")
