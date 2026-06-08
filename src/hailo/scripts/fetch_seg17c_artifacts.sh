#!/usr/bin/env bash
set -euo pipefail

# Download seg17c artifact bundle from Hugging Face Hub without browser popups.
# Required env vars:
#   HF_REPO_ID      e.g. your-user/sea-me-artifacts
#   HF_REMOTE_PATH  e.g. seg17c/20260427_120101/seg17c_artifacts_bundle.zip
# Optional env vars:
#   HF_REPO_TYPE    dataset|model (default: dataset)
#   HF_TOKEN        Hugging Face token (for private repos)
#   LOCAL_DIR       destination dir (default: ~/Documents/AI/Yolo_benchmark/trained/seg17c_bundle)

HF_REPO_ID="${HF_REPO_ID:-}"
HF_REMOTE_PATH="${HF_REMOTE_PATH:-}"
HF_REPO_TYPE="${HF_REPO_TYPE:-dataset}"
LOCAL_DIR="${LOCAL_DIR:-$HOME/Documents/AI/Yolo_benchmark/trained/seg17c_bundle}"

if [[ -z "$HF_REPO_ID" || -z "$HF_REMOTE_PATH" ]]; then
  echo "ERROR: set HF_REPO_ID and HF_REMOTE_PATH first." >&2
  echo "Example:" >&2
  echo "  HF_REPO_ID=your-user/sea-me-artifacts HF_REMOTE_PATH=seg17c/20260427_120101/seg17c_artifacts_bundle.zip bash fetch_seg17c_artifacts.sh" >&2
  exit 1
fi

if ! command -v hf >/dev/null 2>&1 && ! command -v huggingface-cli >/dev/null 2>&1; then
  python3 -m pip install --user -q huggingface_hub
  export PATH="$HOME/.local/bin:$PATH"
fi

mkdir -p "$LOCAL_DIR"

# Download bundle (non-interactive when HF_TOKEN is provided)
if command -v hf >/dev/null 2>&1; then
  download_cmd=(
    hf download "$HF_REPO_ID" "$HF_REMOTE_PATH"
    --type "$HF_REPO_TYPE"
    --local-dir "$LOCAL_DIR"
  )
elif command -v huggingface-cli >/dev/null 2>&1; then
  download_cmd=(
    huggingface-cli download "$HF_REPO_ID" "$HF_REMOTE_PATH"
    --repo-type "$HF_REPO_TYPE"
    --local-dir "$LOCAL_DIR"
  )
else
  echo "ERROR: neither 'hf' nor 'huggingface-cli' command is available." >&2
  exit 1
fi

if [[ -n "${HF_TOKEN:-}" ]]; then
  download_cmd+=(--token "$HF_TOKEN")
fi

"${download_cmd[@]}"

# hf download can place files under nested subfolders inside LOCAL_DIR.
BUNDLE_BASENAME="$(basename "$HF_REMOTE_PATH")"
BUNDLE_PATH="$(find "$LOCAL_DIR" -type f -name "$BUNDLE_BASENAME" | head -n 1 || true)"
if [[ -z "$BUNDLE_PATH" || ! -f "$BUNDLE_PATH" ]]; then
  echo "ERROR: bundle not found after download under: $LOCAL_DIR (expected name: $BUNDLE_BASENAME)" >&2
  exit 1
fi

# Extract and copy to final trained paths
unzip -o "$BUNDLE_PATH" -d "$LOCAL_DIR/extracted" >/dev/null

mkdir -p "$HOME/Documents/AI/Yolo_benchmark/trained/yolov8n_seg_17c/weights"
mkdir -p "$HOME/Documents/AI/Yolo_benchmark/trained/yolo26n_seg_17c/weights"

cp "$LOCAL_DIR/extracted/yolov8n_seg_17c_best.pt" "$HOME/Documents/AI/Yolo_benchmark/trained/yolov8n_seg_17c/weights/best.pt"
cp "$LOCAL_DIR/extracted/yolo26n_seg_17c_best.pt" "$HOME/Documents/AI/Yolo_benchmark/trained/yolo26n_seg_17c/weights/best.pt"

if [[ -f "$LOCAL_DIR/extracted/yolov8n_seg_17c_best.onnx" ]]; then
  cp "$LOCAL_DIR/extracted/yolov8n_seg_17c_best.onnx" "$HOME/Documents/AI/Yolo_benchmark/trained/yolov8n_seg_17c/weights/best.onnx"
fi
if [[ -f "$LOCAL_DIR/extracted/yolo26n_seg_17c_best.onnx" ]]; then
  cp "$LOCAL_DIR/extracted/yolo26n_seg_17c_best.onnx" "$HOME/Documents/AI/Yolo_benchmark/trained/yolo26n_seg_17c/weights/best.onnx"
fi

if [[ -f "$LOCAL_DIR/extracted/metrics.json" ]]; then
  cp "$LOCAL_DIR/extracted/metrics.json" "$HOME/Documents/AI/Yolo_benchmark/trained/yolov8n_seg_17c/metrics.json"
  cp "$LOCAL_DIR/extracted/metrics.json" "$HOME/Documents/AI/Yolo_benchmark/trained/yolo26n_seg_17c/metrics.json"
fi

echo "OK: artifacts available at:"
echo "  $HOME/Documents/AI/Yolo_benchmark/trained/yolov8n_seg_17c/weights"
echo "  $HOME/Documents/AI/Yolo_benchmark/trained/yolo26n_seg_17c/weights"
