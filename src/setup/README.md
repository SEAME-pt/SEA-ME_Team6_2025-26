# 🐋 Docker Setup Environments

> **Disclaimer:** This document was created with the assistance of AI technology. The AI provided information and suggestions based on the references, which were then reviewed and edited for clarity and relevance. While the AI aided in generating content, the final document reflects our thoughts and decisions.

## 📚 Index
  - [👋 Introduction](#sec-intro)
  - [🧠 Core Ideas](#sec-core-ideas)
  - [🏗️ Architecture Overview](#sec-architecture)
  - [🔧 Docker Components](#sec-docker-components)
  - [🛠️ Development Workflow](#sec-workflow)
  - [🚨 Issues Log](#sec-issues)
  - [🔗 Links](#sec-links)

---

<a id="sec-intro"></a>
## 👋 Introduction

Software development demands consistency, reproducibility and portability across different environments. Docker provides a containerization solution that encapsulates applications and their dependencies into isolated, portable units that run identically regardless of the host machine.  
This project uses a multi-stage Docker architecture to support both development and production workflows. The setup is designed to:  
- Provide a consistent development environment across all team members.
- Eliminate "works on my machine" issues by containerizing all dependencies.
- Support hot-reload during development for rapid iteration.
- Enable seamless integration with CI/CD pipeline.
- Facilitate cross-compilation for ARM64 targets (Raspberry Pi).
- Optimize production builds through layer caching and multi-stage building.

The containerized approach allows developers to work in an environment identical to production, run tests in isolation, and deploy with confidence knowing the exact runtime behavior. Wheter building locally, running tests or deploying to the hardware, the Docker setup ensures consistency at every stage of development lifecycle.

---

<a id="sec-core-ideas"></a>
## 🧠 Core Ideas

- **Shared base, specialized children.** A single `Dockerfile.base` builds the heavy, slow-changing dependencies once (CMake, Protobuf, gRPC). Every other image `FROM`s it, so we never rebuild these from source more than once.
- **Native dev vs. cross-compilation are separate concerns.** `Dockerfile.dev` targets your development machine (x86_64) with a full Qt6 + X11 stack for running and debugging the cluster UI locally. `Dockerfile.agl-sdk` / `Dockerfile.r5-agl-sdk` target the actual hardware (Raspberry Pi 4 / 5) via the AGL cross SDK, and never run the UI themselves — they just produce ARM binaries.
- **Toolchain-per-target.** Raspberry Pi 4 (armv7, 32-bit) and Raspberry Pi 5 (aarch64, 64-bit) need different AGL SDKs, toolchain files, and helper scripts, so each has its own Dockerfile instead of branching logic inside one image.
- **Reproducible builds.** Pinning `PROTOBUF_TAG`, `GRPC_TAG`, `ABSEIL_TAG`, `QT_VERSION`, and `SDK_VERSION` as build args means every teammate (and CI) compiles against the exact same versions.
- **Bind-mount the SDK installer instead of baking it into the repo.** The AGL SDK `.sh` installers are large binaries fetched separately and passed in via `--mount=type=bind` at build time, keeping the repo itself lightweight.

---

<a id="sec-architecture"></a>
## 🏗️ Architecture Overview

All images build on top of the same base layer. From there, the tree splits into a native development branch and two hardware-specific cross-compilation branches:

```
                     ┌──────────────────────────┐
                     │   Dockerfile.base         │
                     │   souzitaaaa/team6-base   │
                     │   (Debian bookworm)       │
                     │                           │
                     │  - Build essentials       │
                     │  - CMake (from source)    │
                     │  - Protobuf v25.0         │
                     │  - grpc_cpp_plugin        │
                     │  - aarch64 cross-gcc/g++  │
                     └─────────────┬─────────────┘
                                   │
              ┌────────────────────┼────────────────────┐
              │                    │                     │
              ▼                    ▼                     ▼
  ┌──────────────────────┐  ┌──────────────────────┐  ┌──────────────────────┐
  │ Dockerfile.dev       │  │ Dockerfile.agl-sdk    │  │ Dockerfile.r5-agl-sdk│
  │ cluster-dev          │  │ team6-agl-sdk         │  │ (RPi5 variant)        │
  │                      │  │                       │  │                       │
  │ Native x86_64        │  │ AGL SDK (armv7vet2hf) │  │ AGL SDK (aarch64)     │
  │ Qt6 built from       │  │ → Raspberry Pi 4      │  │ → Raspberry Pi 5      │
  │ source + X11         │  │   cross toolchain     │  │   cross toolchain     │
  │ (run/debug the UI    │  │ (produces ARM         │  │ (produces ARM         │
  │  on your machine)    │  │  binaries to deploy)  │  │  binaries to deploy)  │
  └──────────────────────┘  └──────────────────────┘  └──────────────────────┘
```

**Why this shape:** the base layer is expensive to build (compiling CMake, Protobuf and gRPC from source) but rarely changes, so it's built once and pushed to Docker Hub as `souzitaaaa/team6-base:bookworm`. The three leaf images then only add what's specific to their purpose — either a full native Qt6 toolchain for local development, or an AGL cross-SDK for a specific target board.

---

<a id="sec-docker-components"></a>
## 🔧 Docker Components

### `Dockerfile.base`
**Purpose:** shared foundation image with every heavy build dependency compiled once, so downstream images don't repeat the work.

- **Base:** `debian:bookworm`
- **Installs:** the full native + cross toolchain (`build-essential`, `gcc`/`g++-12-aarch64-linux-gnu` for ARM64 cross-compiling, X11/XCB dev headers, OpenGL, etc.)
- **Builds from source:**
  - **CMake** (latest, via `Kitware/CMake` bootstrap+make) — Debian's packaged CMake is too old for our needs.
  - **Protobuf `v25.0`** (with Abseil `20230802.0` as a submodule)
  - **gRPC `v1.60.1`** — only the `grpc_cpp_plugin` is built (tests, and non-C++ plugins are disabled) since we only need it for code generation.
- **Also installs:** a `sysroot-relativelinks.py` helper script (used when working with Yocto/AGL sysroots).
- **Published as:** `souzitaaaa/team6-base:bookworm` on Docker Hub — the other three Dockerfiles pull this image with `FROM` rather than rebuilding it.
- **Not run directly** — it has no entrypoint of its own; it exists purely to be built and pushed as a base layer.

### `Dockerfile.dev`
**Purpose:** native development environment for building, running, and debugging directly on your machine.

- **Extends:** `souzitaaaa/team6-base:bookworm`
- **Adds:** X11/XCB runtime libraries, `gdb`, `valgrind`, `clang-format`.
- **Rebuilds Abseil, Protobuf and gRPC** with `-DCMAKE_POSITION_INDEPENDENT_CODE=ON` and `gRPC_INSTALL=ON` — different flags than the base image, since here they need to be installed system-wide as shared libs for the native build rather than just supplying `grpc_cpp_plugin`.
- **Builds Qt `6.7.3` from source** (`qtbase`, `qtshadertools`, `qtdeclarative`, `qtsvg`) into `/opt/qt6`, enabling XCB and D-Bus, since this is what the QML cluster UI needs to render locally.
- **Sets up Kuksa TLS/JWT material** at `/etc/kuksa/tls` and `/etc/kuksa/jwt` (copies in `ca.crt` and `publisher.jwt` — these must exist in the build context).
- **Environment:** `PATH`, `LD_LIBRARY_PATH`, `CMAKE_PREFIX_PATH`, `QT_PLUGIN_PATH`, `QML_IMPORT_PATH` all point at `/opt/qt6`; `DISPLAY=:0` by default (overridden at `docker run` time).
- **Entry:** drops into `/bin/bash` in `/workspace`.

### `Dockerfile.agl-sdk`
**Purpose:** cross-compilation environment for **Raspberry Pi 4** (armv7, 32-bit, NEON/VFPv4), using the AGL (Automotive Grade Linux) SDK.

- **Extends:** `souzitaaaa/team6-base:bookworm`
- **Requires at build time:** the AGL SDK installer script `poky-agl-glibc-x86_64-agl-image-minimal-crosssdk-armv7vet2hf-neon-vfpv4-raspberrypi4-toolchain-20.0.2.sh` present in the build context (it's bind-mounted in, not copied into the image).
- **Installs the SDK** to `/opt/agl-sdk`, then verifies it via `verify-sdk.sh`.
- **Copies in helper scripts** (see [Helper Scripts](#helper-scripts) below) and a CMake toolchain file (`toolchains/agl-toolchain.cmake`).
- **Exposes env vars:** `AGL_SDK_PATH`, `AGL_SDK_VERSION`, `AGL_ENV_SCRIPT` (points at the SDK's `environment-setup-armv7vet2hf-neon-vfpv4-agl-linux-gnueabi`), `AGL_TOOLCHAIN_FILE`, `QT_TOOLCHAIN_FILE`.
- **Entrypoint:** `agl-env`, which sources the AGL environment-setup script before dropping into (or running a command in) the SDK-configured shell.

### `Dockerfile.r5-agl-sdk`
**Purpose:** the same idea as `Dockerfile.agl-sdk`, but for **Raspberry Pi 5** (aarch64, 64-bit).

- Structurally identical to `Dockerfile.agl-sdk`, but everything is the aarch64 variant:
  - SDK installer: `poky-agl-glibc-x86_64-agl-image-minimal-crosssdk-aarch64-raspberrypi5-toolchain-20.0.2.sh`
  - Toolchain file: `toolchains/r5-agl-toolchain.cmake`
  - Helper scripts: the `r5-*` variants (`r5-agl-env`, `r5-qt6-build.sh`, `r5-proto-gen.sh`, `r5-verify-sdk.sh`)
  - `AGL_ENV_SCRIPT` points at `environment-setup-aarch64-agl-linux`
- Same entrypoint pattern (`agl-env` inside the image, backed by the `r5-agl-env` script).

---

<a id="sec-workflow"></a>
## 🛠️ Development Workflow

### 1. Build the base image (once, or whenever its deps change)
```bash
DOCKER_BUILDKIT=1 docker build -f Dockerfile.base -t souzitaaaa/team6-base:bookworm .
```
This is the slow build (compiling CMake/Protobuf/gRPC from source). You normally won't need to rebuild it unless `PROTOBUF_TAG`, `GRPC_TAG`, or the base dependency list changes.

### 2. Native development (`Dockerfile.dev`)
Use this for day-to-day UI work — editing QML/C++, running the cluster app, and debugging with `gdb`/`valgrind` against a real display.

**Build:**
```bash
docker build -f Dockerfile.dev -t cluster-dev:latest .
```

**Run (Linux):**
```bash
sudo docker run -it --rm \
  -v $(pwd):/workspace \
  -e DISPLAY=$DISPLAY \
  -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
  --network host \
  --privileged \
  cluster-dev:latest
```

**Run (Windows):**
```bash
docker run -it --rm \
  -e DISPLAY=host.docker.internal:0 \
  --add-host=host.docker.internal:host-gateway \
  -v ${PWD}/cluster:/workspace \
  cluster-dev:latest
```

Once inside, `/workspace` source directory, Qt6 is already on `PATH`, and you can build with CMake as usual. Since the source is bind-mounted rather than copied in, edits made on the host are reflected immediately — rebuild inside the container to pick them up (there's no hot-reload/watch mode configured, so a rebuild step is required after each change).

### 3. Cross-compiling for the target hardware
Use `Dockerfile.agl-sdk` (RPi4) or `Dockerfile.r5-agl-sdk` (RPi5) once you're ready to produce binaries that will actually run on the board.

**Prerequisite:** download the matching AGL SDK installer `.sh` and place it in the same directory as the Dockerfile before building — the build will fail without it, since it's bind-mounted in via `--mount=type=bind`.

**Build (RPi4 example):**
```bash
DOCKER_BUILDKIT=1 docker build -f Dockerfile.agl-sdk -t team6-agl-sdk:latest .
```

**Run:**
```bash
sudo docker run -it --rm -v $(pwd):/workspace team6-agl-sdk:latest
```

The entrypoint (`agl-env`) automatically sources the AGL cross-environment, so inside the container `$CC`, `$CXX`, and pkg-config paths are already pointed at the cross-toolchain. From there:
```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$AGL_TOOLCHAIN_FILE
cmake --build build -j$(nproc)
```
produces ARM binaries under `build/`, ready to be copied to the Pi.

Repeat the same steps with `Dockerfile.r5-agl-sdk` / `team6-r5-agl-sdk` for the Raspberry Pi 5 target.

### Debugging tips
- `Dockerfile.dev` includes `gdb`, `valgrind`, and `clang-format` — run them directly inside the container against the native build.
- Cross-compiled binaries (from the AGL SDK images) aren't runnable on the host; debug those on-device or via a cross-`gdb` from the SDK, if available.


---

<a id="sec-issues"></a>
## 🚨 Issues Log

This section tracks common issues encountered during development, testing or deployment of the instrument cluster.  
Each issue includes its symptoms, probable cause and recommended fix or workaround.

<!-- <a id="issue-x"></a>

### Issue #x - 
**- Error Example:**

**- Cause:** 

**- Solution:** -->

---

<a id="sec-links"></a>
## 🔗 Links

- [Automotive Grade Linux (AGL)](https://www.automotivelinux.org/)
- [Qt 6 Documentation](https://doc.qt.io/qt-6/)
- [Eclipse Kuksa Project](https://eclipse.dev/kuksa/)
- [Protocol Buffers](https://protobuf.dev/)
- [gRPC](https://grpc.io/)
- [CMake Toolchain Files](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html)

---

> **Document Version:** 2.0  
  **Last Updated:** 16st July 2026
  **Contributor:** souzitaaaa
