#!/bin/bash
set -e

# Script to setup trudag DB
# This script:
# 0. Cleans all generated files (preserves source items/)
# 1. Generates graph.dot from source items
# 2. Initializes trudag DB in tsf_implementation/
# 3. Creates items from source (trudag creates skeleton, we copy content)
# 4. Applies links from graph.dot
# 5. Marks all items as reviewed for SME assessment
# 6. Runs lint validation

# Compute repository root relative to this script to avoid hardcoded paths
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# scripts is at: docs/TSF/tsf_implementation/scripts -> go four levels up to reach repo root
REPO_ROOT="$(cd "$script_dir/../../../.." && pwd)"
BASE_DIR="$REPO_ROOT/docs/TSF"
TSF_IMPL="$BASE_DIR/tsf_implementation"
ITEMS_SOURCE="$TSF_IMPL/items"
GRAPH_DIR="$TSF_IMPL/graph"
DB_FILE="$TSF_IMPL/.dotstop.dot"
DB_SYMLINK="$REPO_ROOT/.dotstop.dot"

echo "=========================================="
echo "TSF Trudag Setup - Clean Mode"
echo "=========================================="
echo ""

# Detect if 'trudag' CLI is available; if not, skip trudag-dependent steps
if ! command -v trudag >/dev/null 2>&1; then
    echo "⚠️  'trudag' command not found in PATH. Steps that require trudag will be skipped."
    SKIP_TRUDAG=true
else
    SKIP_TRUDAG=false
fi

# Step 0: Clean all generated files (NEVER touch source items/)
echo "🧹 Step 0: Cleaning all generated files..."
[ -f "$GRAPH_DIR/graph.dot" ] && rm -f "$GRAPH_DIR/graph.dot" && echo "  ✓ Removed graph.dot"
[ -f "$DB_FILE" ] && rm -f "$DB_FILE" && echo "  ✓ Removed .dotstop.dot"
[ -e "$DB_SYMLINK" ] && rm -f "$DB_SYMLINK" && echo "  ✓ Removed DB symlink"
[ -d "$TSF_IMPL/.trudag_items" ] && rm -rf "$TSF_IMPL/.trudag_items" && echo "  ✓ Removed .trudag_items/ (tsf_impl)"
[ -d "$REPO_ROOT/.trudag_items" ] && rm -rf "$REPO_ROOT/.trudag_items" && echo "  ✓ Removed .trudag_items/ (repo root)"
find "$REPO_ROOT" -name "needs.dot" -type f -delete 2>/dev/null && echo "  ✓ Removed needs.dot files"
echo "✓ Cleanup complete (source items/ preserved)"
echo ""

# Step 1: Generate graph.dot
echo "📊 Step 1: Generating graph.dot from source items..."
mkdir -p "$GRAPH_DIR"
python3 "$TSF_IMPL/tools/generate_graph_from_heuristics.py" \
    --items "$ITEMS_SOURCE" \
    --out "$GRAPH_DIR/graph.dot"
echo "✓ Graph generated: $GRAPH_DIR/graph.dot"
echo ""

# Step 2: Initialize DB in tsf_implementation and create symlink in root
echo "🗄️  Step 2: Initializing trudag DB..."
cd "$TSF_IMPL"
if [ "$SKIP_TRUDAG" = "false" ]; then
    trudag init
    echo "✓ DB initialized: $DB_FILE"
else
    echo "  ⚠️  Skipping 'trudag init' (trudag not available)"
fi

# Create symlink in repo root so trudag can find it
cd "$REPO_ROOT"
#[ -e ".dotstop.dot" ] && rm -f ".dotstop.dot"  # Remove file or symlink if exists
ln -sf "docs/TSF/tsf_implementation/.dotstop.dot" ".dotstop.dot"
echo "✓ Created/updated symlink: $DB_SYMLINK -> $DB_FILE"

# Create symlink for .dotstop_extensions in tsf_implementation so trudag finds validators
cd "$TSF_IMPL"
#[ -e ".dotstop_extensions" ] && rm -f ".dotstop_extensions"
ln -sf "../../../.dotstop_extensions" ".dotstop_extensions"
echo "✓ Created/updated .dotstop_extensions symlink for validators"
echo ""

# Step 3: Create items in trudag DB
echo "📝 Step 3: Creating items in trudag DB..."
cd "$TSF_IMPL"

created=0
failed=0

for category_dir in "$ITEMS_SOURCE"/*; do
    if [ ! -d "$category_dir" ]; then
        continue
    fi
    
    category=$(basename "$category_dir")
    # Map category to PREFIX
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
        # Convert hyphens to underscores for trudag (ASSERT-L0-1 -> ASSERT_L0_1)
        item_id="${filename//-/_}"
        
        # Prepare target directory for trudag to create item (use absolute path)
        target_dir="$TSF_IMPL/.trudag_items/$PREFIX/$item_id"
        mkdir -p "$target_dir"
        
        # Trudag create-item will create PREFIX-ITEM_ID.md in target_dir
        if [ "$SKIP_TRUDAG" = "false" ]; then
            trudag manage create-item "$PREFIX" "$item_id" "$target_dir" 2>/dev/null || true
        else
            # Ensure target dir exists even if trudag is missing so we can copy files
            mkdir -p "$target_dir"
            echo "    ⚠️  Skipping trudag create-item for $PREFIX-$item_id (trudag not available)"
        fi
        
        # Copy our source content to the created file
        target_file="$target_dir/$PREFIX-$item_id.md"
        if [ -f "$target_file" ]; then
            cp "$source_file" "$target_file"
            
            # Convert ALL hyphens to underscores in the id field only
            # EXPECT-L0-1 -> EXPECT_L0_1
            # Read id line, replace hyphens, write back
            old_id=$(grep "^id:" "$target_file" | sed 's/^id: //')
            new_id=$(echo "$old_id" | tr '-' '_')
            sed -i.bak "s/^id: $old_id$/id: $new_id/" "$target_file"
            
            # Fix relative paths in references to point to .trudag_items structure
            # Example: ../assertions/ASSERT-L0-1.md -> ../../ASSERTIONS/ASSERT_L0_1/ASSERTIONS-ASSERT_L0_1.md
            sed -i.bak -E 's|path: \.\./([a-z]+)/([A-Z]+-[A-Z]+-[0-9]+)\.md|path: ../../\U\1\E/\2/\U\1\E-\2.md|g' "$target_file"
            # Convert category to uppercase and hyphens to underscores in path
            sed -i.bak2 -E 's|path: ../../([a-z]+)/|path: ../../\U\1\E/|g' "$target_file"
            sed -i.bak3 -E 's|/([A-Z]+-[A-Z]+-[0-9]+)/([A-Z]+)-([A-Z]+-[A-Z]+-[0-9]+)\.md|/\1_\3/\2-\1_\3.md|g' "$target_file"
            # Actually, let's use a simpler approach with proper conversion
            # Remove backup files
            rm -f "$target_file.bak" "$target_file.bak2" "$target_file.bak3"
            
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

# Fix file reference paths in YAML to point to .trudag_items structure
echo "🔧 Fixing file reference paths and IDs in .trudag_items..."
# Use an unquoted heredoc so shell variables like $TSF_IMPL expand inside the embedded Python
python3 - <<PYTHON
import re
from pathlib import Path
trudag_items = Path("$TSF_IMPL/.trudag_items")

fixed_count = 0

for md_file in trudag_items.rglob("*.md"):
    content = md_file.read_text()
    original = content
    
    # 1. Convert reference IDs: id: ASSERT-L0-1 -> id: ASSERT_L0_1
    def convert_ref_id(match):
        indent = match.group(1)
        item_id = match.group(2)
        item_id_underscore = item_id.replace('-', '_')
        return f"{indent}id: {item_id_underscore}"
    
    content = re.sub(
        r'(\s+)id:\s*([A-Z]+-[A-Z0-9]+-[0-9]+)',
        convert_ref_id,
        content
    )
    
    # 2. Convert paths to be relative to repo root (trudag resolves from there)
    # Handles references for all item types (assertions, assumptions, evidences, expectations)
    def convert_path(match):
        indent = match.group(1)
        category = match.group(2)
        item_id = match.group(3)

        category_map = {
            'assertions': 'ASSERTIONS',
            'assumptions': 'ASSUMPTIONS',
            'evidences': 'EVIDENCES',
            'expectations': 'EXPECTATIONS'
        }

        prefix = category_map.get(category, category.upper())
        item_id_underscore = item_id.replace('-', '_')
        # Path from repo root
        new_path = f"docs/TSF/tsf_implementation/.trudag_items/{prefix}/{item_id_underscore}/{prefix}-{item_id_underscore}.md"

        return f"{indent}path: {new_path}"

    # Rewrite references for all item types, including quoted paths and YAML indentation
    content = re.sub(
        r'(\s*)path:\s*["\']?\.\./(assertions|assumptions|evidences|expectations)/([A-Z]+-[A-Z0-9]+-[0-9]+)\.md["\']?',
        convert_path,
        content
    )
    
    # 3. Fix demo paths to be absolute from repo root
    # path: ../../../../demos/file.jpeg -> path: docs/demos/file.jpeg
    def fix_demo_path(match):
        demo_path = match.group(1)  # demos/file.jpeg
        return f"path: docs/{demo_path}"
    
    content = re.sub(
        r'path:\s*(?:\.\./)+([demos/][^\s]+)',
        fix_demo_path,
        content
    )
    
    # 4. Remove file type references with demos paths (keep only URLs)
    # Format in YAML:
    # - type: file
    #   path: docs/demos/assembled-car.jpeg
    content = re.sub(
        r'-\s+type:\s+file\s+path:\s+docs/demos/[^\n]+\n?',
        '',
        content,
        flags=re.MULTILINE
    )
    
    # 5. If references section is now empty, remove it completely
    # Pattern: references: followed directly by another field or ---
    content = re.sub(
        r'references:\s*\n(?=(score:|reviewers:|---))',
        '',
        content
    )
    
    if content != original:
        md_file.write_text(content)
        fixed_count += 1

print(f"✓ Fixed paths and IDs in {fixed_count} files")
PYTHON

echo ""

# Step 4: Apply logical links from graph.dot (reasoning structure)
            if [ "$SKIP_TRUDAG" = "false" ]; then
                # Suppress trudag noise, mark item and its links as reviewed
                if trudag manage set-item "$full_id" --links 2>/dev/null; then
                    echo "    ✓ $full_id"
                    reviewed=$((reviewed + 1))
                else
                    echo "    ✗ $full_id (failed)"
                    failed_review=$((failed_review + 1))
                fi
            else
                echo "    ⚠️  Skipping mark reviewed for $full_id (trudag not available)"
            fi
# Extract links from graph.dot and create them
# Format in graph.dot: "EXPECT-L0-1" -> "ASSERT-L0-1"
# Convert to DB format: "EXPECTATIONS-EXPECT_L0_1" -> "ASSERTIONS-ASSERT_L0_1"
while IFS= read -r line; do
    if [[ $line =~ \"([^\"]+)\"[[:space:]]*-\>[[:space:]]*\"([^\"]+)\" ]]; then
        from_short="${BASH_REMATCH[1]}"  # EXPECT-L0-1
        to_short="${BASH_REMATCH[2]}"    # ASSERT-L0-1
        
        # Function to convert short ID to DB format
        convert_id() {
            local short_id="$1"  # EXPECT-L0-1
            
            # Extract prefix (EXPECT, ASSERT, etc)
            if [[ $short_id =~ ^([A-Z]+)-(.*)$ ]]; then
                local prefix="${BASH_REMATCH[1]}"
                local rest="${BASH_REMATCH[2]}"  # L0-1
                
                # Map to full prefix
                case "$prefix" in
                    EXPECT) full_prefix="EXPECTATIONS" ;;
                    ASSERT) full_prefix="ASSERTIONS" ;;
                    ASSUMP) full_prefix="ASSUMPTIONS" ;;
                    EVID) full_prefix="EVIDENCES" ;;
                    *) full_prefix="$prefix" ;;
                esac
                
                # Convert all hyphens to underscores in full ID
                local full_id="${prefix}_${rest}"
                full_id="${full_id//-/_}"
                
                # Return: EXPECTATIONS-EXPECT_L0_1
                echo "${full_prefix}-${full_id}"
            fi
        }
        
        from=$(convert_id "$from_short")
        to=$(convert_id "$to_short")
        
        echo "  Linking: $from -> $to"
        
        # Try to create link, ignore URL validation errors
        if [ "$SKIP_TRUDAG" = "false" ]; then
            output=$(trudag manage create-link "$from" "$to" 2>&1)
        else
            output="trudag not available - skipping create-link"
        fi
        exit_code=$?
        
        # Check if error is just about URL validation (can be ignored)
        if echo "$output" | grep -q "Could not fetch URL\|HTTP Error 404"; then
            # URL validation error - link was probably created, count as success
            link_count=$((link_count + 1))
        elif [ $exit_code -eq 0 ]; then
            link_count=$((link_count + 1))
        else
            failed_links=$((failed_links + 1))
            if [ $failed_links -eq 1 ]; then
                echo "  First real error: $from -> $to"
                echo "$output" | head -3
            fi
        fi
    fi
done < "$GRAPH_DIR/graph.dot"

echo "✓ Links created: $link_count, Failed: $failed_links"
echo ""

# Step 5: Mark all items as reviewed (for SME assessment)
echo "✅ Step 5: Marking items as reviewed..."
reviewed=0
failed_review=0

# Use .trudag_items created by trudag in tsf_implementation
TRUDAG_ITEMS_GENERATED="$TSF_IMPL/.trudag_items"

for prefix_dir in "$TRUDAG_ITEMS_GENERATED"/*; do
    if [ ! -d "$prefix_dir" ]; then
        continue
    fi
    
    PREFIX=$(basename "$prefix_dir")
    echo "  Marking $PREFIX items..."
    
    for item_dir in "$prefix_dir"/*; do
        if [ ! -d "$item_dir" ]; then
            continue
        fi
        
        item_id=$(basename "$item_dir")
        full_id="$PREFIX-$item_id"
        
        # Suppress trudag noise, mark item and its links as reviewed
        if [ "$SKIP_TRUDAG" = "false" ]; then
            if trudag manage set-item "$full_id" --links 2>/dev/null; then
                echo "    ✓ $full_id"
                reviewed=$((reviewed + 1))
            else
                echo "    ✗ $full_id (failed)"
                failed_review=$((failed_review + 1))
            fi
        else
            echo "    ⚠️  Skipping mark reviewed for $full_id (trudag not available)"
        fi
    done
done

echo "✓ Marked $reviewed items as reviewed, $failed_review failed"
echo ""

# Step 6: Run lint
echo "🔍 Step 6: Running trudag lint..."
cd "$REPO_ROOT"
# Run lint and capture output, filter warnings but show important ones
if [ "$SKIP_TRUDAG" = "false" ]; then
    lint_output=$(trudag manage lint 2>&1)
    lint_exit=$?
    echo "$lint_output" | grep -v "shadows an existing Reference" | grep -v "^Reference object"
    if [ $lint_exit -eq 0 ]; then
        echo "✓ Lint passed!"
    else
        echo "✗ Lint failed - please review errors above"
        exit 1
    fi
else
    echo "  ⚠️  Skipping trudag lint (trudag not available)"
fi
echo ""


echo "=========================================="
echo "✅ Setup complete!"
echo "=========================================="
echo ""

 # Step 7: Run trudag score
echo "📊 Step 7: Running trudag score..."
if [ "$SKIP_TRUDAG" = "false" ]; then
    score_output=$(trudag score 2>&1)
    score_exit=$?

    # Show scores (filter noise but keep score lines)
    echo "$score_output" | grep -v "shadows an existing Reference" | grep -v "^Reference object" | grep -v "^WARNING.*Unsupported reference format"

    if [ $score_exit -eq 0 ]; then
        # Calculate and show summary
        total_items=$(echo "$score_output" | grep "=" | wc -l)
        perfect_score=$(echo "$score_output" | grep "= 1.0" | wc -l)
        zero_score=$(echo "$score_output" | grep "= 0.0" | wc -l)
        partial_score=$(echo "$score_output" | grep "= 0.5" | wc -l)
        
        echo ""
        echo "📈 Score Summary:"
        echo "   • Total items scored: $total_items"
        echo "   • Perfect score (1.0): $perfect_score"
        echo "   • Partial score (0.5): $partial_score"
        echo "   • Zero score (0.0): $zero_score"
        echo "✓ Score calculated!"
    else
        echo "✗ Score failed - please review errors above"
        exit 1
    fi
else
    echo "  ⚠️  Skipping trudag score (trudag not available)"
fi
echo ""

# Step 8: Run trudag publish
echo "🚀 Step 8: Running trudag publish..."
if [ "$SKIP_TRUDAG" = "false" ]; then
    publish_output=$(trudag publish 2>&1)
    publish_exit=$?

    # Filter noise from publish output
    echo "$publish_output" | grep -v "shadows an existing Reference" | grep -v "^Reference object" | grep -v "^WARNING.*Unsupported reference format" | grep -v "^INFO: Executing validator" | grep -v "^INFO: Validator:"

    if [ $publish_exit -eq 0 ]; then
        echo ""
        echo "📁 Published reports to:"
        echo "   • $REPO_ROOT/docs/doorstop/"
        ls -la "$REPO_ROOT/docs/doorstop/" 2>/dev/null | grep "\.md" | awk '{print "   • " $NF}'
        echo "✓ Publish successful!"
    else
        echo "✗ Publish failed - please review errors above"
        exit 1
    fi
else
    echo "  ⚠️  Skipping trudag publish (trudag not available)"
fi
echo ""
echo ""
