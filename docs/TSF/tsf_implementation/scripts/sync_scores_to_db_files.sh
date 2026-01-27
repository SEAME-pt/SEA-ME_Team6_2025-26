#!/usr/bin/env bash
set -euo pipefail

# Script to copy scores from item.md to the trudag DB file (PREFIX-ID.md)

ROOT="$(git rev-parse --show-toplevel 2>/dev/null || pwd)"
TSF_DIR="${TSF_DIR:-docs/TSF/tsf_implementation}"
TSF_ROOT="$ROOT/$TSF_DIR"

echo "Syncing scores from item.md to DB files..."

updated=0
skipped=0

for item_dir in "$TSF_ROOT/.trudag_items"/*/*/; do
    [ -d "$item_dir" ] || continue
    
    item_md="$item_dir/item.md"
    [ -f "$item_md" ] || continue
    
    # Extract score from item.md
    score=$(grep "^score:" "$item_md" | head -1 | awk '{print $2}')
    
    # Find the DB file (the other .md file that's not item.md)
    db_file=$(find "$item_dir" -maxdepth 1 -type f -name "*.md" ! -name "item.md" | head -1)
    
    if [ -z "$db_file" ]; then
        echo "Warning: No DB file found in $item_dir"
        skipped=$((skipped+1))
        continue
    fi
    
    if [ -z "$score" ]; then
        echo "Warning: No score found in $item_md"
        skipped=$((skipped+1))
        continue
    fi
    
    # Check if score already exists in DB file
    if grep -q "^score:" "$db_file"; then
        # Update existing score
        sed -i '' "s/^score:.*$/score: $score/" "$db_file"
        echo "Updated: $(basename "$db_file") -> score: $score"
    else
        # Add score after normative line in frontmatter
        # Use awk to insert after normative line
        awk -v score="$score" '
            /^normative:/ { print; print "score: " score; next }
            { print }
        ' "$db_file" > "$db_file.tmp" && mv "$db_file.tmp" "$db_file"
        echo "Added: $(basename "$db_file") -> score: $score"
    fi
    updated=$((updated+1))
done

echo ""
echo "Summary: Updated=$updated Skipped=$skipped"
