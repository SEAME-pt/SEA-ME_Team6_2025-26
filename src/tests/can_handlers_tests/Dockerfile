FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    ca-certificates \
    lcov \
    python3 \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /work
CMD ["bash"]
