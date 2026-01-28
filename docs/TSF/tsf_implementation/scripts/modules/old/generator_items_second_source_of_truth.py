#!/usr/bin/env python3
"""
TSF Source of Truth Generator
==============================

Manages the items/ directory as the source of truth for TSF items.
Implements full CRUD operations: Create, Read, Update, Delete.

This script:
- NEVER deletes items/ unless tsf-requirements-table.md explicitly removes requirement
- ONLY adds/updates based on changeset from detectors.py
- Synchronizes evidences bidirectionally with tsf-requirements-table.md
- Creates EVID-L0-X.md always (even without evidences)

Functions:
    - add_new_requirement() - Create 4 items for new requirement
    - update_requirement() - Update existing items with new text
    - delete_requirement() - Remove 4 items for deleted requirement
    - sync_evidences() - Add/remove evidence references
    - apply_changeset() - Apply complete changeset from detectors
    
Author: SEA-ME Team 6
Date: December 15, 2025
"""

import re
import os
import sys
from pathlib import Path
from typing import Dict, List, Optional, Tuple
from datetime import datetime
import json

# Import detectors
try:
    from detectors import get_complete_changeset, extract_evidence_from_table, get_complete_evidence
except ImportError:
    sys.path.insert(0, str(Path(__file__).parent))
    from detectors import get_complete_changeset, extract_evidence_from_table, get_complete_evidence


# ============================================================================
# Configuration
# ============================================================================

PROJECT_ROOT = Path(__file__).resolve().parents[5]
TSF_ITEMS_BASE = PROJECT_ROOT / "docs/TSF/tsf_implementation/items"

ITEM_TYPES = {
    'expectations': 'EXPECT',
    'assertions': 'ASSERT',
    'evidences': 'EVID',
    'assumptions': 'ASSUMP'
}

DEFAULT_REVIEWER = {
    'name': 'Joao Jesus Silva',
    'email': 'joao.silva@seame.pt'
}

# Extended keyword categories (including heartbeat)
KEYWORDS_EXTENDED = {
    'hardware': ['hardware', 'assembly', 'component', 'motor', 'servo', 'display', 'raspberry', 'hailo', 'stm32', 'board', 'sensor'],
    'software': ['software', 'qt', 'application', 'gui', 'program', 'code', 'crosscompile', 'algorithm'],
    'communication': ['can', 'communication', 'message', 'protocol', 'signal', 'bidirectional', 'can bus', 'data'],
    'system': ['architecture', 'system', 'design', 'integration', 'interface'],
    'testing': ['test', 'verification', 'validation', 'demonstration', 'inspection', 'qa'],
    'documentation': ['document', 'documentation', 'guide', 'readme', 'specification', 'manual'],
    'process': ['agile', 'scrum', 'github', 'ci/cd', 'workflow', 'tsf', 'methodology'],
    'display': ['display', 'ui', 'interface', 'screen', 'visual', 'mockup', 'show', 'gui'],
    'realtime': ['threadx', 'rtos', 'thread', 'real-time', 'scheduler', 'timing', 'latency'],
    'heartbeat': ['heartbeat', 'heart beat', 'heart-beat', 'alive', 'watchdog', 'health check', 'health']
}


# ============================================================================
# Helper Functions
# ============================================================================

def detect_category(requirement_text: str) -> str:
    """
    Detect the primary category of a requirement based on keywords.
    
    Args:
        requirement_text: The requirement description
        
    Returns:
        Category name (hardware, software, communication, etc.)
    """
    text_lower = requirement_text.lower()
    
    # Count keyword matches for each category
    scores = {}
    for category, keywords in KEYWORDS_EXTENDED.items():
        scores[category] = sum(1 for kw in keywords if kw in text_lower)
    
    # Return category with highest score, default to 'system'
    if max(scores.values()) == 0:
        return 'system'
    
    return max(scores, key=scores.get)


def sanitize_header(text: str, max_length: int = 60) -> str:
    """
    Create a clean header from requirement text.
    
    Args:
        text: Full requirement text
        max_length: Maximum header length
        
    Returns:
        Sanitized header string
    """
    # Remove "The system shall" patterns
    text = re.sub(r'^(the\s+)?(\w+\s+)?shall\s+', '', text, flags=re.IGNORECASE)
    
    # Take first sentence or clause
    text = re.split(r'[.;,]', text)[0].strip()
    
    # Truncate if too long
    if len(text) > max_length:
        text = text[:max_length].rsplit(' ', 1)[0] + '...'
    
    return text.capitalize()


def quote_yaml_value(value: str) -> str:
    """Quote YAML value if it contains special characters."""
    if ':' in value or '"' in value or "'" in value or '\n' in value:
        # Use double quotes and escape internal quotes
        return f'"{value.replace(chr(34), chr(92) + chr(34))}"'
    return value


def create_yaml_frontmatter(data: Dict) -> str:
    
    lines = ['---']
    
    # Required fields
    lines.append(f"id: {data['id']}")
    lines.append(f"header: {quote_yaml_value(data['header'])}")
    lines.append(f"text: {quote_yaml_value(data['text'])}")
    lines.append(f"level: '{data['level']}'")
    lines.append(f"normative: {str(data.get('normative', True)).lower()}")
    
    # References
    if 'references' in data and data['references']:
        lines.append('references:')
        for ref in data['references']:
            lines.append(f"- type: {ref['type']}")
            lines.append(f"  path: {quote_yaml_value(ref['path'])}")
            if 'id' in ref:
                lines.append(f"  id: {ref['id']}")
    
    # Reviewers
    if 'reviewers' in data:
        lines.append('reviewers:')
        for reviewer in data['reviewers']:
            lines.append(f"- name: {quote_yaml_value(reviewer['name'])}")
            lines.append(f"  email: {quote_yaml_value(reviewer['email'])}")
    
    # Evidence configuration (for assumptions)
    if 'evidence' in data:
        lines.append('evidence:')
        lines.append(f"  type: {data['evidence']['type']}")
        if 'configuration' in data['evidence']:
            lines.append('  configuration:')
            for key, value in data['evidence']['configuration'].items():
                if isinstance(value, list):
                    lines.append(f"    {key}:")
                    for item in value:
                        lines.append(f'      - "{item}"')
                else:
                    lines.append(f"    {key}: {quote_yaml_value(str(value))}")
    
    # Score (for evidence items)
    if 'score' in data:
        lines.append(f"score: {data['score']}")
    
    lines.append('---')
    return '\n'.join(lines)


def extract_hardware_components(requirement: str) -> List[str]:
    """Extract hardware component names from requirement text."""
    components = []
    hardware_terms = [
        'Raspberry Pi', 'Hailo', 'STM32', 'motor', 'servo', 'display', 
        'DSI', 'CAN', 'sensor', 'board', 'microcontroller'
    ]
    
    text_lower = requirement.lower()
    for term in hardware_terms:
        if term.lower() in text_lower:
            components.append(term)
    
    return components if components else ['Hardware components']


def extract_software_components(requirement: str) -> List[str]:
    """Extract software component names from requirement text."""
    components = []
    software_terms = [
        'Qt', 'QML', 'Python', 'C++', 'ThreadX', 'AGL', 'Linux',
        'compiler', 'toolchain', 'library', 'framework'
    ]
    
    text_lower = requirement.lower()
    for term in software_terms:
        if term.lower() in text_lower:
            components.append(term)
    
    return components if components else ['Software dependencies']


# ============================================================================
# Generator Functions
# ============================================================================

def generate_expectation(req_id: str, requirement: str, acceptance: str, category: str) -> str:
    """
    Generate EXPECT-L0-X.md content.
    
    Args:
        req_id: Requirement ID (e.g., 'L0-18')
        requirement: Requirement text
        acceptance: Acceptance criteria text
        category: Detected category
        
    Returns:
        Complete markdown file content
    """
    item_id = f"EXPECT-{req_id}"
    level_num = req_id.split('-')[1]
    level = f"1.{level_num}"
    
    # Create header
    header = sanitize_header(requirement)
    
    # Create expectation text (more specific than requirement)
    if category == 'hardware':
        text = f"The demonstrator car shall have {sanitize_header(requirement, 100).lower()} with all components properly integrated and functional."
    elif category == 'software':
        text = f"The system shall implement {sanitize_header(requirement, 100).lower()} according to specified requirements."
    elif category == 'communication':
        text = f"The system shall establish {sanitize_header(requirement, 100).lower()} with reliable message exchange."
    elif category == 'display':
        text = f"The user interface shall {sanitize_header(requirement, 100).lower()} with all visual elements properly rendered."
    elif category == 'realtime':
        text = f"The real-time system shall {sanitize_header(requirement, 100).lower()} with proper thread management and scheduling."
    else:
        text = f"The system shall {sanitize_header(requirement, 100).lower()} as specified in the requirement."
    
    # References
    references = [
        {'type': 'file', 'path': f'../assertions/ASSERT-{req_id}.md', 'id': f'ASSERT-{req_id}'}
    ]
    
    # Create front-matter
    frontmatter_data = {
        'id': item_id,
        'header': header,
        'text': text,
        'level': level,
        'normative': True,
        'references': references,
        'reviewers': [DEFAULT_REVIEWER]
    }
    
    frontmatter = create_yaml_frontmatter(frontmatter_data)
    
    # Body content
    body = f"\n\nThis expectation is derived from requirement **{req_id}** in tsf-requirements-table.md.\n"
    
    if acceptance:
        body += f"\n**Acceptance Criteria:**\n{acceptance}\n"
    
    return frontmatter + body


def generate_assertion(req_id: str, requirement: str, verification: str, category: str) -> str:
    """
    Generate ASSERT-L0-X.md content.
    
    Args:
        req_id: Requirement ID (e.g., 'L0-18')
        requirement: Requirement text
        verification: Verification method text
        category: Detected category
        
    Returns:
        Complete markdown file content
    """
    item_id = f"ASSERT-{req_id}"
    level_num = req_id.split('-')[1]
    level = f"1.{level_num}"
    
    # Create header
    header = sanitize_header(requirement) + " is verified"
    
    # Create assertion text (testable/verifiable statement)
    if category == 'hardware':
        text = "The hardware components are correctly assembled, powered, and operational as verified by physical inspection and functional tests."
    elif category == 'software':
        text = "The software implementation meets all specified requirements as verified by code review, unit tests, and integration tests."
    elif category == 'communication':
        text = "Communication channels are established and messages are exchanged successfully as verified by test logs and monitoring tools."
    elif category == 'display':
        text = "The user interface displays all required elements correctly as verified by visual inspection and screenshot comparison."
    elif category == 'realtime':
        text = "The real-time system executes threads with proper prioritization and timing as verified by execution logs and performance measurements."
    elif category == 'documentation':
        text = "Documentation exists in the repository and contains all required information as verified by document inspection."
    elif category == 'process':
        text = "Project processes are implemented and followed as verified by repository inspection and team reports."
    else:
        text = "The requirement is met and verified through appropriate verification methods as documented."
    
    # References
    references = [
        {'type': 'file', 'path': f'../expectations/EXPECT-{req_id}.md'},
        {'type': 'file', 'path': f'../evidences/EVID-{req_id}.md'}
    ]
    
    # Create front-matter
    frontmatter_data = {
        'id': item_id,
        'header': header,
        'text': text,
        'level': level,
        'normative': True,
        'references': references,
        'reviewers': [DEFAULT_REVIEWER]
    }
    
    frontmatter = create_yaml_frontmatter(frontmatter_data)
    
    # Body content
    body = f"\n\n**Verification Method:** {verification}\n"
    body += f"\nThis assertion validates that requirement **{req_id}** is met through documented evidence.\n"
    
    return frontmatter + body


def generate_evidence(req_id: str, requirement: str, evidence_refs: List[Dict]) -> str:
    """
    Generate EVID-L0-X.md content.
    
    Args:
        req_id: Requirement ID (e.g., 'L0-18')
        requirement: Requirement text
        evidence_refs: List of evidence references from table/sprints
        
    Returns:
        Complete markdown file content
    """
    item_id = f"EVID-{req_id}"
    level_num = req_id.split('-')[1]
    level = f"1.{level_num}"
    
    # Create header
    header = sanitize_header(requirement) + " - evidence"
    
    # Create evidence text
    if evidence_refs:
        text = f"Evidence for requirement {req_id} includes repository artifacts, documentation, demos, and test results."
    else:
        text = f"Evidence for requirement {req_id} is pending collection. This item will be updated as artifacts become available."
    
    # References - ALWAYS use type: url for GitHub links (UrlReference extension supports it)
    # For local files, convert to GitHub URLs for consistency
    references = []
    for ref in evidence_refs:
        ref_path = ref['path']
        ref_type = ref['type']
        
        if ref_type == 'url':
            # Already a URL (likely GitHub) - keep as is
            references.append({'type': 'url', 'path': ref_path})
        elif ref_path.startswith('docs/') or ref_path.startswith('src/'):
            # Local file path - convert to GitHub URL only if file exists
            if os.path.exists(PROJECT_ROOT / ref_path):
                github_url = f"https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/{ref_path}"
                references.append({'type': 'url', 'path': github_url})
        else:
            # Relative path - adjust for evidences/ directory only if file exists
            if os.path.exists(PROJECT_ROOT / ref_path):
                adjusted_path = '../../../../' + ref_path if not ref_path.startswith('..') else ref_path
                references.append({'type': 'file', 'path': adjusted_path})
    
    # Create front-matter
    frontmatter_data = {
        'id': item_id,
        'header': header,
        'text': text,
        'level': level,
        'normative': True,
        'reviewers': [DEFAULT_REVIEWER],
        'score': 1.0 if evidence_refs else 0.0
    }
    
    # Only add references if there are any
    if references:
        frontmatter_data['references'] = references
    
    frontmatter = create_yaml_frontmatter(frontmatter_data)
    
    # Body content
    body = "\n\nThis evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.\n"
    
    if evidence_refs:
        body += f"\n**Evidence Count:** {len(evidence_refs)} references\n"
        body += "\n**Evidence Types:**\n"
        
        # Group by type
        files = [r for r in evidence_refs if r['type'] == 'file']
        urls = [r for r in evidence_refs if r['type'] == 'url']
        
        if files:
            body += f"- Files/Documentation: {len(files)} items\n"
        if urls:
            body += f"- External URLs: {len(urls)} items\n"
    else:
        body += "\n*No evidence references available yet. Evidence will be added as implementation progresses.*\n"
    
    return frontmatter + body


def generate_assumption(req_id: str, requirement: str, category: str) -> str:
    """
    Generate ASSUMP-L0-X.md content.
    
    Args:
        req_id: Requirement ID (e.g., 'L0-18')
        requirement: Requirement text
        category: Detected category
        
    Returns:
        Complete markdown file content
    """
    item_id = f"ASSUMP-{req_id}"
    level_num = req_id.split('-')[1]
    level = f"1.{level_num}"
    
    # Create header based on category
    if category == 'hardware':
        header = "Assumption: Availability of hardware components"
        assumption_text = "The project team will provide the required hardware components and they will be operational in the integration environment during verification activities."
        validator_type = "validate_hardware_availability"
        components = extract_hardware_components(requirement)
    elif category == 'software':
        header = "Assumption: Software dependencies available"
        assumption_text = "All required software dependencies, libraries, and development tools are installed and accessible in the build and runtime environments."
        validator_type = "validate_software_dependencies"
        components = extract_software_components(requirement)
    elif category == 'realtime':
        header = "Assumption: RTOS environment configured"
        assumption_text = "The real-time operating system (ThreadX) is properly installed, configured, and operational on the target microcontroller."
        validator_type = "validate_software_dependencies"
        components = ["ThreadX", "STM32 toolchain", "RTOS configuration"]
    else:
        header = "Assumption: Development environment ready"
        assumption_text = "The development environment meets all prerequisites for implementing and verifying this requirement."
        validator_type = "validate_linux_environment"
        components = ["Linux environment", "Development tools", "Test infrastructure"]
    
    # Full text
    text = f"Assumption: {assumption_text}"
    
    # References
    references = [
        {'type': 'file', 'path': f'../expectations/EXPECT-{req_id}.md', 'id': f'EXPECT-{req_id}'}
    ]
    
    # Evidence configuration
    evidence_config = {
        'type': validator_type,
        'configuration': {
            'components': components
        }
    }
    
    # Create front-matter
    frontmatter_data = {
        'id': item_id,
        'header': header,
        'text': text,
        'level': level,
        'normative': True,
        'references': references,
        'reviewers': [DEFAULT_REVIEWER],
        'evidence': evidence_config
    }
    
    frontmatter = create_yaml_frontmatter(frontmatter_data)
    
    # Body content
    body = f"\n\n{assumption_text}\n"
    body += "\n**Acceptance criteria / notes:**\n"
    body += "- Required components are available and configured in the test environment.\n"
    body += "- If a component is unavailable, a documented mitigation plan will be provided.\n"
    body += f"- This assumption is validated by the `{validator_type}` validator.\n"
    body += f"\n**Rationale:** Verification activities for `EXPECT-{req_id}` depend on these prerequisites being met.\n"
    
    return frontmatter + body


# ============================================================================
# CRUD Operations
# ============================================================================

def add_new_requirement(req: Dict[str, str]) -> Dict[str, bool]:
    """
    Create 4 TSF items for a new requirement.
    
    Args:
        req: Requirement dict with keys: id, requirement, acceptance, verification, evidence
        
    Returns:
        Dict with success status for each item type:
        {'expectations': True, 'assertions': True, 'evidences': True, 'assumptions': True}
    """
    req_id = req['id']
    requirement = req['requirement']
    acceptance = req.get('acceptance', '')
    verification = req.get('verification', '')
    
    print(f"  ➕ Creating new requirement: {req_id}")
    
    # Detect category (including heartbeat)
    category = detect_category(requirement)
    print(f"     Category detected: {category}")
    
    # Get evidences from table
    evidences = extract_evidence_from_table(req_id)
    
    results = {}
    
    # Generate each item type
    try:
        # 1. Expectation
        expect_content = generate_expectation(req_id, requirement, acceptance, category)
        expect_file = TSF_ITEMS_BASE / 'expectations' / f'EXPECT-{req_id}.md'
        expect_file.parent.mkdir(parents=True, exist_ok=True)
        expect_file.write_text(expect_content)
        results['expectations'] = True
        print(f"     ✓ Created EXPECT-{req_id}.md")
        
        # 2. Assertion
        assert_content = generate_assertion(req_id, requirement, acceptance, category)
        assert_file = TSF_ITEMS_BASE / 'assertions' / f'ASSERT-{req_id}.md'
        assert_file.parent.mkdir(parents=True, exist_ok=True)
        assert_file.write_text(assert_content)
        results['assertions'] = True
        print(f"     ✓ Created ASSERT-{req_id}.md")
        
        # 3. Evidence (ALWAYS create, even without evidences)
        evid_content = generate_evidence(req_id, requirement, evidences)
        evid_file = TSF_ITEMS_BASE / 'evidences' / f'EVID-{req_id}.md'
        evid_file.parent.mkdir(parents=True, exist_ok=True)
        evid_file.write_text(evid_content)
        results['evidences'] = True
        if evidences:
            print(f"     ✓ Created EVID-{req_id}.md with {len(evidences)} evidence(s)")
        else:
            print(f"     ✓ Created EVID-{req_id}.md (no evidences yet)")
        
        # 4. Assumption
        assump_content = generate_assumption(req_id, requirement, category)
        assump_file = TSF_ITEMS_BASE / 'assumptions' / f'ASSUMP-{req_id}.md'
        assump_file.parent.mkdir(parents=True, exist_ok=True)
        assump_file.write_text(assump_content)
        results['assumptions'] = True
        print(f"     ✓ Created ASSUMP-{req_id}.md")
        
    except Exception as e:
        print(f"     ❌ Error creating items: {e}")
        results = {k: False for k in ITEM_TYPES.keys()}
    
    return results


def update_requirement(req_id: str, old_text: str, new_text: str) -> Dict[str, bool]:
    """
    Update requirement text in existing TSF items.
    
    Args:
        req_id: Requirement ID (e.g., 'L0-1')
        old_text: Previous requirement text
        new_text: New requirement text
        
    Returns:
        Dict with success status for each item type
    """
    print(f"  ✏️  Updating requirement: {req_id}")
    print(f"     Old: {old_text[:60]}...")
    print(f"     New: {new_text[:60]}...")
    
    results = {}
    
    for item_type, prefix in ITEM_TYPES.items():
        try:
            item_file = TSF_ITEMS_BASE / item_type / f'{prefix}-{req_id}.md'
            
            if not item_file.exists():
                print(f"     ⚠️  {prefix}-{req_id}.md not found, skipping")
                results[item_type] = False
                continue
            
            content = item_file.read_text()
            
            # Update requirement text in various locations
            # 1. In YAML text field
            content = re.sub(
                r'(text:\s+)' + re.escape(old_text),
                r'\1' + new_text,
                content
            )
            
            # 2. In markdown headers
            old_header = sanitize_header(old_text)
            new_header = sanitize_header(new_text)
            content = re.sub(
                r'(##\s+)' + re.escape(old_header),
                r'\1' + new_header,
                content
            )
            
            # 3. In body text (if referenced)
            content = content.replace(old_text, new_text)
            
            # Write updated content
            item_file.write_text(content)
            results[item_type] = True
            print(f"     ✓ Updated {prefix}-{req_id}.md")
            
        except Exception as e:
            print(f"     ❌ Error updating {prefix}-{req_id}.md: {e}")
            results[item_type] = False
    
    return results


def delete_requirement(req_id: str) -> Dict[str, bool]:
    """
    Delete all 4 TSF items for a removed requirement.
    
    Args:
        req_id: Requirement ID to delete (e.g., 'L0-18')
        
    Returns:
        Dict with success status for each item type
    """
    print(f"  ➖ Deleting requirement: {req_id}")
    
    results = {}
    
    for item_type, prefix in ITEM_TYPES.items():
        try:
            item_file = TSF_ITEMS_BASE / item_type / f'{prefix}-{req_id}.md'
            
            if item_file.exists():
                item_file.unlink()
                results[item_type] = True
                print(f"     ✓ Deleted {prefix}-{req_id}.md")
            else:
                print(f"     ⚠️  {prefix}-{req_id}.md not found")
                results[item_type] = True  # Consider success if already deleted
                
        except Exception as e:
            print(f"     ❌ Error deleting {prefix}-{req_id}.md: {e}")
            results[item_type] = False
    
    return results


def sync_evidences(req_id: str, new_evidence: List[Dict], removed_evidence: List[Dict]) -> bool:
    """
    Synchronize evidence references in EVID-L0-X.md file.
    
    Args:
        req_id: Requirement ID
        new_evidence: List of evidence dicts to add
        removed_evidence: List of evidence dicts to remove
        
    Returns:
        True if successful, False otherwise
    """
    evid_file = TSF_ITEMS_BASE / 'evidences' / f'EVID-{req_id}.md'
    
    if not evid_file.exists():
        print(f"     ⚠️  EVID-{req_id}.md not found")
        return False
    
    try:
        content = evid_file.read_text()
        
        # Parse YAML frontmatter
        yaml_match = re.search(r'---\n(.*?)\n---', content, re.DOTALL)
        if not yaml_match:
            print(f"     ❌ No YAML frontmatter found in EVID-{req_id}.md")
            return False
        
        yaml_content = yaml_match.group(1)
        rest_content = content[yaml_match.end():]
        
        # Extract current references - find ALL references sections and extract from the first one only
        current_refs = []
        all_ref_matches = list(re.finditer(r'^references:', yaml_content, re.MULTILINE))
        if all_ref_matches:
            # Use the first references section found
            first_ref_match = all_ref_matches[0]
            start_pos = first_ref_match.start()
            
            # Find the end of this references section (next references, reviewers, or ---)
            next_section = re.search(r'^(references:|reviewers:|---)', yaml_content[start_pos+1:], re.MULTILINE)
            if next_section:
                end_pos = start_pos + next_section.start()
            else:
                end_pos = len(yaml_content)
            
            ref_block = yaml_content[start_pos:end_pos].strip()
            # Remove the 'references:' header to get just the content
            ref_content = re.sub(r'^references:\n', '', ref_block, flags=re.MULTILINE)
            
            lines = ref_content.split('\n')
            i = 0
            while i < len(lines):
                line = lines[i].strip()
                if line.startswith('- '):
                    if line.startswith('- type:'):
                        # New format: dict with type and path
                        ref_type = line[8:].strip()
                        i += 1
                        if i < len(lines) and lines[i].strip().startswith('path:'):
                            ref_path = lines[i].strip()[6:].strip()
                            current_refs.append({'type': ref_type, 'path': ref_path})
                    else:
                        # Old format: raw string
                        path = line[2:].strip()
                        # Try to determine type
                        if path.startswith('http'):
                            ref_type = 'url'
                        else:
                            ref_type = 'file'
                        current_refs.append({'type': ref_type, 'path': path})
                i += 1
        else:
            ref_section = None
        
        # Remove evidences
        for ev in removed_evidence:
            ev_path = ev['path']
            current_refs = [ref for ref in current_refs if ref['path'] != ev_path]
            if any(ref['path'] == ev_path for ref in current_refs):
                print(f"     ➖ Removed evidence: {ev_path}")
        
        # Add new evidences
        for ev in new_evidence:
            ev_path = ev['path']
            if not any(ref['path'] == ev_path for ref in current_refs):
                current_refs.append(ev)
                print(f"     ➕ Added evidence: {ev_path}")
        
        # Rebuild references section
        if current_refs:
            new_refs = 'references:\n'
            for ref in current_refs:
                new_refs += f"- type: {ref['type']}\n"
                new_refs += f"  path: {quote_yaml_value(ref['path'])}\n"
        else:
            new_refs = 'references: []\n'
        
        # Remove ALL existing references sections
        all_ref_matches = list(re.finditer(r'^references:', yaml_content, re.MULTILINE))
        if all_ref_matches:
            # Remove all references sections (in reverse order to maintain indices)
            for ref_match in reversed(all_ref_matches):
                start_pos = ref_match.start()
                # Find the end of this references section
                next_section = re.search(r'^(references:|reviewers:|---)', yaml_content[start_pos+1:], re.MULTILINE)
                if next_section:
                    end_pos = start_pos + next_section.start()
                else:
                    end_pos = len(yaml_content)
                yaml_content = yaml_content[:start_pos] + yaml_content[end_pos:]
        
        # Add the new references section before reviewers (or at the end if no reviewers)
        reviewer_match = re.search(r'^reviewers:', yaml_content, re.MULTILINE)
        if reviewer_match:
            yaml_content = yaml_content[:reviewer_match.start()] + new_refs + '\n' + yaml_content[reviewer_match.start():]
        else:
            yaml_content += '\n' + new_refs
        
        # Reconstruct file
        new_content = '---\n' + yaml_content + '---' + rest_content
        evid_file.write_text(new_content)
        
        print(f"     ✓ Synchronized {len(new_evidence)} additions, {len(removed_evidence)} removals")
        return True
        
    except Exception as e:
        print(f"     ❌ Error syncing evidences: {e}")
        return False


def update_lo_requirements_table(req_id: str, all_evidences: List[Dict]) -> bool:
    """
    Update Evidence column in tsf-requirements-table.md table (AUTOMATIC SYNC).
    
    This function closes the bidirectional sync loop:
    items/evidences/EVID-L0-X.md → tsf-requirements-table.md (Evidence column)
    
    Args:
        req_id: Requirement ID (e.g., 'L0-1')
        all_evidences: Complete list of evidence dicts (type, path) to write to table
        
    Returns:
        True if successful, False otherwise
    """
    from detectors import LO_REQUIREMENTS_PATH
    
    if not LO_REQUIREMENTS_PATH.exists():
        print(f"     ❌ tsf-requirements-table.md not found")
        return False
    
    try:
        content = LO_REQUIREMENTS_PATH.read_text()
        
        # Find the row for this requirement ID
        # Pattern: | **L0-1** | ... | ... | ... | EVIDENCE_COLUMN |
        
        # Split table by lines
        lines = content.split('\n')
        updated = False
        
        for i, line in enumerate(lines):
            # Check if this line contains our requirement ID
            if f'| **{req_id}**' in line:
                # This is our row! Now update the Evidence column (last column)
                parts = line.split('|')
                
                if len(parts) < 6:  # Must have: '', ID, Requirement, Acceptance, Verification, Evidence, ''
                    print(f"     ⚠️  Row for {req_id} has unexpected format")
                    continue
                
                # Build new evidence column content
                if all_evidences:
                    evidence_text = f"EXPECT-{req_id} — Evidence updated automatically<br>"
                    for ev in all_evidences:
                        if ev['type'] == 'url':
                            evidence_text += f"{ev['path']}<br>"
                        else:
                            evidence_text += f"`{ev['path']}`<br>"
                    # Remove last <br>
                    evidence_text = evidence_text.rstrip('<br>')
                else:
                    evidence_text = f"EXPECT-{req_id} — No evidence yet"
                
                # Replace Evidence column (5th position, index 5)
                parts[5] = f" {evidence_text} "
                
                # Reconstruct line
                lines[i] = '|'.join(parts)
                updated = True
                print(f"     ✓ Updated Evidence column for {req_id} in tsf-requirements-table.md")
                break
        
        if not updated:
            print(f"     ⚠️  Could not find row for {req_id} in tsf-requirements-table.md")
            return False
        
        # Write back to file
        LO_REQUIREMENTS_PATH.write_text('\n'.join(lines))
        print(f"     ✅ tsf-requirements-table.md updated successfully")
        return True
        
    except Exception as e:
        print(f"     ❌ Error updating tsf-requirements-table.md: {e}")
        return False





# ============================================================================
# Main Changeset Application
# ============================================================================

def apply_changeset(changeset: Optional[Dict] = None, dry_run: bool = False) -> Dict[str, any]:
    """
    Apply complete changeset to items/ directory.
    
    Args:
        changeset: Optional changeset dict (if None, will call get_complete_changeset())
        dry_run: If True, only print what would be done without making changes
        
    Returns:
        Dict with operation results:
        {
            'added': int,
            'updated': int,
            'deleted': int,
            'evidence_synced': int,
            'errors': List[str],
            'success': bool
        }
    """
    if changeset is None:
        print("🔍 Detecting changes...")
        changeset = get_complete_changeset()
    
    if not changeset['has_changes']:
        print("✅ No changes detected - items/ is up to date")
        return {
            'added': 0,
            'updated': 0,
            'deleted': 0,
            'evidence_synced': 0,
            'errors': [],
            'success': True
        }
    
    print("\n" + "="*70)
    print("APPLYING CHANGESET TO items/ (SOURCE OF TRUTH)")
    print("="*70)
    
    results = {
        'added': 0,
        'updated': 0,
        'deleted': 0,
        'evidence_synced': 0,
        'errors': []
    }
    
    if dry_run:
        print("\n⚠️  DRY RUN MODE - No actual changes will be made\n")
    
    # 1. Add new requirements
    if changeset['new_requirements']:
        print(f"\n➕ ADDING {len(changeset['new_requirements'])} NEW REQUIREMENT(S):")
        print("-" * 70)
        for req in changeset['new_requirements']:
            if dry_run:
                print(f"  [DRY RUN] Would create {req['id']}")
            else:
                status = add_new_requirement(req)
                if all(status.values()):
                    results['added'] += 1
                else:
                    results['errors'].append(f"Failed to add {req['id']}")
    
    # 2. Update modified requirements
    if changeset['modified_requirements']:
        print(f"\n✏️  UPDATING {len(changeset['modified_requirements'])} MODIFIED REQUIREMENT(S):")
        print("-" * 70)
        for req_id, changes in changeset['modified_requirements'].items():
            if dry_run:
                print(f"  [DRY RUN] Would update {req_id}")
            else:
                status = update_requirement(req_id, changes['old_text'], changes['new_text'])
                if all(status.values()):
                    results['updated'] += 1
                else:
                    results['errors'].append(f"Failed to update {req_id}")
    
    # 3. Sync evidence changes (items/ EVID files)
    if changeset['evidence_changes']:
        print(f"\n📝 SYNCHRONIZING EVIDENCE FOR {len(changeset['evidence_changes'])} REQUIREMENT(S):")
        print("-" * 70)
        for req_id, evidence_info in changeset['evidence_changes'].items():
            print(f"  📝 Syncing {req_id}...")
            if dry_run:
                print(f"     [DRY RUN] Would sync +{len(evidence_info['new_evidence'])} / -{len(evidence_info['removed_evidence'])}")
                print(f"     [DRY RUN] Would update tsf-requirements-table.md Evidence column")
            else:
                # Step 3a: Update EVID-L0-X.md in items/
                success = sync_evidences(req_id, evidence_info['new_evidence'], evidence_info['removed_evidence'])
                if success:
                    # Step 3b: Update tsf-requirements-table.md table (AUTOMATIC BIDIRECTIONAL SYNC)
                    all_evidences = evidence_info['table_evidence']  # Current evidences from table
                    table_updated = update_lo_requirements_table(req_id, all_evidences)
                    
                    if table_updated:
                        results['evidence_synced'] += 1
                    else:
                        results['errors'].append(f"Failed to update tsf-requirements-table.md for {req_id}")
                else:
                    results['errors'].append(f"Failed to sync evidence for {req_id}")
    
    # 4. Delete removed requirements
    if changeset['deleted_requirements']:
        print(f"\n➖ DELETING {len(changeset['deleted_requirements'])} REMOVED REQUIREMENT(S):")
        print("-" * 70)
        for req_id in changeset['deleted_requirements']:
            if dry_run:
                print(f"  [DRY RUN] Would delete {req_id}")
            else:
                status = delete_requirement(req_id)
                if all(status.values()):
                    results['deleted'] += 1
                else:
                    results['errors'].append(f"Failed to delete {req_id}")
    
    # Summary
    results['success'] = len(results['errors']) == 0
    
    print("\n" + "="*70)
    print("CHANGESET APPLICATION SUMMARY")
    print("="*70)
    print(f"  ➕ Added: {results['added']}")
    print(f"  ✏️  Updated: {results['updated']}")
    print(f"  📝 Evidence synced: {results['evidence_synced']}")
    print(f"  ➖ Deleted: {results['deleted']}")
    print(f"  ❌ Errors: {len(results['errors'])}")
    
    if results['errors']:
        print("\n⚠️  ERRORS ENCOUNTERED:")
        for error in results['errors']:
            print(f"    - {error}")
        results['success'] = False
    else:
        print(f"\n✅ SUCCESS - items/ updated successfully")
    
    return results


# ============================================================================
# Main Entry Point
# ============================================================================

def main():
    """
    Test the source of truth generator.
    """
    print("="*70)
    print("TSF SOURCE OF TRUTH GENERATOR - TEST MODE")
    print("="*70)
    
    # Get changeset
    changeset = get_complete_changeset()
    
    # Apply in dry-run mode for testing
    print("\n🧪 Running in DRY RUN mode (no actual changes)...\n")
    results = apply_changeset(changeset, dry_run=True)
    
    print("\n" + "="*70)
    print("TEST COMPLETE")
    print("="*70)
    print(f"Would have processed {results['added'] + results['updated'] + results['deleted']} operations")
    print("\nTo apply changes for real, call: apply_changeset(dry_run=False)")


if __name__ == '__main__':
    main()
