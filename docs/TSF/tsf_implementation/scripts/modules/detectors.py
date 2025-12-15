#!/usr/bin/env python3
"""
TSF Detectors Module
====================

Detects new requirements in LO_requirements.md and identifies missing TSF items.

Functions:
    - parse_lo_requirements_table(): Parse the markdown table
    - detect_new_items(): Find IDs without corresponding TSF items
    - extract_evidence_from_table(): Extract evidence column content
    - scan_sprint_evidence(): Find evidence references in sprint files
    
Author: SEA-ME Team 6
Date: December 13, 2025
"""

import re
import os
from pathlib import Path
from typing import List, Dict, Optional, Tuple


# ============================================================================
# Configuration
# ============================================================================

PROJECT_ROOT = Path(__file__).resolve().parents[5]
LO_REQUIREMENTS_PATH = PROJECT_ROOT / "docs/TSF/requirements/LO_requirements.md"
TSF_ITEMS_BASE = PROJECT_ROOT / "docs/TSF/tsf_implementation"
SPRINTS_DIR = PROJECT_ROOT / "docs/sprints"

ITEM_TYPES = ['expectations', 'assertions', 'evidences', 'assumptions']
ITEM_PREFIXES = {
    'expectations': 'EXPECT',
    'assertions': 'ASSERT',
    'evidences': 'EVID',
    'assumptions': 'ASSUMP'
}


# ============================================================================
# Table Parsing
# ============================================================================

def parse_lo_requirements_table() -> List[Dict[str, str]]:
    """
    Parse the LO_requirements.md markdown table.
    
    Returns:
        List of dicts with keys: id, requirement, acceptance, verification, evidence
    
    Example:
        [
            {
                'id': 'L0-1',
                'requirement': 'The system architecture shall be defined...',
                'acceptance': 'A system architecture diagram...',
                'verification': 'Documentation inspection.',
                'evidence': 'EXPECT-L0-1 — System architecture...'
            },
            ...
        ]
    """
    if not LO_REQUIREMENTS_PATH.exists():
        raise FileNotFoundError(f"LO requirements file not found: {LO_REQUIREMENTS_PATH}")
    
    with open(LO_REQUIREMENTS_PATH, 'r', encoding='utf-8') as f:
        content = f.read()
    
    requirements = []
    
    # Simple approach: Use regex to find all L0-X IDs in the table
    # Then extract full row content for each ID
    
    # Find IDs with regex
    id_pattern = r'\|\s*\*\*L0-(\d+)\*\*\s*\|'
    id_matches = re.finditer(id_pattern, content)
    
    for match in id_matches:
        req_id = f"L0-{match.group(1)}"
        
        # Find the line position
        line_start = content.rfind('\n', 0, match.start()) + 1
        line_end = content.find('\n', match.end())
        if line_end == -1:
            line_end = len(content)
        
        full_line = content[line_start:line_end]
        
        # Split by | and extract cells
        cells = [cell.strip() for cell in full_line.split('|')]
        # Remove empty first/last cells
        cells = [c for c in cells if c]
        
        if len(cells) >= 5:
            requirements.append({
                'id': req_id,
                'requirement': cells[1].strip('*').strip(),
                'acceptance': cells[2].strip(),
                'verification': cells[3].strip(),
                'evidence': cells[4].strip() if len(cells) > 4 else ''
            })
    
    return requirements


# ============================================================================
# TSF Item Detection
# ============================================================================

def get_existing_tsf_items() -> Dict[str, List[str]]:
    """
    Scan TSF directories and return existing item IDs.
    
    Returns:
        Dict mapping item type to list of IDs
        Example: {'expectations': ['L0-1', 'L0-2'], 'assertions': ['L0-1'], ...}
    """
    existing = {item_type: [] for item_type in ITEM_TYPES}
    
    for item_type in ITEM_TYPES:
        item_dir = TSF_ITEMS_BASE / item_type
        if not item_dir.exists():
            continue
        
        prefix = ITEM_PREFIXES[item_type]
        
        for item_file in item_dir.glob(f'{prefix}-L0-*.md'):
            # Extract ID: EXPECT-L0-1.md -> L0-1
            match = re.search(r'L0-\d+', item_file.stem)
            if match:
                existing[item_type].append(match.group(0))
    
    return existing


def detect_new_items() -> Dict[str, List[Dict[str, str]]]:
    """
    Detect requirements that don't have corresponding TSF items.
    
    Returns:
        Dict mapping item type to list of missing requirement dicts
        Example: 
        {
            'expectations': [{'id': 'L0-18', 'requirement': '...', ...}],
            'assertions': [{'id': 'L0-18', 'requirement': '...', ...}],
            ...
        }
    """
    requirements = parse_lo_requirements_table()
    existing_items = get_existing_tsf_items()
    
    new_items = {item_type: [] for item_type in ITEM_TYPES}
    
    for req in requirements:
        req_id = req['id']
        
        # Check each item type
        for item_type in ITEM_TYPES:
            if req_id not in existing_items[item_type]:
                new_items[item_type].append(req)
    
    return new_items


# ============================================================================
# Evidence Extraction
# ============================================================================

def extract_evidence_from_table(req_id: str) -> List[Dict[str, str]]:
    """
    Extract evidence references from the Evidence column in LO_requirements.md.
    
    Args:
        req_id: Requirement ID (e.g., 'L0-1')
    
    Returns:
        List of evidence dicts with 'type' and 'path' keys
        Example:
        [
            {'type': 'file', 'path': 'docs/demos/assembled-car1.jpeg'},
            {'type': 'url', 'path': 'https://github.com/...'},
            {'type': 'file', 'path': 'docs/guides/Power Consumption Analysis.md'}
        ]
    """
    requirements = parse_lo_requirements_table()
    
    for req in requirements:
        if req['id'] == req_id:
            evidence_text = req['evidence']
            break
    else:
        return []  # Requirement not found
    
    if not evidence_text or evidence_text.strip() in ['', '(vazio inicialmente)']:
        return []
    
    evidence_refs = []
    
    # Pattern 1: `path` — URL
    # Example: `docs/demos/assembled-car1.jpeg` — https://github.com/...
    pattern1 = r'`([^`]+)`\s*—\s*(https?://[^\s<]+)'
    for match in re.finditer(pattern1, evidence_text):
        path = match.group(1).strip()
        url = match.group(2).strip()
        
        # Determine type based on URL or path
        if url:
            evidence_refs.append({'type': 'url', 'path': url})
        
        # Also add file reference if it looks like a relative path
        if not path.startswith('http') and '/' in path:
            evidence_refs.append({'type': 'file', 'path': path})
    
    # Pattern 2: Standalone URLs (https://...)
    pattern2 = r'(?<!`)(https?://[^\s<)]+)'
    for match in re.finditer(pattern2, evidence_text):
        url = match.group(1).strip()
        # Avoid duplicates
        if not any(ref['path'] == url for ref in evidence_refs):
            evidence_refs.append({'type': 'url', 'path': url})
    
    # Pattern 3: Standalone file paths in backticks without URLs
    # Example: `docs/guides/github-guidelines.md`
    pattern3 = r'`([^`]+\.(md|jpg|jpeg|png|pdf|py|sh))`'
    for match in re.finditer(pattern3, evidence_text):
        path = match.group(1).strip()
        if not path.startswith('http') and not any(ref['path'] == path for ref in evidence_refs):
            evidence_refs.append({'type': 'file', 'path': path})
    
    return evidence_refs


# ============================================================================
# Sprint Evidence Scanning
# ============================================================================

def scan_sprint_evidence(req_id: Optional[str] = None) -> Dict[str, List[Dict[str, str]]]:
    """
    Scan sprint files for EXPECT references and extract evidence links.
    
    Args:
        req_id: Optional requirement ID to filter (e.g., 'L0-1'). If None, scan all.
    
    Returns:
        Dict mapping requirement ID to list of evidence references
        Example:
        {
            'L0-1': [
                {'type': 'file', 'path': 'docs/demos/assembled-car1.jpeg', 'sprint': 'sprint2'},
                {'type': 'url', 'path': 'https://github.com/...', 'sprint': 'sprint2'}
            ],
            ...
        }
    """
    if not SPRINTS_DIR.exists():
        print(f"Warning: Sprints directory not found: {SPRINTS_DIR}")
        return {}
    
    sprint_evidence = {}
    
    for sprint_file in SPRINTS_DIR.glob('sprint*.md'):
        sprint_name = sprint_file.stem
        
        with open(sprint_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Find EXPECT-L0-X references
        expect_pattern = r'EXPECT-L0-(\d+)'
        
        for match in re.finditer(expect_pattern, content):
            found_id = f"L0-{match.group(1)}"
            
            # If filtering by req_id, skip others
            if req_id and found_id != req_id:
                continue
            
            # Extract context around the EXPECT reference (±200 chars)
            start = max(0, match.start() - 200)
            end = min(len(content), match.end() + 200)
            context = content[start:end]
            
            # Find URLs in context
            url_pattern = r'(https?://[^\s)]+)'
            urls = re.findall(url_pattern, context)
            
            # Find file paths in context (inside backticks or parentheses)
            file_pattern = r'[`(]([^`)]+\.(md|jpg|jpeg|png|pdf|py|sh))[`)]'
            files = re.findall(file_pattern, context)
            
            if found_id not in sprint_evidence:
                sprint_evidence[found_id] = []
            
            for url in urls:
                sprint_evidence[found_id].append({
                    'type': 'url',
                    'path': url,
                    'sprint': sprint_name
                })
            
            for file_path, _ in files:
                sprint_evidence[found_id].append({
                    'type': 'file',
                    'path': file_path,
                    'sprint': sprint_name
                })
    
    return sprint_evidence


# ============================================================================
# Unified Detection
# ============================================================================

def get_complete_evidence(req_id: str) -> Tuple[List[Dict], List[Dict]]:
    """
    Get evidence from both table and sprint files.
    
    Args:
        req_id: Requirement ID (e.g., 'L0-1')
    
    Returns:
        Tuple of (table_evidence, sprint_evidence)
    """
    table_evidence = extract_evidence_from_table(req_id)
    sprint_evidence_dict = scan_sprint_evidence(req_id)
    sprint_evidence = sprint_evidence_dict.get(req_id, [])
    
    return table_evidence, sprint_evidence


# ============================================================================
# Main Test Function
# ============================================================================

def main():
    """Test detectors functionality."""
    print("=" * 70)
    print("TSF DETECTORS TEST")
    print("=" * 70)
    
    # Test 1: Parse requirements table
    print("\n1. Parsing LO_requirements.md table...")
    requirements = parse_lo_requirements_table()
    print(f"   Found {len(requirements)} requirements")
    if requirements:
        print(f"   First requirement: {requirements[0]['id']} - {requirements[0]['requirement'][:50]}...")
        print(f"   Last requirement: {requirements[-1]['id']} - {requirements[-1]['requirement'][:50]}...")
    
    # Test 2: Detect existing TSF items
    print("\n2. Scanning existing TSF items...")
    existing = get_existing_tsf_items()
    for item_type, ids in existing.items():
        print(f"   {item_type}: {len(ids)} items ({', '.join(ids[:5])}{'...' if len(ids) > 5 else ''})")
    
    # Test 3: Detect new items
    print("\n3. Detecting new (missing) TSF items...")
    new_items = detect_new_items()
    total_missing = sum(len(items) for items in new_items.values())
    print(f"   Total missing items: {total_missing}")
    
    for item_type, items in new_items.items():
        if items:
            print(f"   {item_type}: {len(items)} missing")
            for item in items[:3]:  # Show first 3
                print(f"      - {item['id']}: {item['requirement'][:50]}...")
    
    # Test 4: Extract evidence from table
    print("\n4. Testing evidence extraction from table...")
    test_id = 'L0-1'
    table_evidence = extract_evidence_from_table(test_id)
    print(f"   Evidence for {test_id}: {len(table_evidence)} references")
    for ev in table_evidence[:3]:
        print(f"      - {ev['type']}: {ev['path'][:60]}...")
    
    # Test 5: Scan sprint evidence
    print("\n5. Scanning sprint files for evidence...")
    sprint_evidence = scan_sprint_evidence()
    print(f"   Found evidence for {len(sprint_evidence)} requirements across sprints")
    for req_id, refs in list(sprint_evidence.items())[:3]:
        print(f"   {req_id}: {len(refs)} references")
        for ref in refs[:2]:
            print(f"      - {ref['type']} ({ref['sprint']}): {ref['path'][:50]}...")
    
    # Test 6: Complete evidence for one requirement
    print("\n6. Complete evidence extraction for L0-1...")
    table_ev, sprint_ev = get_complete_evidence('L0-1')
    print(f"   Table evidence: {len(table_ev)} items")
    print(f"   Sprint evidence: {len(sprint_ev)} items")
    print(f"   Total: {len(table_ev) + len(sprint_ev)} evidence references")
    
    print("\n" + "=" * 70)
    print("DETECTORS TEST COMPLETE")
    print("=" * 70)


if __name__ == '__main__':
    main()
