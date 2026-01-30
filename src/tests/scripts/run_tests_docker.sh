#!/usr/bin/env bash
set -euo pipefail

IMAGE="${IMAGE:-ghcr.io/seame-pt/sea-me_team6_2025-26/seame-tests:latest}"
FALLBACK_IMAGE="${FALLBACK_IMAGE:-seame-tests:latest}"

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"

echo "[tests] root: ${ROOT_DIR}"
echo "[tests] preferred image: ${IMAGE}"

# See if PAT (Personal Access Token) is set for GHCR auth
if [[ -n "${GHCR_TOKEN:-}" ]]; then
  if [[ -z "${GHCR_USER:-}" ]]; then
    echo "[tests] ERROR: GHCR_TOKEN is set but GHCR_USER is not set."
    exit 1
  fi
  echo "[tests] logging into GHCR as ${GHCR_USER}..."
  echo "${GHCR_TOKEN}" | docker login ghcr.io -u "${GHCR_USER}" --password-stdin >/dev/null
else
  echo "[tests] GHCR_TOKEN not set -> will try pull without auth, then fallback to local build."
fi

# Try pull
if docker pull "${IMAGE}" >/dev/null 2>&1; then
  USE_IMAGE="${IMAGE}"
  echo "[tests] using GHCR image: ${USE_IMAGE}"
else
  echo "[tests] GHCR pull failed (missing access/image?). Falling back to local image: ${FALLBACK_IMAGE}"

  if ! docker image inspect "${FALLBACK_IMAGE}" >/dev/null 2>&1; then
    echo "[tests] local image not found; building from Dockerfile..."
    docker build -t "${FALLBACK_IMAGE}" -f "${ROOT_DIR}/Dockerfile" "${ROOT_DIR}"
  fi

  USE_IMAGE="${FALLBACK_IMAGE}"
fi

docker run --rm \
  -v "${ROOT_DIR}:/work" \
  -w /work/src/tests \
  "${USE_IMAGE}" \
  bash -lc '
    rm -rf build && \
    cmake -S . -B build -G Ninja -DENABLE_COVERAGE=ON -DENABLE_INTEGRATION_TESTS=OFF && \
    cmake --build build && \
    ctest --test-dir build --output-on-failure
  '