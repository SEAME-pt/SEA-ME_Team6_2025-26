#!/bin/bash
# Find all transitive dependencies

cd /workspace
SYSROOT="/opt/agl-sdk/sysroots/aarch64-agl-linux/usr/lib"

echo "Finding all transitive dependencies..."

# Start with the executable
libs_to_check=("cluster/build-arm/HelloQt6Qml")
found_libs=()
checked_libs=()

while [ ${#libs_to_check[@]} -gt 0 ]; do
    current="${libs_to_check[0]}"
    libs_to_check=("${libs_to_check[@]:1}") # Remove first element
    
    # Skip if already checked
    if [[ " ${checked_libs[@]} " =~ " ${current} " ]]; then
        continue
    fi
    
    checked_libs+=("$current")
    
    # Get dependencies
    deps=$(aarch64-agl-linux-readelf -d "$current" 2>/dev/null | grep NEEDED | awk '{print $5}' | tr -d '[]')
    
    for dep in $deps; do
        # Find the full path in sysroot
        if [ -f "$SYSROOT/$dep" ] || [ -L "$SYSROOT/$dep" ]; then
            full_path="$SYSROOT/$dep"
            
            # Add to found libs if not already there
            if [[ ! " ${found_libs[@]} " =~ " ${dep} " ]]; then
                found_libs+=("$dep")
                echo "Found: $dep"
            fi
            
            # Add to check queue if not already checked
            if [[ ! " ${checked_libs[@]} " =~ " ${full_path} " ]]; then
                libs_to_check+=("$full_path")
            fi
        fi
    done
done

echo ""
echo "========================================="
echo "Total unique libraries needed: ${#found_libs[@]}"
echo "========================================="
echo ""

# Now copy all found libraries
echo "Copying all required libraries..."
mkdir -p deploy/lib

for lib in "${found_libs[@]}"; do
    # Copy the library and all its symlinks
    cp -P "$SYSROOT/$lib"* deploy/lib/ 2>/dev/null
done

echo "Done! Copied to deploy/lib/"
ls deploy/lib | wc -l
echo "files"