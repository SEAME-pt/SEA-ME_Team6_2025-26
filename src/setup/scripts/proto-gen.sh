#!/bin/bash
# ===========================================================================
# proto-gen.sh - Gen C++ stubs for Protobuf/gRPC
# Use: proto-gen.sh <proto-file> [output-dir]
# ===========================================================================

source /opt/agl-sdk/environment-setup-armv7vet2hf-neon-vfpv4-agl-linux-gnueabi

PROTO_FILE="$1"
OUTPUT_DIR="${2:-./generated}"

if [ -z "$PROTO_FILE" ]; then
    echo "Use: proto-gen.sh <proto-file> [output-dir]"
    exit 1
fi

mkdir -p "${OUTPUT_DIR}"

echo "Generating C++ stubs from: ${PROTO_FILE}"
echo "Output directory: ${OUTPUT_DIR}"
echo ""

# Protobuf stubs
protoc -I$(dirname ${PROTO_FILE}) \
    --cpp_out=${OUTPUT_DIR} \
    ${PROTO_FILE}

# gRPC stubs
protoc -I$(dirname ${PROTO_FILE}) \
    --grpc_out=${OUTPUT_DIR} \
    --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) \
    ${PROTO_FILE}

echo ""
echo "Generated files:"
ls -la ${OUTPUT_DIR}/