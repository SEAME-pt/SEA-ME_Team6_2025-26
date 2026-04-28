#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="${1:-$HOME/Documents/AI/hailo/shared_with_docker}"
TIMESTAMP="$(date +%Y%m%d_%H%M%S)"
REPORT_DIR="$ROOT_DIR/ops/reports"
REPORT_FILE="$REPORT_DIR/hailo_audit_${TIMESTAMP}.txt"

mkdir -p "$REPORT_DIR"

print_section() {
  printf '\n==== %s ====\n' "$1" | tee -a "$REPORT_FILE"
}

print_kv() {
  printf '%-30s %s\n' "$1" "$2" | tee -a "$REPORT_FILE"
}

: > "$REPORT_FILE"

echo "Hailo audit report" | tee -a "$REPORT_FILE"
echo "Generated at: $(date +"%Y-%m-%dT%H:%M:%S%z")" | tee -a "$REPORT_FILE"
echo "Root: $ROOT_DIR" | tee -a "$REPORT_FILE"

if [[ ! -d "$ROOT_DIR" ]]; then
  echo "ERROR: root directory not found: $ROOT_DIR" | tee -a "$REPORT_FILE"
  exit 1
fi

print_section "Current top-level tree"
find "$ROOT_DIR" -maxdepth 2 -mindepth 1 -print | sed "s|$ROOT_DIR|.|" | sort | tee -a "$REPORT_FILE"

print_section "Known duplicated archives/logs"
for pattern in \
  "annotations_trainval2017.zip" \
  "annotations_trainval2017.zip.*" \
  "val2017.zip" \
  "*.log"; do
  echo "Pattern: $pattern" | tee -a "$REPORT_FILE"
  find "$ROOT_DIR" -type f -name "$pattern" -print | sort | tee -a "$REPORT_FILE" || true
  echo | tee -a "$REPORT_FILE"
done

print_section "Model artifacts summary"
for ext in onnx pt har hef yaml; do
  count="$(find "$ROOT_DIR" -type f -name "*.${ext}" | wc -l | tr -d ' ')"
  print_kv "*.${ext}" "$count files"
  find "$ROOT_DIR" -type f -name "*.${ext}" -print | sort | tee -a "$REPORT_FILE"
  echo | tee -a "$REPORT_FILE"
done

print_section "COCO and calibration checks"
COCO_VAL="$ROOT_DIR/COCO/val2017"
CALIB="$ROOT_DIR/calibration_images"
OUT_DIR="$ROOT_DIR/outputs"

if [[ -d "$COCO_VAL" ]]; then
  coco_count="$(find "$COCO_VAL" -type f \( -iname '*.jpg' -o -iname '*.jpeg' -o -iname '*.png' \) | wc -l | tr -d ' ')"
  print_kv "COCO val images" "$coco_count"
else
  print_kv "COCO val images" "missing"
fi

if [[ -d "$CALIB" ]]; then
  calib_count="$(find "$CALIB" -type f \( -iname '*.jpg' -o -iname '*.jpeg' -o -iname '*.png' \) | wc -l | tr -d ' ')"
  print_kv "calibration images" "$calib_count"
else
  print_kv "calibration images" "missing"
fi

if [[ -d "$OUT_DIR" ]]; then
  out_count="$(find "$OUT_DIR" -type f \( -iname '*.jpg' -o -iname '*.jpeg' -o -iname '*.png' \) | wc -l | tr -d ' ')"
  print_kv "outputs images" "$out_count"
else
  print_kv "outputs images" "missing"
fi

print_section "Potential duplicate file content (same SHA256)"
TMP_HASH="$REPORT_DIR/.tmp_hash_${TIMESTAMP}.txt"
find "$ROOT_DIR" -type f -maxdepth 6 \( -name '*.onnx' -o -name '*.pt' -o -name '*.har' -o -name '*.hef' -o -name '*.yaml' -o -name '*.zip' \) \
  -exec shasum -a 256 {} \; > "$TMP_HASH" || true

awk '{hash=$1; $1=""; sub(/^ /,""); files[hash]=files[hash] ORS $0; count[hash]++}
END {for (h in count) if (count[h] > 1) {print "SHA256: " h; print files[h]; print ""}}' "$TMP_HASH" | tee -a "$REPORT_FILE"
rm -f "$TMP_HASH"

print_section "Recommended canonical layout"
cat <<'EOF' | tee -a "$REPORT_FILE"
./
├── COCO/
│   └── val2017/
├── calibration_images/
├── outputs/
├── models/
│   ├── yolov8n.pt
│   ├── yolov8n.onnx
│   ├── yolov8n.har
│   └── yolov8n.hef
├── scripts/
├── logs/
└── ops/
    ├── backup/
    └── reports/
EOF

print_section "Report location"
echo "$REPORT_FILE" | tee -a "$REPORT_FILE"

echo "Audit finished. Report: $REPORT_FILE"
