FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    pkg-config \
    ninja-build \
    lcov \
    qt6-base-dev \
    qt6-base-dev-tools \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /work
