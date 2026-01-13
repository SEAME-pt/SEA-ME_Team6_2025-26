#!/bin/bash
set -e

echo "Rebuilding base (no cache)..."
docker build -f cross-compiler/Dockerfile.base -t souzitaaaa/qt-cross-base:bookworm .
docker push souzitaaaa/qt-cross-base:bookworm

echo "Rebuilding sysroot (with cache)..."
docker build -f cross-compiler/Dockerfile.sysroot -t souzitaaaa/qt-cross-sysroot:bookworm .
docker push souzitaaaa/qt-cross-sysroot:bookworm

echo "Rebuilding Qt (with cache)..."
docker build -f cross-compiler/Dockerfile.qt -t souzitaaaa/qt-cross-compiled:6.7.3-bookworm .
docker push souzitaaaa/qt-cross-compiled:6.7.3-bookworm

echo "Done! All images updated."