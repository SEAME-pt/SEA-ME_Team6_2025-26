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
import yaml
import subprocess
from pathlib import Path
from typing import Dict, List, Optional, Tuple, Any
from dataclasses import dataclass
from enum import Enum


# ============================================================================
# CONFIGURATION
# ============================================================================

class AIMethod(Enum):
    """AI generation methods available."""
    MANUAL = "manual"       # Option G: VSCode/Claude semi-automated
    COPILOT_CLI = "copilot_cli"  # Option C: gh copilot CLI


@dataclass
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
        # Get base paths
        self.repo_root = Path(self._config['paths']['repo_root'])
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
    - EXPECT-L0-X - "text": followed by indented links
    - ![image](url) format
    - [text](url) format
    """
    
    def __init__(self, config: Config):
        self.config = config
        # Compile regex patterns
        self.patterns = {
            'expect_header': re.compile(
                r'EXPECT-L0-(\d+)\s*[-–]\s*["\']?([^"\'\n:]+)["\']?:?',
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
        
        for i, line in enumerate(lines):
            # Check for EXPECT-L0-X header
            expect_match = self.patterns['expect_header'].search(line)
            if expect_match:
                current_expect_id = f"EXPECT-L0-{expect_match.group(1)}"
                current_expect_desc = expect_match.group(2).strip()
                continue
            
            # If we have a current EXPECT, look for links in following lines
            if current_expect_id:
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
                
                # Reset if we hit an empty line or new section
                if not line.strip() or line.startswith('#') or line.startswith('**'):
                    # Check if next line also starts with EXPECT to continue
                    if i + 1 < len(lines):
                        next_line = lines[i + 1]
                        if not self.patterns['expect_header'].search(next_line):
                            if not line.strip():
                                current_expect_id = None
                                current_expect_desc = None
        
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
            
            # Detect table header
            if '| ID |' in line or '|ID|' in line:
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
        parts = [p for p in parts if p or p == '']
        
        if len(parts) < 5:
            return None
        
        # Extract L0-X ID
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
            print(f"\n📂 Opening file in VSCode: {file_path}")
            subprocess.run(['code', str(file_path)], check=False)
        
        # Show suggested prompt
        if settings.get('show_prompt_suggestion', True):
            prompt = self._build_prompt(item_type, item_id, requirement, acceptance_criteria)
            print(f"\n📝 SUGGESTED PROMPT for Claude/Copilot Chat:")
            print(f"{'-'*60}")
            print(prompt)
            print(f"{'-'*60}")
        
        # Wait for user confirmation
        if settings.get('wait_for_user_confirmation', True):
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
        
        # Default prompt
        return f"""Generate content for TSF item {item_type}-L0-{item_id}:

**Requirement:** {requirement}
**Acceptance Criteria:** {acceptance_criteria}

Please fill the following fields in the YAML frontmatter:
- header: A concise title for this item
- text: Detailed description/content
- references: Relevant file or URL references

Use the existing file structure, only fill empty fields."""
    
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
        """Create an item file from template."""
        file_path = self.get_item_path(item_type, item_id)
        
        # Ensure directory exists
        file_path.parent.mkdir(parents=True, exist_ok=True)
        
        # Build YAML frontmatter
        content = f"""---
id: {item_type.upper()}-L0-{item_id}
header: "{header}"
text: |
  {text if text else 'TODO: Add content'}
level: "1.1"
normative: true
references:
"""
        # Add references
        if references:
            for ref in references:
                content += f'  - "{ref}"\n'
        else:
            content += '  - "TODO: Add references"\n'
        
        content += "---\n"
        
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
    
    # 5. NEW: Validate content of existing items (detect placeholders/TODOs)
    print("\n🔍 Validating content of existing items...")
    content_validator = ContentValidator(config)
    valid_count, invalid_count = content_validator.print_validation_report()
    
    # Get items needing regeneration
    items_needing_regen = content_validator.get_items_needing_regeneration()
    status['invalid_items'] = items_needing_regen
    
    # 6. Identify sync needs (sprint evidence → table → EVID files)
    print("\n🔄 Identifying sync needs...")
    for row in rows:
        expect_id = f"EXPECT-L0-{row.number}"
        
        # Check if sprint has evidence that table doesn't
        if expect_id in sprint_evidence:
            sprint_urls = set(e.url for e in sprint_evidence[expect_id])
            table_evidence = row.evidence.strip()
            
            # Simple check - could be more sophisticated
            if not table_evidence or table_evidence == 'TODO':
                status['sync_needed'].append((row.id, 'sprint_to_table'))
                print(f"   • {row.id}: needs sync from sprint to table")
    
    print("\n" + "-"*70)
    print("📋 OPEN & CHECK Summary:")
    print(f"   • Total requirements: {len(rows)}")
    print(f"   • Incomplete table rows: {len(incomplete)}")
    print(f"   • Missing item files: {len(status['missing_items'])}")
    print(f"   • Items with invalid content: {len(status['invalid_items'])}")
    print(f"   • Sync needed: {len(status['sync_needed'])}")
    
    return status


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
        'skipped': []
    }
    
    item_manager = ItemFileManager(config)
    ai_generator = AIGenerator(config)
    
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
    
    # 4. AI generation for items needing content
    print("\n🤖 AI generation for items needing content...")
    
    if status['ai_generation_needed']:
        print(f"   {len(status['ai_generation_needed'])} items need AI-generated content")
        
        for item_type, item_id, file_path in status['ai_generation_needed']:
            # Ensure file_path is a Path object
            if isinstance(file_path, str):
                file_path = Path(file_path)
            
            # Find corresponding table row
            row = next((r for r in check_status.get('table_rows', []) 
                       if str(r.number) == str(item_id)), None)
            
            if row:
                success = ai_generator.generate_content(
                    item_type=item_type,
                    item_id=str(item_id),
                    requirement=row.requirement,
                    acceptance_criteria=row.acceptance_criteria,
                    file_path=file_path
                )
                
                if success:
                    status['items_updated'].append(str(file_path))
                else:
                    status['skipped'].append(str(file_path))
    else:
        print("   ✅ No items need AI generation")
    
    print("\n" + "-"*70)
    print("🔄 SYNC & UPDATE Summary:")
    print(f"   • Items created: {len(status['items_created'])}")
    print(f"   • Items regenerated: {len(status['items_regenerated'])}")
    print(f"   • Items updated (AI): {len(status['items_updated'])}")
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
        'errors': [],
        'warnings': []
    }
    
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
            result = subprocess.run(
                ['bash', str(trudag_script)],
                capture_output=True,
                text=True,
                timeout=120,
                cwd=str(config.tsf_implementation)
            )
            
            if result.returncode == 0:
                print("   ✅ TruDAG completed successfully")
                status['trudag_success'] = True
            else:
                print(f"   ⚠️  TruDAG returned warnings/errors:")
                if result.stderr:
                    print(result.stderr)
                status['warnings'].append(result.stderr)
        except Exception as e:
            print(f"   ❌ TruDAG error: {e}")
            status['errors'].append(str(e))
    else:
        print(f"   ⚠️  TruDAG script not found: {trudag_script}")
    
    print("\n" + "-"*70)
    print("✅ VALIDATE, RUN & PUBLISH Summary:")
    print(f"   • Validation: {'✅ Passed' if status['validation_passed'] else '❌ Failed'}")
    print(f"   • TruDAG: {'✅ Success' if status['trudag_success'] else '❌ Failed'}")
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
    
    args = parser.parse_args()
    
    # Default to --all if no args
    if not any([args.check, args.sync, args.validate, args.all]):
        args.all = True
    
    # Load configuration
    try:
        config = Config(args.config)
        print(f"📁 Config loaded from: {config.config_path}")
    except Exception as e:
        print(f"❌ Failed to load config: {e}")
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
