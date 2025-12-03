#!/bin/bash
set -e

# Script to setup trudag DB with CORRECTED workflow
# Based on setup_trudag_clean.sh but with fixes for:
# - Keep TSF references in source files
# - Remove TSF references when copying to .trudag_items
# - Use repo root for DB (no symlink needed)
# - Correct paths for external files

REPO_ROOT="/Volumes/Important_Docs/42/SEA-ME_Team6_2025-26"
TSF_IMPL="$REPO_ROOT/docs/TSF/tsf_implementation"
ITEMS_SOURCE="$TSF_IMPL/items"
TRUDAG_ITEMS="$TSF_IMPL/.trudag_items"
GRAPH_DIR="$TSF_IMPL/graph"
DB_FILE="$REPO_ROOT/.dotstop.dot"

echo "=========================================="
echo "🚀 TSF Trudag Setup - Fixed Version"
echo "=========================================="
echo ""

# Step 0: Clean all generated files (NEVER touch source items/)
echo "🧹 Step 0: Cleaning all generated files..."
[ -f "$GRAPH_DIR/graph.dot" ] && rm -f "$GRAPH_DIR/graph.dot" && echo "  ✓ Removed graph.dot"
[ -f "$REPO_ROOT/graph.dot" ] && rm -f "$REPO_ROOT/graph.dot" && echo "  ✓ Removed root graph.dot"
[ -f "$DB_FILE" ] && rm -f "$DB_FILE" && echo "  ✓ Removed .dotstop.dot"
[ -d "$TRUDAG_ITEMS" ] && rm -rf "$TRUDAG_ITEMS" && echo "  ✓ Removed .trudag_items/"
echo "✓ Cleanup complete (source items/ preserved)"
echo ""

# Step 1: Generate graph.dot from source
echo "📊 Step 1: Generating graph.dot from source items..."
mkdir -p "$GRAPH_DIR"
python3 "$TSF_IMPL/tools/generate_graph_from_heuristics.py" \
    --items "$ITEMS_SOURCE" \
    --out "$GRAPH_DIR/graph.dot"
echo "✓ Graph generated: $GRAPH_DIR/graph.dot"
echo ""

# Step 2: Initialize DB in repo root
echo "🗄️  Step 2: Initializing trudag DB in repo root..."
cd "$REPO_ROOT"
trudag init
echo "✓ DB initialized: $DB_FILE"
echo ""

# Step 3: Create all items
echo "📝 Step 3: Creating 68 items..."
cd "$TSF_IMPL"

created=0
failed=0

for category_dir in "$ITEMS_SOURCE"/*; do
    if [ ! -d "$category_dir" ]; then
        continue
    fi
    
    category=$(basename "$category_dir")
    case "$category" in
        assertions) PREFIX="ASSERTIONS" ;;
        assumptions) PREFIX="ASSUMPTIONS" ;;
        evidences) PREFIX="EVIDENCES" ;;
        expectations) PREFIX="EXPECTATIONS" ;;
        *) PREFIX=$(echo "${category}" | tr '[:lower:]' '[:upper:]') ;;
    esac
    
    echo "  Processing $PREFIX..."
    
    for source_file in "$category_dir"/*.md; do
        if [ ! -f "$source_file" ]; then
            continue
        fi
        
        filename=$(basename "$source_file" .md)
        item_id="${filename//-/_}"
        
        target_dir="$TRUDAG_ITEMS/$PREFIX/$item_id"
        mkdir -p "$target_dir"
        
        trudag manage create-item "$PREFIX" "$item_id" "$target_dir" 2>/dev/null || true
        
        target_file="$target_dir/$PREFIX-$item_id.md"
        if [ -f "$target_file" ]; then
            created=$((created + 1))
            echo "    ✓ $PREFIX-$item_id"
        else
            failed=$((failed + 1))
            echo "    ✗ $PREFIX-$item_id (creation failed)"
        fi
    done
done

echo ""
echo "✓ Items created: $created, Failed: $failed"
echo ""

# Step 4: Copy content and remove TSF references
echo "📄 Step 4: Copying content and removing TSF references..."

python3 - <<'PYTHON'
import re
from pathlib import Path

items_source = Path("/Volumes/Important_Docs/42/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/items")
trudag_items = Path("/Volumes/Important_Docs/42/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/.trudag_items")

category_map = {
    'assertions': 'ASSERTIONS',
    'assumptions': 'ASSUMPTIONS',
    'evidences': 'EVIDENCES',
    'expectations': 'EXPECTATIONS'
}

copied = 0

for category_dir in items_source.iterdir():
    if not category_dir.is_dir():
        continue
    
    category = category_dir.name
    prefix = category_map.get(category, category.upper())
    
    for source_file in category_dir.glob("*.md"):
        filename = source_file.stem
        item_id = filename.replace('-', '_')
        
        target_file = trudag_items / prefix / item_id / f"{prefix}-{item_id}.md"
        
        if not target_file.exists():
            continue
        
        # Read source
        content = source_file.read_text()
        
        # Split by YAML frontmatter
        parts = content.split('---', 2)
        if len(parts) < 3:
            target_file.write_text(content)
            continue
        
        yaml_content = parts[1]
        body = parts[2]
        
        # Remove ONLY TSF item references (../expectations/, ../assertions/, etc)
        # Keep external references (docs/demos/, docs/guides/, URLs)
        lines = yaml_content.split('\n')
        new_lines = []
        i = 0
        
        while i < len(lines):
            line = lines[i]
            
            # Check if this is start of a reference item
            if line.strip().startswith('- '):
                # Collect all lines of this reference
                ref_lines = [line]
                j = i + 1
                while j < len(lines) and lines[j].startswith('  ') and not lines[j].strip().startswith('-'):
                    ref_lines.append(lines[j])
                    j += 1
                
                ref_text = '\n'.join(ref_lines)
                
                # Check if it's a TSF reference
                is_tsf = ('path:' in ref_text and ('../expectations' in ref_text or '../assertions' in ref_text or 
                          '../evidences' in ref_text or '../assumptions' in ref_text)) or '- id:' in ref_text
                
                if not is_tsf:
                    # Keep external references
                    new_lines.extend(ref_lines)
                # else skip TSF references
                
                i = j
                continue
            
            new_lines.append(line)
            i += 1
        
        new_yaml = '\n'.join(new_lines)
        # Remove empty 'references:' line if no references remain
        new_yaml = re.sub(r'references:\s*\n(?=[a-z]+:)', '', new_yaml)
        # Convert ID: EVID-L0-1 -> EVID_L0_1
        new_yaml = re.sub(r'^id: ([A-Z]+-[A-Z0-9]+-[0-9]+)', lambda m: f"id: {m.group(1).replace('-', '_')}", new_yaml, flags=re.MULTILINE)
        
        new_content = f"---{new_yaml}---{body}"
        target_file.write_text(new_content)
        copied += 1

print(f"✓ Copied and cleaned {copied} files")
PYTHON

echo ""

# Step 5: Mark all items as reviewed
echo "✅ Step 5: Marking items as reviewed..."
cd "$REPO_ROOT"

reviewed=0
failed=0
for prefix_dir in "$TRUDAG_ITEMS"/*; do
    if [ ! -d "$prefix_dir" ]; then
        continue
    fi
    
    PREFIX=$(basename "$prefix_dir")
    echo "  Processing $PREFIX..."
    
    for item_dir in "$prefix_dir"/*; do
        if [ ! -d "$item_dir" ]; then
            continue
        fi
        
        item_id=$(basename "$item_dir")
        full_id="$PREFIX-$item_id"
        
        echo "    Reviewing: $full_id"
        if trudag manage set-item "$full_id" 2>&1 | grep -q "Cannot get sha\|Invalid frontmatter\|error"; then
            echo "      ✗ Failed"
            failed=$((failed + 1))
        else
            echo "      ✓ Reviewed"
            reviewed=$((reviewed + 1))
        fi
    done
done

echo "✓ Marked $reviewed items as reviewed, Failed: $failed"
echo ""

# Step 6: Create logical links from graph.dot
echo "🔗 Step 6: Creating logical links from graph.dot..."

link_count=0
failed_links=0

# Function to convert short ID to DB format
convert_id() {
    local short_id="$1"
    
    if [[ $short_id =~ ^([A-Z]+)-(.*)$ ]]; then
        local prefix="${BASH_REMATCH[1]}"
        local rest="${BASH_REMATCH[2]}"
        
        case "$prefix" in
            EXPECT) full_prefix="EXPECTATIONS" ;;
            ASSERT) full_prefix="ASSERTIONS" ;;
            ASSUMP) full_prefix="ASSUMPTIONS" ;;
            EVID) full_prefix="EVIDENCES" ;;
            *) full_prefix="$prefix" ;;
        esac
        
        local full_id="${prefix}_${rest}"
        full_id="${full_id//-/_}"
        
        echo "${full_prefix}-${full_id}"
    fi
}

while IFS= read -r line; do
    if [[ $line =~ \"([^\"]+)\"[[:space:]]*-\>[[:space:]]*\"([^\"]+)\" ]]; then
        from_short="${BASH_REMATCH[1]}"
        to_short="${BASH_REMATCH[2]}"
        
        from=$(convert_id "$from_short")
        to=$(convert_id "$to_short")
        
        echo "  Creating link: $from -> $to"
        
        output=$(trudag manage create-link "$from" "$to" 2>&1)
        exit_code=$?
        
        if echo "$output" | grep -q "Could not fetch URL\|HTTP Error 404"; then
            echo "    ✓ Created (URL warning ignored)"
            link_count=$((link_count + 1))
        elif [ $exit_code -eq 0 ]; then
            echo "    ✓ Created"
            link_count=$((link_count + 1))
        else
            echo "    ✗ Failed: $output"
            failed_links=$((failed_links + 1))
        fi
    fi
done < "$GRAPH_DIR/graph.dot"

echo "✓ Links created: $link_count, Failed: $failed_links"
echo ""

# Step 7: Run lint
echo "🔍 Step 7: Running trudag lint..."
cd "$REPO_ROOT"
if trudag manage lint 2>&1; then
    echo "✓ Lint check complete"
else
    echo "! Some lint warnings/errors - review above"
fi
echo ""

# Step 8: Calculate score
echo "📊 Step 8: Calculating trustability scores..."
trudag score 2>&1
echo ""

echo "=========================================="
echo "✅ Setup complete!"
echo "=========================================="
echo ""
echo "Summary:"
echo "  - Database: $DB_FILE"
echo "  - Items: $TRUDAG_ITEMS"  
echo "  - Created: $created items"
echo "  - Reviewed: $reviewed items"
echo "  - Links: $link_count"
echo ""
echo "Next steps:"
echo "  - Run 'trudag score' for full scores"
echo "  - Run 'trudag manage lint' to validate"
echo "=========================================="
