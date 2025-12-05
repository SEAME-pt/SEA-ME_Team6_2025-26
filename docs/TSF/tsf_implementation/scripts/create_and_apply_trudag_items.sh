#!/usr/bin/env bash
set -euo pipefail

DATE=$(date +%Y%m%d_%H%M%S)
ROOT="$(git rev-parse --show-toplevel 2>/dev/null || true)"
if [ -z "$ROOT" ]; then
  ROOT="$(cd "$(dirname "$0")/../../../../.." && pwd)"
fi
# allow override and compute target dir
TSF_DIR=${TSF_DIR:-docs/TSF/tsf_implementation}
TSF_ROOT="$ROOT/$TSF_DIR"
# place backups inside the tsf_implementation/backups directory
mkdir -p "$TSF_ROOT/backups"
BACKUP="$TSF_ROOT/backups/items_backup_${DATE}.tar.gz"

echo "Backing up items to $BACKUP"
mkdir -p "$ROOT/docs/TSF/tsf_implementation"
if tar -czf "$BACKUP" -C "$ROOT" "$TSF_DIR/items"; then
  echo "Backup created: $BACKUP"
else
  echo "Backup failed or items dir missing; aborting" >&2
  exit 1
fi

# Ensure the dot DB exists in docs/TSF and is initialised there.
# This creates `docs/TSF/.needs.dot` which we prefer as the canonical DB location.
TSF_PARENT_DB_DIR="$ROOT/docs/TSF"
DOTSTOP_PATH="$TSF_PARENT_DB_DIR/.needs.dot"
if [ ! -f "$DOTSTOP_PATH" ]; then
  echo "Initializing trudag DB in $TSF_PARENT_DB_DIR"
  if (cd "$TSF_PARENT_DB_DIR" && command -v trudag >/dev/null 2>&1 && trudag --needs init); then
    echo "trudag DB initialised at $DOTSTOP_PATH"
  else
    echo "Warning: could not run 'trudag init' in $TSF_PARENT_DB_DIR; creating empty dot file instead"
    mkdir -p "$TSF_PARENT_DB_DIR"
    touch "$DOTSTOP_PATH"
  fi
fi

# Create a symlink at repository root `.needs.dot` pointing to the canonical TSF DB
# so calls to trudag that search upward from other CWDs find the DB.
if [ ! -f "$ROOT/.needs.dot" ]; then
  ln -s "$DOTSTOP_PATH" "$ROOT/.needs.dot" || true
fi

# Create missing items
mkdir -p "$TSF_ROOT/.trudag_items"
missing_count=0
failed_count=0

# iterate files
for md in "$TSF_ROOT/items"/*/*.md; do
  [ -f "$md" ] || continue
  folder=$(basename "$(dirname "$md")")
  case "$folder" in
    expectations) PREFIX=EXPECTATIONS ;;
    assertions) PREFIX=ASSERTIONS ;;
    evidences) PREFIX=EVIDENCES ;;
    assumptions) PREFIX=ASSUMPTIONS ;;
    *) PREFIX=UNKNOWN ;;
  esac
  id=$(awk '/^id:/{print $2; exit}' "$md" || true)
  if [ -z "$id" ]; then
    echo "Skipping $md — no id front-matter"
    continue
  fi
  name=$(echo "$id" | sed 's/[^A-Za-z0-9_]/_/g')
  item_key="${PREFIX}-${name}"
  if (cd "$TSF_ROOT" && trudag --needs manage show-item "$item_key") >/dev/null 2>&1; then
    echo "Item exists: $item_key — skipping"
    continue
  fi
  echo "Creating item: $item_key from $md"
  item_dir="$TSF_ROOT/.trudag_items/${PREFIX}/${name}"
  mkdir -p "$(dirname "$item_dir")"
  mkdir -p "$item_dir"
  cp "$md" "$item_dir/item.md"
  if (cd "$TSF_ROOT" && trudag --needs manage create-item "$PREFIX" "$name" "$item_dir"); then
    echo "Created: $item_key"
    missing_count=$((missing_count+1))
  else
    echo "Failed to create: $item_key" >&2
    failed_count=$((failed_count+1))
  fi
done

echo "Create summary: created=$missing_count failed=$failed_count"

# Apply links from graph
GRAPH="$TSF_ROOT/graph/graph.dot"
if [ -f "$GRAPH" ]; then
  echo "Applying links from graph"
  awk '/->/ {print}' "$GRAPH" | while read -r line; do
    edge=$(echo "$line" | sed 's://.*::' | sed 's/;//g')
    from=$(echo "$edge" | awk -F"->" '{print $1}' | sed 's/^[ \t]*//; s/[ \t]*$//; s/^"//; s/"$//')
    to=$(echo "$edge" | awk -F"->" '{print $2}' | sed 's/^[ \t]*//; s/[ \t]*$//; s/^"//; s/"$//')
    if [ -z "$from" ] || [ -z "$to" ]; then
      continue
    fi
    derive_prefix() {
      token="$1"
      case "$token" in
        EXPECT*) echo EXPECTATIONS ;;
        ASSERT*) echo ASSERTIONS ;;
        EVID*) echo EVIDENCES ;;
        ASSUMP*) echo ASSUMPTIONS ;;
        *) echo UNKNOWN ;;
      esac
    }
    parent_prefix=$(derive_prefix "$from")
    child_prefix=$(derive_prefix "$to")
    parent_name=$(echo "$from" | sed 's/[^A-Za-z0-9_]/_/g')
    child_name=$(echo "$to" | sed 's/[^A-Za-z0-9_]/_/g')
    parent_key="${parent_prefix}-${parent_name}"
    child_key="${child_prefix}-${child_name}"
    echo "Creating link: $parent_key -> $child_key"
    if (cd "$TSF_ROOT" && trudag --needs manage create-link "$parent_key" "$child_key"); then
      echo "Linked: $parent_key -> $child_key"
    else
      echo "Failed link: $parent_key -> $child_key" >&2
    fi
  done
else
  echo "No graph.dot found; skipping link application"
fi

# Run lint
echo "Running: trudag manage lint"
(cd "$TSF_ROOT" && trudag --needs manage lint) || echo "lint finished with issues"

echo "All done. Backup: $BACKUP. Created: $missing_count failed: $failed_count"
