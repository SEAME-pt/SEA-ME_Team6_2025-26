#!/usr/bin/env python3
"""Complete TSF validator suite - integrated in a single file.

This validator performs comprehensive checks on TSF items:
1. Front-matter validation: ensures required fields exist (id, header, text, level, normative)
2. Extended validation:
   - EXPECT-L0-N must reference ASSERT-L0-N
   - ASSERT-L0-N must reference EVID-L0-N
   - All items must have valid level format (1.<n>)
   - References block validation (type, path, file existence)
   - Reviewers block validation (name, email format)

Exits with non-zero code when issues are found (suitable for CI).
"""
import sys
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[2] / 'items'
REQUIRED = ['id', 'header', 'text', 'level', 'normative']


# ============================================================================
# FRONT-MATTER VALIDATION
# ============================================================================

def read_frontmatter(path: Path):
    """Extract YAML front-matter from a markdown file."""
    text = path.read_text(encoding='utf-8')
    if not text.strip().startswith('---'):
        return None
    parts = text.split('---')
    if len(parts) < 3:
        return None
    return parts[1]


def parse_keys(fm_text: str):
    """Extract top-level keys from YAML front-matter."""
    keys = []
    for line in (fm_text or '').splitlines():
        m = re.match(r'^\s*([a-zA-Z0-9_-]+)\s*:', line)
        if m:
            keys.append(m.group(1))
    return keys


def validate_frontmatter_basic():
    """Check that all items have required front-matter fields."""
    issues = []
    for p in ROOT.rglob('*.md'):
        fm = read_frontmatter(p)
        if fm is None:
            issues.append(f"MISSING_FRONTMATTER: {p}")
            continue
        keys = parse_keys(fm)
        for r in REQUIRED:
            if r not in keys:
                issues.append(f"MISSING_FIELD {r}: {p}")
    return issues


# ============================================================================
# EXTENDED VALIDATION
# ============================================================================

def parse_level(fm_text: str):
    """Extract the 'level' field value from front-matter."""
    if not fm_text:
        return None
    m = re.search(r'^\s*level\s*:\s*(.+)$', fm_text, flags=re.MULTILINE)
    if not m:
        return None
    val = m.group(1).strip()
    # Strip surrounding quotes if present
    if (val.startswith('"') and val.endswith('"')) or (val.startswith("'") and val.endswith("'")):
        val = val[1:-1]
    return val


def has_reference(fm_text: str, target_id: str, target_path_suffix: str) -> bool:
    """Check if front-matter contains a reference to target_id or path."""
    if not fm_text:
        return False
    if re.search(rf"\b{re.escape(target_id)}\b", fm_text):
        return True
    if target_path_suffix in fm_text:
        return True
    return False


def validate_reviewers_block(fm_text: str, item_path: Path):
    """Validate reviewers section: each entry must have name and valid email."""
    issues = []
    m = re.search(r'(^|\n)reviewers\s*:\s*(\n(?:[ \t].*\n)*)', fm_text, flags=re.MULTILINE)
    if not m:
        return issues
    block = m.group(2)
    entries = re.split(r'\n\s*-\s*', '\n' + block)
    for ent in entries:
        ent = ent.strip()
        if not ent:
            continue
        name_m = re.search(r'\bname\s*:\s*(.+)', ent)
        email_m = re.search(r'\bemail\s*:\s*(.+)', ent)
        if not name_m:
            issues.append(f'MISSING_REVIEWER_NAME: {item_path} entry: {ent[:40]!r}')
        if not email_m:
            issues.append(f'MISSING_REVIEWER_EMAIL: {item_path} entry: {ent[:40]!r}')
        else:
            email = email_m.group(1).strip().strip('"').strip("'")
            if not re.match(r"^[^@\s]+@[^@\s]+\.[^@\s]+$", email):
                issues.append(f'INVALID_REVIEWER_EMAIL: {item_path} -> {email}')
    return issues


def validate_references_block(fm_text: str, item_path: Path):
    """Validate references section: check type/path consistency and file existence."""
    issues = []
    m = re.search(r'(^|\n)references:\s*(\n(?:[ \t].*\n)*)', fm_text, flags=re.MULTILINE)
    if not m:
        return issues
    block = m.group(2)
    entries = re.split(r'\n\s*-\s*', '\n' + block)
    for ent in entries:
        ent = ent.strip()
        if not ent:
            continue
        t_m = re.search(r'\btype\s*:\s*(\S+)', ent)
        p_m = re.search(r'\bpath\s*:\s*(.+)', ent)
        id_m = re.search(r'\bid\s*:\s*(\S+)', ent)
        ref_type = t_m.group(1) if t_m else None
        ref_path_raw = None
        if p_m:
            ref_path_raw = p_m.group(1).strip()
            if (ref_path_raw.startswith('"') and ref_path_raw.endswith('"')) or (
                ref_path_raw.startswith("'") and ref_path_raw.endswith("'")):
                ref_path_raw = ref_path_raw[1:-1]
        
        if not ref_type:
            issues.append(f'MISSING_REFERENCE_TYPE: {item_path} -> entry: {ent.splitlines()[0][:80]!r}')
        
        if ref_path_raw:
            if ref_path_raw.startswith('http://') or ref_path_raw.startswith('https://'):
                continue
            resolved = (item_path.parent / ref_path_raw).resolve()
            if not resolved.exists():
                issues.append(f'MISSING_REFERENCED_FILE: {item_path} -> {ref_path_raw} (resolved: {resolved})')
        else:
            if ref_type == 'file':
                issues.append(f'MISSING_REFERENCE_PATH: {item_path} entry declares type:file but has no path')
            if not ref_type and not ref_path_raw and not id_m:
                issues.append(f'AMBIGUOUS_REFERENCE_ENTRY: {item_path} entry lacks type/id/path')
    
    return issues


def validate_extended():
    """Perform extended validation checks on TSF items."""
    issues = []
    
    # Check EXPECT -> ASSERT references
    exp_dir = ROOT / 'expectations'
    for p in sorted(exp_dir.glob('EXPECT-L0-*.md')):
        m = re.search(r'L0-(\d+)', p.name)
        if not m:
            continue
        n = m.group(1)
        ast_id = f'ASSERT-L0-{n}'
        ast_path = f'../assertions/{ast_id}.md'
        fm_text = read_frontmatter(p)
        if not fm_text:
            issues.append(f'MISSING_FRONTMATTER: {p}')
            continue
        if not has_reference(fm_text, ast_id, ast_path):
            issues.append(f'MISSING_EXPECT_ASSERT_REF: {p} -> {ast_id}')
        issues += validate_references_block(fm_text, p)
    
    # Check ASSERT -> EVID references and level format
    ast_dir = ROOT / 'assertions'
    for p in sorted(ast_dir.glob('ASSERT-L0-*.md')):
        m = re.search(r'L0-(\d+)', p.name)
        if not m:
            continue
        n = m.group(1)
        evid_id = f'EVID-L0-{n}'
        evid_path = f'../evidences/{evid_id}.md'
        fm_text = read_frontmatter(p)
        if not fm_text:
            issues.append(f'MISSING_FRONTMATTER: {p}')
            continue
        if not has_reference(fm_text, evid_id, evid_path):
            issues.append(f'MISSING_ASSERT_EVID_REF: {p} -> {evid_id}')
        issues += validate_references_block(fm_text, p)
        lvl = parse_level(fm_text)
        if not (isinstance(lvl, str) and re.match(r'^1\.\d+$', lvl)):
            issues.append(f'INVALID_LEVEL_FORMAT: {p} level={lvl}')
    
    # Check all items level format
    for p in sorted(ROOT.rglob('*.md')):
        fm_text = read_frontmatter(p)
        if not fm_text:
            continue
        lvl = parse_level(fm_text)
        if lvl is None:
            issues.append(f'MISSING_FIELD level: {p}')
        elif not (isinstance(lvl, str) and re.match(r'^1\.\d+$', lvl)):
            issues.append(f'INVALID_LEVEL_FORMAT: {p} level={lvl}')
        issues += validate_references_block(fm_text, p)
    
    return issues


# ============================================================================
# MAIN EXECUTION
# ============================================================================

def main():
    """Run all validation checks and report results."""
    all_issues = []
    
    # Run basic front-matter validation
    print('Running front-matter validation...')
    fm_issues = validate_frontmatter_basic()
    if fm_issues:
        all_issues.extend(fm_issues)
    else:
        print('✅ Front-matter validation passed')
    
    # Run extended validation
    print('Running extended validation...')
    ext_issues = validate_extended()
    if ext_issues:
        all_issues.extend(ext_issues)
    else:
        print('✅ Extended validation passed')
    
    # Report results
    if all_issues:
        print('\n' + '='*80)
        print('VALIDATION FAILURES:')
        print('='*80)
        print('\n'.join(all_issues))
        print('='*80)
        print(f'\n❌ Found {len(all_issues)} issue(s)')
        sys.exit(2)
    else:
        print('\n' + '='*80)
        print('✅ All TSF validation checks passed!')
        print('='*80)
        sys.exit(0)


if __name__ == '__main__':
    main()
