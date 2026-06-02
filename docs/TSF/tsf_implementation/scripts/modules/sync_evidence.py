#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
sync_evidence.py - Synchronize Evidence from Sprint Files to EVID Items

This module extracts evidence references from sprint outcome sections and updates
the corresponding EVID-L0-X.md files with proper references.

Process:
1. Scan docs/sprints/ for sprint*.md files
2. Parse "Outcomes" and "Demos" sections
3. Extract EXPECT-L0-X mappings with their evidence links
4. Update items/evidences/EVID-L0-X.md with extracted references
5. Validate file existence and format

Usage:
    python sync_evidence.py [--dry-run] [--verbose]
"""

import re
import sys
import os
import glob
import io

class EvidenceItem:
    """Represents an evidence item extracted from sprint files."""
    def __init__(self, expect_id, description, links):
        self.expect_id = expect_id  # e.g., "EXPECT-L0-1"
        self.description = description
        self.links = links  # List of file paths or URLs

class EvidenceSync:
    """Handles synchronization of evidence from sprints to EVID files."""

    def __init__(self, repo_root):
        self.repo_root = repo_root
        self.sprints_dir = os.path.join(repo_root, "docs", "sprints")
        self.items_dir = os.path.join(repo_root, "docs", "TSF", "tsf_implementation", "items")
        self.evidences_dir = os.path.join(self.items_dir, "evidences")
        self.expectations_dir = os.path.join(self.items_dir, "expectations")
        self.assertions_dir = os.path.join(self.items_dir, "assertions")
        self.assumptions_dir = os.path.join(self.items_dir, "assumptions")
        self.requirements_table = os.path.join(repo_root, "docs", "TSF", "requirements", "tsf-requirements-table.md")

        # Patterns for parsing sprint files
        self.expect_pattern = re.compile(r'EXPECT-L0-(\d+).*?-\s*"([^"]+)"')
        self.link_pattern = re.compile(r'-\s*\[([^\]]+)\]\(([^)]+)\)')

    def extract_evidence_from_sprint(self, sprint_file):
        """Extract evidence items from a single sprint file."""
        items = []
        content = io.open(sprint_file, 'r', encoding='utf-8').read()

        # Find all EXPECT-L0-X mentions with their context
        lines = content.split('\n')
        current_expect = None
        current_links = []

        for i, line in enumerate(lines):
            # Check for EXPECT-L0-X pattern
            expect_match = self.expect_pattern.search(line)
            if expect_match:
                # Save previous item if exists
                if current_expect and current_links:
                    items.append(EvidenceItem(current_expect, "", current_links))

                # Start new item
                expect_num = expect_match.group(1)
                current_expect = "EXPECT-L0-{}".format(expect_num)
                description = expect_match.group(2)
                current_links = []

                # Look ahead for links in next few lines
                for j in range(i+1, min(i+10, len(lines))):
                    link_match = self.link_pattern.search(lines[j])
                    if link_match:
                        link_text = link_match.group(1)
                        link_url = link_match.group(2)
                        # Convert relative links to absolute paths if needed
                        if not link_url.startswith('http'):
                            # Assume relative to repo root
                            current_links.append("docs/{}".format(link_url))
                        else:
                            current_links.append(link_url)
                    elif lines[j].strip() == "" and current_links:
                        break  # Empty line after links, stop looking

        # Don't forget the last item
        if current_expect and current_links:
            items.append(EvidenceItem(current_expect, "", current_links))

        return items

    def collect_all_evidence(self):
        """Collect evidence from all sprint files."""
        evidence_map = {}

        if not os.path.exists(self.sprints_dir):
            print("❌ Sprints directory not found: {}".format(self.sprints_dir))
            return evidence_map

        sprint_files = glob.glob(os.path.join(self.sprints_dir, "sprint*.md"))
        print("📂 Found {} sprint files".format(len(sprint_files)))

        for sprint_file in sprint_files:
            print("  📖 Processing {}...".format(os.path.basename(sprint_file)))
            items = self.extract_evidence_from_sprint(sprint_file)

            for item in items:
                if item.expect_id not in evidence_map:
                    evidence_map[item.expect_id] = []
                evidence_map[item.expect_id].extend(item.links)

        # Remove duplicates while preserving order
        for expect_id in evidence_map:
            evidence_map[expect_id] = list(dict.fromkeys(evidence_map[expect_id]))

        return evidence_map

    def fix_tsf_item_references(self, item_type, item_id, dry_run=False):
        """Fix references and level for a TSF item (EXPECT, ASSERT, ASSUMP, EVID)."""
        if item_type == "EVID":
            item_dir = self.evidences_dir
            item_file = os.path.join(item_dir, "{}.md".format(item_id))
        elif item_type == "EXPECT":
            item_dir = self.expectations_dir
            item_file = os.path.join(item_dir, "{}.md".format(item_id))
        elif item_type == "ASSERT":
            item_dir = self.assertions_dir
            item_file = os.path.join(item_dir, "{}.md".format(item_id))
        elif item_type == "ASSUMP":
            item_dir = self.assumptions_dir
            item_file = os.path.join(item_dir, "{}.md".format(item_id))
        else:
            return False

        if not os.path.exists(item_file):
            return False

        content = io.open(item_file, 'r', encoding='utf-8').read()
        lines = content.split('\n')
        
        # Find frontmatter boundaries
        if len(lines) < 2 or lines[0].strip() != '---':
            return False
        
        frontmatter_end = -1
        for i in range(1, len(lines)):
            if lines[i].strip() == '---':
                frontmatter_end = i
                break
        
        if frontmatter_end == -1:
            return False

        # Extract frontmatter
        frontmatter_lines = lines[1:frontmatter_end]
        
        # Parse and fix frontmatter
        new_frontmatter_lines = []
        in_references = False
        references = []
        level_fixed = False
        
        for line in frontmatter_lines:
            stripped = line.strip()
            
            # Fix level
            if stripped.startswith('level:'):
                # Extract number from item_id (e.g., EXPECT-L0-19 -> 19)
                match = re.search(r'L0-(\d+)', item_id)
                if match:
                    correct_level = "'1.{}'".format(match.group(1))
                    if stripped != "level: {}".format(correct_level):
                        new_frontmatter_lines.append("level: {}".format(correct_level))
                        level_fixed = True
                    else:
                        new_frontmatter_lines.append(line)
                else:
                    new_frontmatter_lines.append(line)
                continue
            
            # Handle references section
            if stripped == 'references:':
                in_references = True
                new_frontmatter_lines.append(line)
                # Generate correct references based on item type
                correct_refs = self.generate_correct_references(item_type, item_id)
                for ref in correct_refs:
                    if 'id' in ref:
                        new_frontmatter_lines.append('  - id: {}'.format(ref['id']))
                        new_frontmatter_lines.append('    type: file')
                        new_frontmatter_lines.append('    path: {}'.format(ref['path']))
                    else:
                        new_frontmatter_lines.append('  - type: {}'.format(ref['type']))
                        new_frontmatter_lines.append('    path: {}'.format(ref['path']))
                continue
            elif in_references and (stripped.startswith('-') or stripped.startswith('  ') or stripped.startswith('\t')):
                # Skip old reference lines
                continue
            elif in_references and stripped and not stripped.startswith(' ') and stripped.endswith(':') and stripped not in ['references:', '---']:
                # End of references section
                in_references = False
                new_frontmatter_lines.append(line)
            elif not in_references:
                new_frontmatter_lines.append(line)

        # Rebuild content
        new_frontmatter = '\n'.join(new_frontmatter_lines)
        new_content = '---\n' + new_frontmatter + '\n---' + content[content.find('\n---', content.find('---')+3)+4:]

        if dry_run:
            print("📝 Would fix {}:".format(item_file))
            if level_fixed:
                print("  - Fixed level")
            print("  - Updated references")
            return True

        # Write back
        with io.open(item_file, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print("✅ Fixed {} (level: {}, references: {})".format(item_file, "fixed" if level_fixed else "ok", "updated"))
        return True

    def generate_correct_references(self, item_type, item_id):
        """Generate correct references for a TSF item."""
        # Extract number from item_id (e.g., EXPECT-L0-19 -> 19)
        match = re.search(r'L0-(\d+)', item_id)
        if not match:
            return []
        
        num = match.group(1)
        references = []
        
        if item_type == "EXPECT":
            # EXPECT references ASSERT (and ASSUMP if ASSERT doesn't exist)
            assert_file = os.path.join(self.assertions_dir, "ASSERT-L0-{}.md".format(num))
            if os.path.exists(assert_file):
                references.append({
                    'id': 'ASSERT-L0-{}'.format(num),
                    'path': '../assertions/ASSERT-L0-{}.md'.format(num)
                })
            else:
                # Fallback to ASSUMP if ASSERT doesn't exist
                assump_file = os.path.join(self.assumptions_dir, "ASSUMP-L0-{}.md".format(num))
                if os.path.exists(assump_file):
                    references.append({
                        'id': 'ASSUMP-L0-{}'.format(num),
                        'path': '../assumptions/ASSUMP-L0-{}.md'.format(num)
                    })
        
        elif item_type == "ASSERT":
            # ASSERT references EXPECT and EVID
            references.append({
                'id': 'EXPECT-L0-{}'.format(num),
                'path': '../expectations/EXPECT-L0-{}.md'.format(num)
            })
            evid_file = os.path.join(self.evidences_dir, "EVID-L0-{}.md".format(num))
            if os.path.exists(evid_file):
                references.append({
                    'id': 'EVID-L0-{}'.format(num),
                    'path': '../evidences/EVID-L0-{}.md'.format(num)
                })
        
        elif item_type == "ASSUMP":
            # ASSUMP references EXPECT
            references.append({
                'id': 'EXPECT-L0-{}'.format(num),
                'path': '../expectations/EXPECT-L0-{}.md'.format(num)
            })
        
        # For EVID, references are handled separately (evidence links)
        
        return references

    def check_requirements_sync(self):
        """Check synchronization between sprints, requirements table, and items."""
        print("🔍 Checking requirements synchronization...")
        
        # Get requirements from table
        table_requirements = self.extract_requirements_from_table()
        
        # Get requirements from sprints
        sprint_requirements = self.extract_requirements_from_sprints()
        
        # Get existing items
        existing_items = self.get_existing_items()
        
        # Check synchronization
        issues = []
        
        # Check if all table requirements exist in items
        for req in table_requirements:
            if req not in existing_items:
                issues.append("❌ Requirement {} exists in table but not in items".format(req))
        
        # Check if all sprint requirements exist in table
        for req in sprint_requirements:
            if req not in table_requirements:
                issues.append("❌ Requirement {} exists in sprints but not in table".format(req))
        
        # Check if all items exist in table
        for item in existing_items:
            if item not in table_requirements:
                issues.append("❌ Item {} exists in items but not in table".format(item))
        
        if issues:
            print("⚠️  Synchronization issues found:")
            for issue in issues:
                print("  {}".format(issue))
            return False
        else:
            print("✅ All requirements are synchronized")
            return True

    def extract_requirements_from_table(self):
        """Extract requirements from tsf-requirements-table.md."""
        requirements = set()
        if not os.path.exists(self.requirements_table):
            print("⚠️  Requirements table not found: {}".format(self.requirements_table))
            return requirements
        
        content = io.open(self.requirements_table, 'r', encoding='utf-8').read()
        lines = content.split('\n')
        
        for line in lines:
            # Look for EXPECT-L0-X patterns
            matches = re.findall(r'EXPECT-L0-\d+', line)
            for match in matches:
                requirements.add(match)
        
        return requirements

    def extract_requirements_from_sprints(self):
        """Extract requirements from sprint files."""
        requirements = set()
        
        if not os.path.exists(self.sprints_dir):
            return requirements

        sprint_files = glob.glob(os.path.join(self.sprints_dir, "sprint*.md"))
        
        for sprint_file in sprint_files:
            content = io.open(sprint_file, 'r', encoding='utf-8').read()
            # Look for EXPECT-L0-X patterns
            matches = re.findall(r'EXPECT-L0-\d+', content)
            for match in matches:
                requirements.add(match)
        
        return requirements

    def get_existing_items(self):
        """Get all existing TSF items."""
        items = set()
        
        # Check expectations
        if os.path.exists(self.expectations_dir):
            for file in glob.glob(os.path.join(self.expectations_dir, "EXPECT-L0-*.md")):
                basename = os.path.basename(file)
                items.add(basename.replace('.md', ''))
        
        # Check assertions
        if os.path.exists(self.assertions_dir):
            for file in glob.glob(os.path.join(self.assertions_dir, "ASSERT-L0-*.md")):
                basename = os.path.basename(file)
                items.add(basename.replace('.md', ''))
        
        # Check assumptions
        if os.path.exists(self.assumptions_dir):
            for file in glob.glob(os.path.join(self.assumptions_dir, "ASSUMP-L0-*.md")):
                basename = os.path.basename(file)
                items.add(basename.replace('.md', ''))
        
        # Check evidences
        if os.path.exists(self.evidences_dir):
            for file in glob.glob(os.path.join(self.evidences_dir, "EVID-L0-*.md")):
                basename = os.path.basename(file)
                items.add(basename.replace('.md', ''))
        
        return items

    def update_evid_file(self, expect_id, links, dry_run=False):
        """Update the corresponding EVID file with new references."""
        # Convert EXPECT-L0-X to EVID-L0-X
        evid_id = expect_id.replace("EXPECT", "EVID")
        
        # First fix references and level using the generic method
        self.fix_tsf_item_references("EVID", evid_id, dry_run)
        
        evid_file = os.path.join(self.evidences_dir, "{}.md".format(evid_id))

        if not os.path.exists(evid_file):
            print("⚠️  EVID file not found: {}".format(evid_file))
            return False

        content = io.open(evid_file, 'r', encoding='utf-8').read()

        # Find frontmatter boundaries
        lines = content.split('\n')
        if len(lines) < 2 or lines[0].strip() != '---':
            print("⚠️  Invalid frontmatter in {}".format(evid_file))
            return False
        
        frontmatter_end = -1
        for i in range(1, len(lines)):
            if lines[i].strip() == '---':
                frontmatter_end = i
                break
        
        if frontmatter_end == -1:
            print("⚠️  Could not find frontmatter end in {}".format(evid_file))
            return False

        # Extract frontmatter
        frontmatter_lines = lines[1:frontmatter_end]
        frontmatter_content = '\n'.join(frontmatter_lines)
        
        # Parse frontmatter manually to handle references
        references = []
        current_ref = None
        in_references = False
        
        for line in frontmatter_lines:
            stripped = line.strip()
            if stripped == 'references:':
                in_references = True
                continue
            elif in_references and stripped.startswith('- type:'):
                # Start of a new reference
                if current_ref:
                    references.append(current_ref)
                ref_type = stripped.split(':', 1)[1].strip()
                current_ref = {'type': ref_type}
            elif in_references and stripped.startswith('path:') and current_ref:
                # Path for current reference
                path_value = stripped.split(':', 1)[1].strip()
                current_ref['path'] = path_value
            elif in_references and stripped.startswith('path:') and not current_ref:
                # Malformed reference - path without type
                path_value = stripped.split(':', 1)[1].strip()
                # Determine type
                if path_value.startswith('http'):
                    ref_type = 'url'
                else:
                    ref_type = 'file'
                references.append({'type': ref_type, 'path': path_value})
            elif in_references and stripped and stripped.endswith(':') and not stripped.startswith(' ') and stripped not in ['references:', '---']:
                # This looks like a new top-level field (like 'id:', 'header:', etc.)
                # End of references section
                break
        
        # Don't forget the last reference
        if current_ref:
            references.append(current_ref)

        # Filter out invalid references (only keep type: file or type: url with path, and not TSF items)
        valid_references = []
        for ref in references:
            if 'type' in ref and 'path' in ref and ref['type'] in ['file', 'url']:
                # Skip references to other TSF items (expectations, assertions, assumptions)
                path_lower = ref['path'].lower()
                if ('expect' in path_lower and 'expect-l0' in path_lower) or \
                   ('assert' in path_lower and 'assert-l0' in path_lower) or \
                   ('assump' in path_lower and 'assump-l0' in path_lower):
                    continue
                valid_references.append(ref)
            elif 'path' in ref and not 'type' in ref:
                # Malformed reference without type, but we already handled this above
                pass

        # Remove duplicates from valid_references
        seen_paths = set()
        deduplicated_references = []
        for ref in valid_references:
            if ref['path'] not in seen_paths:
                seen_paths.add(ref['path'])
                deduplicated_references.append(ref)

        # Normalize all reference types based on path
        for ref in deduplicated_references:
            if 'path' in ref:
                if ref['path'].startswith('http'):
                    # Check if it's a GitHub URL that can be converted to local path
                    if 'github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/' in ref['path']:
                        local_path = ref['path'].split('github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/')[1]
                        local_path = local_path.split('?')[0]  # Remove query parameters
                        full_local_path = os.path.join(self.repo_root, local_path)
                        if os.path.exists(full_local_path):
                            ref['type'] = 'file'
                            ref['path'] = local_path
                        else:
                            ref['type'] = 'url'
                    else:
                        ref['type'] = 'url'
                else:
                    ref['type'] = 'file'

        # Add new links if not already present
        existing_paths = set(ref['path'] for ref in deduplicated_references if 'path' in ref)
        new_links_added = 0
        for link in links:
            if link not in existing_paths:
                # Determine type and convert GitHub URLs to local paths if possible
                if link.startswith('http'):
                    # Check if it's a GitHub URL that can be converted to local path
                    if 'github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/' in link:
                        local_path = link.split('github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/')[1]
                        local_path = local_path.split('?')[0]  # Remove query parameters
                        full_local_path = os.path.join(self.repo_root, local_path)
                        if os.path.exists(full_local_path):
                            ref_type = 'file'
                            final_path = local_path
                        else:
                            ref_type = 'url'
                            final_path = link
                    else:
                        ref_type = 'url'
                        final_path = link
                else:
                    ref_type = 'file'
                    final_path = link
                    # Check if file exists
                    full_path = os.path.join(self.repo_root, link)
                    if not os.path.exists(full_path):
                        print("⚠️  Referenced file not found: {}".format(full_path))
                        continue
                
                deduplicated_references.append({'type': ref_type, 'path': final_path})
                new_links_added += 1

        # Always rebuild frontmatter to fix formatting, even if no new links
        if len(valid_references) > 0:  # Always update if there are references to format
            # Rebuild frontmatter
            new_frontmatter_lines = []
            skip_references_section = False
            
            for line in frontmatter_lines:
                stripped = line.strip()
                if stripped == 'references:':
                    skip_references_section = True
                    new_frontmatter_lines.append(line)
                    # Add all references
                    for ref in valid_references:
                        if 'path' in ref:
                            new_frontmatter_lines.append('  - type: {}'.format(ref['type']))
                            new_frontmatter_lines.append('    path: {}'.format(ref['path']))
                    continue
                elif skip_references_section and (stripped.startswith('- type:') or stripped.startswith('path:') or (stripped.startswith(' ') and ('type:' in stripped or 'path:' in stripped))):
                    # Skip old reference lines
                    continue
                elif skip_references_section and stripped and not stripped.startswith(' ') and stripped.endswith(':') and stripped not in ['references:', '---']:
                    # This is the next top-level field, end of references section
                    skip_references_section = False
                    new_frontmatter_lines.append(line)
                elif not skip_references_section:
                    new_frontmatter_lines.append(line)

            new_frontmatter = '\n'.join(new_frontmatter_lines)
        else:
            # No changes to frontmatter, use original
            new_frontmatter = '\n'.join(frontmatter_lines)

        # After updating frontmatter, add evidence links to the body if there are links to add
        if links and not dry_run:
            # Find the body content (after frontmatter)
            body_start = content.find('\n---', content.find('---')+3) + 4
            body_content = content[body_start:]
            
            # Check if there's already an evidence section
            evidence_section_pattern = re.compile(r'## Evidence\s*\n', re.IGNORECASE)
            evidence_match = evidence_section_pattern.search(body_content)
            
            if evidence_match:
                # Add links after existing evidence section
                insert_pos = evidence_match.end()
                evidence_links = '\n### Sprint Evidence\n\n'
                for link in links:
                    if link.startswith('http'):
                        evidence_links += '- [Evidence Link]({})\n'.format(link)
                    else:
                        evidence_links += '- [Evidence File]({})\n'.format(link)
                evidence_links += '\n'
                
                new_body = body_content[:insert_pos] + evidence_links + body_content[insert_pos:]
            else:
                # Add new evidence section at the end
                evidence_links = '\n## Evidence\n\n### Sprint Evidence\n\n'
                for link in links:
                    if link.startswith('http'):
                        evidence_links += '- [Evidence Link]({})\n'.format(link)
                    else:
                        evidence_links += '- [Evidence File]({})\n'.format(link)
                evidence_links += '\n'
                
                new_body = body_content + evidence_links
            
            # Rebuild full content
            new_content = '---\n' + new_frontmatter + '\n---' + new_body
            
            # Write back
            with io.open(evid_file, 'w', encoding='utf-8') as f:
                f.write(new_content)
            print("✅ Added {} evidence links to body of {}".format(len(links), evid_file))
        else:
            # No evidence links to add, but still write back if frontmatter changed
            if len(valid_references) > 0:
                new_content = '---\n' + new_frontmatter + '\n---' + content[content.find('\n---', content.find('---')+3)+4:]
                with io.open(evid_file, 'w', encoding='utf-8') as f:
                    f.write(new_content)

    def sync_all_evidence(self, dry_run=False, verbose=False):
        """Main synchronization function."""
        print("🔄 Starting TSF synchronization...")
        print("📂 Sprints dir: {}".format(self.sprints_dir))
        print("📂 Items dir: {}".format(self.items_dir))
        print()

        # First check requirements synchronization
        print("1️⃣ Checking requirements synchronization...")
        sync_ok = self.check_requirements_sync()
        print()

        # Collect evidence from sprints
        print("2️⃣ Collecting evidence from sprints...")
        evidence_map = self.collect_all_evidence()

        if not evidence_map:
            print("❌ No evidence found in sprint files")
            return

        print("📊 Found evidence for {} EXPECT items:".format(len(evidence_map)))
        for expect_id, links in evidence_map.items():
            print("  {}: {} links".format(expect_id, len(links)))
            if verbose:
                for link in links:
                    print("    - {}".format(link))
        print()

        # Fix all TSF items (EXPECT, ASSERT, ASSUMP, EVID)
        print("3️⃣ Fixing TSF item references and levels...")
        fixed_items = 0
        
        # Fix EXPECT files
        if os.path.exists(self.expectations_dir):
            expect_files = glob.glob(os.path.join(self.expectations_dir, "EXPECT-L0-*.md"))
            for expect_file in expect_files:
                expect_id = os.path.basename(expect_file).replace('.md', '')
                if self.fix_tsf_item_references("EXPECT", expect_id, dry_run):
                    fixed_items += 1
        
        # Fix ASSERT files
        if os.path.exists(self.assertions_dir):
            assert_files = glob.glob(os.path.join(self.assertions_dir, "ASSERT-L0-*.md"))
            for assert_file in assert_files:
                assert_id = os.path.basename(assert_file).replace('.md', '')
                if self.fix_tsf_item_references("ASSERT", assert_id, dry_run):
                    fixed_items += 1
        
        # Fix ASSUMP files
        if os.path.exists(self.assumptions_dir):
            assump_files = glob.glob(os.path.join(self.assumptions_dir, "ASSUMP-L0-*.md"))
            for assump_file in assump_files:
                assump_id = os.path.basename(assump_file).replace('.md', '')
                if self.fix_tsf_item_references("ASSUMP", assump_id, dry_run):
                    fixed_items += 1
        
        print("✅ Fixed {} TSF items".format(fixed_items))
        print()

        # Update EVID files with evidence
        print("4️⃣ Updating EVID files with evidence...")
        updated = 0
        # Get all EVID files
        evid_files = glob.glob(os.path.join(self.evidences_dir, "EVID-L0-*.md"))
        for evid_file in evid_files:
            evid_id = os.path.basename(evid_file).replace('.md', '')
            # Convert EVID-L0-X to EXPECT-L0-X to match evidence_map keys
            expect_id = evid_id.replace("EVID", "EXPECT")
            links = evidence_map.get(expect_id, [])
            if self.update_evid_file(evid_id, links, dry_run):
                updated += 1

        print()
        if dry_run:
            print("📝 Dry run complete: Would update {} EVID files".format(updated))
        else:
            print("✅ Synchronization complete: Updated {} EVID files".format(updated))
            
        if not sync_ok:
            print("⚠️  Requirements synchronization issues detected. Please review and fix manually.")

def main():
    """Command-line interface."""
    import argparse

    parser = argparse.ArgumentParser(description="Sync evidence from sprint files to EVID items")
    parser.add_argument("--dry-run", action="store_true", help="Show what would be changed without making changes")
    parser.add_argument("--verbose", "-v", action="store_true", help="Show detailed output")
    parser.add_argument("--repo-root", default=os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", "..", "..")),
                       help="Repository root directory")

    args = parser.parse_args()

    sync = EvidenceSync(args.repo_root)
    sync.sync_all_evidence(dry_run=args.dry_run, verbose=args.verbose)

if __name__ == "__main__":
    main()
