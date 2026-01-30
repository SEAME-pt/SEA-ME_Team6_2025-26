#!/usr/bin/env bash
set -euo pipefail

IMAGE="${IMAGE:-seame-tests:latest}"
REPO_ROOT="$(git rev-parse --show-toplevel)"
TESTS_DIR="${REPO_ROOT}/src/tests"

docker run --rm -t \
  -v "${REPO_ROOT}:/work" \
  -w /work/src/tests \
  "${IMAGE}" \
  bash -lc '\
    rm -rf build coverage_html coverage.info coverage_filtered.info && \
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_CPP_TESTS=ON && \
    cmake --build build -j && \
    ctest --test-dir build --output-on-failure && \
    lcov --capture --directory build --output-file coverage.info && \
    lcov --remove coverage.info "/usr/*" "*/_deps/*" "*/googletest/*" "*/tests/*" "*/inc/*" "*/unit/cpp/*" "*/build/*" -o coverage_filtered.info && \
    genhtml coverage_filtered.info --output-directory coverage_html \
'