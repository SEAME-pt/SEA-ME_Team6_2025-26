#!/usr/bin/env python3
"""
TSF Generators Module
=====================

Template-based generation of TSF items (EXPECT/ASSERT/EVID/ASSUMP).

Functions:
    - generate_expectation() - Create EXPECT-L0-X.md files
    - generate_assertion() - Create ASSERT-L0-X.md files
    - generate_evidence() - Create EVID-L0-X.md files
    - generate_assumption() - Create ASSUMP-L0-X.md files
    - generate_all_items() - Generate complete set for one requirement
    
Author: SEA-ME Team 6
Date: December 15, 2025
"""

import re
import os
from pathlib import Path
from typing import Dict, List, Optional
from datetime import datetime

# Import detector functions
try:
    from detectors import extract_evidence_from_table, get_complete_evidence
except ImportError:
    import sys
    sys.path.insert(0, str(Path(__file__).parent))
    from detectors import extract_evidence_from_table, get_complete_evidence


# ============================================================================
# Configuration
# ============================================================================

PROJECT_ROOT = Path(__file__).resolve().parents[5]
TSF_ITEMS_BASE = PROJECT_ROOT / "docs/TSF/tsf_implementation/items"

DEFAULT_REVIEWER = {
    'name': 'Joao Jesus Silva',
    'email': 'joao.silva@seame.pt'
}

# Keyword-based rules for intelligent generation
KEYWORDS = {
    'hardware': ['hardware', 'assembly', 'component', 'motor', 'servo', 'display', 'raspberry', 'hailo', 'stm32'],
    'software': ['software', 'qt', 'application', 'gui', 'program', 'code', 'crosscompile'],
    'communication': ['can', 'communication', 'message', 'protocol', 'signal', 'bidirectional'],
    'system': ['architecture', 'system', 'design', 'integration', 'interface'],
    'testing': ['test', 'verification', 'validation', 'demonstration', 'inspection'],
    'documentation': ['document', 'documentation', 'guide', 'readme'],
    'process': ['agile', 'scrum', 'github', 'ci/cd', 'workflow', 'tsf'],
    'display': ['display', 'ui', 'interface', 'screen', 'visual', 'mockup'],
    'realtime': ['threadx', 'rtos', 'thread', 'real-time', 'scheduler']
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
    for category, keywords in KEYWORDS.items():
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


def create_yaml_frontmatter(data: Dict) -> str:
    """
    Create YAML front-matter block.
    
    Args:
        data: Dictionary with front-matter fields
        
    Returns:
        YAML front-matter string
    """
    lines = ['---']
    
    # Required fields
    lines.append(f"id: {data['id']}")
    lines.append(f"header: {data['header']}")
    lines.append(f"text: {data['text']}")
    lines.append(f"level: '{data['level']}'")
    lines.append(f"normative: {str(data.get('normative', True)).lower()}")
    
    # References
    if 'references' in data and data['references']:
        lines.append('references:')
        for ref in data['references']:
            lines.append(f"- type: {ref['type']}")
            lines.append(f"  path: {ref['path']}")
            if 'id' in ref:
                lines.append(f"  id: {ref['id']}")
    
    # Reviewers
    if 'reviewers' in data:
        lines.append('reviewers:')
        for reviewer in data['reviewers']:
            lines.append(f"- name: {reviewer['name']}")
            lines.append(f"  email: {reviewer['email']}")
    
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
                    lines.append(f"    {key}: {value}")
    
    # Score (for evidence items)
    if 'score' in data:
        lines.append(f"score: {data['score']}")
    
    lines.append('---')
    return '\n'.join(lines)


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
    body = f"\n\nThis expectation is derived from requirement **{req_id}** in LO_requirements.md.\n"
    
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
    
    # References - convert relative paths for evidence refs
    references = []
    for ref in evidence_refs:
        ref_path = ref['path']
        # Don't modify URLs
        if ref['type'] == 'url':
            references.append({'type': 'url', 'path': ref_path})
        else:
            # File paths need to be relative to evidences/ directory
            # Adjust path: docs/X → ../../../../docs/X
            if ref_path.startswith('docs/'):
                adjusted_path = '../../../../' + ref_path
            else:
                adjusted_path = ref_path
            references.append({'type': 'file', 'path': adjusted_path})
    
    # Create front-matter
    frontmatter_data = {
        'id': item_id,
        'header': header,
        'text': text,
        'level': level,
        'normative': True,
        'references': references,
        'reviewers': [DEFAULT_REVIEWER],
        'score': 1.0 if evidence_refs else 0.0
    }
    
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
# Main Generation Function
# ============================================================================

def generate_all_items(req_data: Dict, output_dir: Optional[Path] = None) -> Dict[str, str]:
    """
    Generate all 4 TSF items for one requirement.
    
    Args:
        req_data: Dictionary with keys: id, requirement, acceptance, verification, evidence
        output_dir: Optional output directory (defaults to TSF_ITEMS_BASE)
        
    Returns:
        Dictionary mapping item type to file path
    """
    req_id = req_data['id']
    requirement = req_data['requirement']
    acceptance = req_data['acceptance']
    verification = req_data['verification']
    
    # Detect category
    category = detect_category(requirement)
    
    # Get evidence references
    table_evidence, sprint_evidence = get_complete_evidence(req_id)
    all_evidence = table_evidence + sprint_evidence
    
    # Remove duplicates
    seen = set()
    unique_evidence = []
    for ev in all_evidence:
        key = (ev['type'], ev['path'])
        if key not in seen:
            seen.add(key)
            unique_evidence.append(ev)
    
    # Generate content
    expect_content = generate_expectation(req_id, requirement, acceptance, category)
    assert_content = generate_assertion(req_id, requirement, verification, category)
    evid_content = generate_evidence(req_id, requirement, unique_evidence)
    assump_content = generate_assumption(req_id, requirement, category)
    
    # Determine output directory
    if output_dir is None:
        output_dir = TSF_ITEMS_BASE
    
    # Write files
    files_created = {}
    
    items = {
        'expectations': ('EXPECT', expect_content),
        'assertions': ('ASSERT', assert_content),
        'evidences': ('EVID', evid_content),
        'assumptions': ('ASSUMP', assump_content)
    }
    
    for item_type, (prefix, content) in items.items():
        item_dir = output_dir / item_type
        item_dir.mkdir(parents=True, exist_ok=True)
        
        filename = f"{prefix}-{req_id}.md"
        filepath = item_dir / filename
        
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)
        
        files_created[item_type] = str(filepath)
        print(f"✅ Created: {filepath}")
    
    return files_created


# ============================================================================
# Test Function
# ============================================================================

def main():
    """Test generators with L0-18 requirement."""
    print("=" * 70)
    print("TSF GENERATORS TEST - L0-18 (CAN + ThreadX)")
    print("=" * 70)
    
    # Test data for L0-18
    test_req = {
        'id': 'L0-18',
        'requirement': 'The vehicle shall communicate via CAN bus also using ThreadX',
        'acceptance': 'Threads should create and send messages using CAN Bus to sent/receive messages successfully',
        'verification': 'Test logs',
        'evidence': ''
    }
    
    print(f"\n📋 Generating TSF items for: {test_req['id']}")
    print(f"   Requirement: {test_req['requirement']}")
    
    # Detect category
    category = detect_category(test_req['requirement'])
    print(f"   Detected category: {category}")
    
    # Generate items
    print("\n🔧 Generating items...")
    files = generate_all_items(test_req)
    
    print(f"\n✅ Generation complete! Created {len(files)} files:")
    for item_type, filepath in files.items():
        print(f"   - {item_type}: {Path(filepath).name}")
    
    print("\n" + "=" * 70)
    print("GENERATORS TEST COMPLETE")
    print("=" * 70)


if __name__ == '__main__':
    main()
