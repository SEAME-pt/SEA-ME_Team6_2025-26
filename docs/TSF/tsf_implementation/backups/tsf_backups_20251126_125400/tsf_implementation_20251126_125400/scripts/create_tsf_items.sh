#!/usr/bin/env bash
set -euo pipefail

# create_tsf_items.sh
# - Dry-run by default: prints commands to create items for all markdown files under references/
# - To actually run create-item commands set DO_TRUDAG=1 in the environment
# - To actually run link creation set RUN_LINKS=1 in the environment (off by default)

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SEARCH_DIR="$ROOT/references"

echo "TSF create-items script (dry-run by default)"
echo "ROOT=$ROOT"
echo "DO_TRUDAG=${DO_TRUDAG:-0}  RUN_LINKS=${RUN_LINKS:-0}"

# Helper: extract id from YAML front-matter (first 'id:' occurrence)
extract_id() {
	awk '/^---/{p++;next} p==1 && /^id:/{print $2; exit}' "$1" || true
}

declare -A id_to_file

# First pass: collect ids
while IFS= read -r -d '' f; do
	id=$(extract_id "$f" || true)
	if [ -n "$id" ]; then
		id_to_file["$id"]="$f"
	else
		echo "WARN: no id found in $f"
	fi
done < <(find "$SEARCH_DIR" -type f -name "*.md" -print0 | sort -z)

TMPDIR="$ROOT/.trudag_tmp"
rm -rf "$TMPDIR" || true
mkdir -p "$TMPDIR"

# Second pass: create items (we create temp copies with IDs sanitized because
# the local trudag CLI expects filenames like PREFIX-<ID_WITH_UNDERSCORES>.md
for id in "${!id_to_file[@]}"; do
	f=${id_to_file[$id]}
	# split id into PREFIX and numeric id (e.g. EXPECT-L0-001 -> PREFIX=EXPECT, ID=L0-001)
	prefix="${id%%-*}"
	id_suffix="${id#*-}"
	# sanitize id for filesystem/Trudag by replacing '-' with '_'
	id_sanitized="${id_suffix//-/_}"
	dest_name="${prefix}-${id_sanitized}.md"
	dest_path="$TMPDIR/$dest_name"

	# copy the markdown to the temp dir with the expected filename
	cp "$f" "$dest_path"

	if [ "${DO_TRUDAG:-0}" = "1" ]; then
		echo "RUN: trudag manage create-item '$prefix' '$id_sanitized' '$TMPDIR'  (from $f)"
		trudag manage create-item "$prefix" "$id_sanitized" "$TMPDIR" || echo "trudag create-item failed for $f"
	else
		echo "DRY-RUN: trudag manage create-item '$prefix' '$id_sanitized' '$TMPDIR'  (from $f)"
	fi
done

# clean up temp dir (keep it if DO_TRUDAG=0 for inspection)
if [ "${DO_TRUDAG:-0}" = "1" ]; then
	rm -rf "$TMPDIR"
fi

# Simple linking heuristic: for EXPECT-<X> -> ASSERT-<X>, ASSERT-<X> -> EVID-<X>
if [ "${RUN_LINKS:-0}" = "1" ]; then
	echo "RUNNING link creation (heuristic)..."
	for id in "${!id_to_file[@]}"; do
		case "$id" in
			EXPECT-*)
				suffix=${id#EXPECT-}
				from="$id"
				to_assert="ASSERT-$suffix"
				if [ -n "${id_to_file[$to_assert]:-}" ]; then
					  echo "RUN: trudag manage create-link '$from' '$to_assert'"
					  trudag manage create-link "$from" "$to_assert" || echo "link create failed: $from -> $to_assert"
				fi
				;;
			ASSERT-*)
				suffix=${id#ASSERT-}
				from="$id"
				to_evid="EVID-$suffix"
				if [ -n "${id_to_file[$to_evid]:-}" ]; then
					  echo "RUN: trudag manage create-link '$from' '$to_evid'"
					  trudag manage create-link "$from" "$to_evid" || echo "link create failed: $from -> $to_evid"
				fi
				;;
		esac
	done
else
	echo "DRY-RUN: link creation skipped. To run links set RUN_LINKS=1"
	echo "Suggested link commands (preview):"
	for id in "${!id_to_file[@]}"; do
		case "$id" in
			EXPECT-*)
				suffix=${id#EXPECT-}
				from="$id"
				to_assert="ASSERT-$suffix"
				if [ -n "${id_to_file[$to_assert]:-}" ]; then
					  echo "trudag manage create-link '$from' '$to_assert'"
				fi
				;;
			ASSERT-*)
				suffix=${id#ASSERT-}
				from="$id"
				to_evid="EVID-$suffix"
				if [ -n "${id_to_file[$to_evid]:-}" ]; then
					  echo "trudag manage create-link '$from' '$to_evid'"
				fi
				;;
		esac
	done
fi

echo "Done."
