# AGL (Automotive Grade Linux) - Installation Guide

## Raspberry Pi 5 Build Instructions

This guide provides comprehensive instructions for building and deploying Automotive Grade Linux (AGL) on Raspberry Pi 5 using the Yocto Project build system.

---

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Git Configuration](#git-configuration)
3. [Build Environment Setup](#build-environment-setup)
4. [Source Code Download](#source-code-download)
5. [Raspberry Pi 5 Configuration](#raspberry-pi-5-configuration)
6. [Build Optimization](#build-optimization)
7. [Available Build Targets](#available-build-targets)
8. [Building the Image](#building-the-image)
9. [Flashing to microSD Card](#flashing-to-microsd-card)
10. [Hardware Setup and Boot](#hardware-setup-and-boot)

---

## Prerequisites

Install the required dependencies on your Ubuntu/Debian-based system:

```bash
sudo apt-get update
sudo apt-get install -y repo gawk wget git diffstat unzip texinfo \
    gcc-multilib build-essential chrpath socat cpio python3 python3-pip \
    python3-pexpect xz-utils debianutils iputils-ping python3-git \
    python3-jinja2 libegl1-mesa libsdl1.2-dev pylint3 xterm \
    python3-subunit mesa-common-dev zstd liblz4-tool curl
```

---

## Git Configuration

Configure your Git identity for the build process:

```bash
git config --global user.name "Your Name"
git config --global user.email "your@email.com"
```

---

## Build Environment Setup

### Create Directory Structure

Set up the workspace directory structure:

```bash
export AGL_TOP=$HOME/agl-workspace
mkdir -p $AGL_TOP
cd $AGL_TOP
```

### Install Repo Tool

The Repo tool is required for managing the AGL source repositories:

```bash
cd $AGL_TOP
mkdir -p ~/bin
export PATH=~/bin:$PATH
curl https://storage.googleapis.com/git-repo-downloads/repo > ~/bin/repo
chmod a+x ~/bin/repo
```

**Note:** Add `export PATH=~/bin:$PATH` to your `~/.bashrc` or `~/.zshrc` for persistence.

---

## Source Code Download

### Initialize Repository

Download the AGL source code for the **Quirky Quillback 17.1.10** ('old-stable' version) release:

```bash
cd $AGL_TOP
repo init -b quillback -m quillback_17.1.10.xml \
    -u https://gerrit.automotivelinux.org/gerrit/AGL/AGL-repo
```

### Synchronize Repositories

Fetch all required source repositories:

```bash
repo sync
```

**Note:** This process may take significant time depending on your internet connection speed.

---

## Raspberry Pi 5 Configuration

Initialize the build environment for Raspberry Pi 5 with demo and development features:

```bash
cd $AGL_TOP
source meta-agl/scripts/aglsetup.sh -m raspberrypi5 -b build-rpi5 agl-demo agl-devel
```

**Parameters:**
- `-m raspberrypi5`: Target machine
- `-b build-rpi5`: Build directory name
- `agl-demo`: Include demo applications
- `agl-devel`: Include development tools

---

## Build Optimization

### Create Download and State Cache Configuration

To optimize rebuild times, configure shared download and state cache directories:

```bash
# Create site configuration file
echo "# Shared directories for build optimization" >> $AGL_TOP/site.conf
echo "DL_DIR = \"$HOME/downloads/\"" >> $AGL_TOP/site.conf
echo "SSTATE_DIR = \"$AGL_TOP/sstate-cache/\"" >> $AGL_TOP/site.conf
```

### Create Symbolic Link

Link the site configuration to the build directory:

```bash
cd $AGL_TOP/build-rpi5
ln -sf $AGL_TOP/site.conf conf/
```

### Configure local.conf

Edit `$AGL_TOP/build-rpi5/conf/local.conf` and ensure the following configurations:

```bash
# Machine Configuration
# Comment out other MACHINE definitions and set:
MACHINE = "raspberrypi5"

# Parallel Build Configuration
# Adjust based on your CPU core count
BB_NUMBER_THREADS = "8"
PARALLEL_MAKE = "-j 8"

# License Configuration
LICENSE_FLAGS_ACCEPTED = "commercial"

# GPU Memory Allocation (optional)
GPU_MEM = "256"

# Enable build artifact cleanup to save disk space
INHERIT += "rm_work"

# Additional useful packages
IMAGE_INSTALL:append = " nano vim htop net-tools"
```

**Optimization Notes:**
- Adjust `BB_NUMBER_THREADS` and `PARALLEL_MAKE` based on your CPU capabilities
- Typical setting: number of CPU cores for both parameters
- `rm_work` removes temporary work files after each recipe completes

---

## Available Build Targets

To list all available image targets:

```bash
ls $AGL_TOP/meta-agl-demo/recipes-platform/images/
```

Common targets include:
- `agl-ivi-demo-qt`: IVI demo with Qt framework
- `agl-ivi-demo-qt-crosssdk`: IVI demo with Qt and cross-compilation SDK
- `agl-demo-platform`: Complete AGL demo platform

---

## Building the Image

### Start the Build Process

Build the AGL image with Qt SDK support:

```bash
cd $AGL_TOP/build-rpi5
bitbake agl-ivi-demo-qt-crosssdk
```

### Build Process Overview

The build process typically takes **2-4 hours** and includes:

1. Downloading all source packages
2. Compiling the Linux kernel
3. Compiling the complete Qt framework
4. Building Wayland compositor
5. Compiling all demo applications
6. Generating the final system image

**System Requirements:**
- Minimum 100GB free disk space
- 8GB+ RAM recommended
- Multi-core processor recommended for parallel builds

---

## Flashing to microSD Card

### Prepare the microSD Card

1. Insert the microSD card into your computer using an adapter
2. Identify the device name:

```bash
lsblk
```

3. Unmount the device (replace `mmcblk0` with your actual device):

```bash
sudo umount /dev/mmcblk0*
```

### Flash the Image

Navigate to the images directory and write the image:

```bash
cd $AGL_TOP/build-rpi5/tmp/deploy/images/raspberrypi5/
xzcat agl-ivi-demo-qt-crosssdk-raspberrypi5.wic.xz | \
    sudo dd of=/dev/mmcblk0 bs=4M status=progress
sudo sync
```

**Warning:** 
- Double-check the device name before running the `dd` command
- Using the wrong device will result in data loss
- The `sync` command ensures all data is written before removal

---

## Hardware Setup and Boot

### Assembly

1. Remove the microSD card safely from your computer
2. Insert the microSD card into the Raspberry Pi 5
3. Connect the M.2 HAT with SSD to the Raspberry Pi 5 (if applicable)
4. Connect a monitor via HDMI
5. Connect a USB keyboard
6. Connect the power supply

### First Boot

Power on the Raspberry Pi 5 to boot into AGL. The system will perform initial setup on first boot.

---

## Troubleshooting

### Common Issues

**Build Failures:**
- Ensure all dependencies are installed
- Check available disk space (minimum 100GB required)
- Verify internet connectivity for package downloads

**Performance Issues:**
- Reduce `BB_NUMBER_THREADS` and `PARALLEL_MAKE` if system becomes unresponsive
- Ensure adequate RAM (swap space may be needed for systems with <8GB RAM)

**Boot Issues:**
- Verify the image was written correctly to the microSD card
- Check monitor connection and HDMI cable
- Ensure power supply meets Raspberry Pi 5 requirements (5V/5A recommended)

---

## Version Information

- **AGL Version:** Quirky Quillback 17.1.10
- **Target Hardware:** Raspberry Pi 5
- **Image Type:** IVI Demo with Qt Cross-SDK


## Credits goes to..... 

- **https://docs.automotivelinux.org/en/trout/#01_Getting_Started/02_Building_AGL_Image/03_Downloading_AGL_Software/**
---
