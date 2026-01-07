#!/usr/bin/env bash
set -euo pipefail

BIN="$1"
LOG_PREFIX="[gen-deps]"
CACHE_FILE="/tmp/gen_deps_cache.txt"
SKIP_LIBS=(
  "linux-vdso.so.1"
  "libc.so.6"
  "libm.so.6"
  "libstdc++.so.6"
  "libgcc_s.so.1"
  "ld-linux-x86-64.so.2"
)


log() {
  echo "${LOG_PREFIX} [$1] $2"
}

if [ -z "${BIN:-}" ]; then
  log ERROR "No binary provided"
  echo "Usage: $0 <binary>"
  exit 1
fi

if [ ! -x "$BIN" ]; then
  log ERROR "Binary not executable or not found: $BIN"
  exit 1
fi

log INFO "Started at $(date)"
log INFO "Analyzing binary: $BIN"
log INFO "Binary info: $(file "$BIN")"

log INFO "Running ldd"
if ldd "$BIN" | grep -q "not found"; then
  log ERROR "Missing shared libraries detected"
  ldd "$BIN" | grep "not found"
  exit 1
fi

LIBS=$(ldd "$BIN" | awk '/=>/ {print $1}' | sort -u)
LIB_COUNT=$(echo "$LIBS" | wc -l)

log INFO "Found $LIB_COUNT shared libraries"
log INFO "Resolving Debian packages via apt-file"

touch "$CACHE_FILE"

PACKAGES=()
UNRESOLVED=()
i=1

for lib in $LIBS; do
  if printf '%s\n' "${SKIP_LIBS[@]}" | grep -qx "$lib"; then
    log INFO "Skipping base system library: $lib"
    continue
  fi

  log INFO "[$i/$LIB_COUNT] Resolving $lib"
  i=$((i+1))

  # Cache lookup
  if grep -q "^$lib " "$CACHE_FILE"; then
    PKG=$(grep "^$lib " "$CACHE_FILE" | cut -d' ' -f2-)
  else
    PKG=$(apt-file search -x "^.*/$lib$" 2>/dev/null | cut -d: -f1 | sort -u | tr '\n' ' ')
    echo "$lib $PKG" >> "$CACHE_FILE"
  fi

  if [ -z "$PKG" ]; then
    UNRESOLVED+=("$lib")
  else
    PACKAGES+=($PKG)
  fi
done

UNIQUE_PACKAGES=$(printf "%s\n" "${PACKAGES[@]}" | sort -u)

log INFO "--------------------------------------------"
log INFO "Resolved packages:"
echo "$UNIQUE_PACKAGES"

if [ ${#UNRESOLVED[@]} -ne 0 ]; then
  log WARN "--------------------------------------------"
  log WARN "Unresolved libraries:"
  printf "%s\n" "${UNRESOLVED[@]}"
fi

log INFO "Finished at $(date)"
