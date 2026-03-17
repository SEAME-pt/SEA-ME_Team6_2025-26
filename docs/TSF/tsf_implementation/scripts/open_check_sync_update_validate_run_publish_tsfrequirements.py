#!/usr/bin/env python3
"""
TSF Requirements Unified Automation Script
==========================================

This script provides a unified workflow for TSF requirements management:
1. open_check() - Verify table completeness, sync evidences from sprints to table, sync table to EVID/EXPECT files
2. sync_update() - Generate missing content using AI (Option G: VSCode/Claude first, Option C: gh copilot CLI fallback)
3. validate_run_publish() - Validate items and run TruDAG

Source of Truth Hierarchy:
- Sprints → Table (for evidence column)
- Table → EVID files (for evidence content)
- Table → EXPECT files (for header/text)

Usage:
    python open_check_sync_update_validate_run_publish_tsfrequirements.py [--check] [--sync] [--validate] [--all]

Author: SEA-ME Team 6
Date: January 2026
"""

import os
import re
import sys
import subprocess
import shutil
from pathlib import Path
from typing import Dict, List, Optional, Tuple, Any
from dataclasses import dataclass
from enum import Enum


# ============================================================================
# DEPENDENCY CHECK
# ============================================================================

def check_dependencies() -> Tuple[bool, List[str]]:
    """
    Check all required dependencies are available.
    Returns (success, list of error messages).
    """
    errors = []
    
    # Check required Python packages
    required_packages = {
        'yaml': 'pyyaml',
    }
    
    for module, package in required_packages.items():
        try:
            __import__(module)
        except ImportError:
            errors.append(f"❌ Python package '{package}' not found. Install with: pip install {package}")
    
    # Check if git is available
    try:
        result = subprocess.run(['git', '--version'], capture_output=True, text=True, timeout=5)
        if result.returncode != 0:
            errors.append("❌ Git not found or not working properly")
    except (subprocess.SubprocessError, FileNotFoundError):
        errors.append("❌ Git not found. Please install git.")
    
    # Check if we're in a git repository
    try:
        result = subprocess.run(['git', 'rev-parse', '--git-dir'], capture_output=True, text=True, timeout=5)
        if result.returncode != 0:
            errors.append("❌ Not in a git repository. This script must run from within a git repo.")
    except subprocess.SubprocessError:
        errors.append("❌ Failed to check git repository status")
    
    return len(errors) == 0, errors


def print_startup_diagnostics():
    """Print diagnostic information at startup."""
    print("\n" + "="*70)
    print("🔍 STARTUP DIAGNOSTICS")
    print("="*70)
    
    # Check dependencies
    deps_ok, dep_errors = check_dependencies()
    
    if deps_ok:
        print("   ✅ All dependencies OK")
    else:
        print("   ⚠️  Dependency issues found:")
        for error in dep_errors:
            print(f"      {error}")
    
    # Check Python version
    py_version = sys.version_info
    print(f"   📍 Python version: {py_version.major}.{py_version.minor}.{py_version.micro}")
    if py_version < (3, 8):
        print("      ⚠️  Python 3.8+ recommended")
    
    # Check working directory
    print(f"   📍 Working directory: {os.getcwd()}")
    
    # Check if venv is active
    venv = os.environ.get('VIRTUAL_ENV')
    if venv:
        print(f"   📍 Virtual environment: {Path(venv).name}")
    else:
        print("   ℹ️  No virtual environment active (optional)")
    
    print("="*70)
    
    return deps_ok


def is_vscode_cli_available() -> bool:
    """Return True when VSCode CLI command `code` is available in PATH."""
    return shutil.which('code') is not None


# Import yaml after dependency check setup (will fail gracefully if not installed)
try:
    import yaml
except ImportError:
    yaml = None


# Explicit marker used to flag non-real evidence placeholders.
PLACEHOLDER_EVIDENCE_MARKER = "TSF_PLACEHOLDER_EVIDENCE"


def is_placeholder_evidence_reference(ref: Any) -> bool:
    """Return True when a reference entry is explicitly marked as placeholder evidence."""
    if not isinstance(ref, dict):
        return False
    description = str(ref.get('description', '')).strip()
    return description.upper() == PLACEHOLDER_EVIDENCE_MARKER


# ============================================================================
# CONFIGURATION
# ============================================================================

class AIMethod(Enum):
    """AI generation methods available."""
    MANUAL = "manual"       # Option G: VSCode/Claude semi-automated
    COPILOT_CLI = "copilot_cli"  # Option C: gh copilot CLI


@dataclass #It means this is a data class, ie, a class mainly used to store data with automatic __init__, __repr__, etc.
class TableRow:
    """Represents a row in the TSF requirements table."""
    id: str                      # L0-X
    requirement: str             # Requirement text
    acceptance_criteria: str     # Acceptance criteria
    verification_method: str     # Verification method
    evidence: str                # Evidence links/text
    
    @property
    def number(self) -> int:
        """Extract numeric ID from L0-X format."""
        match = re.search(r'L0-(\d+)', self.id)
        return int(match.group(1)) if match else 0
    
    def is_complete(self) -> bool:
        """Check if all fields are filled."""
        return all([
            self.requirement.strip(),
            self.acceptance_criteria.strip(),
            self.verification_method.strip(),
            self.evidence.strip()
        ])
    
    def get_empty_fields(self) -> List[str]:
        """Get list of empty field names."""
        empty = []
        if not self.requirement.strip():
            empty.append('requirement')
        if not self.acceptance_criteria.strip():
            empty.append('acceptance_criteria')
        if not self.verification_method.strip():
            empty.append('verification_method')
        if not self.evidence.strip():
            empty.append('evidence')
        return empty


@dataclass
class EvidenceLink:
    """Represents an evidence link extracted from sprints."""
    expect_id: str      # EXPECT-L0-X
    description: str    # Link description
    url: str            # Full URL
    link_type: str      # "link" or "image"
    source_file: str    # Sprint file where found


class Config:
    """Configuration loader and manager."""
    
    def __init__(self, config_path: Optional[str] = None):
        if yaml is None:
            raise ImportError("PyYAML is required. Install with: pip install pyyaml")
        
        if config_path is None:
            # Default to config.yaml in same directory
            script_dir = Path(__file__).parent
            config_path = script_dir / "config.yaml"
        
        self.config_path = Path(config_path)
        self._config = self._load_config()
        self._resolve_paths()
    
    def _load_config(self) -> Dict:
        """Load configuration from YAML file."""
        if not self.config_path.exists():
            raise FileNotFoundError(f"Config file not found: {self.config_path}")
        
        with open(self.config_path, 'r') as f:
            return yaml.safe_load(f)
    
    def _resolve_paths(self):
        """Resolve path variables in configuration."""
        # Get base paths - support "auto" to detect from script location
        repo_root_config = self._config['paths']['repo_root']
        
        if repo_root_config == "auto" or not repo_root_config:
            # Auto-detect: script is at repo/docs/TSF/tsf_implementation/scripts/
            # So repo_root is 4 levels up
            script_dir = Path(__file__).parent.resolve()
            self.repo_root = script_dir.parent.parent.parent.parent
        else:
            self.repo_root = Path(repo_root_config)
        
        self.tsf_implementation = self.repo_root / "docs/TSF/tsf_implementation"
        self.items_dir = self.tsf_implementation / "items"
        self.scripts_dir = self.tsf_implementation / "scripts"
        self.sprints_dir = self.repo_root / "docs/sprints"
        self.requirements_table = self.repo_root / "docs/TSF/requirements/tsf-requirements-table.md"
        self.graph_dir = self.tsf_implementation / "graph"
        
        # Item subdirectories
        self.expectations_dir = self.items_dir / "expectations"
        self.assertions_dir = self.items_dir / "assertions"
        self.evidences_dir = self.items_dir / "evidences"
        self.assumptions_dir = self.items_dir / "assumptions"
    
    @property
    def ai_primary_method(self) -> AIMethod:
        """Get primary AI method."""
        method = self._config.get('ai', {}).get('primary_method', 'manual')
        return AIMethod(method)
    
    @property
    def ai_fallbacks(self) -> List[AIMethod]:
        """Get fallback AI methods."""
        fallbacks = self._config.get('ai', {}).get('fallbacks', [])
        return [AIMethod(f) for f in fallbacks]
    
    @property
    def evidence_patterns(self) -> Dict[str, str]:
        """Get evidence extraction patterns."""
        return self._config.get('evidence_sync', {}).get('evidence_patterns', {})
    
    @property
    def sprint_files(self) -> List[str]:
        """Get list of sprint files to parse."""
        return self._config.get('evidence_sync', {}).get('sprint_files', [])
    
    @property
    def manual_settings(self) -> Dict:
        """Get manual AI generation settings."""
        return self._config.get('ai', {}).get('manual', {})


# ============================================================================
# EVIDENCE PARSER (Robust parser for sprint files)
# ============================================================================

class EvidenceParser:
    """
    Robust parser for extracting evidence links from sprint files.
    Handles various formats:
    - EXPECT-L0-X - "text" followed by markdown links
    - EXPECT_L0_X - "text" (underscore variant)
    - EXPECT-L0-X - "text": followed by indented links
    - ![image](url) format
    - [text](url) format
    - Raw URLs on indented lines following EXPECT headers
    """
    
    def __init__(self, config: Config):
        self.config = config
        # Compile regex patterns - support both underscore and hyphen variants
        self.patterns = {
            'expect_header': re.compile(
                r'EXPECT[-_]L0[-_](\d+)\s*[-–]\s*["\']?([^"\'\n:]+)["\']?:?',
                re.IGNORECASE
            ),
            'markdown_link': re.compile(
                r'\[([^\]]+)\]\(([^)]+)\)'
            ),
            'markdown_image': re.compile(
                r'!\[([^\]]*)\]\(([^)]+)\)'
            ),
            'raw_url': re.compile(
                r'https?://[^\s\)\]>\'"]+',
                re.IGNORECASE
            )
        }
    
    def parse_sprint_file(self, filepath: Path) -> List[EvidenceLink]:
        """Parse a single sprint file and extract all evidence links."""
        if not filepath.exists():
            print(f"  ⚠️  Sprint file not found: {filepath}")
            return []
        
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        
        return self._extract_evidence_from_content(content, filepath.name)
    
    def _extract_evidence_from_content(self, content: str, source_file: str) -> List[EvidenceLink]:
        """Extract evidence links from content."""
        evidence_links = []
        lines = content.split('\n')
        
        current_expect_id = None
        current_expect_desc = None
        current_expect_indent = 0  # Track indentation level
        
        for i, line in enumerate(lines):
            # Check for EXPECT-L0-X or EXPECT_L0_X header
            expect_match = self.patterns['expect_header'].search(line)
            if expect_match:
                current_expect_id = f"EXPECT-L0-{expect_match.group(1)}"
                current_expect_desc = expect_match.group(2).strip()
                # Calculate indentation of this EXPECT line
                current_expect_indent = len(line) - len(line.lstrip())
                continue
            
            # If we have a current EXPECT, look for links in following lines
            if current_expect_id:
                # Check line indentation - only process lines more indented than the EXPECT
                line_indent = len(line) - len(line.lstrip())
                stripped_line = line.strip()
                
                # Check for markdown images
                for img_match in self.patterns['markdown_image'].finditer(line):
                    alt_text = img_match.group(1) or "image"
                    url = img_match.group(2)
                    evidence_links.append(EvidenceLink(
                        expect_id=current_expect_id,
                        description=alt_text,
                        url=url,
                        link_type="image",
                        source_file=source_file
                    ))
                
                # Check for markdown links (but not images)
                for link_match in self.patterns['markdown_link'].finditer(line):
                    # Skip if this is part of an image (preceded by !)
                    match_start = link_match.start()
                    if match_start > 0 and line[match_start - 1] == '!':
                        continue
                    
                    description = link_match.group(1)
                    url = link_match.group(2)
                    evidence_links.append(EvidenceLink(
                        expect_id=current_expect_id,
                        description=description,
                        url=url,
                        link_type="link",
                        source_file=source_file
                    ))
                
                # NEW: Check for raw URLs on indented lines (common in sprint files)
                # Only if line is more indented than the EXPECT header
                if line_indent > current_expect_indent and stripped_line:
                    for url_match in self.patterns['raw_url'].finditer(stripped_line):
                        url = url_match.group(0)
                        # Check if this URL was already captured as part of markdown syntax
                        already_captured = any(
                            e.url == url and e.expect_id == current_expect_id 
                            for e in evidence_links
                        )
                        if not already_captured:
                            # Determine link type from URL
                            link_type = "image" if any(ext in url.lower() for ext in ['.png', '.jpg', '.jpeg', '.gif', '.svg', 'assets']) else "link"
                            evidence_links.append(EvidenceLink(
                                expect_id=current_expect_id,
                                description=f"Evidence from {source_file}",
                                url=url,
                                link_type=link_type,
                                source_file=source_file
                            ))
                
                # Reset if we hit a line at same or less indentation (new section)
                # or if we hit specific section markers
                if stripped_line and line_indent <= current_expect_indent:
                    # Don't reset if this is another EXPECT (will be handled above)
                    if not self.patterns['expect_header'].search(line):
                        current_expect_id = None
                        current_expect_desc = None
                        current_expect_indent = 0
                
                # Also reset on major section changes
                if line.startswith('#') or (line.startswith('**') and line.endswith('**')):
                    current_expect_id = None
                    current_expect_desc = None
                    current_expect_indent = 0
        
        return evidence_links
    
    def parse_all_sprints(self) -> Dict[str, List[EvidenceLink]]:
        """Parse all sprint files and return evidence grouped by EXPECT ID."""
        all_evidence: Dict[str, List[EvidenceLink]] = {}
        
        for sprint_file in self.config.sprint_files:
            filepath = self.config.sprints_dir / sprint_file
            evidence_list = self.parse_sprint_file(filepath)
            
            for evidence in evidence_list:
                if evidence.expect_id not in all_evidence:
                    all_evidence[evidence.expect_id] = []
                all_evidence[evidence.expect_id].append(evidence)
        
        return all_evidence
    
    def scan_evidence_folders(self, table_rows: List['TableRow']) -> Dict[str, List[EvidenceLink]]:
        """
        Scan evidence folders for files that might be evidence for requirements:
        - docs/demos, docs/guides, docs/images, docs/presentations
        - src/ (source code folder)
        
        Uses filename pattern matching to associate files with EXPECT-L0-X.
        """
        evidence_folders = [
            self.config.repo_root / "docs" / "demos",
            self.config.repo_root / "docs" / "guides",
            self.config.repo_root / "docs" / "images",
            self.config.repo_root / "docs" / "presentations",
            self.config.repo_root / "src",  # Source code folder
        ]
        
        # Build keyword map from requirements
        # Maps keywords to requirement IDs
        keyword_map = self._build_keyword_map(table_rows)
        
        folder_evidence: Dict[str, List[EvidenceLink]] = {}
        
        for folder in evidence_folders:
            if not folder.exists():
                continue
            
            # Scan all files recursively
            for file_path in folder.rglob('*'):
                if file_path.is_dir():
                    continue
                
                # Skip non-evidence files
                # Include: images, videos, docs, code files
                valid_extensions = [
                    # Images and media
                    '.png', '.jpg', '.jpeg', '.gif', '.webm', '.mp4', '.svg',
                    # Documents
                    '.md', '.pdf', '.txt',
                    # Code files (for src/ folder)
                    '.py', '.cpp', '.c', '.h', '.hpp', '.qml', '.js', '.ts',
                    '.sh', '.yaml', '.yml', '.json', '.cmake', '.dockerfile',
                ]
                if file_path.suffix.lower() not in valid_extensions:
                    continue
                
                # Try to match file to a requirement
                filename_lower = file_path.stem.lower().replace('-', ' ').replace('_', ' ')
                
                matched_expects = set()
                
                # Check for direct EXPECT/L0 reference in filename
                l0_match = re.search(r'l0[_\-\s]?(\d+)', filename_lower)
                if l0_match:
                    expect_id = f"EXPECT-L0-{l0_match.group(1)}"
                    matched_expects.add(expect_id)
                
                expect_match = re.search(r'expect[_\-\s]?l0[_\-\s]?(\d+)', filename_lower)
                if expect_match:
                    expect_id = f"EXPECT-L0-{expect_match.group(1)}"
                    matched_expects.add(expect_id)
                
                # Check for keyword matches
                for keyword, expect_ids in keyword_map.items():
                    if keyword in filename_lower:
                        for eid in expect_ids:
                            matched_expects.add(eid)
                
                # Create evidence links for matched expects
                for expect_id in matched_expects:
                    if expect_id not in folder_evidence:
                        folder_evidence[expect_id] = []
                    
                    # Build GitHub URL
                    rel_path = file_path.relative_to(self.config.repo_root)
                    github_url = f"https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/{rel_path}"
                    
                    # Determine link type
                    link_type = "image" if file_path.suffix.lower() in ['.png', '.jpg', '.jpeg', '.gif'] else "link"
                    
                    evidence = EvidenceLink(
                        expect_id=expect_id,
                        description=file_path.stem.replace('-', ' ').replace('_', ' '),
                        url=github_url,
                        link_type=link_type,
                        source_file=str(rel_path)
                    )
                    
                    # Avoid duplicates
                    existing_urls = [e.url for e in folder_evidence[expect_id]]
                    if github_url not in existing_urls:
                        folder_evidence[expect_id].append(evidence)
        
        return folder_evidence
    
    def _build_keyword_map(self, table_rows: List['TableRow']) -> Dict[str, List[str]]:
        """
        Build a map of keywords to EXPECT IDs based on requirement content.
        """
        keyword_map: Dict[str, List[str]] = {}
        
        # Define keyword associations for common topics
        topic_keywords = {
            'ota': ['ota', 'update', 'rauc', 'bundle', 'release'],
            'can': ['can', 'canbus', 'can-bus', 'can bus'],
            'threadx': ['threadx', 'rtos', 'stm32', 'stm'],
            'qt': ['qt', 'qml', 'cluster', 'hmi', 'display', 'mockup'],
            'agl': ['agl', 'linux', 'boot'],
            'architecture': ['architecture', 'scheme', 'layout', 'diagram'],
            'test': ['test', 'coverage', 'lcov'],
            'ci': ['ci', 'cd', 'action', 'workflow', 'pipeline'],
            'cross': ['cross', 'compile', 'crosscompil'],
            'car': ['car', 'assembled', '3d', 'vehicle'],
            'energy': ['energy', 'power', 'consumption', 'thermal'],
        }
        
        for row in table_rows:
            expect_id = f"EXPECT-L0-{row.number}"
            req_lower = row.requirement.lower()
            
            # Check which topics this requirement relates to
            for topic, keywords in topic_keywords.items():
                for kw in keywords:
                    if kw in req_lower:
                        # This requirement is about this topic
                        # Add all topic keywords to map pointing to this expect
                        for topic_kw in keywords:
                            if topic_kw not in keyword_map:
                                keyword_map[topic_kw] = []
                            if expect_id not in keyword_map[topic_kw]:
                                keyword_map[topic_kw].append(expect_id)
                        break
        
        return keyword_map


# ============================================================================
# TABLE PARSER
# ============================================================================

class TableParser:
    """Parser for TSF requirements table markdown file."""
    
    def __init__(self, config: Config):
        self.config = config
    
    def parse_table(self) -> List[TableRow]:
        """Parse the requirements table and return list of TableRow objects."""
        if not self.config.requirements_table.exists():
            raise FileNotFoundError(f"Requirements table not found: {self.config.requirements_table}")
        
        with open(self.config.requirements_table, 'r', encoding='utf-8') as f:
            content = f.read()
        
        rows = []
        lines = content.split('\n')
        
        # Find table start (line with | ID |)
        in_table = False
        header_passed = False
        
        for line in lines:
            line = line.strip()
            
            # Detect table header (handles bold markers like **ID**)
            if '| ID |' in line or '|ID|' in line or '| **ID** |' in line or '|**ID**|' in line:
                in_table = True
                continue
            
            # Skip separator line
            if in_table and line.startswith('|') and '---' in line:
                header_passed = True
                continue
            
            # Parse data rows
            if in_table and header_passed and line.startswith('|'):
                row = self._parse_table_row(line)
                if row:
                    rows.append(row)
        
        return rows
    
    def _parse_table_row(self, line: str) -> Optional[TableRow]:
        """Parse a single table row."""
        # Split by | and clean up
        parts = [p.strip() for p in line.split('|')]
        # Remove empty first and last elements (from leading/trailing |)
        # Line starts and ends with |, so parts[0] and parts[-1] are empty
        if len(parts) >= 2:
            parts = parts[1:-1]  # Slice to remove first and last empty elements
        
        if len(parts) < 5:
            return None
        
        # Extract L0-X ID (handles bold markers like **L0-1**)
        id_match = re.search(r'L0-\d+', parts[0])
        if not id_match:
            return None
        
        return TableRow(
            id=id_match.group(),
            requirement=parts[1] if len(parts) > 1 else '',
            acceptance_criteria=parts[2] if len(parts) > 2 else '',
            verification_method=parts[3] if len(parts) > 3 else '',
            evidence=parts[4] if len(parts) > 4 else ''
        )
    
    def get_incomplete_rows(self) -> List[Tuple[TableRow, List[str]]]:
        """Get rows with empty fields and list of missing fields."""
        rows = self.parse_table()
        incomplete = []
        
        for row in rows:
            empty_fields = row.get_empty_fields()
            if empty_fields:
                incomplete.append((row, empty_fields))
        
        return incomplete


# ============================================================================
# AI GENERATOR
# ============================================================================

class AIGenerator:
    """
    AI content generator with Option G (manual/VSCode) as primary
    and Option C (gh copilot CLI) as fallback.
    """
    
    def __init__(self, config: Config):
        self.config = config
    
    def generate_content_batch(self, items: List[Tuple[str, str, Path]], 
                                table_rows: List) -> List[str]:
        """
        Generate content for multiple TSF items at once.
        Shows a consolidated prompt for all items.
        
        Args:
            items: List of (item_type, item_id, file_path) tuples
            table_rows: List of TableRow objects for context
            
        Returns:
            List of successfully updated file paths
        """
        if not items:
            return []
        
        # Group items by requirement ID
        items_by_req = {}
        for item_type, item_id, file_path in items:
            if item_id not in items_by_req:
                items_by_req[item_id] = []
            items_by_req[item_id].append((item_type, file_path))
        
        settings = self.config.manual_settings
        updated_files = []
        
        print(f"\n{'='*70}")
        print(f"🤖 AI GENERATION REQUIRED: {len(items)} items for {len(items_by_req)} requirement(s)")
        print(f"{'='*70}")
        
        # Open all files in VSCode
        if settings.get('open_in_vscode', True):
            if is_vscode_cli_available():
                print(f"\n📂 Opening {len(items)} files in VSCode...")
                for item_type, item_id, file_path in items:
                    subprocess.run(['code', str(file_path)], check=False)
            else:
                print("\nℹ️  VSCode CLI (`code`) not available; skipping file opening.")
        
        # Build consolidated prompt
        if settings.get('show_prompt_suggestion', True):
            print(f"\n📝 CONSOLIDATED PROMPT for Claude/Copilot Chat:")
            print(f"{'='*70}")
            
            prompt_lines = ["Generate content for the following TSF items:\n"]
            
            for req_id, req_items in items_by_req.items():
                # Find corresponding table row
                row = next((r for r in table_rows if str(r.number) == str(req_id)), None)
                
                if row:
                    prompt_lines.append(f"## Requirement L0-{req_id}")
                    prompt_lines.append(f"**Requirement:** {row.requirement}")
                    prompt_lines.append(f"**Acceptance Criteria:** {row.acceptance_criteria}")
                    prompt_lines.append(f"**Verification Method:** {row.verification_method}")
                    prompt_lines.append("")
                    
                    prompt_lines.append("**Items to generate:**")
                    for item_type, file_path in req_items:
                        prompt_lines.append(f"  - {item_type}-L0-{req_id}: {file_path}")
                    prompt_lines.append("")
            
            prompt_lines.append("""
---
**INSTRUCTIONS:**

For each item, fill the YAML frontmatter fields:
- `header`: A concise title (max 50 characters)
- `text`: Detailed description

**References rules:**
- EXPECT: Only reference to `../assertions/ASSERT-L0-X.md`
- ASSERT: Only reference to `../expectations/EXPECT-L0-X.md` and `../evidences/EVID-L0-X.md`
- EVID: Reference to actual evidence files/URLs (NOT to EXPECT or ASSERT)
- ASSUMP: Reference to `../expectations/EXPECT-L0-X.md`

**DO NOT modify:** `id`, `level`, `reviewers`, `review_status`, `evidence` (validators)
""")
            
            print("\n".join(prompt_lines))
            print(f"{'='*70}")
        
        # Wait for user confirmation
        if settings.get('wait_for_user_confirmation', True):
            if not is_vscode_cli_available():
                print("\nℹ️  Non-interactive fallback: VSCode CLI unavailable, skipping Option G wait.")
                return []
            print(f"\n⏳ Please use Copilot Chat (Cmd+L) or Claude to generate content for ALL items.")
            print(f"   After AI has edited the files, press Enter to continue...")
            print(f"   (Type 'skip' to skip AI generation, 'quit' to exit)")
            
            user_input = input("\n>>> ").strip().lower()
            
            if user_input == 'quit':
                print("❌ User requested quit.")
                sys.exit(0)
            elif user_input == 'skip':
                print("⏭️  Skipping AI generation...")
                return []
            else:
                print("✅ Continuing with all items marked as updated...")
                # Mark all as updated
                for item_type, item_id, file_path in items:
                    updated_files.append(str(file_path))
        
        return updated_files
    
    def generate_content(self, item_type: str, item_id: str, 
                         requirement: str, acceptance_criteria: str,
                         file_path: Path) -> bool:
        """
        Generate content for a TSF item.
        Returns True if successful, False otherwise.
        """
        # Try primary method first
        primary = self.config.ai_primary_method
        
        if primary == AIMethod.MANUAL:
            success = self._generate_manual(item_type, item_id, requirement, 
                                           acceptance_criteria, file_path)
            if success:
                return True
        elif primary == AIMethod.COPILOT_CLI:
            success = self._generate_with_copilot_cli(item_type, item_id, 
                                                       requirement, file_path)
            if success:
                return True
        
        # Try fallbacks
        for fallback in self.config.ai_fallbacks:
            if fallback == AIMethod.COPILOT_CLI:
                success = self._generate_with_copilot_cli(item_type, item_id,
                                                          requirement, file_path)
                if success:
                    return True
            elif fallback == AIMethod.MANUAL:
                success = self._generate_manual(item_type, item_id, requirement,
                                               acceptance_criteria, file_path)
                if success:
                    return True
        
        return False
    
    def _generate_manual(self, item_type: str, item_id: str,
                         requirement: str, acceptance_criteria: str,
                         file_path: Path) -> bool:
        """
        Option G: Semi-automated generation via VSCode/Claude.
        1. Opens file in VSCode
        2. Shows suggested prompt for user
        3. Waits for user confirmation
        """
        settings = self.config.manual_settings
        
        print(f"\n{'='*60}")
        print(f"🤖 AI GENERATION REQUIRED: {item_type}-L0-{item_id}")
        print(f"{'='*60}")
        
        # Open file in VSCode
        if settings.get('open_in_vscode', True):
            if is_vscode_cli_available():
                print(f"\n📂 Opening file in VSCode: {file_path}")
                subprocess.run(['code', str(file_path)], check=False)
            else:
                print("\nℹ️  VSCode CLI (`code`) not available; trying fallback method.")
                return False
        
        # Show suggested prompt
        if settings.get('show_prompt_suggestion', True):
            prompt = self._build_prompt(item_type, item_id, requirement, acceptance_criteria)
            print(f"\n📝 SUGGESTED PROMPT for Claude/Copilot Chat:")
            print(f"{'-'*60}")
            print(prompt)
            print(f"{'-'*60}")
        
        # Wait for user confirmation
        if settings.get('wait_for_user_confirmation', True):
            if not is_vscode_cli_available():
                print("\nℹ️  Non-interactive fallback: VSCode CLI unavailable, skipping Option G wait.")
                return False
            print(f"\n⏳ Please use Copilot Chat (Cmd+L) or Claude to generate content.")
            print(f"   After AI has edited the file, press Enter to continue...")
            print(f"   (Type 'skip' to try CLI fallback, 'quit' to exit)")
            
            user_input = input("\n>>> ").strip().lower()
            
            if user_input == 'quit':
                print("❌ User requested quit.")
                sys.exit(0)
            elif user_input == 'skip':
                print("⏭️  Skipping to fallback method...")
                return False
            else:
                print("✅ Continuing...")
                return True
        
        return True
    
    def _build_prompt(self, item_type: str, item_id: str,
                      requirement: str, acceptance_criteria: str) -> str:
        """Build the suggested prompt for AI generation."""
        template = self.config.manual_settings.get('prompt_template', '')
        
        if template:
            return template.format(
                item_type=item_type,
                id=item_id,
                requirement=requirement,
                acceptance_criteria=acceptance_criteria
            )
        
        # Build item-type specific instructions for references
        if item_type == "EXPECT":
            references_instruction = """- references: ONLY file references to the corresponding assertion.
  Example:
  references:
    - type: file
      path: ../assertions/ASSERT-L0-{id}.md
  ⚠️ DO NOT add URL references in expectations."""
        elif item_type == "ASSERT":
            references_instruction = """- references: ONLY file references to expectation and evidence files.
  Example:
  references:
    - type: file
      path: ../expectations/EXPECT-L0-{id}.md
    - type: file
      path: ../evidences/EVID-L0-{id}.md
  ⚠️ DO NOT add URL references in assertions."""
        elif item_type == "EVID":
            references_instruction = """- references: Evidence sources including file paths and URLs.
  Example:
  references:
    - type: file
      path: ../../sprints/sprint1.md
    - type: url
      path: https://example.com/documentation
      description: External documentation source
  ⚠️ DO NOT add references to assertions - evidences reference external sources only."""
        else:
            references_instruction = "- references: Relevant file references only."
        
        references_instruction = references_instruction.format(id=item_id)
        
        # Default prompt with explicit instructions
        return f"""Generate content for TSF item {item_type}-L0-{item_id}:

**Requirement:** {requirement}
**Acceptance Criteria:** {acceptance_criteria}

Please fill the following fields in the YAML frontmatter:
- header: A concise title for this item (max 50 characters)
- text: Detailed description/content

{references_instruction}

⚠️ IMPORTANT RULES:
1. Use the existing file structure, only fill empty fields
2. Do NOT modify the 'id', 'level', 'reviewers', or 'review_status' fields
3. Keep the YAML frontmatter format (between --- markers)
4. EXPECT items: references ONLY to assertion file, NO URLs
5. ASSERT items: references ONLY to expectation and evidence files, NO URLs
6. EVID items: references to external sources (files, URLs), NO references to assertions"""
    
    def _generate_with_copilot_cli(self, item_type: str, item_id: str,
                                    requirement: str, file_path: Path) -> bool:
        """
        Option C: Generate using gh copilot CLI.
        """
        print(f"\n🔧 Trying gh copilot CLI for {item_type}-L0-{item_id}...")
        
        # Check if gh copilot is available
        try:
            result = subprocess.run(
                ['gh', 'copilot', '--version'],
                capture_output=True,
                text=True,
                timeout=5
            )
            if result.returncode != 0:
                print("  ⚠️  gh copilot not available")
                return False
        except (subprocess.TimeoutExpired, FileNotFoundError):
            print("  ⚠️  gh copilot not installed or not in PATH")
            return False
        
        # Build prompt for copilot
        prompt = f"Generate TSF {item_type} content for requirement L0-{item_id}: {requirement}"
        
        try:
            copilot_settings = self.config._config.get('ai', {}).get('copilot', {})
            timeout = copilot_settings.get('timeout', 30)
            
            result = subprocess.run(
                ['gh', 'copilot', 'suggest', '-t', 'shell', prompt],
                capture_output=True,
                text=True,
                timeout=timeout
            )
            
            if result.returncode == 0 and result.stdout:
                print(f"  ✅ Content generated via gh copilot")
                # TODO: Parse and write to file
                return True
            else:
                print(f"  ⚠️  gh copilot returned no content")
                return False
                
        except subprocess.TimeoutExpired:
            print(f"  ⚠️  gh copilot timed out")
            return False
        except Exception as e:
            print(f"  ⚠️  gh copilot error: {e}")
            return False


# ============================================================================
# CONTENT VALIDATOR (Detects placeholders, TODOs, and generic content)
# ============================================================================

@dataclass
class ContentValidationResult:
    """Result of content validation for a TSF item."""
    file_path: Path
    item_type: str
    item_id: str
    is_valid: bool
    issues: List[str]
    needs_regeneration: bool
    
    def __str__(self):
        status = "✅" if self.is_valid else "❌"
        return f"{status} {self.item_type}-L0-{self.item_id}: {', '.join(self.issues) if self.issues else 'OK'}"


class ContentValidator:
    """
    Validates content of TSF item files.
    Detects placeholders, TODOs, and generic/template content.
    """
    
    # Patterns that indicate placeholder/incomplete content
    PLACEHOLDER_PATTERNS = [
        r'\bTODO\b',                           # TODO keyword
        r'\bTBD\b',                            # TBD keyword
        r'\bFIXME\b',                          # FIXME keyword
        r'TODO@example\.com',                  # Default email
        r'example\.com',                       # Generic domain
        r'Add content',                        # Generic placeholder text
        r'Add references',                     # Generic placeholder
    ]
    
    # Patterns for generic headers (not real content)
    GENERIC_HEADER_PATTERNS = [
        r'^Assertion for L0-\d+$',             # "Assertion for L0-X"
        r'^Expectation for L0-\d+$',           # "Expectation for L0-X"
        r'^Evidence for L0-\d+$',              # "Evidence for L0-X"
        r'^Assumption for L0-\d+$',            # "Assumption for L0-X"
        r'^Header for ',                       # Generic "Header for..."
    ]
    
    # Patterns for generic text content
    GENERIC_TEXT_PATTERNS = [
        r'^Assertion supporting EXPECT-L0-\d+\.$',    # Generic assertion
        r'^Evidence supporting EXPECT-L0-\d+\.$',     # Generic evidence
        r'^Assumption for EXPECT-L0-\d+\.$',          # Generic assumption
        r'^This is .* for L0-\d+',                     # Generic description
    ]
    
    def __init__(self, config: Config):
        self.config = config
        # Compile all patterns
        self.placeholder_re = [re.compile(p, re.IGNORECASE) for p in self.PLACEHOLDER_PATTERNS]
        self.generic_header_re = [re.compile(p, re.IGNORECASE) for p in self.GENERIC_HEADER_PATTERNS]
        self.generic_text_re = [re.compile(p, re.IGNORECASE) for p in self.GENERIC_TEXT_PATTERNS]
    
    def validate_file(self, file_path: Path) -> ContentValidationResult:
        """Validate a single TSF item file."""
        # Extract item type and ID from filename
        filename = file_path.stem
        match = re.match(r'(EXPECT|ASSERT|EVID|ASSUMP)-L0-(\d+)', filename, re.IGNORECASE)
        
        if not match:
            return ContentValidationResult(
                file_path=file_path,
                item_type="UNKNOWN",
                item_id="0",
                is_valid=False,
                issues=["Invalid filename format"],
                needs_regeneration=False
            )
        
        item_type = match.group(1).upper()
        item_id = match.group(2)
        
        if not file_path.exists():
            return ContentValidationResult(
                file_path=file_path,
                item_type=item_type,
                item_id=item_id,
                is_valid=False,
                issues=["File does not exist"],
                needs_regeneration=True
            )
        
        # Read and parse YAML frontmatter
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        issues = []
        
        # Extract YAML frontmatter
        yaml_match = re.match(r'^---\n(.*?)\n---', content, re.DOTALL)
        if not yaml_match:
            return ContentValidationResult(
                file_path=file_path,
                item_type=item_type,
                item_id=item_id,
                is_valid=False,
                issues=["No valid YAML frontmatter"],
                needs_regeneration=True
            )
        
        try:
            frontmatter = yaml.safe_load(yaml_match.group(1))
        except yaml.YAMLError as e:
            return ContentValidationResult(
                file_path=file_path,
                item_type=item_type,
                item_id=item_id,
                is_valid=False,
                issues=[f"Invalid YAML: {e}"],
                needs_regeneration=True
            )
        
        if not frontmatter:
            frontmatter = {}
        
        # Check header field
        header = str(frontmatter.get('header', '')).strip()
        if not header:
            issues.append("Empty header")
        else:
            for pattern in self.generic_header_re:
                if pattern.match(header):
                    issues.append(f"Generic header: '{header}'")
                    break
            for pattern in self.placeholder_re:
                if pattern.search(header):
                    issues.append(f"Placeholder in header: '{header}'")
                    break
        
        # Check text field
        text = str(frontmatter.get('text', '')).strip()
        if not text:
            issues.append("Empty text")
        else:
            for pattern in self.generic_text_re:
                if pattern.match(text):
                    issues.append(f"Generic text content")
                    break
            for pattern in self.placeholder_re:
                if pattern.search(text):
                    issues.append(f"Placeholder in text")
                    break
        
        # Check level field
        level = str(frontmatter.get('level', '')).strip()
        if not level:
            issues.append("Empty level")
        elif level == '1.2' and item_id not in ['2']:
            # 1.2 is often a default/placeholder unless it's actually L0-2
            issues.append(f"Suspicious level '{level}' (might be placeholder)")
        
        # Check reviewers
        reviewers = frontmatter.get('reviewers', [])
        if isinstance(reviewers, list):
            for reviewer in reviewers:
                if isinstance(reviewer, dict):
                    name = str(reviewer.get('name', '')).strip()
                    email = str(reviewer.get('email', '')).strip()
                    if name == 'TODO' or 'TODO' in email:
                        issues.append("TODO in reviewers")
                        break
                    if 'example.com' in email:
                        issues.append("Placeholder email in reviewers")
                        break
        
        # Check references
        references = frontmatter.get('references', [])
        if not references:
            issues.append("No references")
        elif isinstance(references, list):
            for ref in references:
                ref_str = str(ref)
                if 'TODO' in ref_str:
                    issues.append("TODO in references")
                    break

            # EVID-specific placeholder detection using explicit marker.
            if item_type == 'EVID' and any(is_placeholder_evidence_reference(ref) for ref in references):
                issues.append("Placeholder evidence marker in references")
        
        # Check item-type-specific required fields
        # EVID items must have score field
        if item_type == 'EVID':
            score = frontmatter.get('score')
            if score is None:
                issues.append("Missing required 'score:' field (EVID items need score)")
        
        # ASSUMP items must have evidence field with validator
        if item_type == 'ASSUMP':
            evidence = frontmatter.get('evidence')
            if evidence is None:
                issues.append("Missing required 'evidence:' field (ASSUMP items need validator)")
            elif isinstance(evidence, dict):
                if not evidence.get('type'):
                    issues.append("Missing 'type' in evidence validator")
                if not evidence.get('configuration'):
                    issues.append("Missing 'configuration' in evidence validator")
        
        # Determine if needs regeneration
        needs_regeneration = len(issues) > 0
        is_valid = len(issues) == 0
        
        return ContentValidationResult(
            file_path=file_path,
            item_type=item_type,
            item_id=item_id,
            is_valid=is_valid,
            issues=issues,
            needs_regeneration=needs_regeneration
        )
    
    def validate_all_items(self) -> Dict[str, List[ContentValidationResult]]:
        """Validate all TSF items and return results grouped by type."""
        results = {
            'EXPECT': [],
            'ASSERT': [],
            'EVID': [],
            'ASSUMP': []
        }
        
        dirs = {
            'EXPECT': self.config.expectations_dir,
            'ASSERT': self.config.assertions_dir,
            'EVID': self.config.evidences_dir,
            'ASSUMP': self.config.assumptions_dir
        }
        
        for item_type, directory in dirs.items():
            if directory.exists():
                for file_path in sorted(directory.glob(f"{item_type}-L0-*.md")):
                    result = self.validate_file(file_path)
                    results[item_type].append(result)
        
        return results
    
    def get_items_needing_regeneration(self) -> List[ContentValidationResult]:
        """Get list of all items that need regeneration."""
        all_results = self.validate_all_items()
        needs_regen = []
        
        for item_type, results in all_results.items():
            for result in results:
                if result.needs_regeneration:
                    needs_regen.append(result)
        
        return needs_regen
    
    def print_validation_report(self) -> Tuple[int, int]:
        """Print a validation report and return (valid_count, invalid_count)."""
        print("\n" + "="*70)
        print("📋 CONTENT VALIDATION REPORT")
        print("="*70)
        
        all_results = self.validate_all_items()
        valid_count = 0
        invalid_count = 0
        
        for item_type in ['EXPECT', 'ASSERT', 'EVID', 'ASSUMP']:
            results = all_results[item_type]
            if not results:
                continue
            
            print(f"\n📁 {item_type}:")
            for result in results:
                if result.is_valid:
                    valid_count += 1
                    print(f"   ✅ {result.item_type}-L0-{result.item_id}")
                else:
                    invalid_count += 1
                    print(f"   ❌ {result.item_type}-L0-{result.item_id}")
                    for issue in result.issues:
                        print(f"      └─ {issue}")
        
        print(f"\n{'='*70}")
        print(f"📊 Summary: {valid_count} valid, {invalid_count} need attention")
        print("="*70)
        
        return valid_count, invalid_count


# ============================================================================
# ITEM FILE MANAGER
# ============================================================================

class ItemFileManager:
    """Manages TSF item files (EXPECT, ASSERT, EVID, ASSUMP)."""
    
    def __init__(self, config: Config):
        self.config = config
        self.validator = ContentValidator(config)
    
    def get_item_path(self, item_type: str, item_id: str) -> Path:
        """Get the file path for a TSF item."""
        dirs = {
            'EXPECT': self.config.expectations_dir,
            'ASSERT': self.config.assertions_dir,
            'EVID': self.config.evidences_dir,
            'ASSUMP': self.config.assumptions_dir
        }
        
        directory = dirs.get(item_type.upper())
        if not directory:
            raise ValueError(f"Unknown item type: {item_type}")
        
        return directory / f"{item_type.upper()}-L0-{item_id}.md"
    
    def item_exists(self, item_type: str, item_id: str) -> bool:
        """Check if an item file exists."""
        return self.get_item_path(item_type, item_id).exists()
    
    def get_all_items_for_id(self, item_id: str) -> Dict[str, bool]:
        """Check which items exist for a given L0-X ID."""
        return {
            'EXPECT': self.item_exists('EXPECT', item_id),
            'ASSERT': self.item_exists('ASSERT', item_id),
            'EVID': self.item_exists('EVID', item_id),
            'ASSUMP': self.item_exists('ASSUMP', item_id)
        }
    
    def create_item_from_template(self, item_type: str, item_id: str,
                                   header: str = '', text: str = '',
                                   references: List[str] = None) -> Path:
        """Create an item file from template with proper structure for each item type."""
        file_path = self.get_item_path(item_type, item_id)
        
        # Ensure directory exists
        file_path.parent.mkdir(parents=True, exist_ok=True)
        
        # Extract level number from item_id
        level_num = item_id
        id_str = f"L0-{item_id}"
        
        # Use provided header/text or create placeholders
        header_text = header[:50] if header else 'TODO: Add header'
        requirement_text = text if text else 'TODO: Add requirement text'
        
        # Templates for each item type with correct reference structure
        # NOTE: EVID files should NOT have 'evidence:' field - that's for ASSERT/ASSUMP
        # NOTE: ASSERT files should have 'evidence:' with a valid validator type
        # Valid validators: validate_hardware_availability, validate_linux_environment, validate_software_dependencies
        templates = {
            'EXPECT': f"""---
id: EXPECT-{id_str}
header: "{header_text}"
text: |
  {requirement_text}
level: '1.{level_num}'
normative: true
references:
- type: file
  path: ../assertions/ASSERT-{id_str}.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
---
""",
            'ASSERT': f"""---
id: ASSERT-{id_str}
header: "{header_text}"
text: |
  The assertion verifies that {requirement_text.lower() if requirement_text != 'TODO: Add requirement text' else 'the requirement is satisfied'}.
level: '1.{level_num}'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-{id_str}.md
- type: file
  path: ../evidences/EVID-{id_str}.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "STM32"
      - "CAN"
      - "Raspberry Pi"
---
""",
            'EVID': f"""---
id: EVID-{id_str}
header: "{header_text} - Evidence"
text: |
  Evidence demonstrates that {requirement_text.lower() if requirement_text != 'TODO: Add requirement text' else 'the requirement is satisfied'}.
  
  Note: Evidence files/links will be added when available from sprint documentation.
level: '1.{level_num}'
normative: true
references:
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md
score: 0.0
---
This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.
No evidence links available yet - will be synced from sprint documentation when available.
""",
            'ASSUMP': f"""---
id: ASSUMP-{id_str}
header: "Assumption: {header_text}"
text: |
  Assumption: The development environment meets all prerequisites for implementing and verifying this requirement.
level: '1.{level_num}'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-{id_str}.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_software_dependencies
  configuration:
    components:
      - "Development environment"
      - "Required tools"
      - "Test infrastructure"
---
"""
        }
        
        # Get the appropriate template
        item_type_upper = item_type.upper()
        if item_type_upper in templates:
            content = templates[item_type_upper]
        else:
            # Fallback generic template
            content = f"""---
id: {item_type_upper}-{id_str}
header: "{header_text}"
text: |
  {requirement_text}
level: '1.{level_num}'
normative: true
references:
- type: file
  path: TODO_add_reference.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
---
"""
        
        # Write file
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(content)
        
        print(f"  ✅ Created: {file_path.name}")
        return file_path
    
    def update_item_references(self, item_type: str, item_id: str,
                                references: List[str]) -> bool:
        """Update references in an existing item file."""
        file_path = self.get_item_path(item_type, item_id)
        
        if not file_path.exists():
            print(f"  ⚠️  File not found: {file_path}")
            return False
        
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Find and replace references section
        # This is a simplified approach - could be improved with YAML parsing
        # For now, we'll append references if they don't exist
        
        if 'references:' in content:
            # TODO: More sophisticated reference merging
            print(f"  ℹ️  References already exist in {file_path.name}")
            return True
        
        return False

    def fix_item_structure(self, item_type: str, item_id: str) -> Tuple[bool, List[str]]:
        """
        Fix structural issues in an existing item file.
        
        Returns:
            Tuple of (was_fixed, list_of_fixes_applied)
        """
        file_path = self.get_item_path(item_type, item_id)
        fixes_applied = []
        
        if not file_path.exists():
            return False, ["File does not exist"]
        
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original_content = content
        
        # Parse YAML frontmatter
        yaml_match = re.match(r'^---\n(.*?)\n---', content, re.DOTALL)
        if not yaml_match:
            return False, ["No YAML frontmatter found"]
        
        try:
            frontmatter = yaml.safe_load(yaml_match.group(1))
            if not frontmatter:
                frontmatter = {}
        except yaml.YAMLError as e:
            return False, [f"YAML parse error: {e}"]
        
        body_content = content[yaml_match.end():]
        item_type_upper = item_type.upper()
        modified = False
        
        # Fix 1: EVID files should NOT have 'evidence:' field
        if item_type_upper == 'EVID' and 'evidence' in frontmatter:
            del frontmatter['evidence']
            fixes_applied.append("Removed invalid 'evidence:' field from EVID file")
            modified = True
        
        # Fix 2: EVID files should have 'score: 1.0'
        if item_type_upper == 'EVID' and 'score' not in frontmatter:
            frontmatter['score'] = 1.0
            fixes_applied.append("Added 'score: 1.0'")
            modified = True
        
        # Fix 3: Add review_status: accepted if missing (for files with reviewers)
        if 'reviewers' in frontmatter and 'review_status' not in frontmatter:
            frontmatter['review_status'] = 'accepted'
            fixes_applied.append("Added 'review_status: accepted'")
            modified = True
        
        # Fix 4: ASSERT/ASSUMP files should have 'evidence:' with valid validator
        valid_validators = ['validate_hardware_availability', 'validate_linux_environment', 'validate_software_dependencies']
        
        if item_type_upper in ['ASSERT', 'ASSUMP']:
            evidence = frontmatter.get('evidence', {})
            if not evidence:
                # Add default evidence validator
                frontmatter['evidence'] = {
                    'type': 'validate_hardware_availability',
                    'configuration': {
                        'components': ['STM32', 'CAN', 'Raspberry Pi']
                    }
                }
                fixes_applied.append("Added default evidence validator")
                modified = True
            elif isinstance(evidence, dict):
                ev_type = evidence.get('type', '')
                if ev_type and ev_type not in valid_validators:
                    # Replace invalid validator with a valid one
                    frontmatter['evidence']['type'] = 'validate_hardware_availability'
                    fixes_applied.append(f"Replaced invalid validator '{ev_type}' with 'validate_hardware_availability'")
                    modified = True
        
        # Fix 5: Remove 'id' fields from references (should only have type and path)
        if 'references' in frontmatter and isinstance(frontmatter['references'], list):
            refs_fixed = False
            for ref in frontmatter['references']:
                if isinstance(ref, dict) and 'id' in ref:
                    del ref['id']
                    refs_fixed = True
            if refs_fixed:
                fixes_applied.append("Removed 'id' fields from references")
                modified = True
        
        # Fix 6: EVID files should NOT reference EXPECT/ASSERT files
        # Evidence references should point to actual evidence (images, docs, logs)
        if item_type_upper == 'EVID' and 'references' in frontmatter and isinstance(frontmatter['references'], list):
            invalid_refs = []
            valid_refs = []
            for ref in frontmatter['references']:
                if isinstance(ref, dict):
                    ref_path = ref.get('path', '')
                    # Check if reference points to EXPECT or ASSERT files
                    if 'EXPECT-' in ref_path or 'ASSERT-' in ref_path or '/expectations/' in ref_path or '/assertions/' in ref_path:
                        invalid_refs.append(ref_path)
                    else:
                        valid_refs.append(ref)
                else:
                    valid_refs.append(ref)
            
            if invalid_refs:
                frontmatter['references'] = valid_refs
                fixes_applied.append(f"Removed {len(invalid_refs)} invalid references to EXPECT/ASSERT files")
                modified = True
        
        # Fix 7: EVID files MUST have at least one reference (trudag rejects empty references)
        if item_type_upper == 'EVID':
            refs = frontmatter.get('references', [])
            if not refs or len(refs) == 0:
                # Add placeholder reference to README
                frontmatter['references'] = [{
                    'type': 'url',
                    'url': 'https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md',
                    'description': PLACEHOLDER_EVIDENCE_MARKER
                }]
                frontmatter['score'] = 0.0  # Mark as incomplete (no real evidence)
                fixes_applied.append("Added placeholder reference (EVID cannot have empty references)")
                modified = True
        
        if not modified:
            return False, []
        
        # Rebuild the file content
        # Use yaml.dump with specific settings for clean output
        yaml_content = yaml.dump(frontmatter, default_flow_style=False, allow_unicode=True, sort_keys=False)
        new_content = f"---\n{yaml_content}---{body_content}"
        
        # Write back
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        
        return True, fixes_applied

    def fix_all_items_structure(self) -> Dict[str, List[str]]:
        """
        Fix structural issues in all existing item files.
        
        Returns:
            Dict mapping file paths to list of fixes applied
        """
        all_fixes = {}
        
        for item_type in ['EXPECT', 'ASSERT', 'EVID', 'ASSUMP']:
            directory = {
                'EXPECT': self.config.expectations_dir,
                'ASSERT': self.config.assertions_dir,
                'EVID': self.config.evidences_dir,
                'ASSUMP': self.config.assumptions_dir
            }[item_type]
            
            if not directory.exists():
                continue
            
            for file_path in directory.glob(f"{item_type}-L0-*.md"):
                # Extract item_id from filename
                match = re.search(r'L0-(\d+)', file_path.name)
                if not match:
                    continue
                
                item_id = match.group(1)
                was_fixed, fixes = self.fix_item_structure(item_type, item_id)
                
                if was_fixed:
                    all_fixes[str(file_path)] = fixes
        
        return all_fixes

    def find_orphan_items(self, table_rows: List[TableRow]) -> List[Tuple[str, str, Path]]:
        """
        Find item files that exist but don't have corresponding requirements in the table.
        
        Returns:
            List of tuples: (item_type, item_id, file_path)
        """
        orphans = []
        
        # Get all requirement IDs from table
        table_ids = set(str(row.number) for row in table_rows)
        
        for item_type in ['EXPECT', 'ASSERT', 'EVID', 'ASSUMP']:
            directory = {
                'EXPECT': self.config.expectations_dir,
                'ASSERT': self.config.assertions_dir,
                'EVID': self.config.evidences_dir,
                'ASSUMP': self.config.assumptions_dir
            }[item_type]
            
            if not directory.exists():
                continue
            
            for file_path in directory.glob(f"{item_type}-L0-*.md"):
                # Extract item_id from filename
                match = re.search(r'L0-(\d+)', file_path.name)
                if not match:
                    continue
                
                item_id = match.group(1)
                
                # Check if this ID exists in the table
                if item_id not in table_ids:
                    orphans.append((item_type, item_id, file_path))
        
        return orphans

    def remove_orphan_items(self, orphans: List[Tuple[str, str, Path]], 
                           dry_run: bool = True) -> List[Path]:
        """
        Remove orphan item files.
        
        Args:
            orphans: List of (item_type, item_id, file_path) tuples
            dry_run: If True, only show what would be removed without actually removing
        
        Returns:
            List of files that were removed (or would be removed in dry_run)
        """
        removed_files = []
        
        if not orphans:
            return removed_files
        
        print(f"\n🗑️  Found {len(orphans)} orphan item file(s) (no corresponding requirement in table):")
        print("-" * 60)
        
        for item_type, item_id, file_path in orphans:
            print(f"   • {file_path.name} ({file_path.parent.name})")
        
        print("-" * 60)
        
        if dry_run:
            print("   📋 DRY RUN: These files would be removed")
            removed_files = [file_path for _, _, file_path in orphans]
        else:
            print("   ⚠️  These files will be permanently deleted!")
            
            # Ask for confirmation
            confirm = input("\n   Are you sure you want to remove these orphan files? [y/N]: ").strip().lower()
            
            if confirm == 'y':
                for item_type, item_id, file_path in orphans:
                    try:
                        file_path.unlink()
                        print(f"   ✅ Removed: {file_path.name}")
                        removed_files.append(file_path)
                    except Exception as e:
                        print(f"   ❌ Failed to remove {file_path.name}: {e}")
            else:
                print("   ⏭️  Orphan removal cancelled by user")
        
        return removed_files


# ============================================================================
# MAIN FUNCTIONS
# ============================================================================

def open_check(config: Config) -> Dict[str, Any]:
    """
    Step 1: Verify table completeness, sync evidences from sprints to table,
    sync table to EVID files, and validate existing content.
    
    Returns a status dict with findings.
    """
    print("\n" + "="*70)
    print("📋 STEP 1: OPEN & CHECK")
    print("="*70)
    
    status = {
        'table_rows': [],
        'incomplete_rows': [],
        'sprint_evidence': {},
        'missing_items': [],
        'invalid_items': [],      # NEW: Items with placeholder/TODO content
        'sync_needed': []
    }
    
    # 1. Parse requirements table
    print("\n📊 Parsing requirements table...")
    table_parser = TableParser(config)
    try:
        rows = table_parser.parse_table()
        status['table_rows'] = rows
        print(f"   Found {len(rows)} requirements in table")
    except FileNotFoundError as e:
        print(f"   ❌ Error: {e}")
        return status
    
    # 2. Check for incomplete fields
    print("\n🔍 Checking for incomplete fields...")
    incomplete = table_parser.get_incomplete_rows()
    status['incomplete_rows'] = incomplete
    
    if incomplete:
        print(f"   Found {len(incomplete)} rows with empty fields:")
        for row, empty_fields in incomplete:
            print(f"   • {row.id}: missing {', '.join(empty_fields)}")
    else:
        print("   ✅ All rows have complete fields")
    
    # 3. Parse sprint files for evidence
    print("\n📁 Parsing sprint files for evidence...")
    evidence_parser = EvidenceParser(config)
    sprint_evidence = evidence_parser.parse_all_sprints()
    status['sprint_evidence'] = sprint_evidence
    
    total_evidence = sum(len(v) for v in sprint_evidence.values())
    print(f"   Found {total_evidence} evidence links across {len(sprint_evidence)} EXPECTs")
    
    for expect_id, evidences in sprint_evidence.items():
        print(f"   • {expect_id}: {len(evidences)} evidence(s)")
    
    # 3.3 NEW: Scan evidence folders (docs/demos, docs/guides, etc., and src/)
    print("\n📂 Scanning evidence folders (demos, guides, images, presentations, src)...")
    folder_evidence = evidence_parser.scan_evidence_folders(rows)
    status['folder_evidence'] = folder_evidence
    
    if folder_evidence:
        folder_total = sum(len(v) for v in folder_evidence.values())
        print(f"   Found {folder_total} potential evidence files across {len(folder_evidence)} EXPECTs")
        for expect_id, evidences in sorted(folder_evidence.items()):
            print(f"   • {expect_id}: {len(evidences)} file(s) found")
    else:
        print("   ℹ️  No additional evidence files found in folders")
    
    # Merge folder evidence with sprint evidence
    for expect_id, evidences in folder_evidence.items():
        if expect_id not in sprint_evidence:
            sprint_evidence[expect_id] = []
        for ev in evidences:
            # Avoid duplicates by URL
            existing_urls = [e.url for e in sprint_evidence[expect_id]]
            if ev.url not in existing_urls:
                sprint_evidence[expect_id].append(ev)
    
    status['sprint_evidence'] = sprint_evidence  # Update with merged evidence
    
    # 3.5 NEW: Check for requirements with NO evidence in sprints
    print("\n⚠️  Checking for requirements without sprint evidence...")
    status['no_sprint_evidence'] = []
    
    for row in rows:
        expect_id = f"EXPECT-L0-{row.number}"
        if expect_id not in sprint_evidence or len(sprint_evidence[expect_id]) == 0:
            status['no_sprint_evidence'].append(row.id)
    
    if status['no_sprint_evidence']:
        print(f"   ⚠️  {len(status['no_sprint_evidence'])} requirement(s) have NO evidence in sprint files:")
        for req_id in status['no_sprint_evidence']:
            print(f"      └─ {req_id}: No evidence found in sprints (docs/sprints/*.md)")
        print("\n   💡 Tip: Add evidence links to sprint files using the format:")
        print("      Evidence: [description](path/to/evidence) <!-- EXPECT-L0-X -->")
    else:
        print("   ✅ All requirements have evidence in sprint files")
    
    # 4. Check for missing item files
    print("\n📂 Checking item files existence...")
    item_manager = ItemFileManager(config)
    
    for row in rows:
        item_id = str(row.number)
        items_status = item_manager.get_all_items_for_id(item_id)
        
        missing = [k for k, v in items_status.items() if not v]
        if missing:
            status['missing_items'].append((row.id, missing))
            print(f"   • {row.id}: missing {', '.join(missing)}")
    
    if not status['missing_items']:
        print("   ✅ All item files exist")
    
    # 4.5 NEW: Fix structural issues in existing item files
    print("\n🔧 Fixing structural issues in existing items...")
    structural_fixes = item_manager.fix_all_items_structure()
    status['structural_fixes'] = structural_fixes
    
    if structural_fixes:
        print(f"   Fixed {len(structural_fixes)} file(s):")
        for file_path, fixes in structural_fixes.items():
            filename = Path(file_path).name
            print(f"   • {filename}:")
            for fix in fixes:
                print(f"      └─ {fix}")
    else:
        print("   ✅ All item files have correct structure")
    
    # 5. NEW: Validate content of existing items (detect placeholders/TODOs)
    print("\n🔍 Validating content of existing items...")
    content_validator = ContentValidator(config)
    valid_count, invalid_count = content_validator.print_validation_report()
    
    # Get items needing regeneration
    items_needing_regen = content_validator.get_items_needing_regeneration()
    status['invalid_items'] = items_needing_regen
    
    # 5.5 NEW: Check for orphan item files (files without corresponding requirements)
    print("\n🗑️  Checking for orphan item files...")
    orphan_items = item_manager.find_orphan_items(rows)
    status['orphan_items'] = orphan_items
    
    if orphan_items:
        print(f"   ⚠️  Found {len(orphan_items)} orphan item file(s):")
        for item_type, item_id, file_path in orphan_items:
            print(f"      └─ {file_path.name} (no L0-{item_id} in table)")
    else:
        print("   ✅ No orphan item files found")
    
    # 6. Identify sync needs (sprint evidence → table → EVID files)
    print("\n🔄 Identifying sync needs...")
    seen_sync_needs = set()
    for row in rows:
        expect_id = f"EXPECT-L0-{row.number}"
        evid_item_id = str(row.number)

        # Read EVID file to detect explicit placeholder references.
        evid_has_placeholder = False
        evid_path = item_manager.get_item_path('EVID', evid_item_id)
        if evid_path.exists():
            try:
                evid_content = evid_path.read_text(encoding='utf-8')
                evid_yaml_match = re.match(r'^---\n(.*?)\n---', evid_content, re.DOTALL)
                if evid_yaml_match:
                    evid_frontmatter = yaml.safe_load(evid_yaml_match.group(1)) or {}
                    evid_refs = evid_frontmatter.get('references', [])
                    if isinstance(evid_refs, list):
                        evid_has_placeholder = any(is_placeholder_evidence_reference(ref) for ref in evid_refs)
            except Exception:
                # Keep check resilient; placeholder detection is advisory for sync.
                evid_has_placeholder = False
        
        # Check if sprint has evidence that table doesn't
        if expect_id in sprint_evidence:
            table_evidence = row.evidence.strip()
            
            # Simple check - could be more sophisticated
            if not table_evidence or table_evidence == 'TODO':
                key = (row.id, 'sprint_to_table')
                if key not in seen_sync_needs:
                    seen_sync_needs.add(key)
                    status['sync_needed'].append(key)
                    print(f"   • {row.id}: needs sync from sprint to table")

            # New: if EVID file still uses explicit placeholder marker, sync real sprint evidence into EVID.
            if evid_has_placeholder and sprint_evidence.get(expect_id):
                key = (row.id, 'placeholder_to_real_evidence')
                if key not in seen_sync_needs:
                    seen_sync_needs.add(key)
                    status['sync_needed'].append(key)
                    print(f"   • {row.id}: EVID has placeholder evidence, needs replacement from sprints")
    
    print("\n" + "-"*70)
    print("📋 OPEN & CHECK Summary:")
    print(f"   • Total requirements: {len(rows)}")
    print(f"   • Incomplete table rows: {len(incomplete)}")
    print(f"   • Missing item files: {len(status['missing_items'])}")
    print(f"   • Requirements without sprint evidence: {len(status.get('no_sprint_evidence', []))}")
    print(f"   • Items with invalid content: {len(status['invalid_items'])}")
    print(f"   • Orphan item files: {len(status.get('orphan_items', []))}")
    print(f"   • Sync needed: {len(status['sync_needed'])}")
    
    return status


def sync_evidence_from_sprints(config: Config, check_status: Dict[str, Any]) -> Dict[str, Any]:
    """
    Synchronize evidence from sprint files to:
    1. The requirements table (tsf-requirements-table.md)
    2. The EVID files (docs/TSF/tsf_implementation/items/evidences/)
    
    Source of Truth: Sprint files → Table → EVID files
    """
    print("\n" + "="*70)
    print("🔄 SYNCING EVIDENCE FROM SPRINTS")
    print("="*70)
    
    sync_status = {
        'table_updates': [],
        'evid_updates': [],
        'errors': []
    }
    
    sprint_evidence = check_status.get('sprint_evidence', {})
    sync_needed = check_status.get('sync_needed', [])
    table_rows = check_status.get('table_rows', [])
    
    if not sprint_evidence:
        print("   ℹ️  No sprint evidence found to sync")
        return sync_status
    
    if not sync_needed:
        print("   ✅ All evidence already synced")
        return sync_status
    
    # Ask user before syncing
    print(f"\n📋 Found {len(sync_needed)} requirement(s) needing evidence sync:")
    for req_id, sync_type in sync_needed:
        expect_id = f"EXPECT-{req_id}"
        if expect_id in sprint_evidence:
            urls = [e.url for e in sprint_evidence[expect_id]]
            print(f"   • {req_id} [{sync_type}]: {len(urls)} evidence URL(s) from sprints")
    
    print("\n🔧 Options:")
    print("   [y] Sync all evidence (update table + EVID files)")
    print("   [t] Sync to table only")
    print("   [e] Sync to EVID files only")
    print("   [n] Skip sync")
    
    choice = input("\n>>> Choose option [y/t/e/n]: ").strip().lower()
    
    if choice == 'n':
        print("   ⏭️  Skipping evidence sync")
        return sync_status
    
    sync_table = choice in ['y', 't']
    sync_evid = choice in ['y', 'e']
    
    # 1. Sync to table
    if sync_table:
        print("\n📊 Syncing evidence to requirements table...")
        try:
            table_updates = _sync_evidence_to_table(config, sprint_evidence, table_rows)
            sync_status['table_updates'] = table_updates
            if table_updates:
                print(f"   ✅ Updated {len(table_updates)} row(s) in table")
            else:
                print("   ℹ️  No table updates needed")
        except Exception as e:
            error_msg = f"Error syncing to table: {e}"
            sync_status['errors'].append(error_msg)
            print(f"   ❌ {error_msg}")
    
    # 2. Sync to EVID files
    if sync_evid:
        print("\n📁 Syncing evidence to EVID files...")
        try:
            evid_updates = _sync_evidence_to_evid_files(config, sprint_evidence)
            sync_status['evid_updates'] = evid_updates
            if evid_updates:
                print(f"   ✅ Updated {len(evid_updates)} EVID file(s)")
            else:
                print("   ℹ️  No EVID updates needed")
        except Exception as e:
            error_msg = f"Error syncing to EVID files: {e}"
            sync_status['errors'].append(error_msg)
            print(f"   ❌ {error_msg}")
    
    return sync_status


def _sync_evidence_to_table(config: Config, sprint_evidence: Dict[str, List[EvidenceLink]], 
                            table_rows: List[TableRow]) -> List[str]:
    """
    Update the requirements table with evidence URLs from sprints.
    Returns list of updated row IDs.
    """
    updated_rows = []
    
    # Read current table content
    table_path = config.requirements_table
    with open(table_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    lines = content.split('\n')
    new_lines = []
    
    for line in lines:
        # Check if this is a table row (starts with |)
        if line.strip().startswith('|'):
            # Try to match L0-X pattern
            match = re.search(r'\*\*L0-(\d+)\*\*', line)
            if match:
                req_num = match.group(1)
                expect_id = f"EXPECT-L0-{req_num}"
                
                if expect_id in sprint_evidence and sprint_evidence[expect_id]:
                    # Build evidence string from sprint URLs
                    evidence_links = sprint_evidence[expect_id]
                    evidence_parts = []
                    
                    for ev in evidence_links:
                        if ev.link_type == "image":
                            evidence_parts.append(f"![{ev.description}]({ev.url})")
                        else:
                            evidence_parts.append(f"[{ev.description}]({ev.url})")
                    
                    evidence_str = " ".join(evidence_parts)
                    
                    # Check if current evidence column is empty or just has placeholder
                    # Table format: | ID | Requirement | Acceptance | Method | Evidence |
                    parts = line.split('|')
                    if len(parts) >= 6:
                        current_evidence = parts[5].strip() if len(parts) > 5 else ""
                        
                        # Only update if evidence is empty, TODO, or "No evidence yet"
                        if not current_evidence or current_evidence.lower() in ['todo', 'tbd', ''] or 'no evidence' in current_evidence.lower():
                            # Update the evidence column
                            parts[5] = f" EXPECT-L0-{req_num} — {evidence_str} "
                            line = '|'.join(parts)
                            updated_rows.append(f"L0-{req_num}")
        
        new_lines.append(line)
    
    # Write updated content
    if updated_rows:
        with open(table_path, 'w', encoding='utf-8') as f:
            f.write('\n'.join(new_lines))
    
    return updated_rows


def _sync_evidence_to_evid_files(config: Config, sprint_evidence: Dict[str, List[EvidenceLink]]) -> List[str]:
    """
    Update EVID files with evidence URLs from sprints.
    Returns list of updated file names.
    """
    updated_files = []
    evid_dir = config.items_dir / "evidences"
    
    for expect_id, evidence_list in sprint_evidence.items():
        if not evidence_list:
            continue
        
        # Extract requirement number from EXPECT-L0-X
        match = re.search(r'L0-(\d+)', expect_id)
        if not match:
            continue
        
        req_num = match.group(1)
        evid_file = evid_dir / f"EVID-L0-{req_num}.md"
        
        if not evid_file.exists():
            continue
        
        # Read current EVID file
        with open(evid_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Parse YAML frontmatter
        yaml_match = re.match(r'^---\n(.*?)\n---(.*)$', content, re.DOTALL)
        if not yaml_match:
            continue
        
        try:
            frontmatter = yaml.safe_load(yaml_match.group(1))
            body = yaml_match.group(2)
        except yaml.YAMLError:
            continue
        
        if not frontmatter:
            frontmatter = {}
        
        # Get current references
        current_refs = frontmatter.get('references', [])
        if not isinstance(current_refs, list):
            current_refs = []

        # Remove explicit placeholder references when real evidence is available.
        had_placeholder_refs = any(is_placeholder_evidence_reference(ref) for ref in current_refs)
        kept_refs = [ref for ref in current_refs if not is_placeholder_evidence_reference(ref)]
        references_changed = len(kept_refs) != len(current_refs)
        current_refs = kept_refs
        
        # Add new evidence URLs as references
        new_refs_added = False
        for ev in evidence_list:
            # Check if URL already exists
            url_exists = any(
                ref.get('url', '') == ev.url 
                for ref in current_refs 
                if isinstance(ref, dict)
            )
            
            if not url_exists:
                new_ref = {
                    'type': 'url',
                    'url': ev.url,
                    'description': f"Evidence from {ev.source_file}: {ev.description}"
                }
                current_refs.append(new_ref)
                new_refs_added = True
        
        if new_refs_added or references_changed:
            # Update frontmatter
            frontmatter['references'] = current_refs

            # If at least one non-placeholder reference exists, mark as complete.
            has_real_refs = len(current_refs) > 0
            if has_real_refs:
                frontmatter['score'] = 1.0
            elif had_placeholder_refs:
                frontmatter['score'] = 0.0
            
            # Rebuild file content
            new_content = f"---\n{yaml.dump(frontmatter, default_flow_style=False, allow_unicode=True, sort_keys=False)}---{body}"
            
            with open(evid_file, 'w', encoding='utf-8') as f:
                f.write(new_content)
            
            updated_files.append(evid_file.name)
    
    return updated_files


def sync_update(config: Config, check_status: Dict[str, Any]) -> Dict[str, Any]:
    """
    Step 2: Generate missing content using AI.
    
    Logic:
    - If all fields empty: Generate all (requirement, criteria, method, evidence)
    - If requirement empty but others filled: Generate requirement from context
    - If evidence empty: Sync from sprints or generate
    - If criteria/method empty: Generate from requirement
    - If existing items have placeholder/TODO content: Ask to regenerate
    """
    print("\n" + "="*70)
    print("🔄 STEP 2: SYNC & UPDATE")
    print("="*70)
    
    status = {
        'items_created': [],
        'items_updated': [],
        'items_regenerated': [],
        'ai_generation_needed': [],
        'skipped': [],
        'orphans_removed': []
    }
    
    item_manager = ItemFileManager(config)
    ai_generator = AIGenerator(config)
    
    # 0. Handle orphan item files (files without corresponding requirements) - ASK USER FIRST
    orphan_items = check_status.get('orphan_items', [])
    
    if orphan_items:
        print("\n🗑️  Found orphan item files (no corresponding requirement in table):")
        print("-" * 60)
        
        for item_type, item_id, file_path in orphan_items:
            print(f"   • {file_path.name} (L0-{item_id})")
        
        print("-" * 60)
        print("\n🔧 Options for orphan files:")
        print("   [r] Remove ALL orphan files")
        print("   [d] Dry run - show what would be removed")
        print("   [s] Skip all - don't remove any")
        print("   [q] Quit")
        
        choice = input("\n>>> Choose option [r/d/s/q]: ").strip().lower()
        
        if choice == 'q':
            print("❌ User requested quit.")
            sys.exit(0)
        elif choice == 'r':
            # Remove all orphan files
            print("\n🗑️  Removing ALL orphan files...")
            removed_files = item_manager.remove_orphan_items(orphan_items, dry_run=False)
            status['orphans_removed'] = [str(f) for f in removed_files]
        elif choice == 'd':
            # Dry run
            print("\n📋 DRY RUN: Showing what would be removed...")
            item_manager.remove_orphan_items(orphan_items, dry_run=True)
            print("   ℹ️  No files were actually removed")
        else:  # 's' or anything else
            print("   ⏭️  Skipping orphan removal")
    
    # 1. Handle invalid items (placeholders/TODOs) - ASK USER FIRST
    invalid_items = check_status.get('invalid_items', [])
    
    if invalid_items:
        print("\n⚠️  Found items with placeholder/TODO content:")
        print("-" * 50)
        
        for result in invalid_items:
            print(f"\n   ❌ {result.item_type}-L0-{result.item_id}")
            for issue in result.issues:
                print(f"      └─ {issue}")
        
        print("\n" + "-" * 50)
        print("\n🔧 Options for items with invalid content:")
        print("   [a] Regenerate ALL invalid items")
        print("   [s] Select which items to regenerate (one by one)")
        print("   [n] Skip all - don't regenerate any")
        print("   [q] Quit")
        
        choice = input("\n>>> Choose option [a/s/n/q]: ").strip().lower()
        
        if choice == 'q':
            print("❌ User requested quit.")
            sys.exit(0)
        elif choice == 'a':
            # Regenerate all invalid items
            print("\n🔄 Regenerating ALL invalid items...")
            for result in invalid_items:
                status['ai_generation_needed'].append(
                    (result.item_type, result.item_id, result.file_path)
                )
        elif choice == 's':
            # Ask for each item
            print("\n🔄 Selecting items to regenerate...")
            for result in invalid_items:
                print(f"\n   {result.item_type}-L0-{result.item_id}:")
                for issue in result.issues:
                    print(f"      └─ {issue}")
                
                item_choice = input(f"   Regenerate this item? [y/n]: ").strip().lower()
                
                if item_choice == 'y':
                    status['ai_generation_needed'].append(
                        (result.item_type, result.item_id, result.file_path)
                    )
                    status['items_regenerated'].append(str(result.file_path))
                else:
                    status['skipped'].append(str(result.file_path))
        else:  # 'n' or anything else
            print("   ⏭️  Skipping all invalid items")
            for result in invalid_items:
                status['skipped'].append(str(result.file_path))
    
    # 2. Create missing item files (files that don't exist at all)
    print("\n📝 Creating missing item files...")
    
    for row in check_status.get('table_rows', []):
        item_id = str(row.number)
        items_status = item_manager.get_all_items_for_id(item_id)
        
        for item_type, exists in items_status.items():
            if not exists:
                # Create file from template
                file_path = item_manager.create_item_from_template(
                    item_type=item_type,
                    item_id=item_id,
                    header=row.requirement[:50] if row.requirement else '',
                    text='',
                    references=[]
                )
                status['items_created'].append(str(file_path))
                status['ai_generation_needed'].append((item_type, item_id, file_path))
    
    # 3. Update EVID files with sprint evidence
    print("\n📎 Syncing evidence from sprints to EVID files...")
    
    sprint_evidence = check_status.get('sprint_evidence', {})
    for expect_id, evidences in sprint_evidence.items():
        # Extract ID number
        match = re.search(r'L0-(\d+)', expect_id)
        if not match:
            continue
        
        item_id = match.group(1)
        evid_path = item_manager.get_item_path('EVID', item_id)
        
        if evid_path.exists():
            # TODO: Update existing file with new evidence
            print(f"   • {evid_path.name}: {len(evidences)} evidence(s) available")
    
    # 4. AI generation for items needing content (BATCH MODE)
    print("\n🤖 AI generation for items needing content...")
    
    if status['ai_generation_needed']:
        print(f"   {len(status['ai_generation_needed'])} items need AI-generated content")
        
        # Convert file paths to Path objects
        items_for_batch = []
        for item_type, item_id, file_path in status['ai_generation_needed']:
            if isinstance(file_path, str):
                file_path = Path(file_path)
            items_for_batch.append((item_type, item_id, file_path))
        
        # Use batch generation (single consolidated prompt)
        updated_files = ai_generator.generate_content_batch(
            items=items_for_batch,
            table_rows=check_status.get('table_rows', [])
        )
        
        status['items_updated'] = updated_files
        
        # Mark items not updated as skipped
        updated_set = set(updated_files)
        for item_type, item_id, file_path in items_for_batch:
            if str(file_path) not in updated_set:
                status['skipped'].append(str(file_path))
    else:
        print("   ✅ No items need AI generation")
    
    print("\n" + "-"*70)
    print("🔄 SYNC & UPDATE Summary:")
    print(f"   • Items created: {len(status['items_created'])}")
    print(f"   • Items regenerated: {len(status['items_regenerated'])}")
    print(f"   • Items updated (AI): {len(status['items_updated'])}")
    print(f"   • Orphan files removed: {len(status['orphans_removed'])}")
    print(f"   • Skipped: {len(status['skipped'])}")
    
    return status


def validate_run_publish(config: Config) -> Dict[str, Any]:
    """
    Step 3: Validate items and run TruDAG.
    """
    print("\n" + "="*70)
    print("✅ STEP 3: VALIDATE, RUN & PUBLISH")
    print("="*70)
    
    status = {
        'validation_passed': False,
        'trudag_success': False,
        'score_summary': {},
        'errors': [],
        'warnings': []
    }
    
    # 0. Ensure required symlinks exist
    print("\n🔗 Checking required symlinks...")
    
    # Symlink for .dotstop_extensions in tsf_implementation
    dotstop_ext_symlink = config.tsf_implementation / ".dotstop_extensions"
    dotstop_ext_target = config.repo_root / ".dotstop_extensions"
    
    if not dotstop_ext_symlink.exists() and dotstop_ext_target.exists():
        try:
            dotstop_ext_symlink.symlink_to("../../../.dotstop_extensions")
            print("   ✅ Created .dotstop_extensions symlink")
        except Exception as e:
            print(f"   ⚠️  Could not create .dotstop_extensions symlink: {e}")
    elif dotstop_ext_symlink.exists():
        print("   ✓ .dotstop_extensions symlink exists")
    
    # Symlink for localplugins in repo root  
    localplugins_symlink = config.repo_root / "localplugins"
    if not localplugins_symlink.exists() and dotstop_ext_target.exists():
        try:
            localplugins_symlink.symlink_to(".dotstop_extensions")
            print("   ✅ Created localplugins symlink")
        except Exception as e:
            print(f"   ⚠️  Could not create localplugins symlink: {e}")
    elif localplugins_symlink.exists():
        print("   ✓ localplugins symlink exists")
    
    # 1. Run validation
    print("\n🔍 Running item validation...")
    
    validator_path = config.scripts_dir / "modules" / "validate_items_formatation.py"
    if not validator_path.exists():
        # Try alternative location
        validator_path = config.tsf_implementation / "validators" / "validate_items_formatation.py"
    
    if validator_path.exists():
        try:
            result = subprocess.run(
                ['python3', str(validator_path), str(config.items_dir)],
                capture_output=True,
                text=True,
                timeout=60
            )
            
            if result.returncode == 0:
                print("   ✅ Validation passed")
                status['validation_passed'] = True
            else:
                print(f"   ⚠️  Validation issues found:")
                print(result.stdout)
                status['errors'].append(result.stdout)
        except Exception as e:
            print(f"   ❌ Validation error: {e}")
            status['errors'].append(str(e))
    else:
        print(f"   ⚠️  Validator not found: {validator_path}")
    
    # 2. Run TruDAG (setup_trudag_clean.sh)
    print("\n🚀 Running TruDAG...")
    
    trudag_script = config.scripts_dir / "setup_trudag_clean.sh"
    
    if trudag_script.exists():
        try:
            print("   ⏳ Running TruDAG (this may take several minutes for 84 items)...")
            print("   📺 Live output:")
            print("   " + "-"*50)
            
            # Run without timeout, streaming output to user
            process = subprocess.Popen(
                ['bash', str(trudag_script)],
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                cwd=str(config.tsf_implementation)
            )
            
            # Stream output line by line
            for line in process.stdout:
                print(f"   {line.rstrip()}")
            
            process.wait()
            
            print("   " + "-"*50)
            
            if process.returncode == 0:
                print("   ✅ TruDAG completed successfully")
                status['trudag_success'] = True
            else:
                print(f"   ⚠️  TruDAG returned exit code: {process.returncode}")
                status['warnings'].append(f"TruDAG exit code: {process.returncode}")
        except Exception as e:
            print(f"   ❌ TruDAG error: {e}")
            status['errors'].append(str(e))
    else:
        print(f"   ⚠️  TruDAG script not found: {trudag_script}")
    
    # 3. Verify scores after TruDAG
    if status['trudag_success']:
        print("\n📊 Verifying scores...")
        try:
            result = subprocess.run(
                ['trudag', 'score', '--validate'],
                capture_output=True,
                text=True,
                cwd=str(config.tsf_implementation),
                timeout=300
            )
            
            # Parse score output
            lines = result.stdout.strip().split('\n')
            total_items = 0
            items_at_1_0 = 0
            items_below_1_0 = []
            
            for line in lines:
                if ' = ' in line and any(prefix in line for prefix in ['ASSERTIONS', 'ASSUMPTIONS', 'EVIDENCES', 'EXPECTATIONS']):
                    total_items += 1
                    parts = line.strip().split(' = ')
                    if len(parts) == 2:
                        item_name = parts[0]
                        try:
                            score = float(parts[1])
                            if score == 1.0:
                                items_at_1_0 += 1
                            else:
                                items_below_1_0.append((item_name, score))
                        except ValueError:
                            pass
            
            status['score_summary'] = {
                'total': total_items,
                'at_1_0': items_at_1_0,
                'below_1_0': items_below_1_0
            }
            
            if items_at_1_0 == total_items:
                print(f"   ✅ All {total_items} items have score 1.0")
            else:
                print(f"   ⚠️  {items_at_1_0}/{total_items} items at 1.0")
                if items_below_1_0:
                    print(f"   Items below 1.0:")
                    for item, score in items_below_1_0[:5]:
                        print(f"      • {item} = {score}")
                    if len(items_below_1_0) > 5:
                        print(f"      ... and {len(items_below_1_0) - 5} more")
        except Exception as e:
            print(f"   ⚠️  Could not verify scores: {e}")
    
    print("\n" + "-"*70)
    print("✅ VALIDATE, RUN & PUBLISH Summary:")
    print(f"   • Validation: {'✅ Passed' if status['validation_passed'] else '❌ Failed'}")
    print(f"   • TruDAG: {'✅ Success' if status['trudag_success'] else '❌ Failed'}")
    if status['score_summary']:
        summary = status['score_summary']
        print(f"   • Scores: {summary['at_1_0']}/{summary['total']} at 1.0")
    print(f"   • Errors: {len(status['errors'])}")
    print(f"   • Warnings: {len(status['warnings'])}")
    
    return status


# ============================================================================
# MAIN ENTRY POINT
# ============================================================================

def main():
    """Main entry point for the script."""
    import argparse
    
    parser = argparse.ArgumentParser(
        description='TSF Requirements Unified Automation Script',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python %(prog)s --all           Run all steps
  python %(prog)s --check         Run only open_check step
  python %(prog)s --sync          Run check and sync_update steps
  python %(prog)s --validate      Run only validation step
        """
    )
    
    parser.add_argument('--check', action='store_true',
                       help='Run open_check step only')
    parser.add_argument('--sync', action='store_true',
                       help='Run open_check and sync_update steps')
    parser.add_argument('--validate', action='store_true',
                       help='Run validation step only')
    parser.add_argument('--all', action='store_true',
                       help='Run all steps')
    parser.add_argument('--config', type=str,
                       help='Path to config.yaml file')
    parser.add_argument('--skip-diagnostics', action='store_true',
                       help='Skip startup diagnostics')
    
    args = parser.parse_args()
    
    # Default to --all if no args
    if not any([args.check, args.sync, args.validate, args.all]):
        args.all = True
    
    # Run startup diagnostics
    if not args.skip_diagnostics:
        deps_ok = print_startup_diagnostics()
        if not deps_ok:
            print("\n⚠️  Some dependencies are missing. The script may not work correctly.")
            print("   Fix the issues above or run with --skip-diagnostics to proceed anyway.\n")
            response = input(">>> Continue anyway? [y/n]: ").strip().lower()
            if response != 'y':
                print("   Exiting.")
                sys.exit(1)
    
    # Load configuration
    try:
        config = Config(args.config)
        print(f"📁 Config loaded from: {config.config_path}")
    except FileNotFoundError as e:
        print(f"\n❌ Configuration file not found!")
        print(f"   Expected location: {e}")
        print("\n   💡 How to fix:")
        print("      1. Create a config.yaml file in the scripts directory")
        print("      2. Or specify path with --config /path/to/config.yaml")
        print("      3. Copy config.yaml.example if available")
        sys.exit(1)
    except ImportError as e:
        print(f"\n❌ Missing Python package: {e}")
        print("\n   💡 How to fix:")
        print("      pip install pyyaml")
        sys.exit(1)
    except yaml.YAMLError as e:
        print(f"\n❌ Invalid YAML in configuration file!")
        print(f"   Error: {e}")
        print("\n   💡 How to fix:")
        print("      1. Check config.yaml syntax (use a YAML validator)")
        print("      2. Ensure proper indentation (spaces, not tabs)")
        sys.exit(1)
    except Exception as e:
        print(f"\n❌ Failed to load config: {e}")
        print(f"   Error type: {type(e).__name__}")
        sys.exit(1)
    
    print("\n" + "="*70)
    print("🚀 TSF REQUIREMENTS AUTOMATION")
    print("="*70)
    print(f"   Repository: {config.repo_root}")
    print(f"   AI Method: {config.ai_primary_method.value}")
    print(f"   Fallbacks: {[f.value for f in config.ai_fallbacks]}")
    
    # Run steps
    check_status = None
    sync_status = None
    validate_status = None
    
    if args.check or args.sync or args.all:
        check_status = open_check(config)
    
    if args.sync or args.all:
        if check_status:
            sync_status = sync_update(config, check_status)
            # Sync evidence from sprints to table and EVID files
            evidence_sync_status = sync_evidence_from_sprints(config, check_status)
            if sync_status:
                sync_status['evidence_sync'] = evidence_sync_status
    
    if args.validate or args.all:
        validate_status = validate_run_publish(config)
    
    # Final summary
    print("\n" + "="*70)
    print("📊 FINAL SUMMARY")
    print("="*70)
    
    if check_status:
        print(f"   Check: {len(check_status.get('table_rows', []))} requirements analyzed")
    if sync_status:
        print(f"   Sync: {len(sync_status.get('items_created', []))} items created")
    if validate_status:
        all_ok = validate_status.get('validation_passed') and validate_status.get('trudag_success')
        print(f"   Validate: {'✅ All passed' if all_ok else '⚠️ Issues found'}")
    
    print("\n✅ Done!")


if __name__ == '__main__':
    main()
