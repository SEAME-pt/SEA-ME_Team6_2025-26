# 🚀 Cross-Compiler

## 📚 Index
- [👋 Introduction](#sec-intro)
- [🧠 Core Ideas](#sec-core-ideas)
- [🤔 Why we use Cross-Compilation](#sec-why-cross)
- [🏗️ System Architecture](#sec-system-architecture)
- [⚙️ How it works (Step by Step)](#sec-how-it-works)
- [🧰 Building with Docker](#sec-build-docker)
- [📦 Deploying to Raspberry Pi](#sec-deploy-rasp)
- [🚨 Issues Log](#sec-issues)
- [🔗 Links](#sec-links)

---

<a id="sec-intro"></a>
## 👋 Introduction

Cross compiler is a type of compiler capable of **creating executable code** for a operating system other than the on which the compiler is running.  
A Cross compiler is useful whe the target system has **limited resources**, a **different architecture**, or a **different operating system** than the development machine.  
Cross-compilation allows developers to *build software* for **multiple platforms** from a single computer without needing the target device to compile the code itself.   

---

<a id="sec-core-ideas"></a>
## 🧠 Core Ideas

- **Host**: The system where the compiler runs.
- **Target**: The system where the compiled program will run.
- **Cross-compilation**: The process of compiling for a **target** that is **different** from the **host**
- **Toolchain**: Collection of **compilers**, **tools** and **libraries** required for compiling.
- **Sysroot**: Directory containing the target system's **libraries** and **headers** to ensure proper **linking** and **compatibility**.

---

<a id="sec-why-cross"></a>
## 🤔 Why we use Cross-Compilation

? 

---

<a id="sec-system-architecture"></a>
## 🏗️ System Architecture

? 

---

<a id="sec-how-it-works"></a>
## ⚙️ How it works (Step by Step)

? 

---

<a id="sec-build-docker"></a>
## 🧰 Building with Docker

? 

---

<a id="sec-deploy-rasp"></a>
## 📦 Deploying to Raspberry Pi

? 

---

<a id="sec-issues"></a>
## 🚨 Issues Log

<a id="issue-x"></a>

### Issue #x - 
**- Error Example:**

**- Cause:** 

**- Solution:**

<!-- <a id="issue-x"></a>

### Issue #x - 
**- Error Example:**

**- Cause:** 

**- Solution:** -->

---

<!-- <a id="sec-summary"></a>
## 🧩 Summary

1. **Write Source Code** → Program in a high-level language like C++.
2. Instead of a native compiler, we use a compiler configurated for a **different target platform**.
3. Source code is preprocessed: **macros** are expanded, **headers** are included and code is prepared for compilation.
4. Cross-compiler translates the **preprocessed code** into **target-specific** assembly code.
5. The object is linked with **target libraries** and **system APIs** from the sysroot to produce the final executable.
6. It creates a program that runs on the target system.
7. The compiled executable is copied or **deployed** to the **target** device.

<div align="center">
  <img src="https://media.geeksforgeeks.org/wp-content/uploads/20220915123006/Crosscompiler-660x330.png" />
</div>

--- -->

<!-- <a id="sec-how-it-works"></a>
## 📄 How our Cross-Compiler works

Our **host machines** are running `Ubuntu x86_64`, while the **target machine** uses `Bookworm aarch64`. To simplify working across two different operating systems, we decided to use a **cross-compiler** with Docker.  

Thanks to the work of Mr. [PhysicsX](https://github.com/PhysicsX), we were able to incorporate mostly of his work into our project by following the instructions and explanations provided in his [GitHub repository](https://github.com/PhysicsX/QTonRaspberryPi?tab=readme-ov-file) and [Youtube tutorial](https://youtu.be/5XvQ_fLuBX0?si=WcI7c6L5jJSLzCvF).  

The cross-compiler works by creating **two Docker containers** using different Dockerfiles: one simulates the OS of our target, (this allow us to build Qt without needing a Raspberry Pi) and the other serves as the **cross-compiler**, responsible for preparing the executable so it can be run on the target machine.  

This README will only cover the core components of how the Dockerfiles work. For more detailed information about how it was done and how to install it, we recommend consulting the work of Mr. [PhysicsX's](https://github.com/PhysicsX) work.  

`DockerFileRasp (Target):`  

```dockerfile
FROM arm64v8/debian:bookworm

ENV DEBIAN_FRONTEND=noninteractive

RUN touch /build.log
```

Definition of the base image **Debian Bookworn (arm64)** and environment setup by disabling **interactive prompts** during installation and creation of a **build.log file** to log the build output.  

```dockerfile
RUN { \
   echo "deb http://deb.debian.org/debian bookworm main contrib non-free" > /etc/apt/sources.list && \
    echo "deb-src http://deb.debian.org/debian bookworm main contrib non-free" >> /etc/apt/sources.list && \
    echo "deb http://security.debian.org/debian-security bookworm-security main contrib non-free" >> /etc/apt/sources.list && \
    echo "deb-src http://security.debian.org/debian-security bookworm-security main contrib non-free" >> /etc/apt/sources.list && \

  ...

  apt-get update && \
    apt-get full-upgrade -y && \
    apt-get install -y \
    libboost-all-dev libudev-dev libinput-dev libts-dev libmtdev-dev \
    libjpeg-dev libfontconfig1-dev libssl-dev libdbus-1-dev libglib2.0-dev \
    libxkbcommon-dev libegl1-mesa-dev libgbm-dev libgles2-mesa-dev \
  ...
}
```

Configuration of **APT Repositories** ensuring up-to-date and complete package access and install a set of **development tools and libraries** with different purposes: Build tools, Graphics & X11 libs, Audio, GStreamer & multimedia codecs, Common dev libraries and Math/science libs.  

```dockerfile
WORKDIR /build

RUN tar czf rasp.tar.gz -C / lib usr/include usr/lib etc/alternatives
```

Sets the working directory for any following commands and packages key parts of the system into a compressed archive named rasp.tar.gz.  

`DockerFile (Cross Compiler):`  

```dockerfile
FROM debian:bookworm

ENV DEBIAN_FRONTEND=noninteractive

ARG BUILD_OPENCV=FALSE
```

Definition of the base image **Debian Bookworm** and environment setup by disabling **interactive prompts** during installation and declaration of a **optional build argument** that can be passed at build time to control whether OpenCV should be built.  

```dockerfile
RUN { \
  set -e && \
  apt-get update && apt-get install -y \
  wget git build-essential make rsync sed \
  libclang-dev ninja-build gcc bison python3 gperf pkg-config \
  ...
}
```

**Updates** the packages lists and **installs** a large number of **development tools and libraries** that cover: Core build tools, Compilers and parsers, Version control and utilities, Math & system libraries, Graphics/X11 libraries, Security & encryption, Database and data formats, General development libs, Build automation tools

```dockerfile
RUN ( \
    git clone https://github.com/Kitware/CMake.git && \
    ./bootstrap && make -j$(nproc) && make install \
)

RUN mkdir sysroot sysroot/usr sysroot/opt
COPY rasp.tar.gz /build/rasp.tar.gz
RUN tar xvfz /build/rasp.tar.gz -C /build/sysroot

RUN wget https://raw.githubusercontent.com/riscv/riscv-poky/master/scripts/sysroot-relativelinks.py && \
    python3 sysroot-relativelinks.py /build/sysroot
```

Clones and builds **CMake** directly from source, creates a sysroot (replica of the Raspberry Pi filesystem and used for cross-compilation) extracting the prebuilt Raspberry Pi environment, **rasp.tar.gz** into a directory.  
Lastly downloads a python script to **fix absolute symlinks** inside the sysroot, converting them to relative links so the toolchain reference them correctly.  

```dockerfile
COPY toolchain.cmake /build/

RUN { \
    mkdir -p qt6/... && \
    wget ...qtbase...qtshadertools...qtdeclarative... && \
    cmake ... && cmake --build ... && cmake --install ... \
}

RUN mkdir /build/project
COPY project /build/project
RUN { \
    cd /build/project && \
    /build/qt6/pi/bin/qt-cmake . && \
    cmake --build .; \
}
```

Copies the **Qt-specific CMake toolchain file** used to cross-compile for ARM, followed by the **download and build of Qt 6.9.1**, and lastly copies the project source code into the container, builds it using the **Qt for Raspberry toolchain** using the cross-built Qt binaries for consistent target behavior.  

---
<a id="sec-how-to-use"></a>
## 📝 How to use

1.  **Setup Docker**  
The first step is to correctly setup the base of the cross compiler, [Docker](https://www.docker.com/)  
2. **Create the target Docker image**  
On the `/cross-compiler` folder run the command to create the target image
```bash
docker buildx build --platform linux/arm64 --load -f DockerFileRasp -t raspimage .
```  
3. **Extract the compressed sysroot**  
After the image is created, extract from the image the `rasp.tar.gz`, that serves as the sysroot, which the cross-compiler will use to link against correct ARM libraries
```bash
docker create --name temp-arm raspimage
docker cp temp-arm:/build/rasp.tar.gz ./rasp.tar.gz
```  
4. **Create Debian image and Qt compilation**  
On the `/src` run the build of the Dockerfile
```bash
docker build -t qtcrossbuild -f cross-compiler/Dockerfile .
```  
5. **Extract the executable and binaries**
After the image is created (will take a couple hours) copy the executable and `qt-pi-binaries.tar.qz`
```bash
docker create --name tmpbuild qtcrossbuild
docker cp tmpbuild:/build/project/HelloQt6Qml ./HelloQt6Qml
docker cp tmpbuild:/build/qt-pi-binaries.tar.qz ./qt-pi-binaries.tar.qz
```  
6. **Send to Raspberry**  
After having all the necessary files send them to the raspberry via ssh  
7. **Config and execute**  
With the files already on the Rasp  
```bash
sudo mkdir /usr/local/qt6
sudo tar -xvf qt-pi-binaries.tar.gz -C /usr/local/qt6

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/qt6/lib/

./HelloQt6Qml
``` -->


<a id="sec-links"></a>

## 🔗 Links

  - https://ruvi-d.medium.com/a-master-guide-to-linux-cross-compiling-b894bf909386  
  - https://www.geeksforgeeks.org/compiler-design/what-is-cross-compiler/  
  - https://wiki.qt.io/Cross-Compile_Qt_6_for_Raspberry_Pi  
  - https://medium.com/@janiethedev/cross-compile-qt-6-7-2-for-raspberry-pi-4-with-ubuntu-1e951af83bb5  
  - https://github.com/PhysicsX/QTonRaspberryPi  
  - https://youtu.be/5XvQ_fLuBX0?si=WcI7c6L5jJSLzCvF  
  - https://www.docker.com/  