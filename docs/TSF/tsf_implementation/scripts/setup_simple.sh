#!/bin/bash
set -e

echo "=========================================="
echo "TSF Trudag Setup - Simple Mode"
echo "=========================================="

REPO_ROOT="/Volumes/Important_Docs/42/SEA-ME_Team6_2025-26"
TSF_IMPL="$REPO_ROOT/docs/TSF/tsf_implementation"
ITEMS_SOURCE="$TSF_IMPL/items"
TRUDAG_ITEMS="$TSF_IMPL/.trudag_items"
GRAPH_DIR="$TSF_IMPL/graph"
DB_FILE="$TSF_IMPL/.dotstop.dot"
DB_SYMLINK="$REPO_ROOT/.dotstop.dot"

# Step 0: Cleanup
echo "🧹 Cleanup..."
rm -rf "$TRUDAG_ITEMS" "$GRAPH_DIR/graph.dot" "$DB_FILE"
[ -L "$DB_SYMLINK" ] && rm -f "$DB_SYMLINK"
echo "✓ Cleanup done"

# Step 1: Generate graph
echo "📊 Generating graph..."
mkdir -p "$GRAPH_DIR"
python3 "$TSF_IMPL/tools/generate_graph_from_heuristics.py" \
    --items "$ITEMS_SOURCE" \
    --out "$GRAPH_DIR/graph.dot"
echo "✓ Graph generated"

# Step 2: Init DB
echo "🗄️  Initializing DB..."
cd "$TSF_IMPL"
trudag init
cd "$REPO_ROOT"
ln -s "docs/TSF/tsf_implementation/.dotstop.dot" ".dotstop.dot"
echo "✓ DB initialized with symlink"

# Step 3: Create items
echo "📝 Creating items..."
cd "$REPO_ROOT"

create_items() {
    local category="$1"
    local prefix="$2"
    
    echo "  Processing $prefix..."
    
    for source_file in "$ITEMS_SOURCE/$category"/*.md; do
        [ -f "$source_file" ] || continue
        
        filename=$(basename "$source_file" .md)
        item_id="${filename//-/_}"
        
        # Create item directory
        item_dir="$TRUDAG_ITEMS/$prefix/$item_id"
        mkdir -p "$item_dir"
        
        # Check if already exists
        if trudag manage show-item "$prefix-$item_id" >/dev/null 2>&1; then
            echo "    ○ $prefix-$item_id (already exists)"
            continue
        fi
        
        # Create item (trudag creates template)
        if trudag manage create-item "$prefix" "$item_id" "$item_dir" >/dev/null 2>&1; then
            # Copy our content over the template
            cp "$source_file" "$item_dir/$prefix-$item_id.md"
            echo "    ✓ $prefix-$item_id"
        else
            echo "    ✗ $prefix-$item_id (failed)"
        fi
    done
}

create_items "assertions" "ASSERTIONS"
create_items "assumptions" "ASSUMPTIONS"
create_items "evidences" "EVIDENCES"
create_items "expectations" "EXPECTATIONS"

echo "✓ Items created"

# Step 4: Create links
echo "🔗 Creating links..."
cd "$REPO_ROOT"
link_count=0

# Function to convert graph ID to trudag ID
convert_id() {
    local id="$1"
    # EXPECT-L0-1 -> EXPECTATIONS-EXPECT_L0_1
    local short_prefix="${id%%-*}"  # EXPECT
    local rest="${id#*-}"           # L0-1
    local item_id="${rest//-/_}"    # L0_1
    
    case "$short_prefix" in
        EXPECT) echo "EXPECTATIONS-${short_prefix}_${item_id}" ;;
        ASSERT) echo "ASSERTIONS-${short_prefix}_${item_id}" ;;
        ASSUMP) echo "ASSUMPTIONS-${short_prefix}_${item_id}" ;;
        EVID) echo "EVIDENCES-${short_prefix}_${item_id}" ;;
        *) echo "${short_prefix}-${item_id}" ;;
    esac
}

while IFS= read -r line; do
    if [[ $line =~ \"([^\"]+)\"[[:space:]]*-\>[[:space:]]*\"([^\"]+)\" ]]; then
        from_graph="${BASH_REMATCH[1]}"
        to_graph="${BASH_REMATCH[2]}"
        
        # Convert to trudag format
        from=$(convert_id "$from_graph")
        to=$(convert_id "$to_graph")
        
        if trudag manage create-link "$from" "$to" 2>/dev/null; then
            ((link_count++)) || true
        fi
    fi
done < "$GRAPH_DIR/graph.dot"

echo "✓ Created $link_count links"

# Step 5: Mark as reviewed
echo "✅ Marking items as reviewed..."
reviewed=0

for prefix_dir in "$TRUDAG_ITEMS"/*; do
    [ -d "$prefix_dir" ] || continue
    PREFIX=$(basename "$prefix_dir")
    
    for item_dir in "$prefix_dir"/*; do
        [ -d "$item_dir" ] || continue
        item_id=$(basename "$item_dir")
        
        if trudag manage set-item "$PREFIX-$item_id" 2>/dev/null; then
            ((reviewed++)) || true
        fi
    done
done

echo "✓ Marked $reviewed items as reviewed"

# Step 6: Lint
echo "🔍 Running lint..."
cd "$REPO_ROOT"
trudag manage lint

echo ""
echo "=========================================="
echo "✅ Setup complete!"
echo "=========================================="
echo "Run 'trudag score' to calculate scores"
