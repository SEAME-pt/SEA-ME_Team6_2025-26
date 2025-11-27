#!/usr/bin/env bash
set -euo pipefail

DATE=$(date +%Y%m%d_%H%M%S)
ROOT="$(git rev-parse --show-toplevel 2>/dev/null || true)"
if [ -z "$ROOT" ]; then
  ROOT="$(cd "$(dirname "$0")/../../../../.." && pwd)"
fi
BACKUP="$ROOT/docs/TSF/tsf_implementation/items_backup_${DATE}.tar.gz"

echo "Backing up items to $BACKUP"
mkdir -p "$ROOT/docs/TSF/tsf_implementation"
if tar -czf "$BACKUP" -C "$ROOT" docs/TSF/tsf_implementation/items; then
  echo "Backup created: $BACKUP"
else
  echo "Backup failed or items dir missing; aborting" >&2
  exit 1
fi

# Create missing items
mkdir -p "$ROOT/.trudag_items"
missing_count=0
failed_count=0

# iterate files
for md in "$ROOT"/docs/TSF/tsf_implementation/items/*/*.md; do
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
  if trudag manage show-item "$item_key" >/dev/null 2>&1; then
    echo "Item exists: $item_key — skipping"
    continue
  fi
  echo "Creating item: $item_key from $md"
  item_dir="$ROOT/.trudag_items/${PREFIX}/${name}"
  mkdir -p "$item_dir"
  cp "$md" "$item_dir/item.md"
  if trudag manage create-item "$PREFIX" "$name" "$item_dir"; then
    echo "Created: $item_key"
    missing_count=$((missing_count+1))
  else
    echo "Failed to create: $item_key" >&2
    failed_count=$((failed_count+1))
  fi
done

echo "Create summary: created=$missing_count failed=$failed_count"

# Apply links from graph
GRAPH="$ROOT/docs/TSF/tsf_implementation/graph/graph.dot"
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
    if trudag manage create-link "$parent_key" "$child_key"; then
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
trudag manage lint || echo "lint finished with issues"

echo "All done. Backup: $BACKUP. Created: $missing_count failed: $failed_count"
