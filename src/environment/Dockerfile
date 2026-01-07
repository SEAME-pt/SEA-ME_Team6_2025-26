FROM ubuntu:24.04 as build

#! Base Tools
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    bash \
    wget \
    unzip \
    python3 \
    python3-pip \
    libssl-dev \
    lcov \
    ffmpeg \
    libxcb-xinerama0 \
    libxcb-cursor0 \
    libxkbcommon-x11-0 \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libxcb-icccm4 \
    libxcb-image0 \
    libxcb-keysyms1 \
    libxcb-render-util0 \
    libxcb-shape0 \
    && rm -rf /var/lib/apt/lists/*

#! Download and install Qt 6.7.3
#! REVER ESTA LÒGICA (COPIAR A DO CROSS COMPILER)
# WORKDIR /tmp
# RUN wget https://download.qt.io/official_releases/qt/6.7/6.7.3/single/qt-everywhere-src-6.7.3.tar.xz && \
#     tar xf qt-everywhere-src-6.7.3.tar.xz && \
#     cd qt-everywhere-src-6.7.3 && \
#     ./configure -prefix /opt/Qt/6.7.3 -opensource -confirm-license -nomake examples -nomake tests && \
#     cmake --build . --parallel $(nproc) && \
#     cmake --install . && \
#     cd /tmp && rm -rf qt-everywhere-src-6.7.3*

# ENV PATH="/opt/Qt/6.7.3/bin:${PATH}"
# ENV CMAKE_PREFIX_PATH="/opt/Qt/6.7.3"

WORKDIR /app
COPY ./tests/unit/python/requirements.txt ./tests/unit/python/requirements.txt
RUN pip install --break-system-packages -r ./tests/unit/python/requirements.txt
COPY . .
RUN cmake -S cluster -B cluster/build
RUN cmake --build cluster/build
RUN cmake -S tests -B tests/build
RUN cmake --build tests/build
RUN ctest --test-dir tests/build --output-on-failure