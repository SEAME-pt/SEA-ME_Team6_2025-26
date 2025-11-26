#!/usr/bin/env bash
# Simple helper: prints the graph file path and optionally runs dot (if installed)
set -e
GRAPH="$(dirname "$0")/../graph/graph.dot"
if [ "$1" = "render" ]; then
  if command -v dot >/dev/null 2>&1; then
    dot -Tpng "$GRAPH" -o graph.png && echo "Rendered graph.png"
  else
    echo "Graphviz 'dot' not installed"
    exit 2
  fi
else
  echo "Graph file: $GRAPH"
fi
