#!/usr/bin/env bash
# Wrapper to run the trudag publish flow for the repo
set -e
# allow safe runs: set DRY_RUN=1 or NO_PUBLISH=1 to skip publish/score/lint
DRY_RUN=${DRY_RUN:-0}
NO_PUBLISH=${NO_PUBLISH:-0}
ROOT="$(cd "$(dirname "$0")/../../../.." && pwd)"
# directory inside the repo where the TSF implementation lives (changeable via env)
TSF_DIR=${TSF_DIR:-docs/TSF/tsf_implementation}
TSF_ROOT="$ROOT/$TSF_DIR"

# operate from repo root by default; commands that use the dot DB will `cd` into $TSF_ROOT
cd "$ROOT"

if ! command -v trudag >/dev/null 2>&1; then
  echo "trudag not found in PATH. Install trudag and retry." >&2
  exit 2
fi

echo "Ensure trudag DB exists (init if missing)"
if [ ! -f "$TSF_ROOT/.dotstop.dot" ]; then
  echo ".dotstop.dot not found in $TSF_ROOT — running 'trudag init' there"
  (cd "$TSF_ROOT" && mkdir -p "$TSF_ROOT" && trudag init) || true
else
  echo "Using existing .dotstop.dot"
fi

echo "Creating items from files (idempotent)"
# helper to create items: expects prefix (folder name), and path pattern
create_items_from_dir() {
  prefix="$1"
  pattern="$2"
  for p in $pattern; do
    [ -f "$p" ] || continue
    # extract id from front-matter (first occurrence of 'id:' at line start)
    id=$(awk '/^id:/{print $2; exit}' "$p" || true)
    # sanitize item name for trudag (avoid reserved chars like '-')
    name=$(echo "$id" | sed 's/[^A-Za-z0-9_]/_/g')
    if [ -z "$id" ]; then
      echo "Skipping $p — no 'id:' front-matter found"
      continue
    fi
    echo "Creating item: prefix=$prefix id=$id name=$name source=$p"
    # create a persistent item directory for trudag (trudag expects a directory path)
    item_dir="${ROOT}/.trudag_items/${prefix}/${name}"
    mkdir -p "$(dirname "$item_dir")"
    mkdir -p "$item_dir"
    cp "$p" "$item_dir/item.md"
    # skip creation if item already exists in dot DB
    item_key="${prefix}-${name}"
    if trudag manage show-item "$item_key" >/dev/null 2>&1; then
      echo "Item $item_key already exists — skipping create-item"
      continue
    fi
    if [ "$DRY_RUN" -eq 1 ]; then
      echo "DRY RUN: trudag manage create-item $prefix $name $item_dir"
    else
      trudag manage create-item "$prefix" "$name" "$item_dir" || echo "create-item failed for $p"
    fi
  done
}

create_items_from_dir EXPECTATIONS "$TSF_ROOT/items/expectations"/*.md
create_items_from_dir ASSERTIONS "$TSF_ROOT/items/assertions"/*.md
create_items_from_dir EVIDENCES "$TSF_ROOT/items/evidences"/*.md

# also create assumptions items if present
for p in "$TSF_ROOT/items/assumptions"/*.md; do
  [ -f "$p" ] || continue
  id=$(awk '/^id:/{print $2; exit}' "$p" || true)
  name=$(echo "$id" | sed 's/[^A-Za-z0-9_]/_/g')
  if [ -z "$id" ]; then
    echo "Skipping $p — no 'id:' front-matter found"
    continue
  fi
  echo "Creating assumption item: prefix=assumptions id=$id name=$name source=$p"
  # prepare item dir and copy source
  item_dir="${TSF_ROOT}/.trudag_items/ASSUMPTIONS/${name}"
  mkdir -p "$(dirname "$item_dir")"
  mkdir -p "$item_dir"
  cp "$p" "$item_dir/item.md"
  item_key="ASSUMPTIONS-${name}"
  if (cd "$TSF_ROOT" && trudag manage show-item "$item_key") >/dev/null 2>&1; then
    echo "Item $item_key already exists — skipping create-item"
    continue
  fi
  if [ "$DRY_RUN" -eq 1 ]; then
    echo "DRY RUN: trudag manage create-item ASSUMPTIONS $name $item_dir"
  else
    trudag manage create-item "ASSUMPTIONS" "$name" "$item_dir" || echo "create-item failed for $p"
  fi
done

# create basic links using graph if present
  if [ -f "$TSF_ROOT/graph/graph.dot" ]; then
  echo "Graph exists; ensure links correspond to graph (manual step may be required)."
# If graph.dot contains edges, parse them and try to create corresponding links.
# Expected simple edge formats: A -> B  or "A" -> "B"
  if grep -F -- "->" "$TSF_ROOT/graph/graph.dot" >/dev/null 2>&1; then
    echo "Graph exists and contains edges; creating links from graph..."
    # read edges and create links
    awk '/->/ {print}' "$TSF_ROOT/graph/graph.dot" | while read -r line; do
      # strip comments and trailing semicolons
      edge=$(echo "$line" | sed 's://.*::' | sed 's/;//g')
      # extract from/to tokens
      from=$(echo "$edge" | awk -F"->" '{print $1}' | sed 's/^[ \t]*//; s/[ \t]*$//; s/^\"//; s/\"$//')
      to=$(echo "$edge" | awk -F"->" '{print $2}' | sed 's/^[ \t]*//; s/[ \t]*$//; s/^\"//; s/\"$//')
      if [ -n "$from" ] && [ -n "$to" ]; then
        parent_name=$(echo "$from" | sed 's/[^A-Za-z0-9_]/_/g')
        child_name=$(echo "$to" | sed 's/[^A-Za-z0-9_]/_/g')
        # derive uppercase plural prefix from ID token
        derive_prefix() {
          token="$1"
          case "$token" in
            EXPECT* ) echo "EXPECTATIONS" ;;
            ASSERT* ) echo "ASSERTIONS" ;;
            EVID* | EVIDENCE* ) echo "EVIDENCES" ;;
            ASSUMP* ) echo "ASSUMPTIONS" ;;
            * ) echo "UNKNOWN" ;;
          esac
        }
        parent_prefix=$(derive_prefix "$from")
        child_prefix=$(derive_prefix "$to")
        parent_key="${parent_prefix}-${parent_name}"
        child_key="${child_prefix}-${child_name}"
        echo "Creating link: $from -> $to (using keys: $parent_key -> $child_key)"
        if [ "$DRY_RUN" -eq 1 ]; then
          echo "DRY RUN: (cd $TSF_ROOT && trudag manage create-link $parent_key $child_key)"
        else
          (cd "$TSF_ROOT" && trudag manage create-link "$parent_key" "$child_key") || echo "create-link failed: $parent_key -> $child_key"
        fi
      fi
    done
  else
    echo "Graph exists but contains no edges; skipping automatic link creation."
  fi
fi

 
if [ "$DRY_RUN" -eq 1 ] || [ "$NO_PUBLISH" -eq 1 ]; then
  echo "DRY RUN / NO_PUBLISH set; skipping lint/score/publish. Set DRY_RUN=0 NO_PUBLISH=0 to run."
else
  echo "Linting"
  (cd "$TSF_ROOT" && trudag manage lint) || true

  echo "Scoring"
  (cd "$TSF_ROOT" && trudag score) || true

  echo "Publishing (this will generate/refresh .dotstop.dot)"
  (cd "$TSF_ROOT" && trudag publish) || true
fi

echo "trudag publish complete"
