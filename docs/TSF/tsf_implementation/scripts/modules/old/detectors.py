#!/usr/bin/env python3
"""
TSF Detectors Module
====================

Detects new requirements in tsf-requirements-table.md and identifies missing TSF items.

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
from urllib.parse import urlparse
from typing import List, Dict, Optional, Tuple


# ============================================================================
# Configuration
# ============================================================================

PROJECT_ROOT = Path(__file__).resolve().parents[5]
LO_REQUIREMENTS_PATH = PROJECT_ROOT / "docs/TSF/requirements/tsf-requirements-table.md"
TSF_ITEMS_BASE = PROJECT_ROOT / "docs/TSF/tsf_implementation"
SPRINTS_DIR = PROJECT_ROOT / "docs/sprints"

ITEM_TYPES = ['expectations', 'assertions', 'evidences', 'assumptions']
ITEM_PREFIXES = {
    'expectations': 'EXPECT',
    'assertions': 'ASSERT',
    'evidences': 'EVID',
    'assumptions': 'ASSUMP'
}


def normalize_file_path(path: str) -> Optional[str]:
    """
    Normalize a file path to be relative to PROJECT_ROOT.
    
    If the path exists as-is, return it.
    Otherwise, try common prefixes based on the repo structure.
    If no valid path found, return None.
    
    Args:
        path: The raw path from source files
    
    Returns:
        Normalized path relative to PROJECT_ROOT, or None if not found
    """
    if not path:
        return None
    
    # If it's an absolute path or starts with http, leave as-is
    if path.startswith('/') or path.startswith('http'):
        return path
    
    # Check if exists as-is from PROJECT_ROOT
    full_path = PROJECT_ROOT / path
    if full_path.exists() and full_path.is_file():
        return path
    
    # Try prepending docs/TSF/tsf_implementation/ for files in scripts/modules or TSF_docs
    candidate = PROJECT_ROOT / "docs/TSF/tsf_implementation" / path
    if candidate.exists() and candidate.is_file():
        return str(candidate.relative_to(PROJECT_ROOT))
    
    # Try prepending docs/ for files in docs/ subdirs
    candidate = PROJECT_ROOT / "docs" / path
    if candidate.exists() and candidate.is_file():
        return str(candidate.relative_to(PROJECT_ROOT))
    
    # If path starts with docs/TSF/, try adjusting to tsf_implementation/TSF_docs/
    if path.startswith('docs/TSF/'):
        adjusted_path = path.replace('docs/TSF/', 'docs/TSF/tsf_implementation/TSF_docs/', 1)
        candidate = PROJECT_ROOT / adjusted_path
        if candidate.exists() and candidate.is_file():
            return adjusted_path
    
    # If none found, return None
    return None


# ============================================================================
# Table Parsing
# ============================================================================

def parse_lo_requirements_table() -> List[Dict[str, str]]:
    """
    Parse the tsf-requirements-table.md markdown table.
    
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
        cells = [cell.strip() for cell in full_line.split('|', 5)]
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


def detect_deleted_requirements() -> List[str]:
    """
    Detect requirements that exist in items/ but were deleted from tsf-requirements-table.md.
    
    Returns:
        List of requirement IDs that should be removed from items/
        Example: ['L0-5', 'L0-12']
    """
    # Get all IDs from tsf-requirements-table.md
    requirements = parse_lo_requirements_table()
    table_ids = {req['id'] for req in requirements}
    
    # Get all IDs from items/ directory (check expectations as reference)
    existing_items = get_existing_tsf_items()
    items_ids = set(existing_items['expectations'])
    
    # Find IDs in items/ but not in table (deleted requirements)
    deleted_ids = items_ids - table_ids
    
    return sorted(list(deleted_ids))


def detect_modified_requirements() -> Dict[str, Dict[str, str]]:
    """
    Detect requirements where the requirement text has been modified.
    
    Returns:
        Dict mapping req_id to changes:
        {
            'L0-1': {
                'old_text': 'Old requirement text',
                'new_text': 'New requirement text',
                'changed': True
            },
            ...
        }
    """
    requirements = parse_lo_requirements_table()
    modifications = {}
    
    for req in requirements:
        req_id = req['id']
        new_text = req['requirement']
        
        # Read existing EXPECT file to get current requirement text
        expect_file = TSF_ITEMS_BASE / 'items' / 'expectations' / f'EXPECT-{req_id}.md'
        
        if expect_file.exists():
            content = expect_file.read_text()
            
            # Extract requirement from header (first ## after frontmatter)
            header_match = re.search(r'---\n.*?\n---\n\n## (.+?)\n', content, re.DOTALL)
            
            if header_match:
                old_text = header_match.group(1).strip()
                
                # Compare (normalize whitespace)
                if old_text != new_text:
                    modifications[req_id] = {
                        'old_text': old_text,
                        'new_text': new_text,
                        'changed': True
                    }
    
    return modifications


def detect_requirement_changes() -> Dict[str, List[str]]:
    """
    Detect changes in requirement IDs (new requirements added to tsf-requirements-table.md).
    
    Returns:
        Dict with keys:
        - 'new_ids': List of new requirement IDs not in items/
        - 'all_ids': List of all requirement IDs in tsf-requirements-table.md
    """
    requirements = parse_lo_requirements_table()
    all_ids = [req['id'] for req in requirements]
    
    # Get existing IDs from any item type (if exists in one type, not new)
    existing_items = get_existing_tsf_items()
    existing_ids_set = set()
    for item_type in ITEM_TYPES:
        existing_ids_set.update(existing_items[item_type])
    
    new_ids = [req_id for req_id in all_ids if req_id not in existing_ids_set]
    
    return {
        'new_ids': new_ids,
        'all_ids': all_ids
    }


def detect_evidence_changes() -> Dict[str, Dict[str, any]]:
    """
    Detect changes in Evidence column for each requirement.
    
    Compares current evidence in tsf-requirements-table.md with evidence in existing EVID-L0-X.md files.
    
    Returns:
        Dict mapping req_id to change info:
        {
            'L0-1': {
                'has_changes': False,
                'table_evidence': [...],  # Current evidence in tsf-requirements-table.md
                'file_evidence': [...],   # Evidence in EVID-L0-1.md (if exists)
                'new_evidence': [...],    # Items in table but not in file
                'removed_evidence': [...]  # Items in file but not in table
            },
            ...
        }
    """
    requirements = parse_lo_requirements_table()
    changes = {}
    
    for req in requirements:
        req_id = req['id']
        
        # Get current evidence from table
        table_evidence = extract_evidence_from_table(req_id)
        
        # Get existing evidence from EVID file (if exists)
        evid_file = TSF_ITEMS_BASE / 'items' / 'evidences' / f'EVID-{req_id}.md'
        file_evidence = []
        
        if evid_file.exists():
            # Parse YAML frontmatter to extract evidence references
            content = evid_file.read_text()
            
            # Extract evidence section from YAML frontmatter
            yaml_match = re.search(r'---\n(.*?)\n---', content, re.DOTALL)
            if yaml_match:
                yaml_content = yaml_match.group(1)
                
                # Find evidence references in YAML
                # Pattern: - path/to/file.md or - https://url
                evidence_lines = re.findall(r'^\s*-\s+(.+)$', yaml_content, re.MULTILINE)
                
                for line in evidence_lines:
                    line = line.strip()
                    if line.startswith('http'):
                        file_evidence.append({'type': 'url', 'path': line})
                    else:
                        file_evidence.append({'type': 'file', 'path': line})
        
        # Compare evidence sets
        table_paths = {ev['path'] for ev in table_evidence}
        file_paths = {ev['path'] for ev in file_evidence}
        
        new_evidence = [ev for ev in table_evidence if ev['path'] not in file_paths]
        removed_evidence = [ev for ev in file_evidence if ev['path'] not in table_paths]
        
        has_changes = len(new_evidence) > 0 or len(removed_evidence) > 0
        
        changes[req_id] = {
            'has_changes': has_changes,
            'table_evidence': table_evidence,
            'file_evidence': file_evidence,
            'new_evidence': new_evidence,
            'removed_evidence': removed_evidence
        }
    
    return changes


def get_complete_changeset() -> Dict[str, any]:
    """
    Get complete changeset for all requirements (CRUD operations).
    
    This is the MASTER function that detects:
    - New requirements added to tsf-requirements-table.md
    - Deleted requirements removed from tsf-requirements-table.md
    - Modified requirement text
    - Evidence changes (additions/removals)
    
    Returns:
        Dict with complete changeset:
        {
            'new_requirements': List[Dict],      # New req dicts from table
            'deleted_requirements': List[str],   # Req IDs to delete
            'modified_requirements': Dict,       # Req IDs with text changes
            'evidence_changes': Dict,            # Evidence additions/removals per req
            'has_changes': bool                  # True if any changes detected
        }
    """
    # Detect all types of changes
    new_items = detect_new_items()
    deleted_ids = detect_deleted_requirements()
    modified_reqs = detect_modified_requirements()
    evidence_changes = detect_evidence_changes()
    
    # Get full requirement dicts for new items
    new_requirements = new_items.get('expectations', [])  # Use expectations as reference
    
    # Filter evidence_changes to only those with actual changes
    evidence_with_changes = {
        req_id: change_info 
        for req_id, change_info in evidence_changes.items() 
        if change_info['has_changes']
    }
    
    # Check if any changes exist
    has_changes = (
        len(new_requirements) > 0 or 
        len(deleted_ids) > 0 or 
        len(modified_reqs) > 0 or 
        len(evidence_with_changes) > 0
    )
    
    changeset = {
        'new_requirements': new_requirements,
        'deleted_requirements': deleted_ids,
        'modified_requirements': modified_reqs,
        'evidence_changes': evidence_with_changes,
        'has_changes': has_changes
    }
    
    return changeset


def suggest_new_evidences() -> Dict[str, List[Dict]]:
    """
    Scan sprints/ directory for new evidence references that should be added to tsf-requirements-table.md.
    
    This function:
    1. Scans all sprint*.md files for EXPECT-L0-X references
    2. Extracts URLs and file paths from context (±200 chars)
    3. Compares with existing Evidence column in tsf-requirements-table.md
    4. Returns NEW evidences that should be added (automatic update)
    
    Returns:
        Dict mapping req_id to list of new evidence suggestions:
        {
            'L0-1': [
                {'type': 'url', 'path': 'https://...', 'source': 'sprint1.md', 'context': '...'},
                {'type': 'file', 'path': 'docs/guides/...', 'source': 'sprint2.md', 'context': '...'}
            ],
            ...
        }
    """
    # Get current evidences from table
    requirements = parse_lo_requirements_table()
    table_evidences = {}
    for req in requirements:
        req_id = req['id']
        table_evidences[req_id] = {ev['path'] for ev in extract_evidence_from_table(req_id)}
    
    # Scan sprints for new evidences
    sprint_evidences = scan_sprint_evidence()
    
    # Find NEW evidences (in sprints but not in table)
    suggestions = {}
    
    for req_id, sprint_refs in sprint_evidences.items():
        new_evidences = []
        
        for ref in sprint_refs:
            path = ref['path']
            
            # Check if evidence already exists in table
            if req_id in table_evidences and path in table_evidences[req_id]:
                continue  # Already in table, skip
            
            # NEW evidence found!
            new_evidences.append({
                'type': ref['type'],
                'path': path,
                'source': ref.get('sprint', 'unknown'),
                'context': ref.get('context', '')[:100] + '...'  # Truncate context
            })
        
        if new_evidences:
            suggestions[req_id] = new_evidences
    
    return suggestions


def has_any_changes() -> bool:
    """
    Check if there are ANY changes in tsf-requirements-table.md (new IDs or evidence changes).
    
    Returns:
        True if changes detected, False otherwise
    """
    # Check for new requirement IDs
    req_changes = detect_requirement_changes()
    if req_changes['new_ids']:
        return True
    
    # Check for evidence changes
    evidence_changes = detect_evidence_changes()
    for req_id, change_info in evidence_changes.items():
        if change_info['has_changes']:
            return True
    
    return False


# ============================================================================
# Evidence Extraction
# ============================================================================

def extract_evidence_from_table(req_id: str) -> List[Dict[str, str]]:
    """
    Extract evidence references from the Evidence column in tsf-requirements-table.md.
    
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
        
        # Add file reference if it looks like a relative path
        if not path.startswith('http') and '/' in path:
            normalized = normalize_file_path(path)
            if normalized is not None and os.path.exists(PROJECT_ROOT / normalized):
                evidence_refs.append({'type': 'file', 'path': normalized})
        
        # Add URL reference only if it's not a GitHub blob link or project link to our own repo
        if url and not ('github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob' in url or 
                        'github.com/orgs/SEAME-pt/projects' in url):
            evidence_refs.append({'type': 'url', 'path': url})
    
    # Pattern 2: Standalone URLs (https://...)
    pattern2 = r'(?<!`)(https?://[^\s<)]+)'
    for match in re.finditer(pattern2, evidence_text):
        url = match.group(1).strip()
        # Clean URL by removing trailing invalid characters
        url = re.sub(r'[|.,;:!?]+$', '', url)
        # Avoid duplicates
        if not any(ref['path'] == url for ref in evidence_refs):
            evidence_refs.append({'type': 'url', 'path': url})
    
    # Pattern 3: Standalone file paths in backticks without URLs
    # Example: `docs/guides/github-guidelines.md`
    pattern3 = r'`([^`]+\.(md|jpg|jpeg|png|pdf|py|sh))`'
    for match in re.finditer(pattern3, evidence_text):
        path = match.group(1).strip()
        if not path.startswith('http') and not any(ref['path'] == path for ref in evidence_refs):
            normalized = normalize_file_path(path)
            if normalized is not None and os.path.exists(PROJECT_ROOT / normalized):
                evidence_refs.append({'type': 'file', 'path': normalized})
    
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
            
            # Extract context around the EXPECT reference (±500 chars)
            start = max(0, match.start() - 500)
            end = min(len(content), match.end() + 500)
            context = content[start:end]
            
            # Find URLs in context
            url_pattern = r'(https?://[^\s)]+)'
            urls = re.findall(url_pattern, context)
            
            # Filter valid URLs (exclude GitHub blob links and project links to avoid fetch errors)
            valid_urls = []
            for url in urls:
                parsed = urlparse(url)
                if (parsed.netloc and parsed.scheme in ['http', 'https'] and 
                    not ('github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob' in url or
                         'github.com/orgs/SEAME-pt/projects' in url)):
                    valid_urls.append(url)
            
            # Find file paths in context (inside backticks or parentheses)
            file_pattern = r'[`(]([^`)]+\.(md|jpg|jpeg|png|pdf|py|sh))[`)]'
            files = re.findall(file_pattern, context)
            
            if found_id not in sprint_evidence:
                sprint_evidence[found_id] = []
            
            for url in valid_urls:
                sprint_evidence[found_id].append({
                    'type': 'url',
                    'path': url,
                    'sprint': sprint_name
                })
            
            for file_path, _ in files:
                if not file_path.startswith('http'):
                    normalized = normalize_file_path(file_path)
                    if normalized is not None and os.path.exists(PROJECT_ROOT / normalized):
                        sprint_evidence[found_id].append({
                            'type': 'file',
                            'path': normalized,
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
    print("\n1. Parsing tsf-requirements-table.md table...")
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
    
    # Test 7: Get complete changeset (MASTER function)
    print("\n7. Getting COMPLETE CHANGESET...")
    changeset = get_complete_changeset()
    
    print(f"   🔍 Changes detected: {'YES ✅' if changeset['has_changes'] else 'NO ❌'}")
    print(f"   📦 Backup needed: {'YES' if changeset['has_changes'] else 'NO (skip)'}")
    print()
    
    # New requirements
    print(f"   ➕ New requirements: {len(changeset['new_requirements'])}")
    if changeset['new_requirements']:
        for req in changeset['new_requirements'][:3]:
            print(f"      - {req['id']}: {req['requirement'][:60]}...")
    
    # Deleted requirements
    print(f"   ➖ Deleted requirements: {len(changeset['deleted_requirements'])}")
    if changeset['deleted_requirements']:
        print(f"      IDs to remove: {', '.join(changeset['deleted_requirements'])}")
    
    # Modified requirements
    print(f"   ✏️  Modified requirements: {len(changeset['modified_requirements'])}")
    if changeset['modified_requirements']:
        for req_id in list(changeset['modified_requirements'].keys())[:3]:
            print(f"      - {req_id}: Text changed")
    
    # Evidence changes
    print(f"   📝 Evidence changes: {len(changeset['evidence_changes'])}")
    if changeset['evidence_changes']:
        for req_id, change_info in list(changeset['evidence_changes'].items())[:3]:
            print(f"      - {req_id}: +{len(change_info['new_evidence'])} / -{len(change_info['removed_evidence'])}")
    
    # Test 8: Suggest new evidences from sprints (AUTO-UPDATE feature)
    print("\n8. Suggesting NEW evidences from sprints/ (for auto-update)...")
    suggestions = suggest_new_evidences()
    print(f"   Requirements with new evidence suggestions: {len(suggestions)}")
    
    if suggestions:
        for req_id, new_evs in list(suggestions.items())[:3]:
            print(f"   {req_id}: {len(new_evs)} new evidence(s) found")
            for ev in new_evs[:2]:  # Show first 2
                print(f"      - [{ev['type']}] {ev['path'][:60]}... (from {ev['source']})")
    else:
        print("   ✅ No new evidences found - table is up to date with sprints")
    
    print("\n" + "=" * 70)
    print("DETECTORS TEST COMPLETE")
    print("=" * 70)


if __name__ == '__main__':
    main()
