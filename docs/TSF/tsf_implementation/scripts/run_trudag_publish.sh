#!/usr/bin/env bash
# Wrapper to run the trudag publish flow for the repo
set -e
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"

if ! command -v trudag >/dev/null 2>&1; then
  echo "trudag not found in PATH. Install trudag and retry." >&2
  exit 2
fi

echo "Running trudag init (idempotent)"
trudag init || true

echo "Creating items from files (idempotent)"
for p in docs/TSF/tsf_implementation/items/expectations/*.md; do
  [ -f "$p" ] || continue
  trudag manage create-item "$p" || true
done
for p in docs/TSF/tsf_implementation/items/assertions/*.md; do
  [ -f "$p" ] || continue
  trudag manage create-item "$p" || true
done
for p in docs/TSF/tsf_implementation/items/evidences/*.md; do
  [ -f "$p" ] || continue
  trudag manage create-item "$p" || true
done

# create basic links using graph if present
if [ -f docs/TSF/tsf_implementation/graph/graph.dot ]; then
  echo "Graph exists; ensure links correspond to graph (manual step may be required)."
fi

echo "Linting"
trudag manage lint || true

echo "Scoring"
trudag score || true

echo "Publishing (this will generate/refresh .dotstop.dot)"
trudag publish || true

echo "trudag publish complete"
