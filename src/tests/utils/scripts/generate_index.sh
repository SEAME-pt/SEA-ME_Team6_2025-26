#!/bin/bash
set -e

COVERAGE_DIR="$1"

mkdir -p "$COVERAGE_DIR"

cat > "$COVERAGE_DIR/index.html" << 'EOF'
<!DOCTYPE html>
<html>
<head>
  <meta charset='utf-8'>
  <title>SEA:ME Team 6 – Coverage Report</title>
  <style>
    body { font-family: Arial, sans-serif; margin: 40px; }
    h1 { color: #333; }
    ul { list-style-type: none; padding: 0; }
    li { margin: 10px 0; }
    a { color: #0066cc; text-decoration: none; font-size: 18px; }
    a:hover { text-decoration: underline; }
  </style>
</head>
<body>
  <h1>SEA:ME Team 6 – Coverage Report</h1>
  <ul>
    <li><a href='cpp/index.html'>C++ Coverage</a></li>
    <li><a href='python/index.html'>Python Coverage</a></li>
  </ul>
</body>
</html>
EOF

echo "✓ Coverage index generated at $COVERAGE_DIR/index.html"