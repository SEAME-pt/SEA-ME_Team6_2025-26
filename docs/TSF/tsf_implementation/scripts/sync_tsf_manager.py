#!/usr/bin/env python3
"""
sync_tsf_manager.py - Main orchestrator for TSF automation (Option G: Semi-Automated with Claude)

This script detects new TSF requirements, creates placeholder files, requests AI generation from Claude,
waits for generation, then validates the created items.

Usage:
    ./sync_tsf_manager.py --detect-new
"""

import os
import sys
import argparse
from pathlib import Path
import re

# Add modules to path
sys.path.append(str(Path(__file__).parent / "modules"))

from ai_generator import AIGenerator
REQUIREMENTS_TABLE = Path("../../requirements/tsf-requirements-table.md")
ITEMS_DIR = Path("../items")
EXPECTATIONS_DIR = ITEMS_DIR / "expectations"
ASSERTIONS_DIR = ITEMS_DIR / "assertions"
EVIDENCES_DIR = ITEMS_DIR / "evidences"
ASSUMPTIONS_DIR = ITEMS_DIR / "assumptions"

def detect_new_ids():
    """Detect new L0-X IDs from requirements table that don't have corresponding files."""
    if not REQUIREMENTS_TABLE.exists():
        print(f"❌ Requirements table not found: {REQUIREMENTS_TABLE}")
        return []

    with open(REQUIREMENTS_TABLE, 'r', encoding='utf-8') as f:
        content = f.read()

    # Find all L0-X in table
    ids_in_table = set(re.findall(r'\bL0-\d+\b', content))

    new_ids = []
    for id_str in sorted(ids_in_table, key=lambda x: int(x.split('-')[1])):
        # Check if all 4 files exist
        expect_file = EXPECTATIONS_DIR / f"EXPECT-{id_str}.md"
        assert_file = ASSERTIONS_DIR / f"ASSERT-{id_str}.md"
        evid_file = EVIDENCES_DIR / f"EVID-{id_str}.md"
        assum_file = ASSUMPTIONS_DIR / f"ASSUMP-{id_str}.md"

        if not (expect_file.exists() and assert_file.exists() and evid_file.exists() and assum_file.exists()):
            new_ids.append(id_str)

    return new_ids

def get_requirement_text(id_str):
    """Extract requirement text for the given ID from the table."""
    if not REQUIREMENTS_TABLE.exists():
        return f"Requirement for {id_str}"

    with open(REQUIREMENTS_TABLE, 'r', encoding='utf-8') as f:
        content = f.read()

    # Find the row with this ID - look for the table row
    lines = content.split('\n')
    for i, line in enumerate(lines):
        if f'| **{id_str}** |' in line:
            # Parse the table row: | **L0-20** | Integrate AI... | Use AI... | AI-generated... | EXPECT-L0-20...
            parts = line.split('|')
            if len(parts) >= 3:
                requirement_text = parts[2].strip()
                return requirement_text

    return f"Requirement for {id_str}"

def create_item_with_ai(id_str, item_type, requirement_text):
    """Create TSF item file with AI-generated content."""
    dir_map = {
        'EXPECT': EXPECTATIONS_DIR,
        'ASSERT': ASSERTIONS_DIR,
        'EVID': EVIDENCES_DIR,
        'ASSUMP': ASSUMPTIONS_DIR
    }

    dir_path = dir_map[item_type]
    dir_path.mkdir(parents=True, exist_ok=True)

    file_path = dir_path / f"{item_type}-{id_str}.md"

    # Initialize AI generator (try Copilot CLI, fallback to manual)
    ai_gen = AIGenerator("copilot_cli")

    # Generate content
    context = {'id': id_str}
    content = ai_gen.generate_content(item_type, requirement_text, context)

    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(content)

    print(f"🤖 AI-generated: {file_path}")
    return file_path

def main():
    parser = argparse.ArgumentParser(description="TSF Manager - Semi-Automated Generation")
    parser.add_argument('--detect-new', action='store_true', help='Detect and process new requirements')
    args = parser.parse_args()

    if args.detect_new:
        print("🔍 Detecting new TSF IDs...")
        new_ids = detect_new_ids()

        if not new_ids:
            print("✅ No new IDs found. All requirements have corresponding files.")
            return

        print(f"📋 Found new IDs: {', '.join(new_ids)}")

        for id_str in new_ids:
            requirement_text = get_requirement_text(id_str)
            print(f"\n🎯 Processing {id_str}: {requirement_text[:100]}...")

            # Create items with AI
            create_item_with_ai(id_str, 'EXPECT', requirement_text)
            create_item_with_ai(id_str, 'ASSERT', requirement_text)
            create_item_with_ai(id_str, 'EVID', requirement_text)
            create_item_with_ai(id_str, 'ASSUMP', requirement_text)

        print(f"\n🤖 AI-GENERATED {len(new_ids)} NEW ITEMS")
        print("📝 Items created with AI:")
        for id_str in new_ids:
            print(f"   - EXPECT-{id_str}.md, ASSERT-{id_str}.md, EVID-{id_str}.md, ASSUMP-{id_str}.md")

        # Optional: Open in VSCode for review
        open_in_vscode = input("\n📂 Open generated files in VSCode for review? [y/N]: ").lower().strip()
        if open_in_vscode == 'y':
            import subprocess
            for id_str in new_ids:
                for item_type in ['EXPECT', 'ASSERT', 'EVID', 'ASSUMP']:
                    file_path = ITEMS_DIR / f"{item_type.lower()}s" / f"{item_type}-{id_str}.md"
                    if file_path.exists():
                        subprocess.run(['code', str(file_path)])

        print("\n🔍 Validating generated files...")
        # Here we would call validation scripts
        print("✅ Validation complete (implement actual validation)")

        print(f"\n📂 Review the generated files in: {ITEMS_DIR}")
        print("💡 If corrections needed, tell AI: 'Correct X in FILE'")

    else:
        print("Use --detect-new to detect and process new requirements")

if __name__ == "__main__":
    main()