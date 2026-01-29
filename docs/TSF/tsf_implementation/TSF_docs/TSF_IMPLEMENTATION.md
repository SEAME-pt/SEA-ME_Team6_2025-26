# TSF Implementation Guide — Sea:ME

This document describes the exact steps and commands to maintain and publish the project TSF using `trudag`.

## Prerequisites

- `python3` (3.9+ recommended)
- `trudag` installed and available in PATH on the machine that will publish
- `graphviz` (`dot`) if you want to render `graph/graph.dot`

## Project Structure

```
SEA-ME_Team6_2025-26/
├── .dotstop_extensions/           # Custom trudag extensions (references + validators)
│   ├── __init__.py                # Exports all references and validators
│   ├── references.py              # FileReference, UrlReference classes
│   └── validators.py              # validate_hardware_availability, validate_linux_environment, validate_software_dependencies
├── .dotstop.dot                   # Symlink → docs/TSF/tsf_implementation/.dotstop.dot
├── localplugins                   # Symlink → .dotstop_extensions (for trudag module imports)
└── docs/TSF/tsf_implementation/
    ├── .dotstop.dot               # TruDAG database (generated)
    ├── .dotstop_extensions        # Symlink → ../../../.dotstop_extensions
    ├── .trudag_items/             # Generated trudag items (from items/)
    ├── items/                     # SOURCE TSF items (EXPECT, ASSERT, EVID, ASSUMP)
    │   ├── assertions/
    │   ├── assumptions/
    │   ├── evidences/
    │   └── expectations/
    ├── graph/
    │   └── graph.dot              # Dependency graph (generated)
    └── scripts/
        ├── open_check_sync_update_validate_run_publish_tsfrequirements.py  # Main automation script
        ├── setup_trudag_clean.sh  # TruDAG setup script
        └── modules/               # Helper modules
```

## Quick Commands

### Main Automation Script (Recommended)

The unified script handles all TSF operations:

```bash
# Check current state (read-only)
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check

# Full validation, TruDAG run, and publish
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate
```

### Individual Commands

- Generate items from requirements (creates/updates EXPECT/ASSERT/EVID skeletons):

```bash
python3 docs/TSF/tsf_implementation/scripts/generate_from_requirements.py --input docs/TSF/requirements/tsf-requirements-table.md
```

- Run TruDAG setup directly:

```bash
bash docs/TSF/tsf_implementation/scripts/setup_trudag_clean.sh
```

- Run TruDAG score with validators:

```bash
cd docs/TSF/tsf_implementation && trudag score --validate
```

- Publish reports:

```bash
cd docs/TSF/tsf_implementation && trudag publish --output-dir ../../../docs/doorstop --validate --all-bodies
```

## Validators

Custom validators in `.dotstop_extensions/validators.py` are executed automatically by `trudag score --validate`:

| Validator | Purpose | Used by |
|-----------|---------|---------|
| `validate_hardware_availability` | Checks if hardware components are documented in the repo | ASSUMP items |
| `validate_linux_environment` | Validates Linux tools/packages are documented | ASSUMP items |
| `validate_software_dependencies` | Checks if software dependencies are documented | ASSUMP items |

### Validator Configuration Example

In ASSUMP items, use the `evidence:` field:

```yaml
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "Raspberry Pi 5"
      - "Hailo Hat"
      - "DC motors"
```

## Symlinks Explained

The project uses symlinks for trudag compatibility:

| Symlink | Target | Purpose |
|---------|--------|---------|
| `.dotstop.dot` | `docs/TSF/tsf_implementation/.dotstop.dot` | TruDAG finds DB from repo root |
| `localplugins` | `.dotstop_extensions` | TruDAG imports custom references |
| `tsf_implementation/.dotstop_extensions` | `../../../.dotstop_extensions` | TruDAG finds validators when running from tsf_implementation/ |

## CI (GitHub Actions)

A CI workflow (`.github/workflows/validate_items_formatation.yml`) validates TSF items on PRs to `development`.

## Published Reports

Reports are generated in `docs/doorstop/`:

- `ASSERTIONS.md` - All assertion items
- `ASSUMPTIONS.md` - All assumption items  
- `EVIDENCES.md` - All evidence items
- `EXPECTATIONS.md` - All expectation items
- `dashboard.md` - Score distribution summary
- `trustable_report_for_Software.md` - Full trustable report

## Troubleshooting

### "Cannot find a validator function for type X"

The trudag cannot find the validators. Ensure:
1. `.dotstop_extensions/validators.py` exists in repo root
2. Symlink exists: `tsf_implementation/.dotstop_extensions → ../../../.dotstop_extensions`

### "Cannot get non-existent or non-regular file"

Path in references is incorrect. Paths should be:
- Relative to repo root: `docs/TSF/tsf_implementation/.trudag_items/...`
- NOT relative paths like `../expectations/...` (these don't work in `.trudag_items/`)

### Item stays "unreviewed" after set-item

Usually caused by:
1. Invalid URL references (e.g., `/actions` pages instead of file URLs)
2. Non-existent file paths
3. Hash mismatch (file changed after set-item)

Fix: Correct the references and run `trudag manage set-item ITEM_ID --links` again.

## Automation Notes

- The generator will try to match evidences automatically by scanning `docs/sprints/`, `docs/demos/`, `docs/guides/`, `docs/images/`, and `src/`.
- Matches are suggestions — review before publishing.
- The final `.dotstop.dot` manifest with SHAs is generated by `trudag publish`.
- All 84 items should achieve score 1.0 after successful validation.
