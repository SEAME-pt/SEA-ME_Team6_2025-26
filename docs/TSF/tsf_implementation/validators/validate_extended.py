#!/usr/bin/env python3
"""Extended front-matter validator.

Checks performed:
- Every `EXPECT-L0-N.md` has a reference to `ASSERT-L0-N` (by id or path).
- Every `ASSERT-L0-N.md` has a reference to `EVID-L0-N` (by id or path).
- Every item's `level` follows the format `1.<n>` (string matching /^1\.\d+$/).

Exits with non-zero status on any error (suitable for CI).
"""
import sys
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[2] / 'items'
REQUIRED = ['id', 'header', 'text', 'level', 'normative']


def parse_keys(fm_text: str):
    keys = []
    for line in (fm_text or '').splitlines():
        m = re.match(r'^\s*([a-zA-Z0-9_-]+)\s*:', line)
        if m:
            keys.append(m.group(1))
    return keys


def validate_reviewers_block(fm_text: str, item_path: Path):
    issues = []
    m = re.search(r'(^|\n)reviewers\s*:\s*(\n(?:[ \t].*\n)*)', fm_text, flags=re.MULTILINE)
    if not m:
        return issues
    block = m.group(2)
    # ensure each entry has name and email
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


def read_frontmatter(path: Path):
    txt = path.read_text(encoding='utf-8')
    if not txt.strip().startswith('---'):
        return None
    parts = txt.split('---')
    if len(parts) < 3:
        return None
    fm_text = parts[1]
    return fm_text


def has_reference(fm_text: str, target_id: str, target_path_suffix: str) -> bool:
    if not fm_text:
        return False
    # simple text-based checks: id or path occurrence in the front-matter
    if re.search(rf"\b{re.escape(target_id)}\b", fm_text):
        return True
    if target_path_suffix in fm_text:
        return True
    return False


def parse_level(fm_text: str):
    if not fm_text:
        return None
    m = re.search(r'^\s*level\s*:\s*(.+)$', fm_text, flags=re.MULTILINE)
    if not m:
        return None
    val = m.group(1).strip()
    # strip surrounding quotes if present
    if (val.startswith('"') and val.endswith('"')) or (val.startswith("'") and val.endswith("'")):
        val = val[1:-1]
    return val


def validate_references_block(fm_text: str, item_path: Path):
    """Validate the `references` section in the front-matter.

    Rules enforced (best-effort text parsing):
    - If an entry specifies `type: file` then a `path:` must exist and be a string.
    - If a file path is local (not starting with http:// or https://), check whether the
      resolved path exists in the repository relative to the item file.
    - If an entry provides no `type` but provides `path`, accept it but check path existence.
    Returns a list of issue strings.
    """
    issues = []
    m = re.search(r'(^|\n)references:\s*(\n(?:[ \t].*\n)*)', fm_text, flags=re.MULTILINE)
    if not m:
        return issues
    block = m.group(2)
    # Split by top-level '-' entry (lines starting with optional space then '-')
    entries = re.split(r'\n\s*-\s*', '\n' + block)
    for ent in entries:
        ent = ent.strip()
        if not ent:
            continue
        # find type
        t_m = re.search(r'\btype\s*:\s*(\S+)', ent)
        p_m = re.search(r'\bpath\s*:\s*(.+)', ent)
        id_m = re.search(r'\bid\s*:\s*(\S+)', ent)
        ref_type = t_m.group(1) if t_m else None
        ref_path_raw = None
        if p_m:
            ref_path_raw = p_m.group(1).strip()
            # remove surrounding quotes
            if (ref_path_raw.startswith('"') and ref_path_raw.endswith('"')) or (
                ref_path_raw.startswith("'") and ref_path_raw.endswith("'")):
                ref_path_raw = ref_path_raw[1:-1]
        # type is recommended — enforce presence
        if not ref_type:
            issues.append(f'MISSING_REFERENCE_TYPE: {item_path} -> entry: {ent.splitlines()[0][:80]!r}')

        # If path exists, validate local file presence when not a URL
        if ref_path_raw:
            if ref_path_raw.startswith('http://') or ref_path_raw.startswith('https://'):
                # URL-based reference; accept
                continue
            # Resolve relative to the item file location
            resolved = (item_path.parent / ref_path_raw).resolve()
            if not resolved.exists():
                issues.append(f'MISSING_REFERENCED_FILE: {item_path} -> {ref_path_raw} (resolved: {resolved})')
        else:
            # no path provided
            if ref_type == 'file':
                issues.append(f'MISSING_REFERENCE_PATH: {item_path} entry declares type:file but has no path')
            # if neither type nor path, check id at least
            if not ref_type and not ref_path_raw and not id_m:
                issues.append(f'AMBIGUOUS_REFERENCE_ENTRY: {item_path} entry lacks type/id/path')

    return issues


def main():
    issues = []
    # Check EXPECT -> ASSERT
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
        # validate references content for EXPECT
        issues += validate_references_block(fm_text, p)

    # Check ASSERT -> EVID and levels on ASSERTs
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
        # validate references content for ASSERT
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
        # validate references content for all items as well
        issues += validate_references_block(fm_text, p)

    if issues:
        print('\n'.join(issues))
        sys.exit(2)
    print('OK: extended validation passed')


if __name__ == '__main__':
    main()
