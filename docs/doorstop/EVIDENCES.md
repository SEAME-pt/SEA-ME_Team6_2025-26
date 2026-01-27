

---

### EVIDENCES-EVID_L0_1 | Reviewed: ✔ | Score: 1.0 ### {: #evidences-evid_l0_1 data-toc-label="EVIDENCES-EVID_L0_1" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_1](ASSERTIONS.md#assertions-assert_l0_1) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/guides/Power Consumption Analysis.md`

	??? "Click to view reference"

		````md
		# Power Consumption Analysis - SEA:ME Project (Without Display)
		
		## Component Consumption Table
		
		| Component | Quantity | Voltage (V) | Min Current (mA) | Max Current (mA) | Min Power (W) | Max Power (W) | Notes |
		|-----------|----------|--------------|------------------|------------------|---------------|---------------|-------|
		| **Raspberry Pi 5** | 1 | 5.0 | 500 | 5000 | 2.5 | 25.0 | Idle: ~3W, Max Performance: 25W |
		| **Hailo-8 HAT (26 TOPS)** | 1 | 3.3-5.0 | 200 | 2500 | 0.66 | 12.5 | Typical: 2.5W, Max: ~12.5W |
		| **B-U585I-IOT02A** | 1 | 5.0 | 2 | 500 | 0.007 | 1.65 | With WiFi active: ~170mA @ 5V |
		| **Servo Motor WS-MG99SR** | 1 | 4.8-6.0 | 100 | 1200 | 0.5 | 7.2 | Idle: ~100mA, Stall: ~1200mA |
		| **DC Motors (propulsion)** | 2 | 6.0-12.0 | 200 | 2000 | 2.4 | 48.0 | Per motor: 1.2W-24W (depends on specific motor) |
		| **MCP2515 CAN Transceiver** | 2 | 5.0 | 1 | 5 | 0.01 | 0.05 | Typical: 5mA/module, Standby: 1μA |
		| **SSD M.2 (via PCIe)** | 1 | 3.3 | 50 | 500 | 0.165 | 1.65 | Depends on specific model |
		| **PCI-E to USB Converter** | 1 | 5.0 | 50 | 200 | 0.25 | 1.0 | Passive conversion |
		| **Speed Sensor (Hall)** | 1 | 3.3-5.0 | 5 | 15 | 0.025 | 0.075 | Typical Hall effect sensor |
		| **LM2596 Step-Down** | 1 | - | - | - | - | 0.4 | Efficiency losses (~92%) |
		| **XL4015 Step-Down** | 1 | - | - | - | - | 0.8 | Efficiency losses (~90%) |
		| **TB6612FNG Driver** | 1 | 2.7-5.5 (logic) | 5 | 3200 | 0.025 | 16.0 | VCC + VM, 1.2A cont./channel (3.2A peak) |
		
		---
		
		## System Total Summary (WITHOUT DISPLAY)
		
		| Condition | Battery Voltage | Total Current (A) | Total Power (W) | Observations |
		|-----------|-----------------|-------------------|-----------------|--------------|
		| **Minimum Idle** | 11.1V (3S) | ~1.15 | ~12.8 | All components at rest |
		| **Normal (driving)** | 11.1V (3S) | ~5.25 | ~58.3 | Typical operation with motors |
		| **Maximum (peak)** | 11.1V (3S) | ~15.2 | ~168.8 | All motors/servo at maximum load |
		
		### Comparison - WITH or WITHOUT Display
		
		| Condition | WITH Display (W) | WITHOUT Display (W) | Savings (W) | Savings (%) |
		|-----------|------------------|---------------------|-------------|-------------|
		| **Minimum Idle** | 15.5 | 12.8 | 2.7 | 17.4% |
		| **Normal** | 61.0 | 58.3 | 2.7 | 4.4% |
		| **Maximum** | 172.0 | 168.8 | 3.2 | 1.9% |
		
		
		## System Voltage Distribution
		
		| Voltage Rail | Source | Powered Components |
		|--------------|--------|-------------------|
		| **11.1V (Battery)** | 3S LiPo | Main input, DC Motors, Servo |
		| **5V** | Step-Down LM2596 | Raspberry Pi 5, MCP2515 (2x), USB Converter |
		| **3.3V** | Step-Down XL4015 | B-U585I-IOT02A, Speed Sensor, SSD M.2 |
		| **Motors** | Direct battery | TB6612FNG motor supply |
		
		
		
		## Detailed Autonomy Calculations (WITHOUT Display)
		
		### Scenario 1: Idle (System on, no movement)
		```
		Consumption: ~12.8W
		Current @ 11.1V: 12.8W / 11.1V = 1.15A
		
		3S (2200mAh):   2.2Ah / 1.15A = 1.91h  ≈ 1h 55min
		```
		
		### Scenario 2: Normal (Typical driving)
		```
		Consumption: ~58.3W
		Current @ 11.1V: 58.3W / 11.1V = 5.25A
		
		3S (2200mAh):   2.2Ah / 5.25A = 0.42h  ≈ 25min
		```
		
		### Scenario 3: Maximum (All components at 100%)
		```
		Consumption: ~168.8W
		Current @ 11.1V: 168.8W / 11.1V = 15.2A
		
		3S (2200mAh):   2.2Ah / 15.2A = 0.14h  ≈ 9min
		```
		
		
		## Power Distribution Diagram (WITHOUT Display)
		
		```
		[3S LiPo Battery 11.1V]
		    |
		    +-- [Motors] --&gt; TB6612FNG --&gt; DC Motors (2x)
		    |
		    +-- [Servo Motor WS-MG99SR]
		    |
		    +-- [LM2596 Step-Down] --&gt; 5V Rail
		    |       |
		    |       +-- Raspberry Pi 5
		    |       +-- MCP2515 (2x)
		    |       +-- TB6612FNG (VCC)
		    |       +-- USB Converter
		    |       +-- SSD M.2
		    |       +-- Hailo-8 HAT (via Pi)			
		    |
		    +-- [XL4015 Step-Down] --&gt; 3.3V Rail
		            |
		            +-- B-U585I-IOT02A
		            +-- Speed Sensor
		```
		
		
		## Final Comparison: WITH vs WITHOUT Display
		
		| Metric | WITH Display | WITHOUT Display | Difference |
		|--------|--------------|-----------------|------------|
		| **Idle Power** | 15.5W | 12.8W | -2.7W (-17.4%) |
		| **Normal Power** | 61.0W | 58.3W | -2.7W (-4.4%) |
		| **Maximum Power** | 172.0W | 168.8W | -3.2W (-1.9%) |
		| **Idle Autonomy (3S)** | 1h 34min | 1h 55min | +21min |
		| **Normal Autonomy (3S)** | 24min | 25min | +1min |
		| **LM2596 Load (peak)** | 6.3A | 5.65A | -640mA |
		| **No. Components** | 13 | 12 | -1 |
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_2 | Reviewed: ✔ | Score: 1.0 ### {: #evidences-evid_l0_2 data-toc-label="EVIDENCES-EVID_L0_2" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_2](ASSERTIONS.md#assertions-assert_l0_2) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/guides/Power Consumption Analysis.md`

	??? "Click to view reference"

		````md
		# Power Consumption Analysis - SEA:ME Project (Without Display)
		
		## Component Consumption Table
		
		| Component | Quantity | Voltage (V) | Min Current (mA) | Max Current (mA) | Min Power (W) | Max Power (W) | Notes |
		|-----------|----------|--------------|------------------|------------------|---------------|---------------|-------|
		| **Raspberry Pi 5** | 1 | 5.0 | 500 | 5000 | 2.5 | 25.0 | Idle: ~3W, Max Performance: 25W |
		| **Hailo-8 HAT (26 TOPS)** | 1 | 3.3-5.0 | 200 | 2500 | 0.66 | 12.5 | Typical: 2.5W, Max: ~12.5W |
		| **B-U585I-IOT02A** | 1 | 5.0 | 2 | 500 | 0.007 | 1.65 | With WiFi active: ~170mA @ 5V |
		| **Servo Motor WS-MG99SR** | 1 | 4.8-6.0 | 100 | 1200 | 0.5 | 7.2 | Idle: ~100mA, Stall: ~1200mA |
		| **DC Motors (propulsion)** | 2 | 6.0-12.0 | 200 | 2000 | 2.4 | 48.0 | Per motor: 1.2W-24W (depends on specific motor) |
		| **MCP2515 CAN Transceiver** | 2 | 5.0 | 1 | 5 | 0.01 | 0.05 | Typical: 5mA/module, Standby: 1μA |
		| **SSD M.2 (via PCIe)** | 1 | 3.3 | 50 | 500 | 0.165 | 1.65 | Depends on specific model |
		| **PCI-E to USB Converter** | 1 | 5.0 | 50 | 200 | 0.25 | 1.0 | Passive conversion |
		| **Speed Sensor (Hall)** | 1 | 3.3-5.0 | 5 | 15 | 0.025 | 0.075 | Typical Hall effect sensor |
		| **LM2596 Step-Down** | 1 | - | - | - | - | 0.4 | Efficiency losses (~92%) |
		| **XL4015 Step-Down** | 1 | - | - | - | - | 0.8 | Efficiency losses (~90%) |
		| **TB6612FNG Driver** | 1 | 2.7-5.5 (logic) | 5 | 3200 | 0.025 | 16.0 | VCC + VM, 1.2A cont./channel (3.2A peak) |
		
		---
		
		## System Total Summary (WITHOUT DISPLAY)
		
		| Condition | Battery Voltage | Total Current (A) | Total Power (W) | Observations |
		|-----------|-----------------|-------------------|-----------------|--------------|
		| **Minimum Idle** | 11.1V (3S) | ~1.15 | ~12.8 | All components at rest |
		| **Normal (driving)** | 11.1V (3S) | ~5.25 | ~58.3 | Typical operation with motors |
		| **Maximum (peak)** | 11.1V (3S) | ~15.2 | ~168.8 | All motors/servo at maximum load |
		
		### Comparison - WITH or WITHOUT Display
		
		| Condition | WITH Display (W) | WITHOUT Display (W) | Savings (W) | Savings (%) |
		|-----------|------------------|---------------------|-------------|-------------|
		| **Minimum Idle** | 15.5 | 12.8 | 2.7 | 17.4% |
		| **Normal** | 61.0 | 58.3 | 2.7 | 4.4% |
		| **Maximum** | 172.0 | 168.8 | 3.2 | 1.9% |
		
		
		## System Voltage Distribution
		
		| Voltage Rail | Source | Powered Components |
		|--------------|--------|-------------------|
		| **11.1V (Battery)** | 3S LiPo | Main input, DC Motors, Servo |
		| **5V** | Step-Down LM2596 | Raspberry Pi 5, MCP2515 (2x), USB Converter |
		| **3.3V** | Step-Down XL4015 | B-U585I-IOT02A, Speed Sensor, SSD M.2 |
		| **Motors** | Direct battery | TB6612FNG motor supply |
		
		
		
		## Detailed Autonomy Calculations (WITHOUT Display)
		
		### Scenario 1: Idle (System on, no movement)
		```
		Consumption: ~12.8W
		Current @ 11.1V: 12.8W / 11.1V = 1.15A
		
		3S (2200mAh):   2.2Ah / 1.15A = 1.91h  ≈ 1h 55min
		```
		
		### Scenario 2: Normal (Typical driving)
		```
		Consumption: ~58.3W
		Current @ 11.1V: 58.3W / 11.1V = 5.25A
		
		3S (2200mAh):   2.2Ah / 5.25A = 0.42h  ≈ 25min
		```
		
		### Scenario 3: Maximum (All components at 100%)
		```
		Consumption: ~168.8W
		Current @ 11.1V: 168.8W / 11.1V = 15.2A
		
		3S (2200mAh):   2.2Ah / 15.2A = 0.14h  ≈ 9min
		```
		
		
		## Power Distribution Diagram (WITHOUT Display)
		
		```
		[3S LiPo Battery 11.1V]
		    |
		    +-- [Motors] --&gt; TB6612FNG --&gt; DC Motors (2x)
		    |
		    +-- [Servo Motor WS-MG99SR]
		    |
		    +-- [LM2596 Step-Down] --&gt; 5V Rail
		    |       |
		    |       +-- Raspberry Pi 5
		    |       +-- MCP2515 (2x)
		    |       +-- TB6612FNG (VCC)
		    |       +-- USB Converter
		    |       +-- SSD M.2
		    |       +-- Hailo-8 HAT (via Pi)			
		    |
		    +-- [XL4015 Step-Down] --&gt; 3.3V Rail
		            |
		            +-- B-U585I-IOT02A
		            +-- Speed Sensor
		```
		
		
		## Final Comparison: WITH vs WITHOUT Display
		
		| Metric | WITH Display | WITHOUT Display | Difference |
		|--------|--------------|-----------------|------------|
		| **Idle Power** | 15.5W | 12.8W | -2.7W (-17.4%) |
		| **Normal Power** | 61.0W | 58.3W | -2.7W (-4.4%) |
		| **Maximum Power** | 172.0W | 168.8W | -3.2W (-1.9%) |
		| **Idle Autonomy (3S)** | 1h 34min | 1h 55min | +21min |
		| **Normal Autonomy (3S)** | 24min | 25min | +1min |
		| **LM2596 Load (peak)** | 6.3A | 5.65A | -640mA |
		| **No. Components** | 13 | 12 | -1 |
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_3 | Reviewed: ✔ | Score: 1.0 ### {: #evidences-evid_l0_3 data-toc-label="EVIDENCES-EVID_L0_3" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.
 
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_3](ASSERTIONS.md#assertions-assert_l0_3) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/guides/AGL_Installation_Guide.md`

	??? "Click to view reference"

		````md
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
		10. [Wifi Configuration](#wifi-connection)
		11. [Ssh Set-up](#ssh-configuration-on-agl)
		12. [Hardware Setup and Boot](#hardware-setup-and-boot)
		
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
		curl https://storage.googleapis.com/git-repo-downloads/repo &gt; ~/bin/repo
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
		
		Or.. Download the AGL source code for the **Terrific Trout v20.0.1** ('latest-stable' version) release:
		
		```bash
		cd $AGL_TOP
		mkdir master
		cd master
		repo init -u https://gerrit.automotivelinux.org/gerrit/AGL/AGL-repo \
		          -b trout -m trout_20.0.2.xml
		```
		
		### Synchronize Repositories
		
		Fetch all required source repositories:
		
		```bash
		repo sync
		```
		
		**Note:** This process may take significant time depending on your internet connection speed.
		
		## (What could possibly go wrong here?)
		
		When downloading the meta-qt6 repository from the code.qt.io server, the server is not responding in time.
		
		Create a local manifest with a mirror
		```bash
		cd $AGL_TOP
		mkdir -p .repo/local_manifests
		nano .repo/local_manifests/qt6_mirror.xml
		```
		and this inside
		```bash
		&lt;?xml version="1.0" encoding="UTF-8"?&gt;
		&lt;manifest&gt;
		  &lt;remove-project name="yocto/meta-qt6" /&gt;
		
		  &lt;project name="YoeDistro/meta-qt6"
		           path="external/meta-qt6"
		           remote="github"
		           revision="6.7" /&gt;
		&lt;/manifest&gt;
		```
		and then force the sync with the mirror you set up, and after make a full sync just to be sure :)
		```bash
		repo sync --force-sync external/meta-qt6
		repo sync 
		```
		
		
		---
		
		## Raspberry Pi 5 Configuration
		
		If you choose **Quirky Quillback 17.1.10**, initialize the build environment for Raspberry Pi 5 with demo and development features:
		
		```bash
		cd $AGL_TOP
		source meta-agl/scripts/aglsetup.sh -m raspberrypi5 -b build-rpi5 agl-demo agl-devel
		```
		
		But if you choose **Terrific Trout v20.0.1**...
		```bash
		cd $AGL_TOP
		source master/meta-agl/scripts/aglsetup.sh -m raspberrypi5 -b build-rpi5 agl-demo agl-devel
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
		echo "# Shared directories for build optimization" &gt;&gt; $AGL_TOP/site.conf
		echo "DL_DIR = \"$HOME/downloads/\"" &gt;&gt; $AGL_TOP/site.conf
		echo "SSTATE_DIR = \"$AGL_TOP/sstate-cache/\"" &gt;&gt; $AGL_TOP/site.conf
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
		- `agl-image-minimal-crosssdk`: Minimal Image with cross-compilation SDK
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
		## (What could possibly go wrong here?)
		
		The firmware has a restricted license (synaptics-killswitch) that is not on your list of accepted licenses
		
		Add the new license to you conf/local.conf file (at the end of the file):
		
		```bash
		LICENSE_FLAGS_ACCEPTED += "synaptics-killswitch"
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
		- Ensure adequate RAM (swap space may be needed for systems with &lt;8GB RAM)
		
		**Boot Issues:**
		- Verify the image was written correctly to the microSD card
		- Check monitor connection and HDMI cable
		- Ensure power supply meets Raspberry Pi 5 requirements (5V/5A recommended)
		
		---
		
		## Version Information
		
		- **AGL Version:** Quirky Quillback 17.1.10
		- **Target Hardware:** Raspberry Pi 5
		- **Image Type:** IVI Demo with Qt Cross-SDK
		
		
		## Part of the credits goes to..... 
		
		- **https://docs.automotivelinux.org/en/trout/#01_Getting_Started/02_Building_AGL_Image/03_Downloading_AGL_Software/**
		---
		
		````


- `docs/guides/AGL_and_ThreadX_benefits_Guide.md`

	??? "Click to view reference"

		````md
		# Benefits of Automotive Grade Linux
		
		- Inherits the robustness and security of Linux  
		- Backed by the Linux Foundation and major automakers  
		- Includes ready-to-use frameworks for multimedia, connectivity, HMI, navigation, and vehicle bus (CAN) integration  
		- Based on standard Linux, so developers can modify kernels, drivers, and user-space applications freely to fit specific needs.  
		
		---
		
		# Benefits of ThreadX
		
		- Ultra-low interrupt latency and fast context switching make it ideal for time-critical automotive functions  
		- Requires very little memory, making it perfect for microcontrollers  
		- Simple API and rich debugging tools  
		- ThreadX’s preemptive, priority-based scheduling ensures predictable task execution  
		
		---
		
		## Combined Benefits on a PiRacer
		
		- **Hybrid Architecture:** AGL handles the **high-level user interface, AI inference, and connectivity**, while ThreadX manages **real-time motor and sensor control**.  
		- **Optimized Performance:** Dividing tasks between AGL (Linux-based) and ThreadX (RTOS) maximizes both processing efficiency and determinism.  
		- **Scalable Design:** This setup mirrors modern automotive ECUs, making the PiRacer a strong platform for **autonomous driving research and prototyping**.  
		- **Educational and Practical:** Ideal for learning about real automotive-grade system design — combining open-source Linux flexibility with real-time embedded reliability. 
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_4 | Reviewed: ✔ | Score: 1.0 ### {: #evidences-evid_l0_4 data-toc-label="EVIDENCES-EVID_L0_4" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_4](ASSERTIONS.md#assertions-assert_l0_4) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/guides/AGL_ Joystick_installation_and_test.md`

	??? "Click to view reference"

		````md
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
		
		
		
		
		
		# 🎮 Waveshare Pyracer Controller Setup (via SSH)
		
		### This guide provides a solution for installing the Waveshare Pyracer controller (or similar joysticks) via SSH.
		
		### NOTE: This method focuses on loading the necessary kernel modules at runtime using modprobe, which avoids the need to install new packages or perform a system rebuild.
		
		## 1. Load Required Kernel Modules
		
		First, we must manually load the kernel modules required for joystick and USB Human Interface Device (HID) support.
		Bash
		```
		sudo modprobe joydev
		sudo modprobe usbhid
		sudo modprobe hid_generic
		```
		## 2. Verify Modules are Loaded
		
		After running the commands, check that the modules were loaded correctly into the kernel.
		Bash
		``` 
		lsmod | grep -E "joydev|usbhid"
		```
		You should see output indicating that joydev and usbhid are active.
		
		## 3. Connect the USB Dongle
		
		At this point, connect the controller's USB dongle to the device.
		
		    Important Note: If the dongle was already plugged in before you loaded the modules, disconnect it and plug it back in now. This forces the kernel to recognize the device using the newly loaded drivers.
		
		## 4. Verify USB Device Detection
		
		You can now confirm that the system has detected the dongle.
		
		Use dmesg to see the most recent kernel messages, which should show the new USB device being registered:
		```
		dmesg | tail -30
		```
		
		Alternatively, you can list all connected USB devices:
		```
		lsusb
		```
		
		## 5. Test the Controller Input
		
		Once the drivers are loaded and the dongle is detected, the system should create a device file (usually /dev/input/js0).
		
		You can read the raw event data from the controller to confirm it is working. Run the following command and then press buttons or move the sticks on your controller:
		```
		sudo hexdump -C /dev/input/js0
		```
		
		You should see hexadecimal output appearing on your screen in real-time as you use the controller. This confirms the device is sending data.
		
		    Press Ctrl+C to exit the hexdump command.
		
		
		
		And...
		that's it!
		
		````


- `src/inital_program/main.py`

	??? "Click to view reference"

		````py
		"""
		main.py — Nvidia JetRacer car control using a joystick
		
		This script performs the following steps:
		1. Initializes the car and the joystick.
		2. Continuously reads joystick axes.
		3. Applies throttle and steering values to the car.
		4. Prints the current values to the console for debugging.
		5. Ensures safe shutdown of the car on exit.
		"""
		
		from jetracer.nvidia_racecar import NvidiaRacecar
		import pygame
		import time
		
		car = NvidiaRacecar() # Create an instance of the JetRacer car
		
		pygame.init()  # Initialize the main pygame module
		pygame.joystick.init()  # Initialize the joystick module
		
		# Check if any joystick is connected to the Raspberry Pi
		if pygame.joystick.get_count() == 0:
		    print("No joystick connected!")
		    exit(1)  # Exit the program if no joystick is detected
		
		# Select the first connected joystick and initialize it
		joystick = pygame.joystick.Joystick(0)
		joystick.init()
		
		car.throttle_gain = 1  # Gain factor for throttle (acceleration)
		car.steering_gain = 1  # Gain factor for steering (direction)
		car.steering_offset = 0  # Steering servo offset adjustment
		
		try:
		    while True:
		        # Update pygame events (required for reading joystick input)
		        pygame.event.pump()
		
		        # axis 1: throttle (forward/backward)
		        # axis 2: steering (left/right)
		        throttle_axis = joystick.get_axis(1)
		        steering_axis = joystick.get_axis(2)
		
		        # Apply throttle value, multiplying by the gain
		        # Note: joystick axis usually ranges from -1 (up) to 1 (down)
		        car.throttle = throttle_axis * car.throttle_gain
		
		        # Apply steering value, multiplying by the gain
		        car.steering = steering_axis * car.steering_gain
		
		        # Print current values to the console for debugging
		        print(f"Throttle: {car.throttle:.2f}, Steering: {car.steering:.2f}")
		
		        # Small delay to avoid overloading the CPU
		        time.sleep(0.05)
		
		# Handle user interruption (Ctrl+C)
		except KeyboardInterrupt:
		    print("\nExiting...")
		
		# Safe shutdown
		finally:
		    # Set throttle and steering to zero to safely stop the car
		    car.throttle = 0
		    car.steering = 0
		
		    # Quit joystick and pygame modules
		    pygame.joystick.quit()
		    pygame.quit()
		
		
		````


- `src/inital_program/install_initial_program.md`

	??? "Click to view reference"

		````md
		# Initial Program - Documentation
		
		## 1. About
		This project integrates external libraries for installing PiRacer, focusing on NVIDIA boards.
		We use these libraries as a foundation for initial configurations, testing electronic components, and as a reference for applying reverse engineering to understand how motors and communication protocols, such as I²C, work.
		
		## 2. Install
		
		To access the device remotely, see this SSH connection tutorial:
		- [How to Connect to a Remote Server via SSH (DigitalOcean)](https://www.digitalocean.com/community/tutorials/how-to-use-ssh-to-connect-to-a-remote-server)
		
		Install the main dependencies with:
		
		```bash
		pip install torch jetson-utils jetracer numpy opencv-python pandas matplotlib flask pygame
		```
		
		Check the official repositories for specific installation instructions:
		
		- [PyTorch](https://github.com/pytorch/pytorch) – Library for GPU-accelerated computing and deep learning.
		- [jetson-utils](https://github.com/dusty-nv/jetson-utils) – Utilities for image and video processing optimized for Jetson.
		- [jetracer](https://github.com/NVIDIA-AI-IOT/jetracer) – Framework for autonomous car control with NVIDIA Jetson.
		- [pandas](https://github.com/pandas-dev/pandas) – Data structures and statistical analysis in Python.
		- [matplotlib](https://github.com/matplotlib/matplotlib) – Graphical visualization and chart generation in Python.
		- [flask](https://github.com/pallets/flask) – Microframework for building web applications.
		- [opencv-python](https://github.com/opencv/opencv-python) – Image processing and computer vision.
		- [pygame](https://github.com/pygame/pygame) – Library for game development and multimedia interfaces.
		
		
		
		## 3. Created main.py
		
		The `main.py` file was created as the main entry point of the project. It uses the following libraries:
		
		- `jetracer` ([NVIDIA-AI-IOT/jetracer](https://github.com/NVIDIA-AI-IOT/jetracer)): Autonomous vehicle control.
		- `pygame` ([pygame/pygame](https://github.com/pygame/pygame)): Joystick reading and control interface.
		- `time`: Time control and delays.
		- Other libraries may be added as needed.
		
		## 4. Created run.py and added to crontab -e
		
		The `run.py` file was created for project automation. For automatic execution, add to `crontab -e`:
		```bash
		* * * * * /usr/bin/flock -n /tmp/run.lock /path/to/your/script.sh &gt;&gt; /path/to/your/log.txt 2&gt;&1
		```
		
		This line in crontab does the following:
		
		Runs the script /path/to/your/script.sh every minute, but does not start a new execution if the previous one is still running.
		All output and errors from the script are appended to /path/to/your/log.txt.
		
		The trick is flock -n /tmp/run.lock, which creates a “lock” to prevent multiple simultaneous executions.
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_5 | Reviewed: ✔ | Score: 1.0 ### {: #evidences-evid_l0_5 data-toc-label="EVIDENCES-EVID_L0_5" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.


 
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_5](ASSERTIONS.md#assertions-assert_l0_5) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/guides/QML-guide.md`

	??? "Click to view reference"

		````md
		# 🚀 QML Guide
		
		## 📚 Index
		 - [👋 Introduction](#sec-intro)
		 - [🧠 Core Ideas](#sec-core-ideas)
		 - [🧱 Visual Blocks](#sec-core-visual-blocks)
		 - [📚 QML Elements](#sec-qml-elements)
		 - [🔗 Links](#sec-links)
		
		 ---
		
		&lt;a id="sec-intro"&gt;&lt;/a&gt;
		## 👋 Introduction
		
		The QML Guide serves as a companion to the main Instrument Cluster documentation, focusing on the frontend layer - the part responsible for visuals, interaction and dynamic data binding.  
		While the main README explains the overall system, this document dives into QtQuick, QML syntax and the UI components that make up the cluster interface.  
		QML (Qt Modeling Language) is a declarative language designed for building modern, fluid and responsive user interfaces.  
		Instead of managing widget hierarchies and manual layouts (like in QtWidgets), QML lets you describe your UI as a tree of visual elements - each element (or QML type) defines what appears on screen and how it behaves.  
		Every visual element you see - from the background `Rectangle`, to the data `Text`, to the main `ApplicationWindow` - is a QML Type, defined in the QtQuick or QtQuick.Controls modules.  
		These types can be composed, extended, and dynamically bound to backend data written in C++.  
		This guide provides a detailed overview of:
		- The core QML elements used throughout the project (`Rectangle`, `Text`, `Image`, `ApplicationWindow`, etc).
		- ADD ACCORDINGLY
		
		Use this document as a reference when working on new screens, refactoring components, or connecting new backend data.
		
		---
		
		&lt;a id="sec-core-ideas"&gt;&lt;/a&gt;
		## 🧠 Core Ideas
		
		---
		
		&lt;a id="sec-core-visual-blocks"&gt;&lt;/a&gt;
		## 🧱 Visual Blocks
		
		### Item
		`Item` is the invisible base class for almost all visual QML elements.  
		You can think of it as a container or a positioning box - it doesn't draw anything itself, but defines where and how things appear.
		
		#### Common Properties
		
		- **x, y** - Position relative to the parent.
		- **width, height** - Size of the item.
		- **visible** - Controls if the item is shown on screen.
		- **opacity** - How transparent the item is (1.0 = fully visible, 0.0 = invisible).
		- **rotation, scale** - Transformation properties.
		- **anchors** - Layout system to align items relative to other items.
		- **parent** - Reference to the parent item in the visual tree.
		
		### Anchors
		`Anchors` are QML's built-in layout system for aligning and positioning elements relative to each other.  
		They replace manual `x` and `y` coordinates with logical relationships, making your UI much more flexible.
		
		#### Common Properties
		
		- **anchors.fill: parent** - Fills the parent entirely.
		- **anchors.centerIn: parent** - Centers inside parent.
		- **anchors.top, anchors.bottom, anchors.left, anchors.right** - Attach edges.
		- **anchors.margins** - Add spacing from attached edges.
		
		---
		
		&lt;a id="sec-qml-elements"&gt;&lt;/a&gt;
		## 📚 QML Elements
		
		### ApplicationWindow
		
		**🌐 Module** - `QtQuick.Controls`  
		**🔗 Reference** - [Qt Documentation - Application Window](https://doc.qt.io/qt-6/qml-qtquick-controls-applicationwindow.html)
		
		#### Overview
		`ApplicationWindow` is the root visual window for QML applications that use the `QtQuick.Controls` module.  
		It acts as the top-level container for all UI elements and provides built-in support for headers, footers, menus and toolbars.  
		It provides the base surface where layouts, components and visual themes are rendered.  
		
		**Example:**
		```qml
		ApplicationWindow {
		    visible: true
		    width: 800
		    height: 480
		    title: "Instrument Cluster"
		
		    Rectangle {
		        anchors.fill: parent
		        color: "#1e1e1e"
		    }
		}
		```
		
		#### Basic Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `visible` | `bool` | Determines if the window is displayed on screen. Usually set to `true` so your app appears. | `visible: true` |
		| `width` | `int` | Sets the window’s width in pixels. | `width: 800` |
		| `height` | `int` | Sets the window’s height in pixels. | `height: 480` |
		| `color` | `color` | Defines the background color. | `color: "#ffffff"` |
		| `title` | `string` | Text shown in the OS title bar. | `title: "Instrument Cluster"` |
		| `flags` | `Qt.WindowFlags` | Controls window behavior — frameless, fullscreen, etc. | `flags: Qt.FramelessWindowHint` |
		
		---
		
		#### Layout & Structure Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `header` | `Item` | Optional area at the top for navigation or titles. | `header: ToolBar {}` |
		| `footer` | `Item` | Optional area at the bottom for status info or controls. | `footer: Rectangle { height: 50 }` |
		| `contentItem` | `Item (read-only)` | Main container where your app’s layout goes. | — |
		| `background` | `Item` | Visual element behind everything (color, gradient, or image). | `background: Rectangle { color: "#000" }` |
		| `menubar` | `Item` | Optional area for menus (mostly for desktop apps). | — |
		
		---
		
		#### Functional Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `activeFocusControl` | `Control (read-only)` | The control (like a text field or button) that currently has keyboard focus. Useful for tracking user interaction. | `if (window.activeFocusControl) console.log("focused")` |
		| `font` | `font` | Default font used by all controls (e.g. `Label`, `Button`) inside the window. | `font.family: "Roboto"` |
		| `locale` | `Locale` | Defines the language and number/date formatting settings for the window. | `locale: Qt.locale("en_US")` |
		| `window` | `ApplicationWindow` | Lets any item access its parent window. | `window.title = "My App"` |
		
		
		#### Tips & Notes
		
		- In production or embedded setups, replace `visible: true` with `visibility: "FullScreen"` for a cleaner UI.
		- Keep only one `ApplicationWindow` per app - additional windows should use the `Window``type if needed.
		- Avoid heavy logic or animations in the root window; delegate those to child components.
		
		
		### Loader
		
		**🌐 Module** - `QtQuick`  
		**🔗 Reference** - [Qt Documentation - Loader](https://doc.qt.io/qt-6/qml-qtquick-loader.html)
		
		#### Overview
		`Loader` is a QML type that dynamically loads other QML components at runtime.  
		Instead of declaring everything upfront, you can use `Loader` to load (and unload) visual items as needed - improving performance and flexibility in large UIs.  
		Think of it like a "placeholder" that you can tell what to display later.  
		It's especially useful when switching screens, lazy-loading content, or managing memory efficiently.  
		
		**Example:**
		```qml
		Loader {
		    id: dynamicLoader
		    source: "Dashboard.qml"
		    active: true
		}
		```
		
		#### Basic Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `source` | `url` | Path or URL of the QML file to load. | `source: "MyComponent.qml"` |
		| `sourceComponent` | `Component` | Loads a QML `Component` instead of a file. | `sourceComponent: myComponent` |
		| `active` | `bool` | Controls whether the loader should actually load its source. Set `active: false` to unload. | `active: true` |
		| `asynchronous` | `bool` | If `true`, loads the component in the background (non-blocking). | `asynchronous: true` |
		| `visible` | `bool` | Visibility of the loader item itself. *(Does not affect the visibility of the loaded object.)* | `visible: true` |
		| `opacity` | `real` | Sets the opacity of the loader and its loaded item. | `opacity: 0.8` |
		| `anchors, x, y, width, height` | — | Standard layout properties for positioning the loaded item. | `anchors.centerIn: parent` |
		
		---
		
		#### Layout & Structure Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `item` | `Item (read-only)` | Reference to the loaded QML object. You can access its properties. | `loader.item.color = "red"` |
		| `progress` | `real (read-only)` | Progress value between `0.0` and `1.0` for asynchronous loading. | `console.log(loader.progress)` |
		| `status` | `int` | Current loader state: `Loader.Null`, `Loader.Ready`, `Loader.Loading`, or `Loader.Error`. | `if (loader.status === Loader.Error)` |
		| `activeFocus` | `bool` | Indicates if the loaded item or loader currently has keyboard focus. | `activeFocus: true` |
		
		---
		
		#### Functional Properties
		
		| **Property / Signal / Method** | **Type** | **Description** | **Example** |
		|--------------------------------|-----------|------------------|--------------|
		| `onLoaded` | `signal` | Triggered when the source finishes loading successfully. | `onLoaded: console.log("Loaded!")` |
		| `onStatusChanged` | `signal` | Emitted when the loader's status changes (e.g., loading → ready). | `onStatusChanged: handleStatus(loader.status)` |
		| `setSource(url)` | `method` | Dynamically sets a new source file to load. | `loader.setSource("Settings.qml")` |
		| `setSourceComponent(component)` | `method` | Dynamically changes the loaded component. | `loader.setSourceComponent(myComponent)` |
		| `sourceComponent.destroy()` | `method` | Destroys the currently loaded item (frees memory). | `loader.sourceComponent.destroy()` |
		
		
		#### Tips & Notes
		
		- Set `active: false` initially, and then enable it when you need the component. This avoids long startup times.
		- You can dynamically change screens by calling `setSource("AnotherPage.qml")`.
		- Setting `active: false` unloads the current item, freeing up resources - perfect for embedded systems or dashboards.
		- Use `asynchronous: true` for smoother transitions when loading large QML files or complex UIs.
		- Monitor `status` and `onStatusChanged` to detect loading errors and handle them gracefully.
		
		### Rectangle
		
		**🌐 Module** - `QtQuick`  
		**🔗 Reference** - [Qt Documentation - Rectangle](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)
		
		#### Overview
		The `Rectangle` is one of the most common and fundamental QML elements.  
		It's a simple visual item that draws a rectangle on the screen - often used as backgrounds, containers, or decorative shapes for other components.  
		It's a great building block for almost any visual layout - buttons, card, panels, etc.  
		
		**Example:**
		```qml
		Rectangle {
		    width: 200
		    height: 100
		    color: "#4CAF50"
		    radius: 12
		}
		```
		
		#### Basic Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `color` | `color` | Sets the rectangle's fill color. | `color: "#ffffff"` |
		| `radius` | `real` | Rounds the corners of the rectangle. | `radius: 8` |
		| `border` | `group` | Defines border styling. | `border.color: "black"` / `border.width: 2` |
		| `gradient` | `Gradient` | Fills the rectangle with a smooth color transition. | `gradient: Gradient { GradientStop { position: 0; color: "red" } GradientStop { position: 1; color: "blue" } }` |
		| `antialiasing` | `bool` | Enables smooth edges (useful for rounded rectangles). | `antialiasing: true` |
		
		---
		
		#### Layout & Structure Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `x, y` | `real` | Position of the rectangle relative to its parent. | `x: 150; y: 50` |
		| `width, height` | `int` | Size of the rectangle in pixels. | `width: 200; height: 100` |
		| `anchors` | `group` | Used for aligning the rectangle relative to other items. | `anchors.centerIn: parent` |
		| `visible` | `bool` | Controls whether the rectangle is shown or hidden. | `visible: false` |
		| `opacity` | `real` | Sets the rectangle's opacity. | `opacity: 0.8` |
		| `rotation` | `real` | Rotates the rectangle in degrees. | `rotation: 45` |
		| `scale` | `real` | Scales the rectangle size proportionally. | `scale: 1.2` |
		
		---
		
		#### Functional Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `clip` | `bool` | When `true`, child items are clipped to the rectangle's boundaries (useful for masking overflow). | `clip: true` |
		| `smooth` | `bool` | Alias for antialiasing in older Qt versions. | `smooth: true` |
		| `implicitWidth, implicitHeight` | `real` | Natural size of the rectangle if no explicit size is set. | — |
		
		
		#### Tips & Notes
		
		- Don't overuse `antialiasing` on many items - it can impact rendering speed on low-power devices.
		- Many higher-level QML controls internally use `Rectangle` for their visual background.
		
		### RowLayout
		
		**🌐 Module** - `QtQuick.Layouts`  
		**🔗 Reference** - [Qt Documentation - RowLayout](https://doc.qt.io/qt-6/qml-qtquick-layouts-rowlayout.html)
		
		#### Overview
		`RowLayout` is a layout manager that arranges its child items horizontally in a row.  
		Instead of manually positioning items with x or anchors, RowLayout automatically handles sizing, spacing and alignment - making your UI adaptive and clean.  
		
		**Example:**
		```qml
		RowLayout {
		    width: 300
		    spacing: 10
		
		    Rectangle { color: "lightblue"; Layout.preferredWidth: 50; height: 40 }
		    Rectangle { color: "lightgreen"; Layout.fillWidth: true; height: 40 }
		    Rectangle { color: "tomato"; Layout.preferredWidth: 80; height: 40 }
		}
		```
		
		#### Basic Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `spacing` | `int` | Defines the space (in pixels) between child items. | `spacing: 12` |
		| `enabled` | `bool` | Controls whether the layout is active. If false, children won't be arranged automatically. | `enabled: true` |
		| `visible` | `bool` | Controls whether the layout(and its children) are visible. | `visible: true` |
		
		
		#### Layout & Structure Properties
		Each item inside a `RowLayout` can define how it behaves within the layout using special `Layout.*` properties.  
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `Layout.preferredWidth` | `real` | Preferred width of the item in the layout. | `Layout.preferredWidth: 80` |
		| `Layout.minimumWidth` | `real` | The minimum allowed width before shrinking. | `Layout.minimumWidth: 40: ` |
		| `Layout.maximumWidth` | `real` | The maximum width allowed before stretching. | `Layout.maximumWidth: 150` |
		| `Layout.fillWidth` | `bool` | If true, the item expands to take remaining space. | `Layout.fillWidth: true` |
		| `Layout.alignment` | `enumeration` | Aligns the item vertically or horizontally within its cell. Use constants like `Qt.AlignLeft`, `Qt.AlignVCenter`. | `Layout.alignment: Qt.AlignHCenter` |
		| `Layout.margins` | `int` | Sets outer spacing around the item. | `Layout.margins: 8` |
		| `Layout.row, Layout.column` | `int` | Used when the layout is nested within a grid - not usually needed for `RowLayout`. | `Layout.row: 0` |
		
		#### Functional Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `Automatic Resizing` | - | If a child uses `Layout.fillWidth: true`, it stretches when the parent resizes. | A progress bar expanding across the screen. |
		| `Nested Layouts` | - | You can place a `RowLayout` inside a `ColumnLayout` to build responsive designs. | `ColumnLayout { RowLayout { ... }}` |
		| `Mirroring` | `bool` | Automatically mirrors layout directions for right-to-left locales. | `LayoutMirroring.enabled: true` |
		
		#### Tips & Notes
		
		- `RowLayout` is ideal when you want automatic spacing and resizing without anchors.
		- Combine with `ColumnLayout` for complex dashboard-style UIs.
		- Use `Layout.fillWidth` and `Layout.preferredWidth` instead of fixed widths for a responsive design.
		- For maximum performance, avoid mixing `anchors` and `Layout` properties on the same item.
		- You can use invisible `Item { width: x }` inside a layout as a spacer between controls.
		
		### ColumnLayout
		
		**🌐 Module** - `QtQuick.Layouts`  
		**🔗 Reference** - [Qt Documentation - ColumnLayout](https://doc.qt.io/qt-6/qml-qtquick-layouts-columnlayout.html)
		
		#### Overview
		`ColumnLayout` is a QML type module that arranges its child items vertically in a single column.  
		It's part of the layout system that helps you position and size multiple elements without having to manually calculate coordinates.  
		
		**Example:**  
		```qml
		ColumnLayout {
		    anchors.fill: parent
		    spacing: 12
		
		    Rectangle { color: "#2196F3"; Layout.fillWidth: true; Layout.preferredHeight: 50 }
		    Rectangle { color: "#4CAF50"; Layout.fillWidth: true; Layout.preferredHeight: 100 }
		    Rectangle { color: "#FFC107"; Layout.fillWidth: true; Layout.preferredHeight: 75 }
		}
		```
		
		#### Basic Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `spacing` | `int` | Defines the space (in pixels) between child items. | `spacing: 12` |
		| `enabled` | `bool` | Controls whether the layout is active. If false, children won't be arranged automatically. | `enabled: true` |
		| `visible` | `bool` | Controls whether the layout(and its children) are visible. | `visible: true` |
		
		#### Layout & Structure Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `anchors` | `group` | Allows you to anchor the layout within its parent. | `anchors.fill: parent ` |
		| `Layout.fillWidth` | `bool` | Makes a child expand horizontally to fill available width. | `Layout.fillWidth: true` |
		| `Layout.fillHeight` | `bool` | Makes a child expand vertically (only meaningful in nested layouts). | `Layout.fillHeight: true` |
		| `Layout.preferredWidth` | `real` | Preferred width of the item in the layout. | `Layout.preferredWidth: 80` |
		| `Layout.preferredHeight` | `real` | Preferred height of the item in the layout. | `Layout.preferredHeight: 120` |
		| `Layout.alignment` | `enumeration` | Aligns the item vertically or horizontally within its cell. Use constants like `Qt.AlignLeft`, `Qt.AlignVCenter`. | `Layout.alignment: Qt.AlignHCenter` |
		
		#### Functional Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `children` | `list&lt;item&gt;` | The visual items that are stacked vertically. | (implicit) add items inside `ColumnLayout: { ... } ` |
		| `count` | `int` | Number of child items currently managed by the layout. | `console.log(column.count)` |
		| `addItem(item)` | `method` | Adds a child dynamically at runtime. | `column.addItem(Rectangle {})` |
		| `removeItem(item)` | `method` | Removes a child from the layout. | `column.removeItem(rect)` |
		
		#### Tips & Notes
		
		- Use `ColumnLayout` inside an `ApplicationWindow` or another layout to easily create responsive UIs.
		- Combine with `RowLayout` for flexible, grid-like structures.
		- Avoid mixing `anchors` and layout properties (`Layout.*`) on the same child.
		- Ideal for toolbars, stacked panels, or from layouts.
		
		### Text
		
		**🌐 Module** - `QtQuick`  
		**🔗 Reference** - [Qt Documentation - Text](https://doc.qt.io/qt-6/qml-qtquick-text.html)
		
		#### Overview
		`Text` is one of the most commonly used QML elements - it displays a pice of text on screen.  
		It's simple but powerful, supporting features like font customization, color styling and alignment formatting.  
		You'll often use `Text` to show labels, data values, or dynamic content that changes with backend properties.  
		
		**Example:**
		```qml
		Text {
		  text: "Speed: 120 km/h"
		  color: "white"
		  font.pixelSize: 32
		  anchors.centerIn: parent
		}
		```
		
		#### Basic Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `text` | `string` | The actual text to display. | `text: "Hello QML!"` |
		| `color` | `color` | Text color. | `color: "#ffffff"` |
		| `font.family` | `string` | The font family name. | `font.family: "Roboto"` |
		| `font.pixelSize` | `int` | Font size in pixels. | `font.pixelSize: 24` |
		| `font.bold` | `bool` | Makes the text bold. | `font.bold: true` |
		| `font.italic` | `bool` | Italicizes the text. | `font.italic: true` |
		| `horizontalAlignment` | `enumeration` | Aligns text horizontally: `Text.AlignLeft`, `Text.AlignHCenter`, `Text.AlignRight`. | `horizontalAlignment: Text.AlignHCenter` |
		| `verticalAlignment` | `enumeration` | Aligns text vertically: `Text.AlignTop`, `Text.AlignVCenter`, `Text.AlignBottom`. | `verticalAlignment: Text.AlignVCenter` |
		| `visible` | `bool` | Controls whether the text is visible. | `visible: true` |
		
		#### Layout & Structure Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `x, y` | `real` | Position of the text item in its parent. | `x: 50; y: 20` |
		| `width, height` | `int` | Defines the bounding box of the text. Use within wrapping or clipping. | `width: 200` |
		| `anchors` | `group` | Position text relative to its parent(e.g, center, top, etc). | `anchors.centerIn: parent` |
		| `wrapMode` | `enumeration` | Controls how long text wraps: `Text.NoWrap`, `Text.WordWrap`, `Text.WrapAnywhere` . | `wrapMode: Text.WordWrap` |
		| `elide` | `enumeration` | If text is too long, shows "...": `Text.ElideRight`, `Text.ElideLeft`. | `elide: Text.ElideRight` |
		| `opacity` | `real` | Transparency level from `0.0` to `1.0`. | `opacity: 0.8` |
		| `rotation` | `real` | Rotates the text by a number of degrees. | `rotation: 15` |
		
		#### Functional Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `textFormat` | `enumeration` | Specifies the text format: `Text.PlainText`, `Text.RichText` or `Text.StyledText`. | `textFormat: Text.RichText` |
		| `renderType` | `enumeration` | Rendering method for text(e.g, `Text.NativeRendering`, `Text.QtRendering`). | `renderType: Text.QtRendering` |
		| `lineHeight` | `real` | Adjusts spacing between lines when wrapping. | `lineHeight: 1.2` |
		| `clip` | `bool` | If true, hides text outside the bounding box. | `clip: true` |
		| `maximumLineCount` | `int` | Limits the number of lines shown. | `maximumLineCount: 2` |
		| `linkActivated(link)` | `signal` | Emitted when the user clicks a hyperlink(when using rich text). | `onLinkActivated: console.log(link) ` |
		
		#### Tips & Notes
		
		- `Text` doesn't automatically wrap - remember to set `wrapMode: Text.WordWrap` if you expect long lines.
		- Use `Text.RichText` to include simple HTML formatting like `&lt;b&gt;`, `&lt;i&gt;`, `&lt;font color='red'&gt;`.
		- When animating color, opacity, or rotation, consider using `Behavior` or `NumberAnimation` for smooth effects.
		- For performance with rapidly changing text (like FPS or speed readouts), avoid unnecessary `Text.RichText` - it's heavier than plain text.
		
		&lt;!--
		### x
		
		**🌐 Module** - `x`  
		**🔗 Reference** - [Qt Documentation - x](x)
		
		#### Overview
		
		#### Basic Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `x` | `x` | x. | `x: ` |
		
		#### Layout & Structure Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `x` | `x` | x. | `x: ` |
		
		#### Functional Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `x` | `x` | x. | `x: ` |
		
		#### Tips & Notes
		
		- x
		--&gt;
		
		
		---
		
		&lt;!--
		{
		  "element": "ApplicationWindow",
		  "module": "QtQuick.Controls",
		  "level": "beginner",
		  "goal": "Explain this QML element in a beginner-friendly, documentation-style format.",
		  "source_url": "https://doc.qt.io/qt-6/qml-qtquick-controls-applicationwindow.html",
		  "sections": {
		    "overview": true,
		    "basic_properties": true,
		    "layout_properties": true,
		    "functional_properties": true,
		    "tips_and_notes": true
		  },
		  "style": {
		    "tone": "friendly",
		    "format": "markdown",
		    "grouped_sections": true,
		    "inline_examples": true,
		    "code_comments": false,
		    "properties_display": {
		      "type": "table",
		      "columns": ["Property", "Type", "Description", "Example"],
		      "group_by": ["basic", "layout", "functional"],
		      "collapsible_sections": true,
		      "show_icons": true,
		      "markdown_format": true
		    }
		  }
		}
		--&gt;
		
		
		
		&lt;a id="sec-links"&gt;&lt;/a&gt;
		## 🔗 Links
		
		- https://doc.qt.io/qt-6/qml-qtquick-item.html
		- https://doc.qt.io/qt-6/qml-qtquick-controls-applicationwindow.html
		- https://doc.qt.io/qt-6/qml-qtquick-loader.html
		- https://doc.qt.io/qt-6/qml-qtquick-rectangle.html
		- https://doc.qt.io/qt-6/qml-qtquick-layouts-rowlayout.html
		- https://doc.qt.io/qt-6/qml-qtquick-layouts-columnlayout.html
		- https://doc.qt.io/qt-6/qml-qtquick-text.html
		
		````


- `README.md`

	??? "Click to view reference"

		````md
		# 🚗 SEAME – Team 6 (2025/26)
		
		&gt; A hands-on project to practice automotive/mobility software engineering.
		
		---
		
		## 📚 Index
		- [About](#about)
		- [Scope](#scope)
		- [Current Phase](#current-phase)
		- [How We Work](#how-we-work)
		- [Team](#team)
		
		---
		
		## About
		**SEAME** is a learning-by-building initiative focused on the **software engineering of automotive/mobility systems**.  
		We’re practicing a **end-to-end engineering flow**:
		
		- **Platform:** we assemble a **Raspberry Pi 5 + PiRacer** setup (camera, display, controller, optional Hailo accelerator) and build a **Qt-based UI** (driver info, status).
		- **Engineering practices:** we structure work through **issues/epics → branches → PRs → CI → documentation**, keeping **TSF-style traceability** between requirements, code, and tests.
		- **Automation:** we use **GitHub Actions** for repeatable checks and documentation helpers.
		- **Organization:** we work in **sprints**, doing small increments, and maintain a clear **Project board** so anyone can see scope, status, and blockers.
		
		---
		
		## Scope
		- **Platform setup:** RPi5, camera, display, Hailo Hat.
		- **Workflows & docs:** GitHub Actions, PR templates, TSF traceability.
		
		---
		
		## Current Phase
		**Sprint 1** → Hardware setup, Project setup, labels/board, first Qt UI skeleton.  
		
		---
		
		## How We Work
		**Branches**
		- `main` → protected  
		- `dev` → integration branch  
		- `#&lt;issue&gt;-&lt;epic&gt;/&lt;name&gt;` → feature branches from `dev` (e.g., `#12-QtApp-mockup`)
		
		---
		
		## Team
		
		- @daviduartecf - David Fernandes
		- @jpjpcs - João Silva
		- @rcosta-c - Ruben Carvalho
		- @vasferre - Vasco Ferreira
		- @dinoguei-seame - Diogo NNogueira
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_6 | Reviewed: ✔ | Score: 1.0 ### {: #evidences-evid_l0_6 data-toc-label="EVIDENCES-EVID_L0_6" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_6](ASSERTIONS.md#assertions-assert_l0_6) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/guides/QML-guide.md`

	??? "Click to view reference"

		````md
		# 🚀 QML Guide
		
		## 📚 Index
		 - [👋 Introduction](#sec-intro)
		 - [🧠 Core Ideas](#sec-core-ideas)
		 - [🧱 Visual Blocks](#sec-core-visual-blocks)
		 - [📚 QML Elements](#sec-qml-elements)
		 - [🔗 Links](#sec-links)
		
		 ---
		
		&lt;a id="sec-intro"&gt;&lt;/a&gt;
		## 👋 Introduction
		
		The QML Guide serves as a companion to the main Instrument Cluster documentation, focusing on the frontend layer - the part responsible for visuals, interaction and dynamic data binding.  
		While the main README explains the overall system, this document dives into QtQuick, QML syntax and the UI components that make up the cluster interface.  
		QML (Qt Modeling Language) is a declarative language designed for building modern, fluid and responsive user interfaces.  
		Instead of managing widget hierarchies and manual layouts (like in QtWidgets), QML lets you describe your UI as a tree of visual elements - each element (or QML type) defines what appears on screen and how it behaves.  
		Every visual element you see - from the background `Rectangle`, to the data `Text`, to the main `ApplicationWindow` - is a QML Type, defined in the QtQuick or QtQuick.Controls modules.  
		These types can be composed, extended, and dynamically bound to backend data written in C++.  
		This guide provides a detailed overview of:
		- The core QML elements used throughout the project (`Rectangle`, `Text`, `Image`, `ApplicationWindow`, etc).
		- ADD ACCORDINGLY
		
		Use this document as a reference when working on new screens, refactoring components, or connecting new backend data.
		
		---
		
		&lt;a id="sec-core-ideas"&gt;&lt;/a&gt;
		## 🧠 Core Ideas
		
		---
		
		&lt;a id="sec-core-visual-blocks"&gt;&lt;/a&gt;
		## 🧱 Visual Blocks
		
		### Item
		`Item` is the invisible base class for almost all visual QML elements.  
		You can think of it as a container or a positioning box - it doesn't draw anything itself, but defines where and how things appear.
		
		#### Common Properties
		
		- **x, y** - Position relative to the parent.
		- **width, height** - Size of the item.
		- **visible** - Controls if the item is shown on screen.
		- **opacity** - How transparent the item is (1.0 = fully visible, 0.0 = invisible).
		- **rotation, scale** - Transformation properties.
		- **anchors** - Layout system to align items relative to other items.
		- **parent** - Reference to the parent item in the visual tree.
		
		### Anchors
		`Anchors` are QML's built-in layout system for aligning and positioning elements relative to each other.  
		They replace manual `x` and `y` coordinates with logical relationships, making your UI much more flexible.
		
		#### Common Properties
		
		- **anchors.fill: parent** - Fills the parent entirely.
		- **anchors.centerIn: parent** - Centers inside parent.
		- **anchors.top, anchors.bottom, anchors.left, anchors.right** - Attach edges.
		- **anchors.margins** - Add spacing from attached edges.
		
		---
		
		&lt;a id="sec-qml-elements"&gt;&lt;/a&gt;
		## 📚 QML Elements
		
		### ApplicationWindow
		
		**🌐 Module** - `QtQuick.Controls`  
		**🔗 Reference** - [Qt Documentation - Application Window](https://doc.qt.io/qt-6/qml-qtquick-controls-applicationwindow.html)
		
		#### Overview
		`ApplicationWindow` is the root visual window for QML applications that use the `QtQuick.Controls` module.  
		It acts as the top-level container for all UI elements and provides built-in support for headers, footers, menus and toolbars.  
		It provides the base surface where layouts, components and visual themes are rendered.  
		
		**Example:**
		```qml
		ApplicationWindow {
		    visible: true
		    width: 800
		    height: 480
		    title: "Instrument Cluster"
		
		    Rectangle {
		        anchors.fill: parent
		        color: "#1e1e1e"
		    }
		}
		```
		
		#### Basic Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `visible` | `bool` | Determines if the window is displayed on screen. Usually set to `true` so your app appears. | `visible: true` |
		| `width` | `int` | Sets the window’s width in pixels. | `width: 800` |
		| `height` | `int` | Sets the window’s height in pixels. | `height: 480` |
		| `color` | `color` | Defines the background color. | `color: "#ffffff"` |
		| `title` | `string` | Text shown in the OS title bar. | `title: "Instrument Cluster"` |
		| `flags` | `Qt.WindowFlags` | Controls window behavior — frameless, fullscreen, etc. | `flags: Qt.FramelessWindowHint` |
		
		---
		
		#### Layout & Structure Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `header` | `Item` | Optional area at the top for navigation or titles. | `header: ToolBar {}` |
		| `footer` | `Item` | Optional area at the bottom for status info or controls. | `footer: Rectangle { height: 50 }` |
		| `contentItem` | `Item (read-only)` | Main container where your app’s layout goes. | — |
		| `background` | `Item` | Visual element behind everything (color, gradient, or image). | `background: Rectangle { color: "#000" }` |
		| `menubar` | `Item` | Optional area for menus (mostly for desktop apps). | — |
		
		---
		
		#### Functional Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `activeFocusControl` | `Control (read-only)` | The control (like a text field or button) that currently has keyboard focus. Useful for tracking user interaction. | `if (window.activeFocusControl) console.log("focused")` |
		| `font` | `font` | Default font used by all controls (e.g. `Label`, `Button`) inside the window. | `font.family: "Roboto"` |
		| `locale` | `Locale` | Defines the language and number/date formatting settings for the window. | `locale: Qt.locale("en_US")` |
		| `window` | `ApplicationWindow` | Lets any item access its parent window. | `window.title = "My App"` |
		
		
		#### Tips & Notes
		
		- In production or embedded setups, replace `visible: true` with `visibility: "FullScreen"` for a cleaner UI.
		- Keep only one `ApplicationWindow` per app - additional windows should use the `Window``type if needed.
		- Avoid heavy logic or animations in the root window; delegate those to child components.
		
		
		### Loader
		
		**🌐 Module** - `QtQuick`  
		**🔗 Reference** - [Qt Documentation - Loader](https://doc.qt.io/qt-6/qml-qtquick-loader.html)
		
		#### Overview
		`Loader` is a QML type that dynamically loads other QML components at runtime.  
		Instead of declaring everything upfront, you can use `Loader` to load (and unload) visual items as needed - improving performance and flexibility in large UIs.  
		Think of it like a "placeholder" that you can tell what to display later.  
		It's especially useful when switching screens, lazy-loading content, or managing memory efficiently.  
		
		**Example:**
		```qml
		Loader {
		    id: dynamicLoader
		    source: "Dashboard.qml"
		    active: true
		}
		```
		
		#### Basic Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `source` | `url` | Path or URL of the QML file to load. | `source: "MyComponent.qml"` |
		| `sourceComponent` | `Component` | Loads a QML `Component` instead of a file. | `sourceComponent: myComponent` |
		| `active` | `bool` | Controls whether the loader should actually load its source. Set `active: false` to unload. | `active: true` |
		| `asynchronous` | `bool` | If `true`, loads the component in the background (non-blocking). | `asynchronous: true` |
		| `visible` | `bool` | Visibility of the loader item itself. *(Does not affect the visibility of the loaded object.)* | `visible: true` |
		| `opacity` | `real` | Sets the opacity of the loader and its loaded item. | `opacity: 0.8` |
		| `anchors, x, y, width, height` | — | Standard layout properties for positioning the loaded item. | `anchors.centerIn: parent` |
		
		---
		
		#### Layout & Structure Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `item` | `Item (read-only)` | Reference to the loaded QML object. You can access its properties. | `loader.item.color = "red"` |
		| `progress` | `real (read-only)` | Progress value between `0.0` and `1.0` for asynchronous loading. | `console.log(loader.progress)` |
		| `status` | `int` | Current loader state: `Loader.Null`, `Loader.Ready`, `Loader.Loading`, or `Loader.Error`. | `if (loader.status === Loader.Error)` |
		| `activeFocus` | `bool` | Indicates if the loaded item or loader currently has keyboard focus. | `activeFocus: true` |
		
		---
		
		#### Functional Properties
		
		| **Property / Signal / Method** | **Type** | **Description** | **Example** |
		|--------------------------------|-----------|------------------|--------------|
		| `onLoaded` | `signal` | Triggered when the source finishes loading successfully. | `onLoaded: console.log("Loaded!")` |
		| `onStatusChanged` | `signal` | Emitted when the loader's status changes (e.g., loading → ready). | `onStatusChanged: handleStatus(loader.status)` |
		| `setSource(url)` | `method` | Dynamically sets a new source file to load. | `loader.setSource("Settings.qml")` |
		| `setSourceComponent(component)` | `method` | Dynamically changes the loaded component. | `loader.setSourceComponent(myComponent)` |
		| `sourceComponent.destroy()` | `method` | Destroys the currently loaded item (frees memory). | `loader.sourceComponent.destroy()` |
		
		
		#### Tips & Notes
		
		- Set `active: false` initially, and then enable it when you need the component. This avoids long startup times.
		- You can dynamically change screens by calling `setSource("AnotherPage.qml")`.
		- Setting `active: false` unloads the current item, freeing up resources - perfect for embedded systems or dashboards.
		- Use `asynchronous: true` for smoother transitions when loading large QML files or complex UIs.
		- Monitor `status` and `onStatusChanged` to detect loading errors and handle them gracefully.
		
		### Rectangle
		
		**🌐 Module** - `QtQuick`  
		**🔗 Reference** - [Qt Documentation - Rectangle](https://doc.qt.io/qt-6/qml-qtquick-rectangle.html)
		
		#### Overview
		The `Rectangle` is one of the most common and fundamental QML elements.  
		It's a simple visual item that draws a rectangle on the screen - often used as backgrounds, containers, or decorative shapes for other components.  
		It's a great building block for almost any visual layout - buttons, card, panels, etc.  
		
		**Example:**
		```qml
		Rectangle {
		    width: 200
		    height: 100
		    color: "#4CAF50"
		    radius: 12
		}
		```
		
		#### Basic Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `color` | `color` | Sets the rectangle's fill color. | `color: "#ffffff"` |
		| `radius` | `real` | Rounds the corners of the rectangle. | `radius: 8` |
		| `border` | `group` | Defines border styling. | `border.color: "black"` / `border.width: 2` |
		| `gradient` | `Gradient` | Fills the rectangle with a smooth color transition. | `gradient: Gradient { GradientStop { position: 0; color: "red" } GradientStop { position: 1; color: "blue" } }` |
		| `antialiasing` | `bool` | Enables smooth edges (useful for rounded rectangles). | `antialiasing: true` |
		
		---
		
		#### Layout & Structure Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `x, y` | `real` | Position of the rectangle relative to its parent. | `x: 150; y: 50` |
		| `width, height` | `int` | Size of the rectangle in pixels. | `width: 200; height: 100` |
		| `anchors` | `group` | Used for aligning the rectangle relative to other items. | `anchors.centerIn: parent` |
		| `visible` | `bool` | Controls whether the rectangle is shown or hidden. | `visible: false` |
		| `opacity` | `real` | Sets the rectangle's opacity. | `opacity: 0.8` |
		| `rotation` | `real` | Rotates the rectangle in degrees. | `rotation: 45` |
		| `scale` | `real` | Scales the rectangle size proportionally. | `scale: 1.2` |
		
		---
		
		#### Functional Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `clip` | `bool` | When `true`, child items are clipped to the rectangle's boundaries (useful for masking overflow). | `clip: true` |
		| `smooth` | `bool` | Alias for antialiasing in older Qt versions. | `smooth: true` |
		| `implicitWidth, implicitHeight` | `real` | Natural size of the rectangle if no explicit size is set. | — |
		
		
		#### Tips & Notes
		
		- Don't overuse `antialiasing` on many items - it can impact rendering speed on low-power devices.
		- Many higher-level QML controls internally use `Rectangle` for their visual background.
		
		### RowLayout
		
		**🌐 Module** - `QtQuick.Layouts`  
		**🔗 Reference** - [Qt Documentation - RowLayout](https://doc.qt.io/qt-6/qml-qtquick-layouts-rowlayout.html)
		
		#### Overview
		`RowLayout` is a layout manager that arranges its child items horizontally in a row.  
		Instead of manually positioning items with x or anchors, RowLayout automatically handles sizing, spacing and alignment - making your UI adaptive and clean.  
		
		**Example:**
		```qml
		RowLayout {
		    width: 300
		    spacing: 10
		
		    Rectangle { color: "lightblue"; Layout.preferredWidth: 50; height: 40 }
		    Rectangle { color: "lightgreen"; Layout.fillWidth: true; height: 40 }
		    Rectangle { color: "tomato"; Layout.preferredWidth: 80; height: 40 }
		}
		```
		
		#### Basic Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `spacing` | `int` | Defines the space (in pixels) between child items. | `spacing: 12` |
		| `enabled` | `bool` | Controls whether the layout is active. If false, children won't be arranged automatically. | `enabled: true` |
		| `visible` | `bool` | Controls whether the layout(and its children) are visible. | `visible: true` |
		
		
		#### Layout & Structure Properties
		Each item inside a `RowLayout` can define how it behaves within the layout using special `Layout.*` properties.  
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `Layout.preferredWidth` | `real` | Preferred width of the item in the layout. | `Layout.preferredWidth: 80` |
		| `Layout.minimumWidth` | `real` | The minimum allowed width before shrinking. | `Layout.minimumWidth: 40: ` |
		| `Layout.maximumWidth` | `real` | The maximum width allowed before stretching. | `Layout.maximumWidth: 150` |
		| `Layout.fillWidth` | `bool` | If true, the item expands to take remaining space. | `Layout.fillWidth: true` |
		| `Layout.alignment` | `enumeration` | Aligns the item vertically or horizontally within its cell. Use constants like `Qt.AlignLeft`, `Qt.AlignVCenter`. | `Layout.alignment: Qt.AlignHCenter` |
		| `Layout.margins` | `int` | Sets outer spacing around the item. | `Layout.margins: 8` |
		| `Layout.row, Layout.column` | `int` | Used when the layout is nested within a grid - not usually needed for `RowLayout`. | `Layout.row: 0` |
		
		#### Functional Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `Automatic Resizing` | - | If a child uses `Layout.fillWidth: true`, it stretches when the parent resizes. | A progress bar expanding across the screen. |
		| `Nested Layouts` | - | You can place a `RowLayout` inside a `ColumnLayout` to build responsive designs. | `ColumnLayout { RowLayout { ... }}` |
		| `Mirroring` | `bool` | Automatically mirrors layout directions for right-to-left locales. | `LayoutMirroring.enabled: true` |
		
		#### Tips & Notes
		
		- `RowLayout` is ideal when you want automatic spacing and resizing without anchors.
		- Combine with `ColumnLayout` for complex dashboard-style UIs.
		- Use `Layout.fillWidth` and `Layout.preferredWidth` instead of fixed widths for a responsive design.
		- For maximum performance, avoid mixing `anchors` and `Layout` properties on the same item.
		- You can use invisible `Item { width: x }` inside a layout as a spacer between controls.
		
		### ColumnLayout
		
		**🌐 Module** - `QtQuick.Layouts`  
		**🔗 Reference** - [Qt Documentation - ColumnLayout](https://doc.qt.io/qt-6/qml-qtquick-layouts-columnlayout.html)
		
		#### Overview
		`ColumnLayout` is a QML type module that arranges its child items vertically in a single column.  
		It's part of the layout system that helps you position and size multiple elements without having to manually calculate coordinates.  
		
		**Example:**  
		```qml
		ColumnLayout {
		    anchors.fill: parent
		    spacing: 12
		
		    Rectangle { color: "#2196F3"; Layout.fillWidth: true; Layout.preferredHeight: 50 }
		    Rectangle { color: "#4CAF50"; Layout.fillWidth: true; Layout.preferredHeight: 100 }
		    Rectangle { color: "#FFC107"; Layout.fillWidth: true; Layout.preferredHeight: 75 }
		}
		```
		
		#### Basic Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `spacing` | `int` | Defines the space (in pixels) between child items. | `spacing: 12` |
		| `enabled` | `bool` | Controls whether the layout is active. If false, children won't be arranged automatically. | `enabled: true` |
		| `visible` | `bool` | Controls whether the layout(and its children) are visible. | `visible: true` |
		
		#### Layout & Structure Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `anchors` | `group` | Allows you to anchor the layout within its parent. | `anchors.fill: parent ` |
		| `Layout.fillWidth` | `bool` | Makes a child expand horizontally to fill available width. | `Layout.fillWidth: true` |
		| `Layout.fillHeight` | `bool` | Makes a child expand vertically (only meaningful in nested layouts). | `Layout.fillHeight: true` |
		| `Layout.preferredWidth` | `real` | Preferred width of the item in the layout. | `Layout.preferredWidth: 80` |
		| `Layout.preferredHeight` | `real` | Preferred height of the item in the layout. | `Layout.preferredHeight: 120` |
		| `Layout.alignment` | `enumeration` | Aligns the item vertically or horizontally within its cell. Use constants like `Qt.AlignLeft`, `Qt.AlignVCenter`. | `Layout.alignment: Qt.AlignHCenter` |
		
		#### Functional Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `children` | `list&lt;item&gt;` | The visual items that are stacked vertically. | (implicit) add items inside `ColumnLayout: { ... } ` |
		| `count` | `int` | Number of child items currently managed by the layout. | `console.log(column.count)` |
		| `addItem(item)` | `method` | Adds a child dynamically at runtime. | `column.addItem(Rectangle {})` |
		| `removeItem(item)` | `method` | Removes a child from the layout. | `column.removeItem(rect)` |
		
		#### Tips & Notes
		
		- Use `ColumnLayout` inside an `ApplicationWindow` or another layout to easily create responsive UIs.
		- Combine with `RowLayout` for flexible, grid-like structures.
		- Avoid mixing `anchors` and layout properties (`Layout.*`) on the same child.
		- Ideal for toolbars, stacked panels, or from layouts.
		
		### Text
		
		**🌐 Module** - `QtQuick`  
		**🔗 Reference** - [Qt Documentation - Text](https://doc.qt.io/qt-6/qml-qtquick-text.html)
		
		#### Overview
		`Text` is one of the most commonly used QML elements - it displays a pice of text on screen.  
		It's simple but powerful, supporting features like font customization, color styling and alignment formatting.  
		You'll often use `Text` to show labels, data values, or dynamic content that changes with backend properties.  
		
		**Example:**
		```qml
		Text {
		  text: "Speed: 120 km/h"
		  color: "white"
		  font.pixelSize: 32
		  anchors.centerIn: parent
		}
		```
		
		#### Basic Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `text` | `string` | The actual text to display. | `text: "Hello QML!"` |
		| `color` | `color` | Text color. | `color: "#ffffff"` |
		| `font.family` | `string` | The font family name. | `font.family: "Roboto"` |
		| `font.pixelSize` | `int` | Font size in pixels. | `font.pixelSize: 24` |
		| `font.bold` | `bool` | Makes the text bold. | `font.bold: true` |
		| `font.italic` | `bool` | Italicizes the text. | `font.italic: true` |
		| `horizontalAlignment` | `enumeration` | Aligns text horizontally: `Text.AlignLeft`, `Text.AlignHCenter`, `Text.AlignRight`. | `horizontalAlignment: Text.AlignHCenter` |
		| `verticalAlignment` | `enumeration` | Aligns text vertically: `Text.AlignTop`, `Text.AlignVCenter`, `Text.AlignBottom`. | `verticalAlignment: Text.AlignVCenter` |
		| `visible` | `bool` | Controls whether the text is visible. | `visible: true` |
		
		#### Layout & Structure Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `x, y` | `real` | Position of the text item in its parent. | `x: 50; y: 20` |
		| `width, height` | `int` | Defines the bounding box of the text. Use within wrapping or clipping. | `width: 200` |
		| `anchors` | `group` | Position text relative to its parent(e.g, center, top, etc). | `anchors.centerIn: parent` |
		| `wrapMode` | `enumeration` | Controls how long text wraps: `Text.NoWrap`, `Text.WordWrap`, `Text.WrapAnywhere` . | `wrapMode: Text.WordWrap` |
		| `elide` | `enumeration` | If text is too long, shows "...": `Text.ElideRight`, `Text.ElideLeft`. | `elide: Text.ElideRight` |
		| `opacity` | `real` | Transparency level from `0.0` to `1.0`. | `opacity: 0.8` |
		| `rotation` | `real` | Rotates the text by a number of degrees. | `rotation: 15` |
		
		#### Functional Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `textFormat` | `enumeration` | Specifies the text format: `Text.PlainText`, `Text.RichText` or `Text.StyledText`. | `textFormat: Text.RichText` |
		| `renderType` | `enumeration` | Rendering method for text(e.g, `Text.NativeRendering`, `Text.QtRendering`). | `renderType: Text.QtRendering` |
		| `lineHeight` | `real` | Adjusts spacing between lines when wrapping. | `lineHeight: 1.2` |
		| `clip` | `bool` | If true, hides text outside the bounding box. | `clip: true` |
		| `maximumLineCount` | `int` | Limits the number of lines shown. | `maximumLineCount: 2` |
		| `linkActivated(link)` | `signal` | Emitted when the user clicks a hyperlink(when using rich text). | `onLinkActivated: console.log(link) ` |
		
		#### Tips & Notes
		
		- `Text` doesn't automatically wrap - remember to set `wrapMode: Text.WordWrap` if you expect long lines.
		- Use `Text.RichText` to include simple HTML formatting like `&lt;b&gt;`, `&lt;i&gt;`, `&lt;font color='red'&gt;`.
		- When animating color, opacity, or rotation, consider using `Behavior` or `NumberAnimation` for smooth effects.
		- For performance with rapidly changing text (like FPS or speed readouts), avoid unnecessary `Text.RichText` - it's heavier than plain text.
		
		&lt;!--
		### x
		
		**🌐 Module** - `x`  
		**🔗 Reference** - [Qt Documentation - x](x)
		
		#### Overview
		
		#### Basic Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `x` | `x` | x. | `x: ` |
		
		#### Layout & Structure Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `x` | `x` | x. | `x: ` |
		
		#### Functional Properties
		
		| **Property** | **Type** | **Description** | **Example** |
		|---------------|-----------|------------------|--------------|
		| `x` | `x` | x. | `x: ` |
		
		#### Tips & Notes
		
		- x
		--&gt;
		
		
		---
		
		&lt;!--
		{
		  "element": "ApplicationWindow",
		  "module": "QtQuick.Controls",
		  "level": "beginner",
		  "goal": "Explain this QML element in a beginner-friendly, documentation-style format.",
		  "source_url": "https://doc.qt.io/qt-6/qml-qtquick-controls-applicationwindow.html",
		  "sections": {
		    "overview": true,
		    "basic_properties": true,
		    "layout_properties": true,
		    "functional_properties": true,
		    "tips_and_notes": true
		  },
		  "style": {
		    "tone": "friendly",
		    "format": "markdown",
		    "grouped_sections": true,
		    "inline_examples": true,
		    "code_comments": false,
		    "properties_display": {
		      "type": "table",
		      "columns": ["Property", "Type", "Description", "Example"],
		      "group_by": ["basic", "layout", "functional"],
		      "collapsible_sections": true,
		      "show_icons": true,
		      "markdown_format": true
		    }
		  }
		}
		--&gt;
		
		
		
		&lt;a id="sec-links"&gt;&lt;/a&gt;
		## 🔗 Links
		
		- https://doc.qt.io/qt-6/qml-qtquick-item.html
		- https://doc.qt.io/qt-6/qml-qtquick-controls-applicationwindow.html
		- https://doc.qt.io/qt-6/qml-qtquick-loader.html
		- https://doc.qt.io/qt-6/qml-qtquick-rectangle.html
		- https://doc.qt.io/qt-6/qml-qtquick-layouts-rowlayout.html
		- https://doc.qt.io/qt-6/qml-qtquick-layouts-columnlayout.html
		- https://doc.qt.io/qt-6/qml-qtquick-text.html
		
		````


- `src/cluster/README.md`

	??? "Click to view reference"

		````md
		# 🚀 Instrument Cluster
		
		## 📚 Index
		  - [👋 Introduction](#sec-intro)
		  - [🧠 Core Ideas](#sec-core-ideas)
		  - [🤔 QtWidgets vs QtQuick](#sec-qt-vs-qml)
		  - [⚙️ QML & C++ Integration](#sec-integration)
		  - [ℹ️ Qt setup](#sec-qt-setup)
		  - [🧰 Development Setup](#sec-development-setup)
		  - [🔀 Project Structuring and Layout](#sec-flow)
		  - [🎨 UI Design & Components](#sec-ui-components)
		  - [🚨 Issues Log](#sec-issues)
		  - [🔗 Links](#sec-links)
		
		---
		
		&lt;a id="sec-intro"&gt;&lt;/a&gt;
		## 👋 Introduction
		
		An instrument cluster is a collection of dials and gauges that typically sits behind the steering wheel, giving the driver a range of information about the vehicle's status. All vehicles have an instrument cluster or some instrumentation in other locations within the driver's eye line.  
		Aside from regular driving activities, the driver must also monitor various aspects of the vehicle - such as current speed, fuel level, or battery capacity in an EV. The instrument cluster enables this, ensuring the driver can manage fuel usage and adhere to speed limits. It also provides additional feedback, such as warning lights for oil pressure, tire pressure, or ADS (Advanced Driver Assistance Systems) alerts.  
		
		**Typical Gauges and Indicators:**
		- Odometer
		- Speedometer
		- Fuel level / Battery capacity
		- Rev counter
		- Power / Regeneration level
		- Drive mode or gear selection
		- Turn indicators
		- Headlight status (including main beams)
		- External temperature
		- Engine fault warnings
		- Tire pressure
		- ADAS alerts
		- Engine temperature
		
		Modern vehicle's collect this data through the ECU (Electronic Control Unit) and other real-time subsystems. These values are distributed over vehicle networks and reflected in the cluster display, enabling live updates for metrics like trip distance, fuel economy, or average speed.
		
		This project **simulates a digital instrument cluster** using **Qt and QtQuick**, demonstrating how real-time data visualization and interactive UI elements can be built using modern declarative programming techniques.
		
		---
		
		&lt;a id="sec-core-ideas"&gt;&lt;/a&gt;
		## 🧠 Core Ideas
		
		- **Instrument Cluster:** A digital dashboard displaying critical vehicle information such as speed, fuel level and system alerts in real time.
		- **Simulation Goal:** Recreate the behavior and appearance of a real vehicle cluster through software, focusing on responsiveness and visual clarity.
		- **Data Flow:** Represent how sensor data (speed, temperature, tire pressure, etc) is gathered and fed into visual components that update dynamically.
		- **Real-Time Updates:** Use bindings and signals to reflect changes instantly in the user interface.
		- **Qt Framework:** C++ based cross-platform framework providing the foundation for the application, enabling efficient rendering and modular design.
		- **QtQuick / QML:** A declarative UI language built on Qt that allows for designing animated, GPU-accelerated interfaces - ideal for dashboards and gauges.
		- **C++ Backend Integration:** Logic, data processing and simulated ECU signals are handled in C++, then exposed to QML for real-time display.
		- **Component Based UI:** Reusable QML Components for modularity and maintainability.
		- **Declarative Programming:** Define what the UI should look like and how it reacts to data changes, instead of managing each change imperatively.
		- **Scalability:** Design principles that allow easy adaptation of the cluster layout for various resolutions, form factors, or additional vehicle indicators.
		- **Data Binding & Signals:** QML automatically updates UI elements when data from the backend changes, C++ emits signals and QML onPropertyChanged handlers react accordingly.
		- **Resource Management:** Qt's resource system `.qrc` to embed QML, icons and fonts for easy deployment and portability.
		- **Extensibility:** Designed to integrate with real sensor data via CAN bus without major architectural changes.
		- **MVC/MVVM Architecture:** Separate logic from presentation - data models expose observable properties and the QML view layer simply binds to them.
		- **CMake:** Describes what the project needs, and then produces the build instructions for the compiler
		
		---
		
		&lt;a id="sec-qt-vs-qml"&gt;&lt;/a&gt;
		## 🤔 QtWidgets vs QtQuick
		
		Qt offers two main approaches to building graphical user interfaces: **QtWidgets** and **QtQuick (QML)**.  
		Both are part of the Qt Framework, but they are different in architecture, design and intended use cases.
		
		### QtWidgets - Traditional UI Framework
		
		QtWidgets is the classic, object-oriented UI framework that has existed since the earliest versions of Qt. It is based entirely on C++ and uses CPU to render graphical components.
		
		**Characteristics:**
		  - Every button, label and slider is a QWidget - a rectangular UI element drawn on the CPU.
		  - Interfaces are built imperatively, meaning developers describe how to build and modify the UI step by step.
		  - Layouts, signals and slots are managed in C++.
		  - Rendering is CPU-bound, relying on QPainter to draw widgets, which limits animation performance.
		
		**When to use:**
		  - Classic desktop applications (e.g. editors, file browsers, IDEs).
		  - Form based, static UIs that rarely change layout or animation.
		  - Environments where hardware acceleration is unavailable or unnecessary.
		
		### QtQuick (QML) - Modern Declarative UI
		
		QtQuick is the newer framework, introduced with Qt 5 and fully optimized in Qt 6. It uses QML (Qt Modeling Language) for UI declaration and the Qt Scene Graph for rendering - which leverages GPU acceleration.
		
		**Characteristics:**
		  - The UI is declarative - you describe what the UI should look like, not how to draw it.
		  - Interfaces are made of lightweight QML elements, all rendered by the GPU, enabling smooth transitions and effects.
		  - Bindings and signals make the UI reactive: when data changes, the visuals update automatically.
		  - C++ integration remains powerful, it can extend QML with custom C++ classes, models and performance-critical logic.
		  - Designed for touch interfaces, embedded systems and animated dashboards.
		
		**When to use:**
		  - Applications requiring dynamic, fluid and animated UIs.
		  - Embedded systems ou automotive displays.
		  - Prototyping or iterative UI design.
		  - Projects needing scalable and resolution-independent layouts.
		
		### Why QtQuick makes sense for an instrument cluster
		
		An instrument cluster is not a static interface, it's a dynamic, real-time display with continuously updating data, animations, transitions and indicators.  
		For such requirements, QtQuick outperforms QtWidgets in every relevant dimension.
		
		**Key Reasons:**
		- QtQuick Scene Graph uses the GPU to render animations smoothly - ideal for moving gauges, fading alerts, and rotating needles.
		- QML makes it simple to define animations, bindings and state transitions directly in the markup.
		- When C++ emits new sensor values, QML automatically updates the visual components, no manual refresh logic, timers, or redraws required.
		- C++ backend simulates the ECU and provides data, QML focuses on rendering and interactivity - keeping the architecture modular and maintainable.
		- QtQuick is part of Qt for MCUs and Qt Automotive Suite, making it industry-standard for digital clusters and HMIs.
		
		---
		
		&lt;a id="sec-integration"&gt;&lt;/a&gt;
		## ⚙️ QML & C++ Integration
		
		QtQuick applications combine QML for UI and C++ for logic, creating a powerful separation between presentation and data processing.  
		In this project, C++ handles the application startup, logic and data models, while QML handles rendering and user interaction.
		
		### Why C++ Handles Logic and QML Handles UI?
		- **Performance and Efficiency**  
		  C++ is a compiled, low-level language - it provides:
		  - **High performance** for computations, data processing or simulations.
		  - **Tight memory control**, ideal for embedded systems.
		  - Access to system APIs and hardware interfaces (CAN bus, sockets, etc).
		
		  So anything involving data acquisition, calculations or system-level communication is best done in C++.
		- **UI Responsiveness and Flexibility**
		  QML is a declarative language built on top of the QtQuick scene graph (GPU accelerated). It is designed for:
		  - **Describing interfaces**, animations and transitions in a high-level, readable way.
		  - Automatically updating visuals when data changes - through bindings and property observers.
		  - Rapid prototyping - designers and developers can modify the UI without recompiling C++.
		
		  So QML is used for "what the user sees", while C++ is used for "how the data behaves".
		
		### Architecture Overview
		
		- **C++ Backend**
		  - Initializes the application (QGuiApplication) and loads the QML engine (QQmlApplicationEngine).
		  - Manages system logic such as data simulation, signal handling or communication (e.g. via CAN).
		  - Exposes classes, objects or properties to QML through the QT meta-object system (Q_PROPERTY, Q_INVOKABLE, Q_SIGNAL).
		
		- **QML Frontend**
		  - Declares the layout, animations and visual elements of the instrument cluster.
		  - Binds directly to C++ properties and reacts to emitted signals automatically.
		  - Uses declarative syntax for UI updates - no manual refresh logic is required.
		
		
		---
		
		&lt;a id="sec-qt-setup"&gt;&lt;/a&gt;
		## ℹ️ Qt setup
		
		Qt provides all the tools needed to build modern graphic applications - including the framework, build tools and IDE integration.  
		Before running or developing this project, you'll need to set up your local Qt environment.
		
		**Install Qt**  
		The easiest way to install Qt is through the [Qt Online Installer](https://www.qt.io/download-dev).  
		 - **Steps:**
		    -  Download the Qt Online Installer for your OS (Windows, macOS or Linux).
		    - Log in with a Qt account.
		    - During installation:
		      - Select the version of Qt you want to work on.
		      - Select the `Build Tools` option to install the other necessary requirements.
		      - **[Optional]** You can also select the `Qt Creator` option to download the Qt IDE, but more on that later.
		
		**Install Qt Creator (IDE)**  
		Although Qt projects can be built entirely from command line, Qt Creator simplifies everything:  
		  - Integrated code editor for C++, QML and UI forms.
		  - Built-in CMake support.
		  - Integrated debugger, profiler and QML inspector.
		  - One-click build and run for multiple kits.  
		
		After installation (follow the Optional step on the `Install Qt` section):
		  - Open Qt Creator.
		  - Go to Tools → Options → Kits to verify the installed Qt versions and compilers.
		  - Ensure you have a working Desktop Qt 6.x GCC/Clang/MSVC kit.
		
		**Verify Installation**  
		To confirm everything is configured correctly, you should check for your qmake, cmake and compiler:
		```bash
		$ qmake --version
		$ cmake --version
		$ g++ --version
		```
		If you encounter any errors such as `Command 'qmake' not found`, `Command 'cmake' not found` or `Command 'g++' not found`, refer to the [🚨 Issues Log](#sec-issues) for troubleshooting, specifically:
		
		  - [Issue #1 - QMake not found after installation](#issue-1)
		  - [Issue #2 - CMake not found after installation](#issue-2)
		  - [Issue #3 - g++ not found after installation](#issue-3)
		
		**Create a new QtQuick Project (Optional Test)**  
		Let's try creating a simple example to verify the setup:
		```bash
		$ mkdir test && cd test
		```
		
		Create 4 files named: `main.qml`, `main.cpp`, `CMakeLists.txt` and `qml.qrc`.
		```bash
		$ touch main.qml main.cpp CMakeLists.txt qml.qrc
		```
		
		Now insert the following code blocks on the files respectively:
		```qml
		import QtQuick
		import QtQuick.Controls
		
		ApplicationWindow {
		    visible: true
		    width: 640
		    height: 480
		    title: "Qt Quick Test"
		
		    Text {
		        anchors.centerIn: parent
		        text: "Hello QtQuick!"
		        font.pixelSize: 32
		    }
		}
		```
		
		```cpp
		#include &lt;QGuiApplication&gt;
		#include &lt;QQmlApplicationEngine&gt;
		
		int main(int argc, char *argv[])
		{
		    QGuiApplication app(argc, argv);
		    QQmlApplicationEngine engine;
		    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
		    return app.exec();
		}
		
		```
		
		```cmake
		cmake_minimum_required(VERSION 3.5)
		project(QtQuickTest LANGUAGES CXX)
		
		set(CMAKE_AUTOMOC ON)
		set(CMAKE_AUTORCC ON)
		set(CMAKE_AUTOUIC ON)
		set(CMAKE_CXX_STANDARD 11)
		
		find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)
		
		add_executable(QtQuickTest main.cpp qml.qrc)
		target_link_libraries(QtQuickTest PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick)
		
		```
		
		```html
		&lt;RCC&gt;
		  &lt;qresource prefix="/"&gt;
		    &lt;file&gt;main.qml&lt;/file&gt;
		  &lt;/qresource&gt;
		&lt;/RCC&gt;&gt;
		```
		
		After all this, build and run the project:
		```bash
		$ mkdir build && cd build
		$ cmake ..
		$ cmake --build .
		$ ./QtQuickTest
		```
		
		If a window with "Hello QtQuick!" appears this means the setup is complete.
		
		If you encounter any errors such as `Could NOT find OpenGL & Could NOT find WrapOpenGL`, `CMake Error at CMakeLists.txt:9 (find_package)`, refer to the [🚨 Issues Log](#sec-issues) for troubleshooting, specifically:
		
		  - [Issue #4 - Qt6Gui or WrapOpenGL not found when running cmake ..](#issue-4)
		  - [Issue #5 - Could not find package configuration file provided by "Qt6"](#issue-5)
		
		---
		
		
		&lt;a id="sec-development-setup"&gt;&lt;/a&gt;
		## 🧰 Development Setup
		
		This project demonstrates how to build a QtQuick based vehicle instrument cluster. The following steps describe how to set up the development environment and run the application
		
		**Requirements:**
		- **Qt 6.x** with the following modules:
		  - Qt6::Core
		  - Qt6::Gui
		  - Qt6::Qml
		  - Qt6::Quick
		  - Qt6::QuickControls2
		  - Qt6::DBus
		- CMake 3.5+
		- C++11 or later
		- Qt Creator (recommended IDE)
		- Git (version control)
		
		**Project Structure: (Not definitive nor implemented yet)**
		
		```bash
		cluster/
		├── CMakeLists.txt      #  Root build script
		├── src/                #  C++ Backend source files
		│   ├── main.cpp
		│   ├── backend/        #  Classes handling logic / simulation
		│   │   ├── timeprovider.cpp
		│   │   └── timeprovider.hpp
		│   └── utils/          #  Helper modules
		│       └── a.cpp
		├── qml/                #  QML UI Code
		│   ├── main.qml
		│   ├── components/     #  Reusable UI parts
		│   │   └── TopBar.qml
		│   │   └── NavBar.qml
		│   ├── screens/        #  Multi-view layouts (Cluster, Settings)
		│   │   └── BaseScreen.qml
		│   │   └── ClusterScreen.qml
		│   │   └── HomeScreen.qml
		│   │   └── SettingsScreen.qml
		│   └── themes/         #  Styling / Color schemes
		│       └── BaseTheme.qml
		├── assets/
		│   ├── icons/
		│   ├── images/
		│   └── fonts/
		└── qml.qrc             #  Resource file bundling Qml + assets
		```
		
		**Setup & Build:**
		
		```bash
		$ git clone git@github.com:SEAME-pt/SEA-ME_Team6_2025-26.git
		$ cd SEA-ME_Team6_2025-26/src/cluster
		```
		
		Via terminal:
		
		```bash
		$ mkdir build && cd build
		$ cmake ..
		$ cmake --build .
		
		./HelloQt6Qml
		```
		
		Via Qt Creator:
		
		- Open `CMakeLists.txt` in Qt Creator.
		- Select the Qt Kit (e.g., Desktop Qt 6.6.0 GCC 64-bit).
		- Configure and run the project.
		
		On Raspberry Pi:  
		More information on how to build and deploy the cluster on the Raspberry Pi on `../cross-compiler`
		
		---
		&lt;a id="sec-flow"&gt;&lt;/a&gt;
		## 🔀 Project Structuring and Layout
		
		The application's UI is organized into five primary QML components arranged in a top-center-bottom structure with two auxiliary side screens. This layout ensures a clean separation of responsibilities and makes each screen independently maintainable.
		
		### Overall Layout:
		&lt;img width="1851" height="1138" alt="image" src="https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/docs/images/Flow.jpg" /&gt;
		
		### Component Responsibilities
		
		#### TopBar.qml  
		- Stretches across the full width of the application.
		- Ideal location for:
		  - Application title
		  - Global navigation
		  - Status indicators
		  - Toolbar actions
		
		#### LeftScreen.qml  
		- Occupies the left region of the main content area.
		- Designed to display:
		  - The speedometer gauge
		  - Maximum speed indicator
		
		#### CenterScreen.qml  
		- Serves as the main working area.
		- Intended to display:
		  - Car visualization and dynamic circulation
		  - Current gear
		  - Signals indicators
		
		#### RightScreen.qml  
		- Mirroring the layout role of `LeftScreen.qml`, but positioned on the right.
		- Designed to contain:
		  - The tachometer
		
		#### BottomBar.qml  
		- Spans the entire width of the application.
		- Used to display:
		  - Information about the project sponsor
		
		The structure promotes clarity and modularity, and ease of maintenance. Each QML file is responsible for a specific section of the UI while remaining flexible enough to evolve or expand as the project grows. The three-panel central layout combined with the top and bottom bars forms a robust foundation for the cluster interface.
		
		---
		
		&lt;a id="sec-ui-components"&gt;&lt;/a&gt;
		## 🎨 UI Design & Components
		
		This section covers both:  
		- Frontend Components (QML): Define visuals and interactions.
		- Backend Components (C++): Handle data, simulation, and logic.
		
		They work together through Qt's signal-slot and QT_PROPERTY systems - allowing the frontend to automatically update when backend data changes.
		
		##### Frontend Components (QML)
		
		###### What is a QML Component?  
		In QtQuick, a component is a reusable building block of UI - similar to a widget or custom view in other frameworks.  
		Each component represents a self-contained visual element that can be easily reused, styled, and bound to dynamic data.  
		You can think of QML components as "mini UI modules" - they encapsulate:  
		- Structure (layout & child terms).
		- Behavior (animations, bindings, interactions).
		- Styling (colors, themes, shapes).
		
		A component can be:  
		- Inline - declared inside another QML file.
		- Standalone - saved as a separate `.qml` file inside your `components/` folder and imported wherever needed.
		
		##### Backend Components (C++)
		Backend components are C++ classes responsible for:
		- Simulating or acquiring data (e.g., time, speed, temperature).
		- Processing business logic.
		- Exposing properties and signals to QML.
		
		They don't render anything - they act as data providers or controllers for the QML frontend.  
		
		###### Example: `TimeProvider` Component  
		Let's say you want to show the current day and hour in your QML cluster.
		
		`timeprovider.hpp`
		```hpp
		#include &lt;QObject&gt;
		#include &lt;QTimer&gt;
		#include &lt;QDateTime&gt;
		
		class TimeProvider : public QObject
		{
		    Q_OBJECT
		    Q_PROPERTY(QString currentTime READ currentTime NOTIFY timeChanged)
		    Q_PROPERTY(QString currentDate READ currentDate NOTIFY dateChanged)
		
		public:
		    explicit TimeProvider(QObject *parent = nullptr);
		
		    QString currentTime() const;
		    QString currentDate() const;
		
		signals:
		    void timeChanged();
		    void dateChanged();
		
		private slots:
		    void updateTime();
		
		private:
		    QString m_currentTime;
		    QString m_currentDate;
		    QTimer m_timer;
		};
		```
		
		`timeprovider.cpp`
		```cpp
		#include "timeprovider.hpp"
		
		TimeProvider::TimeProvider(QObject *parent)
		    : QObject(parent)
		{
		    updateTime();
		    connect(&m_timer, &QTimer::timeout, this, &TimeProvider::updateTime);
		    m_timer.start(1000); // update every second
		}
		
		void TimeProvider::updateTime()
		{
		    QDateTime now = QDateTime::currentDateTime();
		
		    QString newTime = now.toString("hh:mm");
		    QString newDate = now.toString("dddd, MMM d");
		
		    if (newTime != m_currentTime) {
		        m_currentTime = newTime;
		        emit timeChanged();
		    }
		
		    if (newDate != m_currentDate) {
		        m_currentDate = newDate;
		        emit dateChanged();
		    }
		}
		
		QString TimeProvider::currentTime() const { return m_currentTime; }
		QString TimeProvider::currentDate() const { return m_currentDate; }
		```
		
		To make this available to your QML files, register it in your main.cpp before loading the QML engine:  
		
		`main.cpp`
		```cpp
		#include &lt;QGuiApplication&gt;
		#include &lt;QQmlApplicationEngine&gt;
		#include &lt;QQmlContext&gt;
		#include "backend/timeprovider.hpp"
		
		int main(int argc, char *argv[])
		{
		    QGuiApplication app(argc, argv);
		    QQmlApplicationEngine engine;
		
		    qmlRegisterType&lt;TimeProvider&gt;("Cluster.Backend", 1, 0, "TimeProvider");
		
		    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
		    if (engine.rootObjects().isEmpty())
		        return -1;
		
		    return app.exec();
		}
		```
		
		Now you can use the C++ Component just like any QML item:
		
		```qml
		import QtQuick
		import QtQuick.Controls
		import Cluster.Backend 1.0
		
		Rectangle {
		    width: 400
		    height: 200
		    color: BaseTheme.black
		
		    TimeProvider {
		        id: clock
		    }
		
		    Column {
		        anchors.centerIn: parent
		        spacing: 8
		
		        Text {
		            text: clock.currentTime
		            color: BaseTheme.white
		            font.pixelSize: 32
		        }
		
		        Text {
		            text: clock.currentDate
		            color: BaseTheme.primaryLight
		            font.pixelSize: 16
		        }
		    }
		}
		```
		
		The `TimeProvider` updates every second, and QML automatically reacts to `timeChanged` and `dateChanged` signals.  
		No manual pooling - that's the magic of Q_PROPERTY bindings.
		
		###### Understanding a C++ QML Component  
		A QML-exposed C++ class is usually a `QObject` subclass that exposed properties, signals and slots to QML.  
		This is what allows he backend logic (C++) and frontend UI (QML) to stay in sync.  
		
		Let's break down the `TimeProvider` example and explain what's happening.
		
		**`Q_OBJECT` - the foundation of all Qt meta-classes**  
		```cpp
		class TimeProvider: public QObject {
		  Q_OBJECT
		```
		
		**What it does:**  
		- Enables the Qt Meta-Object System, which provides:
		  - Reflection (the ability to discover properties/methods/signals at runtime).
		  - Signals & slots.
		  - Dynamic property access (what QML uses to "see" your C++ properties).
		- Required any class that uses `Q_PROPERTY`, `signals`, or `slots`.
		
		Without it, the class compiles but it won't work with QML - it can't send or receive signals, and QML can't see its properties.  
		
		**`Q_PROPERTY` - exposing data to QML**  
		```cpp
		Q_PROPERTY(QString currentTime READ currentTime NOTIFY timeChanged)
		Q_PROPERTY(QString currentDate READ currentDate NOTIFY dateChanged)
		```
		
		Each `Q_PROPERTY` tells Qt:
		- What data type the property has (`QString`).
		- What getter function to use (`READ currentTime`).
		- Optionally, what setter function to use (`WRITE setCurrentTime`).
		- What signal notifies changes (`NOTIFY timeChanged`).
		
		This is how QML knows what data exists on your C++ object.  
		QML can bind to it like this:  
		```qml
		Text { text: timeProvider.currentTime }
		```
		
		When your C++ code emits `timeChanged()`, QML automatically re-renders the bound text.
		
		**`signals` - telling QML that something changed**  
		```cpp
		signals:
		    void timeChanged();
		    void dateChanged();
		```
		
		A signal is a message that something happened.  
		When emitted, QML bindings that depend on the property automatically update.  
		
		Example:  
		```cpp
		    emit timeChanged();
		```
		
		That tells QML: "the `currentTime` property changed - refresh any UI that depends on it."
		
		**`slots` - callable functions (event handlers)**  
		```cpp
		private slots:
		    void updateTime();
		```
		
		A slot is a function that can be:
		- Automatically called when a signal is emitted (via `connect()`).
		- Called manually from QML (if marked public slots).
		
		In our example, the slot `updateTime()` runs every second via a `QTimer`:
		```cpp
		connect(&m_timer, &QTimer::timeout, this, &TimeProvider::updateTime);
		```
		
		For more advanced techniques and examples, refer to `docs/guides/QML-guide.md`.
		
		---
		
		&lt;a id="sec-issues"&gt;&lt;/a&gt;
		## 🚨 Issues Log
		
		This section tracks common issues encountered during development, testing or deployment of the instrument cluster.  
		Each issue includes its symptoms, probable cause and recommended fix or workaround.
		
		&lt;a id="issue-1"&gt;&lt;/a&gt;
		
		### Issue #1 - QMake not found after installation
		**- Error Example:**
		```bash
		$ qmake --version
		
		Command 'qmake' not found, but can be installed with:
		sudo apt install qtchooser
		```
		**- Cause:**  
		The terminal doesn't know where the Qt Tools and compiler are located.  
		When you install Qt using the Qt Online Installer / Qt Creator, it installs Qt inside its own directory, but doesn't automatically add it to the system PATH
		
		**- Solution:**  
		You can check if `qmake` exists in this path:  
		
		```bash
		$ ls ~/Qt/*/gcc_64/bin/qmake
		```    
		The Qt is installed correctly if it returns something like:  
		```bash
		/home/[user]/Qt/x.x.x/gcc_64/bin/qmake
		```
		
		You can now tell the terminal where to find it and temporarily add it to the PATH. 
		```bash
		$ export PATH=$PATH:~/Qt/x.x.x/gcc_64/bin
		```  
		After all those steps if you test again you should see something like:
		```bash
		QMake version 3.1
		Using Qt version x.x.x in /home/[user]/Qt/x.x.x/gcc_64/lib
		```  
		To avoid retyping the PATH every time, edit the `.bashrc` or `.bash_profile`:
		```bash
		$ nano ~/.bashrc
		
		# Add this line at the end of the file:
		export PATH=$PATH:~/Qt/x.x.x/gcc_64/bin
		```
		
		Then apply the change:
		```bash
		$ source ~/.bashrc
		```  
		
		&lt;a id="issue-2"&gt;&lt;/a&gt;
		
		### Issue #2 - CMake not found after installation
		**- Error Example:**
		```bash
		$ cmake --version
		
		Command 'cmake' not found, but can be installed with:
		sudo apt install cmake
		```
		**- Cause:**
		
		CMake is not installed or not added to your system PATH.  
		Even if Qt Creator is installed, its bundled CMAKE is not available globally in your terminal.
		
		**- Solution:**
		
		Install CMake manually using the package manager:  
		
		```bash
		$ sudo apt update
		$ sudo apt install cmake
		```  
		
		Then verify installation:
		
		```bash
		$ cmake --version
		
		cmake version x.x.x
		```  
		
		If you still get an error, restart the terminal or ensure `/usr/bin` is in your PATH:
		```bash
		$ echo $PATH
		``` 
		
		&lt;a id="issue-3"&gt;&lt;/a&gt;
		
		### Issue #3 - g++ not found after installation
		**- Error Example:**
		```bash
		$ g++ --version
		
		Command 'g++' not found, but can be installed with:
		sudo apt install g++
		```
		**- Cause:**
		
		You don't have the compiler and other developers tools installed.
		
		**- Solution:**
		
		You can install it with the `build-essential` package that includes gcc, g++, make and other developer tools  
		
		```bash
		$ sudo apt update
		$ sudo apt install build-essential
		```  
		
		After installation check again:
		
		```bash
		$ g++ --version
		
		g++ (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
		```  
		
		&lt;a id="issue-4"&gt;&lt;/a&gt;
		
		### Issue #4 - Qt6Gui or WrapOpenGL not found when running cmake ..
		**- Error Example:**
		```bash
		-- Could NOT find OpenGL (missing: OPENGL_opengl_LIBRARY OPENGL_glx_LIBRARY OPENGL_INCLUDE_DIR)
		-- Could NOT find WrapOpenGL (missing: WrapOpenGL_FOUND)
		CMake Warning at /home/[user]/Qt/x.x.x/gcc_64/lib/cmake/Qt6/Qt6Config.cmake:235 (find_package):
		  Found package configuration file:
		    /home/[user]/Qt/x.x.x/gcc_64/lib/cmake/Qt6Gui/Qt6GuiConfig.cmake
		  but it set Qt6Gui_FOUND to FALSE so package "Qt6Gui" is considered to be
		  NOT FOUND.  Reason given by package:
		  Qt6Gui could not be found because dependency WrapOpenGL could not be found.
		```
		**- Cause:** 
		
		Your system is missing the OpenGL development libraries that Qt needs to render graphics.  
		QtQuick and QtGui rely on OpenGL or compatible rendering libraries to build the GPU-accelerated UI.  
		This happens frequently on new Linux installations because OpenGL dev packages are not installed by default.
		
		**- Solution:**
		
		Install the required OpenGL and Mesa development packages:
		
		```bash
		$ sudo apt update
		$ sudo apt install build-essential libgl1-mesa-dev libglu1-mesa-dev libgles2-mesa-dev
		```
		  - `build-essential`: Compilers and base dev tools.
		  - `libgl1-mesa-dev`: Software implementation of OpenGL.
		  - `libglu1-mesa-dev`: OpenGL utility library.
		  - `libgles2-mesa-dev`: OpenGL ES support.
		
		Clean and rebuild the project:
		```bash
		$ rm -rf build
		$ mkdir build && cd build
		$ cmake ..
		
		-- Found OpenGL: /usr/lib/x86_64-linux-gnu/libOpenGL.so
		-- Found Qt6Gui
		-- Configuring done
		-- Generating done
		```
		
		&lt;a id="issue-5"&gt;&lt;/a&gt;
		
		### Issue #5 - Could not find package configuration file provided by "Qt6"
		**- Error Example:**
		```bash
		CMake Error at CMakeLists.txt:9 (find_package):
		  By not providing "FindQt6.cmake" in CMAKE_MODULE_PATH this project has
		  asked CMake to find a package configuration file provided by "Qt6", but
		  CMake did not find one.
		
		  Could not find a package configuration file provided by "Qt6" with any of
		  the following names:
		
		    Qt6Config.cmake
		    qt6-config.cmake
		
		  Add the installation prefix of "Qt6" to CMAKE_PREFIX_PATH or set "Qt6_DIR"
		  to a directory containing one of the above files.
		```
		
		**- Cause:** 
		
		CMake can't locate the Qt installation.
		This usually happens when:
		  - Qt isn't added to the system's `CMAKE_PREFIX_PATH`, so CMake doesn't know where to look for it.
		  - You installed Qt using the Qt Online Installed, but didn't set up the environment variables.
		  - The Qt Kit is installed, but you are running `cmake` manually outside of Qt Creator, which normally sets up paths automatically.
		
		**- Solution:**
		
		There are two main ways to fix this, depending on whether we are using Qt Creator or the terminal.
		
		 - **Option 1 - Use Qt Creator**
		  Qt Creator automatically configures `CMAKE_PREFIX_PATH` for you.
		    - Open Qt Creator.
		    - Go to File → Open File or Project → `CMakeLists.txt`.
		    - When prompted, select a Qt x.x.x Kit.
		    - Click Configure Project and then Run CMake
		
		 - **Option 2 - Configure Manually in Terminal**
		  If you installed via the Qt Installed the path for the cmake it's usally here: `~/Qt/x.x.x/gcc_64/lib/cmake`
		  Run CMake with the correct prefix:
		    ```bash
		    $ mkdir build && cd build
		    $ cmake .. -DCMAKE_PREFIX_PATH=$HOME/Qt/x.x.x/gcc_64/lib/cmake
		    
		    -- Found Qt6: /home/[user]/Qt/x.x.x/gcc_64/lib/cmake/Qt6 (found version "x.x.x")
		    -- Configuring done
		    -- Generating done
		    ```  
		
		    Alternatively, set the Qt6_DIR environment variable explicitly:
		    ```bash
		    $ export Qt6_DIR=$HOME/Qt/x.x.x/gcc_64/lib/cmake/Qt6
		    ```  
		
		&lt;!-- &lt;a id="issue-x"&gt;&lt;/a&gt;
		
		### Issue #x - 
		**- Error Example:**
		
		**- Cause:** 
		
		**- Solution:** --&gt;
		
		---
		
		&lt;a id="sec-links"&gt;&lt;/a&gt;
		## 🔗 Links
		
		 - https://blackberry.qnx.com/en/ultimate-guides/software-defined-vehicle/instrument-cluster  
		 - https://tree.nathanfriend.com  
		 - https://doc.qt.io/qt-6/qtqml-cppintegration-overview.html  
		 - https://felgo.com/doc/qt5/qtqml-cppintegration-topic/  
		 - https://doc.qt.io/qtforpython-6/faq/whatisqt.html  
		 - https://somcosoftware.com/en/blog/qml-vs-qt-widgets-detailed-comparison  
		 - https://extenly.com/2024/05/06/qt-widgets-vs-qt-quick-deciding-the-best-approach-for-your-project/  
		 - https://www.qt.io
		 - https://cmake.org  
		 - https://packages.ubuntu.com
		 
		
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_7 | Reviewed: ✔ | Score: 1.0 ### {: #evidences-evid_l0_7 data-toc-label="EVIDENCES-EVID_L0_7" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_7](ASSERTIONS.md#assertions-assert_l0_7) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `src/cross-compiler/Dockerfile`

	??? "Click to view reference"

		```text
		src/cross-compiler/Dockerfile
		```


- `src/cross-compiler/DockerFileRasp`

	??? "Click to view reference"

		```text
		src/cross-compiler/DockerFileRasp
		```


- `src/cross-compiler/opencvToolchain.cmake`

	??? "Click to view reference"

		````cmake
		cmake_minimum_required(VERSION 3.25)
		include_guard(GLOBAL)
		
		# Set the system name and processor for cross-compilation
		set(CMAKE_SYSTEM_NAME Linux)
		set(CMAKE_SYSTEM_PROCESSOR aarch64)
		
		# Set the target sysroot and architecture
		set(TARGET_SYSROOT /build/sysroot)
		set(TARGET_ARCHITECTURE aarch64-linux-gnu)
		set(CMAKE_SYSROOT ${TARGET_SYSROOT})
		set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})
		
		# Configure the pkg-config environment variables
		set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${CMAKE_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}/pkgconfig")
		set(ENV{PKG_CONFIG_LIBDIR} "/usr/lib/pkgconfig:/usr/share/pkgconfig:${CMAKE_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}/pkgconfig:${CMAKE_SYSROOT}/usr/lib/pkgconfig")
		set(ENV{PKG_CONFIG_SYSROOT_DIR} "${CMAKE_SYSROOT}")
		
		# Set the C and C++ compilers
		set(CMAKE_C_COMPILER /usr/bin/${TARGET_ARCHITECTURE}-gcc-14)
		set(CMAKE_CXX_COMPILER /usr/bin/${TARGET_ARCHITECTURE}-g++-14)
		
		# Set compiler optimizations for ARM
		set(CMAKE_C_FLAGS "-march=armv8-a -mtune=cortex-a72 -O2 --sysroot=${CMAKE_SYSROOT}" CACHE STRING "" FORCE)
		set(CMAKE_CXX_FLAGS "-march=armv8-a -mtune=cortex-a72 -O2 --sysroot=${CMAKE_SYSROOT}" CACHE STRING "" FORCE)
		# set(CMAKE_C_FLAGS "-march=armv8.2-a+dotprod+fp16 -mtune=cortex-a72 -ftree-vectorize --sysroot=${CMAKE_SYSROOT}" CACHE STRING "" FORCE)
		# set(CMAKE_CXX_FLAGS "-march=armv8.2-a+dotprod+fp16 -mtune=cortex-a72 -ftree-vectorize --sysroot=${CMAKE_SYSROOT}" CACHE STRING "" FORCE)
		
		# Set linker flags to use sysroot libraries + OpenGL + Math
		set(OPENGL_LIB_PATH "${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu")
		set(MATH_LIB_PATH "${CMAKE_SYSROOT}/usr/lib/aarch64-linux-gnu")
		
		set(CMAKE_EXE_LINKER_FLAGS_INIT "--sysroot=${CMAKE_SYSROOT} \
		    -L${CMAKE_SYSROOT}/usr/lib \
		    -Wl,-rpath-link,${CMAKE_SYSROOT}/lib:${CMAKE_SYSROOT}/usr/lib \
		    -L${MATH_LIB_PATH} -L${OPENGL_LIB_PATH} \
		    -Wl,-rpath-link,${MATH_LIB_PATH}:${OPENGL_LIB_PATH} \
		    -lm -lGLEW -lGLU -lGL -lEGL -lX11 -lGLX -lXext -lXrandr")  # Order matters: GLEW before GL/GLU
		
		set(CMAKE_SHARED_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT}")
		set(CMAKE_MODULE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT}")
		
		# CMake find settings
		SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
		SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
		SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
		
		````


- `src/cross-compiler/toolchain.cmake`

	??? "Click to view reference"

		````cmake
		cmake_minimum_required(VERSION 3.25)
		include_guard(GLOBAL)
		
		# Set the system name and processor for cross-compilation
		set(CMAKE_SYSTEM_NAME Linux)
		set(CMAKE_SYSTEM_PROCESSOR arm)
		
		# Set the target sysroot and architecture
		set(TARGET_SYSROOT /build/sysroot)
		set(TARGET_ARCHITECTURE aarch64-linux-gnu)
		set(CMAKE_SYSROOT ${TARGET_SYSROOT})
		
		# Configure the pkg-config environment variables
		set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${CMAKE_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}/pkgconfig")
		set(ENV{PKG_CONFIG_LIBDIR} "/usr/lib/pkgconfig:/usr/share/pkgconfig:${CMAKE_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}/pkgconfig:${CMAKE_SYSROOT}/usr/lib/pkgconfig")
		set(ENV{PKG_CONFIG_SYSROOT_DIR} "${CMAKE_SYSROOT}")
		
		# Set the C and C++ compilers
		set(CMAKE_C_COMPILER /usr/bin/${TARGET_ARCHITECTURE}-gcc-12)
		set(CMAKE_CXX_COMPILER /usr/bin/${TARGET_ARCHITECTURE}-g++-12)
		
		# Define additional compiler flags
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -isystem=/usr/include -isystem=/usr/local/include -isystem=/usr/include/${TARGET_ARCHITECTURE}")
		set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
		
		# Set Qt-specific compiler and linker flags
		set(QT_COMPILER_FLAGS "-march=armv8-a -mtune=generic -ftree-vectorize")
		set(QT_COMPILER_FLAGS_RELEASE "-O2 -pipe")
		set(QT_LINKER_FLAGS "-Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed -Wl,-rpath-link=${TARGET_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE} -Wl,-rpath-link=$HOME/qt6/pi/lib")
		
		# Configure CMake find root path modes
		set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
		set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
		set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
		set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
		
		# Set the install RPATH use and build RPATH
		set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
		set(CMAKE_BUILD_RPATH ${TARGET_SYSROOT})
		
		# Initialize CMake configuration variables with the specified flags.
		set(CMAKE_C_FLAGS_INIT "${QT_COMPILER_FLAGS} -march=armv8-a")
		set(CMAKE_CXX_FLAGS_INIT "${QT_COMPILER_FLAGS} -march=armv8-a")
		set(CMAKE_EXE_LINKER_FLAGS_INIT "${QT_LINKER_FLAGS} -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed")
		set(CMAKE_SHARED_LINKER_FLAGS_INIT "${QT_LINKER_FLAGS} -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed")
		set(CMAKE_MODULE_LINKER_FLAGS_INIT "${QT_LINKER_FLAGS} -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed")
		
		# Set paths for XCB and OpenGL libraries
		set(XCB_PATH_VARIABLE ${TARGET_SYSROOT})
		set(GL_INC_DIR ${TARGET_SYSROOT}/usr/include)
		set(GL_LIB_DIR ${TARGET_SYSROOT}:${TARGET_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE}:${TARGET_SYSROOT}/usr:${TARGET_SYSROOT}/usr/lib)
		set(EGL_INCLUDE_DIR ${GL_INC_DIR})
		set(EGL_LIBRARY ${XCB_PATH_VARIABLE}/usr/lib/${TARGET_ARCHITECTURE}/libEGL.so)
		set(OPENGL_INCLUDE_DIR ${GL_INC_DIR})
		set(OPENGL_opengl_LIBRARY ${XCB_PATH_VARIABLE}/usr/lib/${TARGET_ARCHITECTURE}/libOpenGL.so)
		set(GLESv2_INCLUDE_DIR ${GL_INC_DIR})
		set(GLIB_LIBRARY ${XCB_PATH_VARIABLE}/usr/lib/${TARGET_ARCHITECTURE}/libGLESv2.so)
		set(GLESv2_LIBRARY ${XCB_PATH_VARIABLE}/usr/lib/${TARGET_ARCHITECTURE}/libGLESv2.so)
		
		# Correct the setting for gbm_LIBRARY
		set(gbm_INCLUDE_DIR ${GL_INC_DIR})
		set(gbm_LIBRARY ${XCB_PATH_VARIABLE}/usr/lib/${TARGET_ARCHITECTURE}/libgbm.so)
		
		set(Libdrm_INCLUDE_DIR ${GL_INC_DIR})
		set(Libdrm_LIBRARY ${XCB_PATH_VARIABLE}/usr/lib/${TARGET_ARCHITECTURE}/libdrm.so)
		set(XCB_XCB_INCLUDE_DIR ${GL_INC_DIR})
		set(XCB_XCB_LIBRARY ${XCB_PATH_VARIABLE}/usr/lib/${TARGET_ARCHITECTURE}/libxcb.so)
		
		# Append to CMake library and prefix paths
		list(APPEND CMAKE_LIBRARY_PATH ${CMAKE_SYSROOT}/usr/lib/${TARGET_ARCHITECTURE})
		list(APPEND CMAKE_PREFIX_PATH "/usr/lib/${TARGET_ARCHITECTURE}/cmake")
		````



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_8 | Reviewed: ✔ | Score: 1.0 ### {: #evidences-evid_l0_8 data-toc-label="EVIDENCES-EVID_L0_8" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_8](ASSERTIONS.md#assertions-assert_l0_8) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/guides/CAN-overview.md`

	??? "Click to view reference"

		````md
		# CAN Bus Overview
		
		## 1. Introduction
		
		The **Controller Area Network (CAN)** is a communication protocol widely used in automotive and embedded systems.  
		It allows multiple electronic control units (ECUs) to exchange data efficiently over a **shared two-wire bus** — **CAN_H** and **CAN_L** — reducing wiring complexity and improving reliability.
		
		CAN is robust against electrical noise, supports real-time communication, and includes built-in error detection and priority-based message arbitration.
		
		---
		
		**First test implementation** - Microcontroller + speed sensor + MCP2551 module
		
		![&lt;CANbus Scheme Updated.png&gt;](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/demos/CANbus%20Scheme%20Updated.png)
		
		## 2. Purpose in Our Project
		
		In this project, we use the **CAN bus** to enable communication between the main computing boards and low-level controllers.  
		Specifically:
		
		| Device | Role | Description |
		|:--|:--|:--|
		| **Raspberry Pi 5** | High-level node | Receives data from STM32 via CAN and displays it in the cluster UI (Qt application). |
		| **STM32U585** | Low-level node | Reads sensor data (wheel speed, etc.), processes it, and sends CAN frames to the Raspberry Pi through the MCP2515 module. |
		| **MCP2515 + MCP2551 CAN Module** | CAN interface | Provides CAN controller and transceiver functions for STM32 and Raspberry Pi communication. |
		
		This architecture replicates how modern vehicles share information between ECUs in real time.
		
		---
		
		## 3. System Architecture
		
		Wheel Speed Sensor -&gt; STM32U585 MCU ⇄ **MCP2515 + MCP2551 CAN Module** ═══ CAN_H / CAN_L ═══ **MCP2515 + MCP2551 CAN Module** ⇄ Raspberry Pi 5
		
		- The **STM32U585** communicates with the **MCP2515 module** via SPI, which handles the CAN protocol logic.  
		- The **MCP2515** also acts as a CAN controller for the **Raspberry Pi 5**, communicating over SPI.  
		- Both modules integrate **MCP2551** transceivers for electrical signaling on CAN_H and CAN_L.  
		- The CAN bus is terminated with **120 Ω resistors** at both ends.
		
		---
		
		## 4. Communication Configuration
		
		| Parameter | Value | Notes |
		|:--|:--|:--|
		| **Protocol** | Classical CAN 2.0A/B | 11-bit identifiers |
		| **Bitrate** | 500 kbit/s | Standard for embedded automotive systems |
		| **Frame ID** | 0x101 | Wheel speed data |
		| **Data Length** | 2 bytes | Encodes speed in 0.1 km/h resolution |
		| **Termination** | 120 Ω × 2 | One at each bus end |
		
		---
		
		## 5. Hardware Components
		
		| Component | Function |
		|:--|:--|
		| **STM32U585 (B-U585I-IOT02A)** | Reads speed sensor, computes speed, and sends data to MCP2515 over SPI |
		| **MCP2515 + MCP2551 CAN Module (x2)** | Provides CAN controller + transceiver interface for both STM32 and Raspberry Pi |
		| **Raspberry Pi 5** | Receives CAN data via MCP2515 (SPI) and displays it via Qt |
		| **120 Ω resistor** | Terminates the bus |
		| **Wheel Speed Sensor** | Generates pulse signal proportional to speed |
		
		---
		
		## 6. Data Flow
		
		1. The **speed sensor** sends digital pulses to the STM32.  
		2. The **STM32** measures the pulse frequency and computes the speed.  
		3. The **STM32** writes this value to the **MCP2515 module**, which transmits a CAN frame (ID **0x101**, **2 bytes**).  
		4. The **Raspberry Pi** receives the frame through its **MCP2515** CAN interface.  
		5. The **Qt application** updates the cluster display in real time.
		
		---
		
		## 7. Benefits of Using CAN
		
		- **Reliability:** Built-in error detection and retransmission.  
		- **Determinism:** Message priority and fixed timing.  
		- **Scalability:** Easy to add more ECUs or sensors.  
		- **Industry Standard:** Mirrors real automotive systems.
		
		---
		
		## 8. Future Extensions
		
		- Add additional CAN messages (temperature, steering angle).
		
		---
		
		
		---
		
		## 9. References
		
		- **Microchip MCP2515 Datasheet**
		- **Raspberry Pi SPI Pinout** – [pinout.xyz](https://pinout.xyz/pinout/spi)
		- **CAN Tutorial** - [CAN-tutorial-setup](https://lastminuteengineers.com/mcp2515-can-module-arduino-tutorial/)   
		
		---
		
		````


- `docs/guides/CAN-test-guide.md`

	??? "Click to view reference"

		````md
		# CAN implementation
		
		![CAN Scheme - New update.png](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/demos/CAN%20Scheme%20-%20New%20update.png)
		Scheme of our wiring
		
		The objective is to communicate via CAN from the STM32 to the Raspberry Pi (and vice versa). 
		To do that follow the following steps:
		
		1. Connect the wiring like the image above
		2. Connect micro-usb to CN8 (STLINK)
		3. Open STM32CubeIDE
			1.  1. Go to 'File' -&gt; 'New' -&gt; 'STM32 Project'
			2. Select the Board on the 'Board Selector'
			3. Common problems at this stage:
			    1. ST-Link not connected: Unplug the MCU and plug it back in :)
			    2. GDB not encountered: Go to STM32CubeIDE → Preferences -&gt; STM32Cube → Toolchain Manager (it will update automatically the libraries that are needed)
		4. Configure Pinout in the STM32CubeIDE
			1. Double click the .ioc file
			2. For the speedometer:
				1. Select the PB0 pin and select mode: GPIO_EXTI
				2. On that pin, select: External Interrupt Mode with **Rising edge** trigger and **Pull:** **Pull-Down**
				3. In the NVIC tab select EXTI line interrupts -&gt; enable
			3. For the MCP2515 (CAN module):
				1. Pinout -&gt; Connectivity -&gt; SPI1: Mode: Full Duplex Master
				2. Configuration tab -&gt; SPI1 -&gt; verify if:
					   - Mode: Full Duplex Master
					   - Hardware NSS Signal: Disabled (use manual CS via GPIO)
				3. Parameters:
					1. Prescaler: SPI_BAUDRATEPRESCALER_16
					2. **Clock Polarity (CPOL):** `Low`
					3. **Clock Phase (CPHA):** `1 Edge`
					4. **CRC Calculation:** _Disabled_
					5. **Data Size:** `8 bits`
				4. NSS Signal Type: Software (NSS = Soft)
				5. GPIO for CS -&gt; Click on PE12
					1. Mode: GPIO Output
					2. Go to 'System Core' in the left tab -&gt; GPIO
					3. **GPIO Output Level:** “High” (so CS is inactive by default)
					4. **GPIO Pull-up/Pull-down:** “No pull-up and no pull-down”
					5. **Maximum Output Speed:** “Low” (fine for CS)
					6. **User Label**: CS_PIN
					7. Problems:
						1. If there isn't #define GPIO_PIN_12 / GPIOE (search in main.h)
						2. You need to define it yourself
						3. #define CS_PIN_Pin GPIO_PIN_12
						4. #define CS_PIN_GPIO_Port GPIOE
				6. INT in PE7
					1. Mode: GPIO EXTI7
					2. External Interrupt Mode with Falling Edge
					3. Pull: No pull-down and no pull-up
					4. In the NVIC tab select EXTI line interrupts -&gt; enable
			4. Configure CNF - time
				1. CAN module 8 MHz -&gt; 500 kbps (this is done by code)
		```	
					//Setting for MCP2515 @ 8MHz, 500 kbits/s:
					CNF1 = 0x00
					CNF2 = 0x90
					CNF3 = 0x02
		```
		5. Need to manually configure the SPI1 pins in the STM32
			1. This includes assigning the following pins in the Pinout View in CubeMX:
			2. PE13: SPI1_SCK
			3. PE14: SPI1_MISO
			4. PE15: SPI1_MOSI
			5. PE12: GPIO_Output 
		6. Click in **Generate Code** (it will create files needed (.c/.h) for the running test on the microcontroller)  
			1. Problems at this stage:
			    1. If it doesn't create .h files (like gpio.h), it is needed to go to configurations -&gt; create the pair (.h/.c) files
		7. Create `mcp2515.h` and `mcp2515.c` (the code will be attached) 
		8. After this, you will need to make sure that:
			1. `main.c` includes `mcp2515.h`
			2. `mcp2515.h` includes `stm32u5xx_hal_conf.h`
			3. `mcp2515.c` includes `mcp2515.h`
			4. `main.h` includes `stm32u5xx_hal.h`
		
		````


- `docs/guides/CAN-explanation.md`

	??? "Click to view reference"

		````md
		# How CAN Bus Works
		
		# Index
		
		1. [Introduction](#1-introduction)  
		2. [Physical Signal and Electrical Layer](#2-physical-signal-and-electrical-layer)  
		3. [Bus Topology](#3-bus-topology)  
		4. [Half-Duplex Communication](#4-half-duplex-communication)  
		5. [Asynchronous Transmission](#5-asynchronous-transmission)  
		6. [Message Arbitration](#6-message-arbitration)  
		7. [CAN Data Frame Structure](#7-can-data-frame-structure)
		8. [Classic CAN vs CAN FD](#8-classic-can-vs-can-fd)
		9. [Summary of Key Properties](#9-summary-of-key-properties)  
		10. [Why It Matters in Automotive Systems](#10-why-it-matters-in-automotive-systems)  
		11. [References](#11-references)
		
		## 1. Introduction
		
		The **Controller Area Network (CAN)** is a serial communication protocol developed by Bosch in the 1980s, designed for reliable data exchange between electronic control units (ECUs) in vehicles and embedded systems.
		
		Unlike simple serial links (UART, SPI, I²C), CAN allows **multiple devices to share the same communication lines** while maintaining **high reliability**, **error detection**, and **real-time deterministic behavior**.
		
		---
		
		## 2. Physical Signal and Electrical Layer
		
		CAN uses **two differential signal lines**:
		- **CAN_H (High)**
		- **CAN_L (Low)**
		
		![alt text](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/images/CANsignal.png)
		
		Both lines carry complementary voltages that represent logical bits:
		
		| Bus State | CAN_H Voltage | CAN_L Voltage | Logical Level | Description |
		|:--|:--|:--|:--|:--|
		| **Dominant** | ≈ 3.5 V | ≈ 1.5 V | `0` | Actively driven by one or more nodes (overrides recessive) |
		| **Recessive** | ≈ 2.5 V | ≈ 2.5 V | `1` | Idle state, no node drives the bus |
		
		🧩 Because CAN uses **differential signaling**, it is highly resistant to electromagnetic noise — both lines rise or fall together, so interference affects them equally and cancels out.
		
		---
		
		## 3. Bus Topology
		
		CAN is implemented as a **multi-drop bus** — all nodes share the same two wires (CAN_H, CAN_L):
		
		![can-topology](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/images/CANtopology2.png)
		
		
		### Topology Characteristics
		- **Linear (bus) structure**, not star or ring.
		- **120 Ω termination resistors** at both physical ends to prevent signal reflections.
		- Any node can transmit, but only **one at a time**.
		- Nodes can be added or removed easily without changing the others.
		
		![can-topology-2](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/images/CANtopology.png)
		
		---
		
		## 4. Half-Duplex Communication
		
		The CAN bus is **half-duplex**, meaning:
		- All nodes share the same physical medium.
		- Data flows **in one direction at a time**.
		- When one node transmits, all others listen simultaneously.
		
		![half-duplex](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/images/Half-duplex-communication.png)
		
		Unlike full-duplex systems (e.g., Ethernet), CAN avoids collisions using an **arbitration mechanism** — nodes monitor the bus and back off when they detect higher-priority messages.
		
		---
		
		## 5. Asynchronous Transmission
		
		CAN is **asynchronous**, meaning:
		- There is **no shared clock line**.
		- Each node uses its **own local oscillator**.
		- Synchronization is achieved through **bit timing segments** and **edge sampling** within the CAN protocol.
		
		Every frame starts with a **Start-of-Frame (SOF)** bit, which synchronizes all nodes before data transmission.
		
		This design allows devices running at slightly different clock frequencies to communicate reliably.
		
		---
		
		## 6. Message Arbitration
		
		When multiple nodes attempt to transmit simultaneously:
		1. Each node sends its **message ID** bit by bit.
		2. The bus operates on a **wired-AND** principle:
		   - If any node transmits a `0` (dominant), it overrides all `1`s (recessive).
		3. Nodes monitor the bus in real-time:
		   - If a node sends a `1` but reads a `0`, it knows a higher-priority message is on the bus and **backs off** immediately.
		
		Result:  
		✅ The node with the **lowest message ID** (highest priority) continues transmitting without collision.
		
		---
		
		## 7. CAN Data Frame Structure
		
		Each CAN message is called a **frame**.  
		In Classical CAN, a standard frame includes:
		
		![can-frame](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/images/CANFrame.png)
		
		| Field | Bits | Description |
		|:--|:--|:--|
		| **Start of Frame (SOF)** | 1 | Marks the beginning of a message (dominant bit). |
		| **Identifier** | 11 | Defines message priority (lower = higher priority). |
		| **RTR (Remote Transmission Request)** | 1 | Differentiates data and request frames. |
		| **IDE (Identifier Extension)** | 1 | `0` for standard 11-bit, `1` for extended 29-bit ID. |
		| **r0 (Reserved)** | 1 | Reserved bit (dominant). |
		| **Data Length Code (DLC)** | 4 | Number of data bytes (0–8). |
		| **Data Field** | 0–64 | The actual payload (0–8 bytes in Classic CAN). |
		| **CRC Field** | 15 + 1 | Error-detection code plus delimiter. |
		| **ACK Field** | 2 | Acknowledgment bits (sent by receivers). |
		| **End of Frame (EOF)** | 7 | Marks the end of transmission (recessive bits). |
		| **Inter-Frame Space** | 3+ | Bus idle period before next frame. |
		
		---
		
		## 8. Classic CAN vs CAN FD
		
		| Feature | 🚗 Classic CAN | 🚀 CAN FD |
		|:--|:--|:--|
		| **Max Payload** | 8 bytes | 64 bytes |
		| **Arbitration Speed** | ≤ 1 Mbps | ≤ 1 Mbps |
		| **Data Speed** | ≤ 1 Mbps | **Up to 8 Mbps** |
		| **CRC** | 15-bit | 17/21-bit (stronger) |
		| **Compatibility** | Legacy systems | Modern systems; backward compatible |
		| **Ideal Use** | Simple sensors & actuators | High-data signals (ADAS, gateways) |
		
		### 🧠 Quick Summary
		- 🚗 **Classic CAN** → small, robust, perfect for simple/periodic signals  
		- 🚀 **CAN FD** → faster, larger frames, ideal for modern vehicles  
		- 🔄 CAN FD networks can still interoperate with Classic CAN nodes  
		
		---
		
		## 9. Summary of Key Properties
		
		| Property | Description |
		|:--|:--|
		| **Signaling** | Differential pair (CAN_H / CAN_L) |
		| **Communication** | Half-duplex, multi-master |
		| **Transmission** | Asynchronous (self-clocking) |
		| **Frame length** | 0–8 bytes (Classic CAN) |
		| **Bus speed** | Up to 1 Mbit/s (Classic)|
		| **Topology** | Linear bus with 120 Ω termination |
		| **Error handling** | Automatic detection, retransmission, and node isolation |
		
		---
		
		## 10. Why It Matters in Automotive Systems
		
		- **Low cost & reliability:** Proven over decades in automotive and industrial environments.
		- **Deterministic timing:** Critical for safety functions (ABS, engine control).  
		- **Shared bus:** Dozens of ECUs communicate over the same wires.  
		- **Prioritized IDs:** Ensures urgent signals (e.g., brake) always transmit first.  
		
		
		---
		
		## 11. References
		 
		- Microchip, *MCP2551 Datasheet*  
		- [Texas Instruments – Understanding and Applying CAN Bus](https://www.ti.com/lit/an/sloa101b/sloa101b.pdf)
		
		---
		
		````


- `docs/images/CANFrame.png`

	??? "Click to view reference"

		```text
		docs/images/CANFrame.png
		```


- `docs/images/CANsignal.png`

	??? "Click to view reference"

		```text
		docs/images/CANsignal.png
		```


- `docs/images/CANtopology.png`

	??? "Click to view reference"

		```text
		docs/images/CANtopology.png
		```


- `docs/images/CANtopology2.png`

	??? "Click to view reference"

		```text
		docs/images/CANtopology2.png
		```


- `docs/images/Half-duplex-communication.png`

	??? "Click to view reference"

		```text
		docs/images/Half-duplex-communication.png
		```



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_9 | Reviewed: ✔ | Score: 1.0 ### {: #evidences-evid_l0_9 data-toc-label="EVIDENCES-EVID_L0_9" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_9](ASSERTIONS.md#assertions-assert_l0_9) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/guides/ThreadX_Installation_Guide.md`

	??? "Click to view reference"

		````md
		# 🧵 Installing ThreadX on an STM32 – Complete Guide
		---------------------------
		# ✅ 1. What You Need
		  # Software
		  - STM32CubeIDE.
		  - STM32CubeMX.
		    
		  # Hardware
		  - An STM32 development board.
		---------------------------
		# 🧱 2. Enable ThreadX Using STM32CubeMX
		  **Step 1 — Create a New Project**
		  1. Open **STM32CubeMX**
		  2. Select your **MCU** or **board**
		  3. Configure your clocks (or accept defaults)
		     
		  **Step 2 — Enable Azure RTOS / ThreadX**
		  1. Go to Middleware in the left sidebar
		  2. Enable Azure RTOS → ThreadX
		
		  **Step 3 — Configure ThreadX**
		  
		  Under Middleware → ThreadX, configure:
		  - Tick rate (usually 1000 Hz).
		  - Preemption threshold.
		  - Timer task priority.
		
		# 🧰 3. Build and Run in STM32CubeIDE
		
		  1. Open the generated project in STM32CubeIDE.
		  2. Build the project.
		  3. Flash it to your STM32 using the Run button.
		
		# 🧪 4. Verify ThreadX Is Running
		
		&lt;img width="1851" height="1138" alt="image" src="https://github.com/user-attachments/assets/3c70aa7d-53c5-42b1-87fd-b4d871169877" /&gt;
		
		  1. Enable a GPIO Output for the LED.
		  - Pick a pin.
		  - Mode: **GPIO_Output**
		  - Name: LED_Pin
		
		  2. Add a ThreadX Application Thread.
		
		      In CubeMX → ThreadX → Application\
		       Create a thread:
		     - Name: blink_thread
		     - Entry function: blink_thread_entry
		     - Priority: 1
		     - Stack size: 1024
		       
		  3. Implement the Blink Thread
		     
		      in app_threadx.c
		        ```c
		        void blink_thread_entry(ULONG thread_input)
		        {
		            while (1)
		            {
		                HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		                tx_thread_sleep(100); // 100 ticks → ~100 ms blink
		            }
		        }
		
		This makes the LED toggle every ~0.1 seconds.
		
		  4. Ensure ThreadX starts in main.c
		
		     CubeMX does this automatically:
		      ```c
		        int main(void)
		        {
		            HAL_Init();
		            SystemClock_Config();
		            MX_GPIO_Init();
		            MX_ThreadX_Init();    // IMPORTANT: starts ThreadX kernel
		        
		            while (1)
		            {
		            }
		        }
		  If everything is working:
		
		✔️ ThreadX starts.
		
		✔️ The scheduler runs your thread.
		
		✔️ You see the LED blinking on the board.
		
		````


- `docs/guides/AGL_and_ThreadX_benefits_Guide.md`

	??? "Click to view reference"

		````md
		# Benefits of Automotive Grade Linux
		
		- Inherits the robustness and security of Linux  
		- Backed by the Linux Foundation and major automakers  
		- Includes ready-to-use frameworks for multimedia, connectivity, HMI, navigation, and vehicle bus (CAN) integration  
		- Based on standard Linux, so developers can modify kernels, drivers, and user-space applications freely to fit specific needs.  
		
		---
		
		# Benefits of ThreadX
		
		- Ultra-low interrupt latency and fast context switching make it ideal for time-critical automotive functions  
		- Requires very little memory, making it perfect for microcontrollers  
		- Simple API and rich debugging tools  
		- ThreadX’s preemptive, priority-based scheduling ensures predictable task execution  
		
		---
		
		## Combined Benefits on a PiRacer
		
		- **Hybrid Architecture:** AGL handles the **high-level user interface, AI inference, and connectivity**, while ThreadX manages **real-time motor and sensor control**.  
		- **Optimized Performance:** Dividing tasks between AGL (Linux-based) and ThreadX (RTOS) maximizes both processing efficiency and determinism.  
		- **Scalable Design:** This setup mirrors modern automotive ECUs, making the PiRacer a strong platform for **autonomous driving research and prototyping**.  
		- **Educational and Practical:** Ideal for learning about real automotive-grade system design — combining open-source Linux flexibility with real-time embedded reliability. 
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_10 | Reviewed: ✔ | Score: 1.0 ### {: #evidences-evid_l0_10 data-toc-label="EVIDENCES-EVID_L0_10" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_10](ASSERTIONS.md#assertions-assert_l0_10) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/guides/Power Consumption Analysis.md`

	??? "Click to view reference"

		````md
		# Power Consumption Analysis - SEA:ME Project (Without Display)
		
		## Component Consumption Table
		
		| Component | Quantity | Voltage (V) | Min Current (mA) | Max Current (mA) | Min Power (W) | Max Power (W) | Notes |
		|-----------|----------|--------------|------------------|------------------|---------------|---------------|-------|
		| **Raspberry Pi 5** | 1 | 5.0 | 500 | 5000 | 2.5 | 25.0 | Idle: ~3W, Max Performance: 25W |
		| **Hailo-8 HAT (26 TOPS)** | 1 | 3.3-5.0 | 200 | 2500 | 0.66 | 12.5 | Typical: 2.5W, Max: ~12.5W |
		| **B-U585I-IOT02A** | 1 | 5.0 | 2 | 500 | 0.007 | 1.65 | With WiFi active: ~170mA @ 5V |
		| **Servo Motor WS-MG99SR** | 1 | 4.8-6.0 | 100 | 1200 | 0.5 | 7.2 | Idle: ~100mA, Stall: ~1200mA |
		| **DC Motors (propulsion)** | 2 | 6.0-12.0 | 200 | 2000 | 2.4 | 48.0 | Per motor: 1.2W-24W (depends on specific motor) |
		| **MCP2515 CAN Transceiver** | 2 | 5.0 | 1 | 5 | 0.01 | 0.05 | Typical: 5mA/module, Standby: 1μA |
		| **SSD M.2 (via PCIe)** | 1 | 3.3 | 50 | 500 | 0.165 | 1.65 | Depends on specific model |
		| **PCI-E to USB Converter** | 1 | 5.0 | 50 | 200 | 0.25 | 1.0 | Passive conversion |
		| **Speed Sensor (Hall)** | 1 | 3.3-5.0 | 5 | 15 | 0.025 | 0.075 | Typical Hall effect sensor |
		| **LM2596 Step-Down** | 1 | - | - | - | - | 0.4 | Efficiency losses (~92%) |
		| **XL4015 Step-Down** | 1 | - | - | - | - | 0.8 | Efficiency losses (~90%) |
		| **TB6612FNG Driver** | 1 | 2.7-5.5 (logic) | 5 | 3200 | 0.025 | 16.0 | VCC + VM, 1.2A cont./channel (3.2A peak) |
		
		---
		
		## System Total Summary (WITHOUT DISPLAY)
		
		| Condition | Battery Voltage | Total Current (A) | Total Power (W) | Observations |
		|-----------|-----------------|-------------------|-----------------|--------------|
		| **Minimum Idle** | 11.1V (3S) | ~1.15 | ~12.8 | All components at rest |
		| **Normal (driving)** | 11.1V (3S) | ~5.25 | ~58.3 | Typical operation with motors |
		| **Maximum (peak)** | 11.1V (3S) | ~15.2 | ~168.8 | All motors/servo at maximum load |
		
		### Comparison - WITH or WITHOUT Display
		
		| Condition | WITH Display (W) | WITHOUT Display (W) | Savings (W) | Savings (%) |
		|-----------|------------------|---------------------|-------------|-------------|
		| **Minimum Idle** | 15.5 | 12.8 | 2.7 | 17.4% |
		| **Normal** | 61.0 | 58.3 | 2.7 | 4.4% |
		| **Maximum** | 172.0 | 168.8 | 3.2 | 1.9% |
		
		
		## System Voltage Distribution
		
		| Voltage Rail | Source | Powered Components |
		|--------------|--------|-------------------|
		| **11.1V (Battery)** | 3S LiPo | Main input, DC Motors, Servo |
		| **5V** | Step-Down LM2596 | Raspberry Pi 5, MCP2515 (2x), USB Converter |
		| **3.3V** | Step-Down XL4015 | B-U585I-IOT02A, Speed Sensor, SSD M.2 |
		| **Motors** | Direct battery | TB6612FNG motor supply |
		
		
		
		## Detailed Autonomy Calculations (WITHOUT Display)
		
		### Scenario 1: Idle (System on, no movement)
		```
		Consumption: ~12.8W
		Current @ 11.1V: 12.8W / 11.1V = 1.15A
		
		3S (2200mAh):   2.2Ah / 1.15A = 1.91h  ≈ 1h 55min
		```
		
		### Scenario 2: Normal (Typical driving)
		```
		Consumption: ~58.3W
		Current @ 11.1V: 58.3W / 11.1V = 5.25A
		
		3S (2200mAh):   2.2Ah / 5.25A = 0.42h  ≈ 25min
		```
		
		### Scenario 3: Maximum (All components at 100%)
		```
		Consumption: ~168.8W
		Current @ 11.1V: 168.8W / 11.1V = 15.2A
		
		3S (2200mAh):   2.2Ah / 15.2A = 0.14h  ≈ 9min
		```
		
		
		## Power Distribution Diagram (WITHOUT Display)
		
		```
		[3S LiPo Battery 11.1V]
		    |
		    +-- [Motors] --&gt; TB6612FNG --&gt; DC Motors (2x)
		    |
		    +-- [Servo Motor WS-MG99SR]
		    |
		    +-- [LM2596 Step-Down] --&gt; 5V Rail
		    |       |
		    |       +-- Raspberry Pi 5
		    |       +-- MCP2515 (2x)
		    |       +-- TB6612FNG (VCC)
		    |       +-- USB Converter
		    |       +-- SSD M.2
		    |       +-- Hailo-8 HAT (via Pi)			
		    |
		    +-- [XL4015 Step-Down] --&gt; 3.3V Rail
		            |
		            +-- B-U585I-IOT02A
		            +-- Speed Sensor
		```
		
		
		## Final Comparison: WITH vs WITHOUT Display
		
		| Metric | WITH Display | WITHOUT Display | Difference |
		|--------|--------------|-----------------|------------|
		| **Idle Power** | 15.5W | 12.8W | -2.7W (-17.4%) |
		| **Normal Power** | 61.0W | 58.3W | -2.7W (-4.4%) |
		| **Maximum Power** | 172.0W | 168.8W | -3.2W (-1.9%) |
		| **Idle Autonomy (3S)** | 1h 34min | 1h 55min | +21min |
		| **Normal Autonomy (3S)** | 24min | 25min | +1min |
		| **LM2596 Load (peak)** | 6.3A | 5.65A | -640mA |
		| **No. Components** | 13 | 12 | -1 |
		
		````


- `docs/guides/Power Consumption Analysis.md`

	??? "Click to view reference"

		````md
		# Power Consumption Analysis - SEA:ME Project (Without Display)
		
		## Component Consumption Table
		
		| Component | Quantity | Voltage (V) | Min Current (mA) | Max Current (mA) | Min Power (W) | Max Power (W) | Notes |
		|-----------|----------|--------------|------------------|------------------|---------------|---------------|-------|
		| **Raspberry Pi 5** | 1 | 5.0 | 500 | 5000 | 2.5 | 25.0 | Idle: ~3W, Max Performance: 25W |
		| **Hailo-8 HAT (26 TOPS)** | 1 | 3.3-5.0 | 200 | 2500 | 0.66 | 12.5 | Typical: 2.5W, Max: ~12.5W |
		| **B-U585I-IOT02A** | 1 | 5.0 | 2 | 500 | 0.007 | 1.65 | With WiFi active: ~170mA @ 5V |
		| **Servo Motor WS-MG99SR** | 1 | 4.8-6.0 | 100 | 1200 | 0.5 | 7.2 | Idle: ~100mA, Stall: ~1200mA |
		| **DC Motors (propulsion)** | 2 | 6.0-12.0 | 200 | 2000 | 2.4 | 48.0 | Per motor: 1.2W-24W (depends on specific motor) |
		| **MCP2515 CAN Transceiver** | 2 | 5.0 | 1 | 5 | 0.01 | 0.05 | Typical: 5mA/module, Standby: 1μA |
		| **SSD M.2 (via PCIe)** | 1 | 3.3 | 50 | 500 | 0.165 | 1.65 | Depends on specific model |
		| **PCI-E to USB Converter** | 1 | 5.0 | 50 | 200 | 0.25 | 1.0 | Passive conversion |
		| **Speed Sensor (Hall)** | 1 | 3.3-5.0 | 5 | 15 | 0.025 | 0.075 | Typical Hall effect sensor |
		| **LM2596 Step-Down** | 1 | - | - | - | - | 0.4 | Efficiency losses (~92%) |
		| **XL4015 Step-Down** | 1 | - | - | - | - | 0.8 | Efficiency losses (~90%) |
		| **TB6612FNG Driver** | 1 | 2.7-5.5 (logic) | 5 | 3200 | 0.025 | 16.0 | VCC + VM, 1.2A cont./channel (3.2A peak) |
		
		---
		
		## System Total Summary (WITHOUT DISPLAY)
		
		| Condition | Battery Voltage | Total Current (A) | Total Power (W) | Observations |
		|-----------|-----------------|-------------------|-----------------|--------------|
		| **Minimum Idle** | 11.1V (3S) | ~1.15 | ~12.8 | All components at rest |
		| **Normal (driving)** | 11.1V (3S) | ~5.25 | ~58.3 | Typical operation with motors |
		| **Maximum (peak)** | 11.1V (3S) | ~15.2 | ~168.8 | All motors/servo at maximum load |
		
		### Comparison - WITH or WITHOUT Display
		
		| Condition | WITH Display (W) | WITHOUT Display (W) | Savings (W) | Savings (%) |
		|-----------|------------------|---------------------|-------------|-------------|
		| **Minimum Idle** | 15.5 | 12.8 | 2.7 | 17.4% |
		| **Normal** | 61.0 | 58.3 | 2.7 | 4.4% |
		| **Maximum** | 172.0 | 168.8 | 3.2 | 1.9% |
		
		
		## System Voltage Distribution
		
		| Voltage Rail | Source | Powered Components |
		|--------------|--------|-------------------|
		| **11.1V (Battery)** | 3S LiPo | Main input, DC Motors, Servo |
		| **5V** | Step-Down LM2596 | Raspberry Pi 5, MCP2515 (2x), USB Converter |
		| **3.3V** | Step-Down XL4015 | B-U585I-IOT02A, Speed Sensor, SSD M.2 |
		| **Motors** | Direct battery | TB6612FNG motor supply |
		
		
		
		## Detailed Autonomy Calculations (WITHOUT Display)
		
		### Scenario 1: Idle (System on, no movement)
		```
		Consumption: ~12.8W
		Current @ 11.1V: 12.8W / 11.1V = 1.15A
		
		3S (2200mAh):   2.2Ah / 1.15A = 1.91h  ≈ 1h 55min
		```
		
		### Scenario 2: Normal (Typical driving)
		```
		Consumption: ~58.3W
		Current @ 11.1V: 58.3W / 11.1V = 5.25A
		
		3S (2200mAh):   2.2Ah / 5.25A = 0.42h  ≈ 25min
		```
		
		### Scenario 3: Maximum (All components at 100%)
		```
		Consumption: ~168.8W
		Current @ 11.1V: 168.8W / 11.1V = 15.2A
		
		3S (2200mAh):   2.2Ah / 15.2A = 0.14h  ≈ 9min
		```
		
		
		## Power Distribution Diagram (WITHOUT Display)
		
		```
		[3S LiPo Battery 11.1V]
		    |
		    +-- [Motors] --&gt; TB6612FNG --&gt; DC Motors (2x)
		    |
		    +-- [Servo Motor WS-MG99SR]
		    |
		    +-- [LM2596 Step-Down] --&gt; 5V Rail
		    |       |
		    |       +-- Raspberry Pi 5
		    |       +-- MCP2515 (2x)
		    |       +-- TB6612FNG (VCC)
		    |       +-- USB Converter
		    |       +-- SSD M.2
		    |       +-- Hailo-8 HAT (via Pi)			
		    |
		    +-- [XL4015 Step-Down] --&gt; 3.3V Rail
		            |
		            +-- B-U585I-IOT02A
		            +-- Speed Sensor
		```
		
		
		## Final Comparison: WITH vs WITHOUT Display
		
		| Metric | WITH Display | WITHOUT Display | Difference |
		|--------|--------------|-----------------|------------|
		| **Idle Power** | 15.5W | 12.8W | -2.7W (-17.4%) |
		| **Normal Power** | 61.0W | 58.3W | -2.7W (-4.4%) |
		| **Maximum Power** | 172.0W | 168.8W | -3.2W (-1.9%) |
		| **Idle Autonomy (3S)** | 1h 34min | 1h 55min | +21min |
		| **Normal Autonomy (3S)** | 24min | 25min | +1min |
		| **LM2596 Load (peak)** | 6.3A | 5.65A | -640mA |
		| **No. Components** | 13 | 12 | -1 |
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_11 | Reviewed: ✔ | Score: 1.0 ### {: #evidences-evid_l0_11 data-toc-label="EVIDENCES-EVID_L0_11" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_11](ASSERTIONS.md#assertions-assert_l0_11) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/guides/AGL_Configuration_Guide.md`

	??? "Click to view reference"

		````md
		# AGL (Automotive Grade Linux) - Configuration Guide 
		
		---
		
		## Table of Contents
		
		1. [Introduction](#introduction)
		2. [Custom Meta-Layer (meta-seame)](#custom-meta-layer-meta-seame)
		3. [local.conf Configuration](#localconf-configuration)
		4. [Installed Components](#installed-components)
		5. [Network Configuration](#network-configuration)
		6. [U-Boot Bypass](#u-boot-bypass)
		7. [Automatic WiFi Support](#automatic-wifi-support)
		8. [USB/Joystick Support](#usbjoystick-support)
		9. [Build and Deploy](#build-and-deploy)
		10. [Troubleshooting](#troubleshooting)
		
		---
		
		## Introduction
		
		This guide documents all custom configurations applied to AGL Terrific Trout 20.0.2 for the SEA:ME project on the Raspberry Pi 5. 
		The base system used is **agl-image-minimal** with extensions to support:
		
		- Weston/Wayland Compositor
		- Qt6 for dashboard development
		- Network connectivity (Ethernet + WiFi)
		- USB device support (Joystick-PiRacer)
		- CAN communication tools
		- SSH for remote access
		- Direct boot without U-Boot (solution for RPi5 boot issues)
		
		---
		
		## Custom Meta-Layer (meta-seame)
		
		### Layer Structure
		
		The custom layer `meta-seame` is created in the AGL workspace to add project-specific configurations:
		
		```
		$AGL_TOP/
		├── meta-agl/
		├── meta-agl-demo/
		├── meta-agl-devel/
		├── meta-seame/                    ← Custom layer
		│   ├── conf/
		│   │   └── layer.conf
		│   ├── recipes-connectivity/
		│   │   ├── network-config/
		│   │   │   ├── files/
		│   │   │   │   └── 20-wired.network
		│   │   │   └── network-config_1.0.bb
		│   │   └── wifi-startup/
		│   │       ├── files/
		│   │       │   ├── wifi-startup.sh
		│   │       │   └── wifi-startup.service
		│   │       └── wifi-startup_1.0.bb
		│   ├── recipes-support/
		│   │   └── joystick-support/
		│   │       ├── files/
		│   │       │   └── joystick-setup.sh
		│   │       └── joystick-support.bb
		│   └── recipes-bsp/
		│       └── bootfiles/
		│           └── rpi-config_git.bbappend
		└── build-rpi5/
		    └── conf/
		        ├── local.conf
		        └── bblayers.conf
		```
		
		### Creating the Layer
		
		```bash
		cd $AGL_TOP
		
		# Create layer structure
		mkdir -p meta-seame/conf
		mkdir -p meta-seame/recipes-connectivity/network-config/files
		mkdir -p meta-seame/recipes-connectivity/wifi-startup/files
		mkdir -p meta-seame/recipes-support/joystick-support/files
		```
		
		### layer.conf
		
		**File:** `meta-seame/conf/layer.conf`
		
		```bitbake
		# Layer configuration for meta-seame
		BBPATH .= ":${LAYERDIR}"
		
		BBFILES += "${LAYERDIR}/recipes*/*/*.bb \
		            ${LAYERDIR}/recipes*/*/*.bbappend"
		
		BBFILE_COLLECTIONS += "seame"
		BBFILE_PATTERN_seame = "^${LAYERDIR}/"
		BBFILE_PRIORITY_seame = "10"
		
		LAYERDEPENDS_seame = "core"
		LAYERSERIES_COMPAT_seame = "scarthgap"
		```
		
		**Note:** `LAYERSERIES_COMPAT` must be "scarthgap" for AGL 20.0.x (Yocto 5.0.x).
		
		### Adding to the Build
		
		```bash
		cd $AGL_TOP/build-rpi5
		bitbake-layers add-layer ../meta-seame
		
		# Verify if it was added
		bitbake-layers show-layers | grep seame
		```
		
		---
		
		## local.conf Configuration
		
		### Location
		
		**File:** `$AGL_TOP/build-rpi5/conf/local.conf`
		
		### Section 1: Machine and Distro
		
		```bitbake
		MACHINE = "raspberrypi5"
		DISTRO = "poky-agl"
		
		# Parallel build configuration
		PARALLEL_MAKE = "-j 8"
		BB_NUMBER_THREADS = "8"
		```
		
		### Section 2: Direct Boot without U-Boot (CRITICAL)
		
		This configuration resolves the issue where the RPi5 hangs at U-Boot:
		
		```bitbake
		##############################################################################
		# RASPBERRY PI 5 - DIRECT BOOT WITHOUT U-BOOT
		##############################################################################
		
		# CRITICAL: Disable U-Boot completely
		RPI_USE_U_BOOT = "0"
		
		# Enable UART for debug
		ENABLE_UART = "1"
		SERIAL_CONSOLES = "115200;ttyAMA0"
		
		# Direct Kernel
		KERNEL_IMAGETYPE = "Image"
		
		# Force newer kernel with better RPi5 support
		PREFERRED_VERSION_linux-raspberrypi = "6.6%"
		
		# GPU Memory
		GPU_MEM = "256"
		
		# Device tree for RPi5
		KERNEL_DEVICETREE:append = " broadcom/bcm2712-rpi-5-b.dtb"
		
		# Extra configurations for config.txt on RPi5
		RPI_EXTRA_CONFIG = " \n\
		# Direct boot without U-Boot \n\
		arm_64bit=1 \n\
		kernel=Image \n\
		disable_overscan=1 \n\
		gpu_mem=256 \n\
		enable_uart=1 \n\
		uart_2ndstage=1 \n\
		\n\
		# Optimizations for RPi5 \n\
		arm_boost=1 \n\
		over_voltage_delta=50000 \n\
		\n\
		# Display \n\
		dtoverlay=vc4-kms-v3d \n\
		max_framebuffers=2 \n\
		"
		
		# Boot delay to allow system initialization time
		RPI_EXTRA_CONFIG:append = " \n\
		boot_delay=1 \n\
		"
		```
		
		### Section 3: Weston/Wayland Compositor
		
		```bitbake
		##############################################################################
		# WESTON/WAYLAND COMPOSITOR
		##############################################################################
		
		DISTRO_FEATURES:append = " wayland"
		DISTRO_FEATURES:remove = " x11"
		
		# Weston compositor packages
		IMAGE_INSTALL:append = " \
		    weston \
		    weston-init \
		    weston-examples \
		    wayland \
		    wayland-protocols \
		    wayland-utils \
		"
		```
		
		### Section 4: Qt6 Development
		
		```bitbake
		##############################################################################
		# QT6 DEVELOPMENT ENVIRONMENT
		##############################################################################
		
		# Qt6 core packages
		IMAGE_INSTALL:append = " \
		    qtbase \
		    qtdeclarative \
		    qtwayland \
		    qtquickcontrols2 \
		    qtsvg \
		    qtserialport \
		"
		
		# Qt6 development tools (optional - increases image size)
		IMAGE_INSTALL:append = " \
		    qtbase-dev \
		    qtdeclarative-dev \
		    cmake \
		"
		
		# GPU acceleration for RPi5
		PACKAGECONFIG:append:pn-qtbase = " gles2 eglfs kms gbm"
		```
		
		### Section 5: Networking
		
		```bitbake
		##############################################################################
		# NETWORK CONFIGURATION
		##############################################################################
		
		# Network tools
		IMAGE_INSTALL:append = " \
		    openssh \
		    openssh-sshd \
		    openssh-sftp-server \
		    iproute2 \
		    net-tools \
		    iputils \
		    wireless-regdb-static \
		    wpa-supplicant \
		"
		
		# WiFi support
		IMAGE_INSTALL:append = " \
		    linux-firmware-bcm43455 \
		    linux-firmware-rpidistro-bcm43455 \
		"
		
		# Enable systemd-networkd
		PACKAGECONFIG:append:pn-systemd = " networkd resolved"
		VIRTUAL-RUNTIME_net_manager = "systemd"
		```
		
		### Section 6: CAN Bus Support
		
		```bitbake
		##############################################################################
		# CAN BUS SUPPORT
		##############################################################################
		
		# CAN utilities
		IMAGE_INSTALL:append = " \
		    can-utils \
		    libsocketcan \
		"
		
		# Kernel modules CAN
		KERNEL_MODULE_AUTOLOAD += "can can-raw can-dev mcp251x"
		```
		
		### Section 7: USB/Input Devices
		
		```bitbake
		##############################################################################
		# USB AND INPUT DEVICES SUPPORT
		##############################################################################
		
		# USB HID support
		DISTRO_FEATURES:append = " usbhost usbgadget"
		
		# Joystick/Gamepad support
		IMAGE_INSTALL:append = " \
		    joystick \
		    linuxconsoletools \
		    evtest \
		    libevdev \
		"
		
		# Kernel modules
		KERNEL_MODULE_AUTOLOAD += "joydev usbhid hid_generic"
		KERNEL_FEATURES += "cfg/hid.scc"
		```
		
		### Section 8: Development Tools
		
		```bitbake
		##############################################################################
		# DEVELOPMENT TOOLS
		##############################################################################
		
		IMAGE_INSTALL:append = " \
		    python3 \
		    python3-pip \
		    git \
		    nano \
		    vim \
		    htop \
		    iotop \
		    lsof \
		    strace \
		"
		```
		
		### Section 9: System Monitoring
		
		```bitbake
		##############################################################################
		# SYSTEM MONITORING
		##############################################################################
		
		IMAGE_INSTALL:append = " \
		    lm-sensors \
		    i2c-tools \
		    spi-tools \
		"
		```
		
		### Section 10: Custom Recipes
		
		```bitbake
		##############################################################################
		# CUSTOM RECIPES FROM META-SEAME
		##############################################################################
		
		IMAGE_INSTALL:append = " \
		    network-config \
		    wifi-startup \
		    joystick-support \
		"
		```
		
		### Section 11: Storage and Filesystem
		
		```bitbake
		##############################################################################
		# STORAGE AND FILESYSTEM
		##############################################################################
		
		# Extra space in rootfs (2GB)
		IMAGE_ROOTFS_EXTRA_SPACE = "2097152"
		
		# Support for additional filesystems
		IMAGE_INSTALL:append = " \
		    e2fsprogs \
		    e2fsprogs-resize2fs \
		    dosfstools \
		    util-linux \
		"
		```
		
		### Section 12: Final Optimizations
		
		```bitbake
		##############################################################################
		# OPTIMIZATIONS
		##############################################################################
		
		# Remove unnecessary AGL demos
		PACKAGE_EXCLUDE = "agl-demo-*"
		
		# Enable useful systemd features
		DISTRO_FEATURES:append = " systemd"
		VIRTUAL-RUNTIME_init_manager = "systemd"
		```
		
		---
		
		## Installed Components
		
		### Base AGL Components
		
		| Component | Description | Notes |
		|------------|-----------|-------|
		| **agl-image-minimal** | Base AGL Image | Core operating system |
		| **Weston 13** | Wayland Compositor | DRM Backend + RDP |
		| **systemd** | System and service manager | Init system |
		| **ConnMan** | Connection Manager | Network manager (disabled in favor of systemd-networkd) |
		
		### Added Components
		
		#### Display & Graphics
		- `weston` - Wayland Compositor
		- `weston-init` - Startup scripts
		- `weston-examples` - Wayland examples
		- `wayland-protocols` - Wayland protocols
		- `mesa-demos` - OpenGL demos (optional)
		
		#### Qt6 Framework
		- `qtbase` - Core Qt6
		- `qtdeclarative` - QML engine
		- `qtwayland` - Wayland Backend for Qt
		- `qtquickcontrols2` - UI Controls
		- `qtsvg` - SVG Support
		- `qtserialport` - Serial communication
		
		#### Networking
		- `openssh` + `openssh-sshd` - SSH Server
		- `iproute2` - Modern network tools
		- `net-tools` - ifconfig, route, etc
		- `wpa-supplicant` - WiFi Client
		- `linux-firmware-bcm43455` - RPi5 WiFi Firmware
		
		#### CAN Bus
		- `can-utils` - CAN Tools (cansend, candump, etc)
		- `libsocketcan` - SocketCAN library
		
		#### USB/Input
		- `joystick` - Joystick support
		- `linuxconsoletools` - Calibration tools
		- `evtest` - Input event tester
		
		#### Development
		- `python3` + `python3-pip`
		- `git`
		- `cmake`
		- `nano`, `vim`
		- `htop`, `iotop`
		
		---
		
		## Network Configuration
		
		### Static Ethernet Configuration
		
		#### Recipe: network-config
		
		**File:** `meta-seame/recipes-connectivity/network-config/network-config_1.0.bb`
		
		```bitbake
		SUMMARY = "Static network configuration for SEA:ME"
		LICENSE = "MIT"
		LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
		
		SRC_URI = "file://20-wired.network"
		
		S = "${WORKDIR}"
		
		do_install() {
		    install -d ${D}${sysconfdir}/systemd/network
		    install -m 0644 ${WORKDIR}/20-wired.network ${D}${sysconfdir}/systemd/network/
		}
		
		FILES:${PN} += "${sysconfdir}/systemd/network/20-wired.network"
		
		RDEPENDS:${PN} = "systemd"
		```
		
		**File:** `meta-seame/recipes-connectivity/network-config/files/20-wired.network`
		
		```ini
		[Match]
		Name=eth0
		
		[Network]
		Address=192.168.1.100/24
		Gateway=192.168.1.1
		DNS=8.8.8.8
		DNS=8.8.4.4
		
		[Link]
		RequiredForOnline=yes
		```
		
		**Customization:** Adjust the IP (192.168.1.100) and gateway (192.168.1.1) according to your network.
		
		### Automatic WiFi Configuration
		
		#### Recipe: wifi-startup
		
		**File:** `meta-seame/recipes-connectivity/wifi-startup/wifi-startup_1.0.bb`
		
		```bitbake
		SUMMARY = "WiFi Startup Script for SEA:ME AGL"
		DESCRIPTION = "Automatically connects to WiFi on boot"
		LICENSE = "MIT"
		LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
		
		SRC_URI = " \
		    file://wifi-startup.sh \
		    file://wifi-startup.service \
		"
		
		S = "${WORKDIR}"
		
		inherit systemd
		
		SYSTEMD_SERVICE:${PN} = "wifi-startup.service"
		SYSTEMD_AUTO_ENABLE = "enable"
		
		RDEPENDS:${PN} = "wpa-supplicant"
		
		do_install() {
		    # Install script
		    install -d ${D}${bindir}
		    install -m 0755 ${WORKDIR}/wifi-startup.sh ${D}${bindir}/
		
		    # Install systemd service
		    install -d ${D}${systemd_unitdir}/system
		    install -m 0644 ${WORKDIR}/wifi-startup.service ${D}${systemd_unitdir}/system/
		}
		
		FILES:${PN} += "${systemd_unitdir}/system/wifi-startup.service"
		```
		
		**File:** `meta-seame/recipes-connectivity/wifi-startup/files/wifi-startup.sh`
		
		```bash
		#!/bin/sh
		
		echo "[WiFi] Starting WiFi configuration..."
		
		# Wait for interface to be available
		sleep 5
		
		# Bring up interface
		ip link set wlan0 up
		sleep 2
		
		# Check if wpa_supplicant is already running
		if pgrep -x "wpa_supplicant" &gt; /dev/null; then
		    echo "[WiFi] wpa_supplicant already running"
		    killall wpa_supplicant
		    sleep 1
		fi
		
		# Start wpa_supplicant
		echo "[WiFi] Starting wpa_supplicant..."
		wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf -D nl80211
		
		# Wait for connection
		sleep 5
		
		# Request DHCP
		echo "[WiFi] Requesting DHCP..."
		udhcpc -i wlan0
		
		# Verify connection
		if ip addr show wlan0 | grep -q "inet "; then
		    IP=$(ip addr show wlan0 | grep "inet " | awk '{print $2}')
		    echo "[WiFi] Connected successfully! IP: $IP"
		    exit 0
		else
		    echo "[WiFi] ERROR: Failed to obtain IP"
		    exit 1
		fi
		```
		
		**File:** `meta-seame/recipes-connectivity/wifi-startup/files/wifi-startup.service`
		
		```ini
		[Unit]
		Description=WiFi Auto-Connect at Boot
		After=network.target
		Before=sshd.service ssh.service multi-user.target
		Wants=network.target
		
		[Service]
		Type=oneshot
		ExecStart=/usr/bin/wifi-startup.sh
		RemainAfterExit=yes
		StandardOutput=journal
		StandardError=journal
		Restart=on-failure
		RestartSec=10
		
		[Install]
		WantedBy=multi-user.target
		```
		
		#### Configuring wpa_supplicant.conf
		
		NOTE: This file must be manually created on the system after the initial boot:
		
		```bash
		# On Raspberry Pi, after boot:
		wpa_passphrase "NetworkName" "Password" &gt; /etc/wpa_supplicant.conf
		```
		
		Or created manually:
		
		```bash
		cat &gt; /etc/wpa_supplicant.conf &lt;&lt; 'EOF'
		ctrl_interface=/var/run/wpa_supplicant
		ctrl_interface_group=0
		update_config=1
		country=PT
		
		network={
		    ssid="NetworkName"
		    psk="YourPasswordHere"
		    key_mgmt=WPA-PSK
		    priority=1
		}
		EOF
		
		chmod 600 /etc/wpa_supplicant.conf
		```
		
		---
		
		## U-Boot Bypass
		
		### The Issue
		
		The Raspberry Pi 5 has known issues with U-Boot on AGL Terrific Trout 20.0.2, resulting in a boot hang with the message "U-BOOT" on the screen.
		
		### Implemented Solution
		
		The solution is to perform a **direct boot of the Linux kernel**, bypassing U-Boot, by configuring the Raspberry Pi firmware to load the kernel directly.
		
		### Configuration in local.conf
		
		```bitbake
		# CRITICAL: Disable U-Boot completely
		RPI_USE_U_BOOT = "0"
		
		# Direct Kernel
		KERNEL_IMAGETYPE = "Image"
		
		# Configurations for config.txt
		RPI_EXTRA_CONFIG = " \n\
		arm_64bit=1 \n\
		kernel=Image \n\
		..."
		```
		
		### Post-Boot Verification
		
		```bash
		# Check if booted without U-Boot
		journalctl -b | grep -i "u-boot"  # Nothing should appear
		
		# Confirm kernel booted directly
		dmesg | head -20
		```
		
		### Manual Troubleshooting
		
		If the problem persists, you can manually edit `config.txt` on the boot partition:
		
		```bash
		# Mount boot partition
		sudo mount /dev/mmcblk0p1 /mnt
		
		# Edit config.txt
		sudo nano /mnt/config.txt
		
		# Add/Verify:
		arm_64bit=1
		kernel=Image
		disable_overscan=1
		enable_uart=1
		dtoverlay=vc4-kms-v3d
		
		# Unmount
		sudo umount /mnt
		```
		
		---
		
		## Automatic WiFi Support
		
		### Initialization Flow
		
		1. **systemd** starts → `network.target`
		2. `wifi-startup.service` is activated (Before=sshd.service)
		3. Script `wifi-startup.sh` executes:
		   - Activates interface `wlan0`
		   - Starts `wpa_supplicant`
		   - Requests IP via DHCP (udhcpc)
		4. `sshd.service` starts (after WiFi is configured)
		
		### Useful Commands
		
		```bash
		# Check WiFi service status
		systemctl status wifi-startup.service
		
		# Check logs
		journalctl -u wifi-startup.service -f
		
		# Restart WiFi
		systemctl restart wifi-startup.service
		
		# Check IP
		ip addr show wlan0
		
		# Test connectivity
		ping -c 4 8.8.8.8
		```
		
		### Manual Configuration (Alternative)
		
		If you prefer to configure manually without using the service:
		
		```bash
		# Activate interface
		ip link set wlan0 up
		
		# Connect with wpa_supplicant
		wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf -D nl80211
		
		# Obtain IP
		udhcpc -i wlan0
		```
		
		---
		
		## USB/Joystick Support
		
		### Recipe: joystick-support
		
		**File:** `meta-seame/recipes-support/joystick-support/joystick-support.bb`
		
		```bitbake
		SUMMARY = "Joystick support for PiRacer"
		LICENSE = "MIT"
		LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
		
		SRC_URI = "file://joystick-setup.sh"
		
		S = "${WORKDIR}"
		
		do_install() {
		    install -d ${D}${bindir}
		    install -m 0755 ${WORKDIR}/joystick-setup.sh ${D}${bindir}/
		}
		
		RDEPENDS:${PN} = "linuxconsoletools joystick"
		```
		
		**File:** `meta-seame/recipes-support/joystick-support/files/joystick-setup.sh`
		
		```bash
		#!/bin/sh
		
		# Load necessary kernel modules
		modprobe joydev
		modprobe usbhid
		modprobe hid_generic
		
		# Set proper permissions
		chmod 666 /dev/input/js*
		chmod 666 /dev/input/event*
		
		echo "Joystick setup complete"
		ls -la /dev/input/js* 2&gt;/dev/null || echo "No joystick devices found"
		```
		
		### Verification
		
		```bash
		# See USB devices
		lsusb
		
		# See input devices
		ls -la /dev/input/
		
		# Test joystick
		evtest /dev/input/js0
		
		# See events in real-time
		cat /dev/input/js0 | hexdump -C
		```
		
		### Kernel Modules
		
		The following modules are automatically loaded via `KERNEL_MODULE_AUTOLOAD`:
		
		- `joydev` - Joystick support
		- `usbhid` - USB HID devices
		- `hid_generic` - Generic HID
		
		---
		
		## Build and Deploy
		
		### Environment Preparation
		
		```bash
		cd $AGL_TOP
		source meta-agl/scripts/aglsetup.sh -m raspberrypi5 -b build-rpi5 agl-demo agl-devel
		
		# Add meta-seame
		cd build-rpi5
		bitbake-layers add-layer ../meta-seame
		
		# Verify layers
		bitbake-layers show-layers
		```
		
		### Initial Build (Clean Build)
		
		```bash
		cd $AGL_TOP/build-rpi5
		
		# Complete build (takes 2-4 hours the first time)
		bitbake agl-image-minimal
		```
		
		### Incremental Build
		
		```bash
		# After changes in local.conf or recipes
		bitbake agl-image-minimal
		
		# Clean cache of a specific recipe
		bitbake -c cleansstate 
		bitbake 
		```
		
		### Image Location
		
		```bash
		cd $AGL_TOP/build-rpi5/tmp/deploy/images/raspberrypi5/
		
		ls -lh *.wic.xz
		```
		
		Relevant files:
		- `agl-image-minimal-raspberrypi5.rootfs.wic.xz` - Compressed full image
		- `agl-image-minimal-raspberrypi5.rootfs.manifest` - List of installed packages
		
		### Deploy to MicroSD
		
		#### Method 1: Using dd (Linux)
		
		```bash
		# Unmount existing partitions
		sudo umount /dev/mmcblk0p1 /dev/mmcblk0p2
		
		# Write image
		cd $AGL_TOP/build-rpi5/tmp/deploy/images/raspberrypi5/
		xzcat agl-image-minimal-raspberrypi5.rootfs.wic.xz | sudo dd of=/dev/mmcblk0 bs=4M status=progress
		
		# Sync
		sudo sync
		
		# Eject
		sudo eject /dev/mmcblk0
		```
		
		#### Method 2: Using bmaptool (Faster)
		
		```bash
		# Install bmaptool
		sudo apt install bmap-tools
		
		# Write with bmaptool
		sudo bmaptool copy --bmap agl-image-minimal-raspberrypi5.rootfs.wic.bmap \
		                   agl-image-minimal-raspberrypi5.rootfs.wic.xz \
		                   /dev/mmcblk0
		```
		
		### First Boot
		
		1. Insert MicroSD into Raspberry Pi 5
		2. Connect HDMI monitor
		3. Connect power (recommended 5V/5A with Official Raspberry Pi Charger)
		4. Wait for boot (2-3 minutes on first boot)
		
		#### Default Login
		
		- **User:** `root`
		- **Password:** (empty - press Enter)
		
		---
		
		## Troubleshooting
		
		### Boot Stuck at U-Boot
		
		**Symptom:** Black screen with text "U-BOOT" in the top corner.
		
		**Solution:**
		1. Verify `RPI_USE_U_BOOT = "0"` in `local.conf`
		2. Verify presence of `RPI_EXTRA_CONFIG` with `kernel=Image`
		3. Clean rebuild:
		```bash
		bitbake -c cleansstate linux-raspberrypi rpi-config
		bitbake agl-image-minimal
		```
		
		### WiFi Does Not Connect
		
		**Diagnosis:**
		```bash
		# Check service status
		systemctl status wifi-startup.service
		
		# Check logs
		journalctl -u wifi-startup.service -n 50
		
		# Check interface
		ip link show wlan0
		
		# Check firmware
		ls /lib/firmware/brcm/
		
		# Check wpa_supplicant.conf
		cat /etc/wpa_supplicant.conf
		```
		
		**Solutions:**
		1. Verify WiFi credentials in `/etc/wpa_supplicant.conf`
		
		2. Test manual connection:
		```bash
		systemctl stop wifi-startup
		wpa_supplicant -i wlan0 -c /etc/wpa_supplicant.conf -D nl80211
		# (view logs in foreground)
		```
		
		3. Check status and stop and disable ConnMan
		```sh
		systemctl status connman
		systemctl stop connman
		systemctl disable connman
		```
		
		4. Connection Test
		```sh
		ping -c 4 8.8.8.8
		```
		If there is a response, the network is functional.
		
		5. Create an Auto-Startup Script
		
		To make Wi-Fi start automatically after login.
		Create the script:
		```sh
		mkdir -p /opt/scripts
		nano /opt/scripts/wifi-autostart.sh
		```
		
		Write this inside:
		```sh
		#!/bin/sh
		
		sleep 5
		ip link set wlan0 up
		sleep 2
		killall wpa_supplicant 2&gt;/dev/null
		sleep 1
		wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant.conf -D nl80211
		sleep 5
		udhcpc -i wlan0
		
		```
		
		Make it executable:
		```sh
		chmod +x /opt/scripts/wifi-autostart.sh
		```
		
		And create the service in systemd
		```
		nano /etc/systemd/system/wifi-autostart.service
		```
		
		and write this inside:
		```
		[Unit]
		Description=WiFi Auto-Connect at Boot
		After=network.target
		Before=sshd.service ssh.service multi-user.target
		Wants=network.target
		
		[Service]
		Type=oneshot
		ExecStart=/opt/scripts/wifi-autostart.sh
		RemainAfterExit=yes
		StandardOutput=journal
		StandardError=journal
		
		# Restart if anything fails
		Restart=on-failure
		RestartSec=10
		
		[Install]
		WantedBy=multi-user.target
		```
		
		And finally, activate this service
		
		```
		# Reload services
		systemctl daemon-reload
		
		# Enable service
		systemctl enable wifi-autostart.service
		
		# Start service
		systemctl start wifi-autostart.service
		
		# Check status
		systemctl status wifi-autostart.service
		
		```
		And that's it!
		
		### SSH Not Working
		
		**Diagnosis:**
		```bash
		# Check SSH status
		systemctl status sshd
		
		# Check if port 22 is listening
		netstat -tlnp | grep :22
		
		# Check logs
		journalctl -u sshd -n 50
		```
		
		**Solutions:**
		1. Start SSH manually:
		```bash
		systemctl start sshd
		systemctl enable sshd
		```
		
		2. Check firewall (if applicable):
		```bash
		iptables -L -n
		```
		
		### Weston Does Not Start
		
		**Diagnosis:**
		```bash
		# Check Weston status
		systemctl status weston
		
		# Check logs
		journalctl -u weston -n 100
		
		# Test manually
		weston --backend=drm-backend.so
		```
		
		**Solutions:**
		1. Check DRM permissions:
		```bash
		ls -la /dev/dri/
		# Should have card0 and renderD128
		```
		
		2. Check video group:
		```bash
		groups root
		# Should include 'video'
		```
		
		### Qt Application Does Not Run
		
		**Diagnosis:**
		```bash
		# Check installed Qt packages
		opkg list-installed | grep qt
		
		# Test simple app
		QT_QPA_PLATFORM=wayland /path/to/app
		
		# Check environment variables
		echo $QT_QPA_PLATFORM
		echo $WAYLAND_DISPLAY
		```
		
		**Solutions:**
		1. Export necessary variables:
		```bash
		export QT_QPA_PLATFORM=wayland
		export WAYLAND_DISPLAY=wayland-0
		```
		
		2. Check compositor:
		```bash
		ps aux | grep weston
		```
		
		### Joystick Not Detected (Please check the AGL_Joystick Installation and test Guide for more info)
		
		**Diagnosis:**
		```bash
		# See USB devices
		lsusb
		
		# See input devices
		ls -la /dev/input/
		
		# Check modules
		lsmod | grep -E "joydev|usbhid"
		
		# See USB logs
		dmesg | grep -i usb
		```
		
		**Solutions:**
		1. Load modules manually:
		```bash
		modprobe joydev
		modprobe usbhid
		```
		
		2. Check udev rules:
		```bash
		udevadm info -a /dev/input/js0
		```
		
		### Performance Issues
		
		**Diagnosis:**
		```bash
		# Monitor CPU/RAM
		htop
		
		# Check temperature
		vcgencmd measure_temp
		
		# Check clock
		vcgencmd measure_clock arm
		
		# Check throttling
		vcgencmd get_throttled
		```
		
		**Solutions:**
		1. Check adequate power supply (5V/5A for RPi5)
		2. Add heatsink/fan
		3. Reduce `GPU_MEM` if not using heavy graphics
		4. Check `arm_boost=1` in `config.txt`
		
		### Build Errors
		
		**Error:** "Layer seame is not compatible with the core layer"
		
		**Solution:** Check `LAYERSERIES_COMPAT_seame = "scarthgap"` in `layer.conf`
		
		---
		
		**Error:** "Nothing PROVIDES 'package-name'"
		
		**Solution:** 1. Check correct package name:
		```bash
		bitbake -s | grep package-name
		```
		
		2. Add layer that provides the package:
		```bash
		bitbake-layers show-recipes package-name
		```
		
		---
		
		**Error:** Task failed (do_compile, do_install, etc)
		
		**Solution:**
		1. See detailed log:
		```bash
		bitbake -c compile package-name -f
		# View log in tmp/work/...
		```
		
		2. Clean and rebuild:
		```bash
		bitbake -c cleansstate package-name
		bitbake package-name
		```
		
		### Network Issues
		
		**Ethernet does not obtain IP:**
		
		```bash
		# Check link
		ip link show eth0
		
		# Check systemd-networkd configuration
		cat /etc/systemd/network/20-wired.network
		
		# Restart networkd
		systemctl restart systemd-networkd
		
		# Manual DHCP request (if using DHCP)
		dhclient eth0
		```
		
		---
		
		## References
		
		### AGL Documentation
		- [AGL Documentation](https://docs.automotivelinux.org/)
		- [AGL Release Notes - Terrific Trout](https://docs.automotivelinux.org/en/trout/#00_Getting_Started/)
		- [Yocto Project Quick Build](https://docs.yoctoproject.org/brief-yoctoprojectqs/index.html)
		
		### Hardware
		- [Raspberry Pi 5 Documentation](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html)
		- [PiRacer Assembly Guide](https://www.waveshare.com/wiki/PiRacer_AI_Kit)
		
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_12 | Reviewed: ✔ | Score: 1.0 ### {: #evidences-evid_l0_12 data-toc-label="EVIDENCES-EVID_L0_12" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_12](ASSERTIONS.md#assertions-assert_l0_12) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/guides/Power Consumption Analysis.md`

	??? "Click to view reference"

		````md
		# Power Consumption Analysis - SEA:ME Project (Without Display)
		
		## Component Consumption Table
		
		| Component | Quantity | Voltage (V) | Min Current (mA) | Max Current (mA) | Min Power (W) | Max Power (W) | Notes |
		|-----------|----------|--------------|------------------|------------------|---------------|---------------|-------|
		| **Raspberry Pi 5** | 1 | 5.0 | 500 | 5000 | 2.5 | 25.0 | Idle: ~3W, Max Performance: 25W |
		| **Hailo-8 HAT (26 TOPS)** | 1 | 3.3-5.0 | 200 | 2500 | 0.66 | 12.5 | Typical: 2.5W, Max: ~12.5W |
		| **B-U585I-IOT02A** | 1 | 5.0 | 2 | 500 | 0.007 | 1.65 | With WiFi active: ~170mA @ 5V |
		| **Servo Motor WS-MG99SR** | 1 | 4.8-6.0 | 100 | 1200 | 0.5 | 7.2 | Idle: ~100mA, Stall: ~1200mA |
		| **DC Motors (propulsion)** | 2 | 6.0-12.0 | 200 | 2000 | 2.4 | 48.0 | Per motor: 1.2W-24W (depends on specific motor) |
		| **MCP2515 CAN Transceiver** | 2 | 5.0 | 1 | 5 | 0.01 | 0.05 | Typical: 5mA/module, Standby: 1μA |
		| **SSD M.2 (via PCIe)** | 1 | 3.3 | 50 | 500 | 0.165 | 1.65 | Depends on specific model |
		| **PCI-E to USB Converter** | 1 | 5.0 | 50 | 200 | 0.25 | 1.0 | Passive conversion |
		| **Speed Sensor (Hall)** | 1 | 3.3-5.0 | 5 | 15 | 0.025 | 0.075 | Typical Hall effect sensor |
		| **LM2596 Step-Down** | 1 | - | - | - | - | 0.4 | Efficiency losses (~92%) |
		| **XL4015 Step-Down** | 1 | - | - | - | - | 0.8 | Efficiency losses (~90%) |
		| **TB6612FNG Driver** | 1 | 2.7-5.5 (logic) | 5 | 3200 | 0.025 | 16.0 | VCC + VM, 1.2A cont./channel (3.2A peak) |
		
		---
		
		## System Total Summary (WITHOUT DISPLAY)
		
		| Condition | Battery Voltage | Total Current (A) | Total Power (W) | Observations |
		|-----------|-----------------|-------------------|-----------------|--------------|
		| **Minimum Idle** | 11.1V (3S) | ~1.15 | ~12.8 | All components at rest |
		| **Normal (driving)** | 11.1V (3S) | ~5.25 | ~58.3 | Typical operation with motors |
		| **Maximum (peak)** | 11.1V (3S) | ~15.2 | ~168.8 | All motors/servo at maximum load |
		
		### Comparison - WITH or WITHOUT Display
		
		| Condition | WITH Display (W) | WITHOUT Display (W) | Savings (W) | Savings (%) |
		|-----------|------------------|---------------------|-------------|-------------|
		| **Minimum Idle** | 15.5 | 12.8 | 2.7 | 17.4% |
		| **Normal** | 61.0 | 58.3 | 2.7 | 4.4% |
		| **Maximum** | 172.0 | 168.8 | 3.2 | 1.9% |
		
		
		## System Voltage Distribution
		
		| Voltage Rail | Source | Powered Components |
		|--------------|--------|-------------------|
		| **11.1V (Battery)** | 3S LiPo | Main input, DC Motors, Servo |
		| **5V** | Step-Down LM2596 | Raspberry Pi 5, MCP2515 (2x), USB Converter |
		| **3.3V** | Step-Down XL4015 | B-U585I-IOT02A, Speed Sensor, SSD M.2 |
		| **Motors** | Direct battery | TB6612FNG motor supply |
		
		
		
		## Detailed Autonomy Calculations (WITHOUT Display)
		
		### Scenario 1: Idle (System on, no movement)
		```
		Consumption: ~12.8W
		Current @ 11.1V: 12.8W / 11.1V = 1.15A
		
		3S (2200mAh):   2.2Ah / 1.15A = 1.91h  ≈ 1h 55min
		```
		
		### Scenario 2: Normal (Typical driving)
		```
		Consumption: ~58.3W
		Current @ 11.1V: 58.3W / 11.1V = 5.25A
		
		3S (2200mAh):   2.2Ah / 5.25A = 0.42h  ≈ 25min
		```
		
		### Scenario 3: Maximum (All components at 100%)
		```
		Consumption: ~168.8W
		Current @ 11.1V: 168.8W / 11.1V = 15.2A
		
		3S (2200mAh):   2.2Ah / 15.2A = 0.14h  ≈ 9min
		```
		
		
		## Power Distribution Diagram (WITHOUT Display)
		
		```
		[3S LiPo Battery 11.1V]
		    |
		    +-- [Motors] --&gt; TB6612FNG --&gt; DC Motors (2x)
		    |
		    +-- [Servo Motor WS-MG99SR]
		    |
		    +-- [LM2596 Step-Down] --&gt; 5V Rail
		    |       |
		    |       +-- Raspberry Pi 5
		    |       +-- MCP2515 (2x)
		    |       +-- TB6612FNG (VCC)
		    |       +-- USB Converter
		    |       +-- SSD M.2
		    |       +-- Hailo-8 HAT (via Pi)			
		    |
		    +-- [XL4015 Step-Down] --&gt; 3.3V Rail
		            |
		            +-- B-U585I-IOT02A
		            +-- Speed Sensor
		```
		
		
		## Final Comparison: WITH vs WITHOUT Display
		
		| Metric | WITH Display | WITHOUT Display | Difference |
		|--------|--------------|-----------------|------------|
		| **Idle Power** | 15.5W | 12.8W | -2.7W (-17.4%) |
		| **Normal Power** | 61.0W | 58.3W | -2.7W (-4.4%) |
		| **Maximum Power** | 172.0W | 168.8W | -3.2W (-1.9%) |
		| **Idle Autonomy (3S)** | 1h 34min | 1h 55min | +21min |
		| **Normal Autonomy (3S)** | 24min | 25min | +1min |
		| **LM2596 Load (peak)** | 6.3A | 5.65A | -640mA |
		| **No. Components** | 13 | 12 | -1 |
		
		````


- `docs/guides/AGL_Installation_Guide.md`

	??? "Click to view reference"

		````md
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
		10. [Wifi Configuration](#wifi-connection)
		11. [Ssh Set-up](#ssh-configuration-on-agl)
		12. [Hardware Setup and Boot](#hardware-setup-and-boot)
		
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
		curl https://storage.googleapis.com/git-repo-downloads/repo &gt; ~/bin/repo
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
		
		Or.. Download the AGL source code for the **Terrific Trout v20.0.1** ('latest-stable' version) release:
		
		```bash
		cd $AGL_TOP
		mkdir master
		cd master
		repo init -u https://gerrit.automotivelinux.org/gerrit/AGL/AGL-repo \
		          -b trout -m trout_20.0.2.xml
		```
		
		### Synchronize Repositories
		
		Fetch all required source repositories:
		
		```bash
		repo sync
		```
		
		**Note:** This process may take significant time depending on your internet connection speed.
		
		## (What could possibly go wrong here?)
		
		When downloading the meta-qt6 repository from the code.qt.io server, the server is not responding in time.
		
		Create a local manifest with a mirror
		```bash
		cd $AGL_TOP
		mkdir -p .repo/local_manifests
		nano .repo/local_manifests/qt6_mirror.xml
		```
		and this inside
		```bash
		&lt;?xml version="1.0" encoding="UTF-8"?&gt;
		&lt;manifest&gt;
		  &lt;remove-project name="yocto/meta-qt6" /&gt;
		
		  &lt;project name="YoeDistro/meta-qt6"
		           path="external/meta-qt6"
		           remote="github"
		           revision="6.7" /&gt;
		&lt;/manifest&gt;
		```
		and then force the sync with the mirror you set up, and after make a full sync just to be sure :)
		```bash
		repo sync --force-sync external/meta-qt6
		repo sync 
		```
		
		
		---
		
		## Raspberry Pi 5 Configuration
		
		If you choose **Quirky Quillback 17.1.10**, initialize the build environment for Raspberry Pi 5 with demo and development features:
		
		```bash
		cd $AGL_TOP
		source meta-agl/scripts/aglsetup.sh -m raspberrypi5 -b build-rpi5 agl-demo agl-devel
		```
		
		But if you choose **Terrific Trout v20.0.1**...
		```bash
		cd $AGL_TOP
		source master/meta-agl/scripts/aglsetup.sh -m raspberrypi5 -b build-rpi5 agl-demo agl-devel
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
		echo "# Shared directories for build optimization" &gt;&gt; $AGL_TOP/site.conf
		echo "DL_DIR = \"$HOME/downloads/\"" &gt;&gt; $AGL_TOP/site.conf
		echo "SSTATE_DIR = \"$AGL_TOP/sstate-cache/\"" &gt;&gt; $AGL_TOP/site.conf
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
		- `agl-image-minimal-crosssdk`: Minimal Image with cross-compilation SDK
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
		## (What could possibly go wrong here?)
		
		The firmware has a restricted license (synaptics-killswitch) that is not on your list of accepted licenses
		
		Add the new license to you conf/local.conf file (at the end of the file):
		
		```bash
		LICENSE_FLAGS_ACCEPTED += "synaptics-killswitch"
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
		- Ensure adequate RAM (swap space may be needed for systems with &lt;8GB RAM)
		
		**Boot Issues:**
		- Verify the image was written correctly to the microSD card
		- Check monitor connection and HDMI cable
		- Ensure power supply meets Raspberry Pi 5 requirements (5V/5A recommended)
		
		---
		
		## Version Information
		
		- **AGL Version:** Quirky Quillback 17.1.10
		- **Target Hardware:** Raspberry Pi 5
		- **Image Type:** IVI Demo with Qt Cross-SDK
		
		
		## Part of the credits goes to..... 
		
		- **https://docs.automotivelinux.org/en/trout/#01_Getting_Started/02_Building_AGL_Image/03_Downloading_AGL_Software/**
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_13 | Reviewed: ✔ | Score: 1.0 ### {: #evidences-evid_l0_13 data-toc-label="EVIDENCES-EVID_L0_13" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_13](ASSERTIONS.md#assertions-assert_l0_13) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/guides/github-guidelines.md`

	??? "Click to view reference"

		````md
		# Contributing Guide – SEA:ME Team 6 (2025/26)
		
		This document explains **how we work** in this repo: branches, commits, PRs, reviews, CI, documentation, and our sprint strategy.
		
		---
		
		## 📚 Index
		
		- [Branching](#branching)
		- [Working Flow](#working-flow)
		- [TSF Traceability (always in PR)](#tsf-traceability-always-in-pr)
		- [Commit Style (Conventional Commits)](#commit-style-conventional-commits)
		- [Issues](#issues)
		- [Pull Requests](#pull-requests)
		- [CI & Automation](#ci--automation)
		- [Sprints](#sprints)
		
		---
		
		&lt;a id="branching"&gt;&lt;/a&gt;
		## 🔀 Branching
		
		- **main** → protected and release-ready.
		- **dev** → integration branch; all features merge here first.
		- **branch number**-&lt;epic&gt;/\&lt;name&gt; → working branches off **dev**.
		  - Example:
		    - `#12-QtApp-mockup`
		
		---
		&lt;a id="working-flow"&gt;&lt;/a&gt;
		## 🧭 Working Flow
		
		```mermaid 
		  sequenceDiagram
		  participant Dev as Developer
		  participant FeatureBranch as feat branch
		  participant Deve as dev branch
		  participant Main as main
		  participant Rev as reviewer
		
		  Dev-&gt;&gt;FeatureBranch: Create #number-Epic-name from dev
		  Dev-&gt;&gt;FeatureBranch: Push commits to feature branch
		  FeatureBranch-&gt;&gt;Deve: Open PR feature -&gt; dev
		  Dev--&gt;&gt;Deve: Run checks (build, test, lint)
		  Deve-&gt;&gt;Main: Open PR dev -&gt; main
		  Dev--&gt;&gt;Deve: Report checks status (pass)
		  Rev--&gt;&gt;Main: Approve PR
		  Deve-&gt;&gt;Main: Merge PR into main
		```
		&lt;a id="tsf-traceability-always-in-pr"&gt;&lt;/a&gt;
		## 🧱 TSF Traceability (always in PR)
		
		** Still working on this part (will update when me and @jpjpcs finish this part) **
		
		Keep the TSF table updated in the PR body:
		
		| Requirement ID | Description | Status |
		|---|---|---|
		| REQ-001 | Display speed on screen | ✅ Implemented |
		| REQ-002 | Remote control capability | ⚙️ In Progress |
		
		---
		&lt;a id="commit-style-conventional-commits"&gt;&lt;/a&gt;
		## 🧠 Commit Style (Conventional Commits)
		
		We should write clear commits, nothing to long.
		Keep it simple for clarity.
		
		- `feat(ui): add speedometer widget`
		- `fix(rt): correct timer overflow`
		- `docs(workflow): add daily meeting explainer`
		
		Small commits are easier to review.
		
		---
		&lt;a id="issues"&gt;&lt;/a&gt;
		## 🔃 Issues
		
		-&gt;The issues are normally open in the begginning of each sprint -- planning.
		
		-&gt;Branches are created from the issue.
		
		-&gt;When the task of the issue is done -&gt; go to the issue -&gt; comment /taskly -&gt; fill the comment -&gt; close the issue ✅
		
		&lt;a id="pull-requests"&gt;&lt;/a&gt;
		## 🔃 Pull Requests
		
		Open PRs from `feature/*` → **dev**. Later, open **dev → main** for releases.
		
		**PR requirements**
		- Use the repo **PR Template**.
		- Clear **Summary**, **Testing steps**, **TSF table**.
		- Link Issues/Epics (`Relates to Epic: ...`).
		
		**Reviews**
		- 1 approval required for **main**.
		
		**Merging**
		- **Merge** into **dev** and **main**.
		- Delete merged branches.
		
		---
		&lt;a id="ci--automation"&gt;&lt;/a&gt;
		## 🔧 CI & Automation
		
		**(Need to work more on this part, later I will create a document to explain GitHub actions)**
		
		- **GitHub Actions** run on PRs/Issues (build/test/lint, docs helpers).
		- **Daily Standup**: Issue form creates an artifact with `YYYY-MM-DD-daily.md` and comments instructions to commit it via normal PR.
		
		---
		&lt;a id="sprints"&gt;&lt;/a&gt;
		## 🗺️ Sprints
		
		- Sprint lives in the **Project board** (Sprint view).  
		- Use labels to filter and track (examples):  
		  `Sprint 1`, `Type: Epic`, `Daily Meeting`, `Blocked`.
		- Link tasks to Epics for traceability.
		
		````


- `docs/guides/github-actions-guide.md`

	??? "Click to view reference"

		````md
		# 🚀 How to Use GitHub Actions
		
		## 📚 Index
		- [Intro](#sec-intro)
		- [Core Ideas](#sec-core-ideas)
		- [Summary](#sec-summary)
		- [Example: “Taskly”](#sec-example-taskly)
		- [Links & tutorials](#sec-links)
		
		---
		
		&lt;a id="sec-intro"&gt;&lt;/a&gt;
		## Intro
		
		GitHub Actions lets you **automate** tasks in your repository.  
		You describe **workflows** in YAML files inside `.github/workflows/`.  
		When specific **events** happen (example, a comment is posted, code is pushed), Actions **run jobs** made of **steps**.
		
		---
		
		&lt;a id="sec-core-ideas"&gt;&lt;/a&gt;
		## 🧠 Core Ideas
		
		- **Workflow**: a YAML file describing when and what to run.
		- **on**: the event that triggers the workflow (example, `push`, `pull_request`, `issue_comment`).
		- **permissions**: minimal access required (example, `issues: write` to post comments).
		- **jobs**: units of work (each job runs on a fresh virtual machine).
		- **runs-on**: which machine image to use (example, `ubuntu-latest`).
		- **steps**: actions or shell commands run in order.
		- **actions**: reusable steps (example, `actions/checkout`, `actions/github-script`).
		
		---
		
		&lt;a id="sec-summary"&gt;&lt;/a&gt;
		## 🧩 Summary
		
		1. **Event happens** → (example, someone comments on an issue).  
		2. GitHub starts your **workflow** → runs your **job** on a **runner**.  
		3. The job executes **steps** (reusable actions or shell commands).  
		4. Your workflow can **comment**, **build**, **test**, **upload artifacts**, etc.
		
		---
		
		&lt;a id="sec-example-taskly"&gt;&lt;/a&gt;
		## 📄 Example: “Taskly” — Post a Closure Template via Comment
		
		This workflow listens to **issue comments**.  
		If someone writes `/taskly` on an **Issue** (not a PR), the bot replies with a **closure summary template** so you can fill it and close the issue.
		
		&gt; File path: `.github/workflows/close-issue.yml`
		
		```yaml
		# Workflow name (shows in Actions tab)
		name: Taskly Template
		
		# When should this run? On new issue comments.
		on:
		  issue_comment:
		    types: [created]
		
		# Permissions needed:
		# - issues: write -&gt; to post a comment
		# - contents: read -&gt; safe default
		permissions:
		  issues: write
		  contents: read
		
		jobs:
		  post-template:
		    # Run this job on a GitHub-hosted Ubuntu VM
		    runs-on: ubuntu-latest
		
		    # Guard: only run if
		    # 1) the comment belongs to an Issue (not a PR)
		    # 2) the comment contains our trigger "/taskly"
		    if: ${{ github.event.issue.pull_request == null && contains(github.event.comment.body, '/taskly') }}
		
		    steps:
		      # Use the github-script action to call the API with small JS
		      - name: Post closure template
		        uses: actions/github-script@v7
		        with:
		          script: |
		            // Build the comment body line by line
		            const lines = [
		              '## ✅ Closure Summary',
		              '',
		              '**What was done**',
		              '- ',
		              '',
		              '**How it was done (approach / steps)**',
		              '- ',
		              '',
		              '**References**',
		              '- Docs: ',
		              '- Links: ',
		            ];
		            const body = lines.join('\n');
		
		            // Post the comment to the same issue
		            await github.rest.issues.createComment({
		              ...context.repo,
		              issue_number: context.payload.issue.number,
		              body
		            });
		```
		---
		
		## 📝 What this workflow does
		
		- on → issue_comment: triggers when a new comment is created.
		
		- permissions: allows the workflow to write comments on issues.
		
		- jobs.post-template:
		
		- runs-on: uses an Ubuntu runner.
		
		- if: only continues if the comment is on an Issue and includes /taskly.
		
		- steps: runs actions/github-script to post a template comment.
		
		---
		
		## ▶️ How to use it
		
		On any Issue, add a comment containing /taskly.
		The bot will reply with the closure template → you fill it → close the issue.
		
		---
		
		&lt;a id="sec-links"&gt;&lt;/a&gt;
		
		## Links
		
		https://docs.github.com/en/actions/get-started/quickstart
		
		https://www.freecodecamp.org/news/learn-to-use-github-actions-step-by-step-guide/#heading-github-actions-syntax
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_14 | Reviewed: ✔ | Score: 1.0 ### {: #evidences-evid_l0_14 data-toc-label="EVIDENCES-EVID_L0_14" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
    
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_14](ASSERTIONS.md#assertions-assert_l0_14) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/TSF_docs/WhatsTSF.md`

	??? "Click to view reference"

		````md
		# 🧩 Trustable Software Framework (TSF) — Overview and Implementation
		
		## 1. What is TSF (Trustable Software Framework)
		
		“TSF specifies how metadata about a software project is stored and managed in a git repository, alongside the software’s source code and documentation.”
		📘 Official source (GitLab project): https://www.codethink.co.uk/trustable-software-framework.
		https://projects.eclipse.org/projects/technology.tsf 
		
		The Trustable Software Framework (TSF) is an open-source toolset designed to manage requirements, evidence, and verification activities for safety- and mission-critical software — especially in aerospace, defense, automotive, and medical domains, where the software must be demonstrably trustworthy.
		
		The **Eclipse Trustable Software Framework (TSF)** is a **model and methodology to assess the "trustability" of software**, based on **verifiable evidence** of how it is developed, tested, and maintained.
		
		Instead of relying on Word documents, Excel sheets, or proprietary requirements tools, TSF integrates **trust metadata directly into the project's Git repository** (alongside code, documentation, and tests).
		
		TSF focuses on **critical systems**—where **security, performance, availability, and reliability** are essential—and allows you to:
		- **Track expectations and evidence** (who guarantees what and based on which proofs);
		- **Quantify trust** (via scores);
		- **Maintain consistency** between what the software claims to do and what it actually does.
		
		### TSF Scaffolding
		
		In the Trustable Software Framework (TSF), **scaffolding** refers to the minimal project structure required to apply TSF methodology.
		
		#### Components of TSF Scaffolding
		
		1. **Statements**  
		   - Expectations, Assertions, and Assumptions
		   - Stored as individual Markdown files (`.md`) in the repository
		   - Example from docs:  
		     &gt; “Statements are stored as markdown files that are uniquely named within a project's git repository.” — GitLab
		
		2. **Links Graph**  
		   - Represents relationships between Statements (which item supports another)
		   - Stored in `.dotstop.dot` (a directed graph / DAG)
		   - Example from docs:  
		     &gt; “The links between items are stored in a .dot file, .dotstop.dot, which is placed in the top-level of a project's git repository.”
		
		3. **Configuration / Frontmatter**  
		   - Metadata in each `.md` file
		   - Includes: normative, evidence, references, score, etc.
		
		4. **Tool Workflow (trudag)**  
		   - Manages creation, linking, scoring, and publishing of the traceability graph
		   - Ensures consistency and automation
		
		#### Summary
		
		Scaffolding = **Markdown files + DOT graph + metadata + trudag workflow**
		
		- Without scaffolding, TSF methodology cannot be applied
		- Provides a reproducible structure for traceability and automated scoring
		
		### Current Status
		
		- TSF is **incubated at the Eclipse Foundation**, with **active development by Codethink**.  
		- It is **open source**, licensed under **EPL 2.0** and **CC BY-SA 4.0**.  
		- Main development occurs on Codethink's GitLab:  
		  👉 [https://gitlab.com/CodethinkLabs/trustable/trustable](https://gitlab.com/CodethinkLabs/trustable/trustable)
		- The **official tooling** is **TruDAG** (Trustable Directed Acyclic Graph tool), implemented in Python.  
		- The model is based on **Directed Acyclic Graphs (DAGs)** composed of **Statements** linked by logical relationships.
		
		### 🧩 1.1. Trustable Software Framework (TSF) — Overview and Technical Context
		
		📚 Official source (GitLab Project)
		
		“Trustable Software Framework (TSF) builds on top of Doorstop to manage requirements, tests, and assurance cases in a traceable, verifiable way.”
		👉 Source: https://gitlab.com/trustable/trustable
		
		In short:
		
		- TSF extends Doorstop to provide full traceability and certification evidence management.
		
		### ⚙️ 1.2. What is Doorstop
		
		- Doorstop is the foundation of TSF.
		- It’s a Python-based tool that uses YAML files to represent requirements, tests, and documentation in a version-controlled and traceable way.
		
		📘 Official source (Doorstop README):
		
		“Doorstop is a requirements management tool that stores data in version control alongside source code.”
		👉 Source: https://github.com/doorstop-dev/doorstop
		
		💡 Instead of using Word or Excel, each requirement is a small .yaml file stored next to your source code in Git.
		This makes it possible to link requirements, implementation, and verification directly.
		
		📘 Official source (GitLab project)
		
		“Trustable Software Framework (TSF) builds on top of Doorstop to manage requirements, tests, and assurance cases in a traceable, verifiable way.”
		👉 Source: https://gitlab.com/trustable/trustable
		
		### 🧠 1.3. What is trudag
		
		- trudag is the command-line interface (CLI) included with the trustable package.
		- It’s used to generate traceability diagrams and documents from TSF repositories.
		
		- It converts relationships between requirements and tests into .dot files (Graphviz format), which can later be exported as PDF or PNG to visualize traceability.
		
		📗 Official source (Trustable CLI docs):
		
		“The trudag CLI builds traceability diagrams (.dot files) from TSF repositories.”
		👉 Source: https://gitlab.com/trustable/trustable/-/tree/main/frontends/cli
		
		### 🔷 1.4. What are .dot files
		
		The extension .dot comes from the Graphviz DOT language (comes and uses), a language (plain text graph description) that is used to describe grafs (nodes and connections).
		
		📚 Official source (Graphviz):
		
		“The DOT language is a plain text graph description language.”
		👉 https://graphviz.org/doc/info/lang.html
		
		💡 In TSF, DOT files are used to visualize relationships like:
		
		[REQ-001] --&gt; [TEST-001]
		
		#### This enables automatic generation of traceability diagrams for:
		
		- Requirements
		- Tests
		- Verification activities
		- Certification evidence
		
		#### Why TSF Uses a `.dotstop.dot` File
		
		The `.dotstop.dot` file is central to TSF for several reasons:
		
		1. **Represents a directed graph (DAG) between Statements**
		   - Links define logical support relationships:  
		     &gt; “Statements are connected by Links. A Link from Statement A to Statement B means that Statement A logically implies Statement B.”
		   - Source: Codethink Labs TSF documentation
		
		2. **Standardized graph format**
		   - DOT language (Graphviz)
		   - `.dotstop.dot` is a strict subset of DOT:
		     &gt; “trudag uses a strict subset of the DOT language … the .dot file `.dotstop.dot`”
		
		3. **Visualization and analysis**
		   - Can be visualized using Graphviz, networkx, Gephi, etc.
		   - Enables identification of:
		     - Dependencies
		     - Suspect links
		     - Items without evidence
		
		#### Example of `.dotstop.dot`
		
		```bash
		digraph Trustable {
		    // Metadata (optional)
		    rankdir=LR;
		    label="Trustable DAG"; 
		    node [shape=box, style=filled, color=lightgrey];
		
		    // Statement nodes (each .md)
		    "CAR-EXPECTED_LANE_KEEPING" [label="Expectation: Keep lane within ±0.3m"];
		    "CAR-LANE_KEEPING_TESTS" [label="Evidence: Simulation and track tests"];
		    "CAR-ASSUMPTIONS" [label="Assumption: Road lines visible under daylight"];
		
		    // Links between Statements
		    "CAR-LANE_KEEPING_TESTS" -&gt; "CAR-EXPECTED_LANE_KEEPING";
		    "CAR-ASSUMPTIONS" -&gt; "CAR-EXPECTED_LANE_KEEPING";
		}
		```
		
		| Line                                                       | Meaning                                                           | Source / Doc                                               |
		| ---------------------------------------------------------- | ----------------------------------------------------------------- | ---------------------------------------------------------- |
		| `digraph Trustable {`                                      | Starts a directed graph. Each link has a direction (A supports B) | Codethink Labs TSF docs                                    |
		| `rankdir=LR;`                                              | Layout left-to-right for easier reading                           | DOT standard                                               |
		| `"CAR-EXPECTED_LANE_KEEPING" ...`                          | Defines a Statement (Expectation) as a node                       | Statements stored as Markdown                              |
		| `"CAR-LANE_KEEPING_TESTS" ...`                             | Node representing Evidence                                        | Same as above                                              |
		| `"CAR-ASSUMPTIONS" ...`                                    | Node representing Assumption                                      | Same as above                                              |
		| `"CAR-LANE_KEEPING_TESTS" -&gt; "CAR-EXPECTED_LANE_KEEPING";` | Link: Evidence supports Expectation                               | TSF doc: “A Link from A to B means A logically implies B.” |
		| `"CAR-ASSUMPTIONS" -&gt; "CAR-EXPECTED_LANE_KEEPING";`        | Link: Assumption supports Expectation                             | Same                                                       |
		
		
		- Practical Usage
		. trudag manage show-link CAR-EXPECTED_LANE_KEEPING
		. trudag publish --figures
		
		Generates a visual graph (Graphviz PDF/PNG) showing support relationships.
		
		### 1.5. What is a Graph (in TSF context)
		
		A graph is a set of nodes connected by edges:
		
		- Each node is a Statement (a claim about the software).
		- Each edge is a logical link, meaning “this leads to that” or “this depends on that”.
		
		----
		
		### 1.6. Directed Acyclic Graph - TSF uses a DAG (Directed Acyclic Graph):
		
		- Directed → edges have direction (A supports B).
		- Acyclic → no cycles allowed (A cannot depend on itself indirectly).
		
		💡 Simple analogy:
		Imagine a family tree: each person (Statement) is linked to parents/children. No one can be their own ancestor → no cycles.
		
		
		## Why TSF Uses a `.dotstop.dot` File
		
		The `.dotstop.dot` file is central to TSF for several reasons:
		
		1. **Represents a directed graph (DAG) between Statements**
		   - Links define logical support relationships:  
		     &gt; “Statements are connected by Links. A Link from Statement A to Statement B means that Statement A logically implies Statement B.”
		   - Source: Codethink Labs TSF documentation
		
		2. **Standardized graph format**
		   - DOT language (Graphviz)
		   - `.dotstop.dot` is a strict subset of DOT:
		     &gt; “trudag uses a strict subset of the DOT language … the .dot file `.dotstop.dot`”
		
		3. **Visualization and analysis**
		   - Can be visualized using Graphviz, networkx, Gephi, etc.
		   - Enables identification of:
		     - Dependencies
		     - Suspect links
		     - Items without evidence
		
		### 1.7. Types of Statements
		
		#### 🔹 Conceptual Structure
		
		«TSF begins with a set of Tenets and Assertions: Concise, verifiable statements about what must be true for software to be considered trustable.»
		“These statements document claims about the software or the project… The graph describes how high-level or external goals for the software project (Expectations) are supported by more specific objectives (Assertions) and ultimately Evidence.”
		📗 Official source (Trustable CLI docs): https://codethinklabs.gitlab.io/trustable/trustable/index.html
		https://projects.eclipse.org/projects/technology.tsf
		
		Like it was said, each project is described by a **trust graph**, composed of:
		- **Expectations** → requirements or goals defined by stakeholders / What the system is expected to do (high-level goals or requirements).
		- **Assertions** → statements connecting expectations and evidence.  
		- **Premises / Evidence** → concrete proofs (documents, code, test results, audits, etc.) / Specific claims that can be verified. 
		- **Assumptions** → external conditions necessary for the project (e.g., OS dependency) / Background assumptions or supporting evidence.
		
		Premises are usually what "support" the Expectations or Assertions.
		. Premises are divided into:
		. Evidence → Concrete items proving or validating the statement (test results, simulations, documents).
		Assumptions → Conditions or contextual information that are assumed to be true for the statement to hold.
		
		Evidence can include:
		. References → Links to external sources, documents, code, or tests.
		. Validators → Automated checks (functions or plugins) that score or verify the evidence.
		. So Evidence = Reference + Optional Validator.
		
		**TSF Statement Hierarchy:**
		
		```bash
		Expectation (Goal)
		│
		├─ Assertion (Claim)
		│   │
		│   └─ Premise
		│       ├─ Evidence
		│       │   ├─ Reference (document, code, test)
		│       │   └─ Validator (automated check)
		│       └─ Assumption (contextual condition)
		````
		
		Example: 
		```bash
		Expectation: Car shall keep lane within ±0.3m
		│
		├─ Assertion: Lane keeping accuracy verified in simulation
		│   │
		│   └─ Premise
		│       ├─ Evidence: Simulation results
		│       │   ├─ Reference: sim_results_2025.md
		│       │   └─ Validator: lane_keeping_validator()
		│       └─ Assumption: Road markings visible in daylight
		---
		```
		
		**Goals → Expectations**
		**Strategies → Assertions**
		**Solutions → Evidence**
		**Context → Assumptions**
		**Principles → Tenets**
		
		From these elements, TSF builds a **traceable model**, allowing you to:
		- Justify each requirement with evidence.
		- Link test results and automated analyses.
		- Automatically evaluate **Confidence Scores** via CI/CD.
		
		#### 🔹 Resuming Table
		| Type                | Description (What it is)                                          | Function (What it does)                                | Example                               |
		|---------------------|-------------------------------------------------------------------|----------------------------------------------------------|----------------------------------------|
		| **Expectation**     | What the software should achieve (goal defined by stakeholders)   | Top of the graph — what we want to prove                | “System responds under 200ms”          |
		| **Assertion**       | Logical justification (reason why something is true)              | Connects Expectations to Evidence                        | “Performance tests are automated”      |
		| **Evidence (Premise)** | Concrete proof                                                   | Shows that the Assertion is true                         | “Automated tests show avg 180ms”       |
		| **Assumption**      | External factor presumed true                                      | Condition required for Assertions/Expectations to hold   | “System runs on Linux”                 |
		
		#### Links in the graph:
		
		- Expectation → supported by Assertions
		
		- Assertions → supported by Evidence
		
		- Assumptions → linked as external conditions
		
		#### Simple Visual Representation
		```bash
		
		Expectation: Software XYZ is safe
		        |
		     Assertion: Automated security tests passed
		        |
		     Evidence: CI/CD logs show 0 failures
		        |
		   Assumption: Runs on Linux
		```
		
		Each level is a layer of the graph.
		
		If something changes (e.g., a test fails), TruDAG marks the Statement as Suspect, signaling a review is needed.
		
		### Summary
		
		![alt text](image-1.png)
		![alt text](image-2.png)
		
		**Summary**
		In this section we will quickly recap the new objects and relationships we introduced in the methodology:
		
		**Concepts in the Methodology**
		- Artifacts are components, products or byproducts of XYZ.
		- Evidence is a Premise that is supported with an Artifact
		- Assumptions are Premises that are unsupported
		- Artifacts can Qualify Assertions
		- Artifacts can Validate Premises, making them Evidence
		- Premises can Reference Artifacts to create Evidence
		
		
		### ✅ 1.8. Difference Between Expectation and Assertion (simple and direct explanation)
		
		Think about it like this:
		
		#### 🔹 Expectation = What we want to be true
		
		An **Expectation** is a **goal, requirement, or intention** of the project.
		
		It is **high-level**, something the stakeholders want.
		
		##### Examples of Expectations:
		- “The system is secure.”
		- “The response time is below 200ms.”
		- “The software behaves reliably under load.”
		
		These are **macro-level statements** that need justification.
		
		---
		
		#### 🔹 Assertion = How we justify that Expectation
		
		An **Assertion** is a **logical statement** that connects an Expectation to the Evidence.
		
		It answers the question: **“Why do we believe this is true?”**
		
		Assertions act as **bridges** in the graph.
		
		##### Examples of Assertions:
		- “Security tests are executed automatically in CI.”
		- “All commits pass static code analysis.”
		- “Load tests run nightly on a production-like environment.”
		
		Assertions are **not proofs** — they are **reasons** that support the Expectation.
		
		---
		
		### 🧩 1.8.1. Simple visual example
		
		#### Expectation
		“The system is secure.”
		
		⬇️ *Why?* (Assertion)
		
		#### Assertion
		“Security tests are executed automatically in CI.”
		
		⬇️ *What proof do we have?* (Evidence)
		
		#### Evidence
		“CI logs show 0 failed security tests in the last 50 runs.”
		
		---
		
		#### Visual summary
		Expectation
		↓
		Assertion
		↓
		Evidence
		
		---
		
		### 💡 Analogy
		
		Imagine you are in an oral exam and the professor asks:
		
		**❓ “Why do you believe the system is secure?”**  
		(Expectation)
		
		You answer:
		
		**💬 “Because the security tests run automatically and never fail.”**  
		(Assertion)
		
		Then you show:
		
		**📄 “Here are the logs from the last 6 months.”**  
		(Evidence)
		
		---
		
		### ✔️ 1.8.2. Golden rule
		
		- **Expectations** = what we want to prove  
		- **Assertions** = how we justify it  
		- **Evidence** = proof supporting the justification
		
		
		### 1.9. TSF Artifacts: Export and Import
		
		#### 📦 What is an Artifact?
		. An *artifact* is a .tar.gz file generated by trudag (we can also see it as a packaged snapshot of a Trustable (TSF) traceability graph), which contains or includes:
		- Statements (Expectations, Assertions, Assumptions)
		- Links between items / Dependencies
		- Metadata and the resolved Graph itself
		- Unresolved “needs” for downstream projects
		
		. An artifact is not just any evidence.
		. It is usually a reusable item exported from one project to another.
		Example: A set of test results or a validator packaged for downstream use.
		
		. Think of it as a “self-contained proof” that can be imported into another project, often linked to a validator.
		. Unlike a generic evidence, an artifact may not be linked to a single statement — it can be reused or shared across multiple projects.
		
		. Artifacts are a mechanism to allow modular TSF projects, multi-project traceability.
		. TSF allow us to modularizise projects, like in ROS, Bazel, AUTOSAR or SOUP.
		
		Example use case:
		
		. Team A creates requirements + evidence → exports as an artifact
		. Team B imports that artifact into their own TSF graph → and “proves” downstream needs
		. Multiple teams can share parts of their trustability graph
		
		They are essentially packages of traceability graphs.
		---
		
		#### Producing an Artifact
		To create and operate on the "needs" graph, use the top-level --needs option:
		
		```bash
		. trudag --needs init will create the .needs.dot file.
		. trudag --needs manage create-item ... will create an item in the needs graph.
		. Any trudag command, except export, can be run with this option.
		```
		
		Run trudag export --artifact --project-name to write the artifact to a file specified by --artifact. This will both resolve references and run validators for the graph, so any required plugins must be available when performing an export.
		
		#### 🚀 Creating an Artifact (producer project)
		
		```bash
		trudag export --artifact myartifact.tar.gz --project-name "SEA-ME Team6"
		```
		
		&lt;br&gt; “Artifacts are generated through trudag export and consumed through trudag import.” &lt;br&gt;
		
		We do NOT create artifacts by manually adding links or copying .dotstop.dot content.
		
		📌 It´s exclusively created by the trudag export command.
		
		. Correct: trudag export --artifact myartifact.tar.gz --project-name "SEA-ME Team6"
		
		. Incorrect:
		```bash
		❌ It´s not created in the .dotstop.dot
		❌ Editing .dotstop.dot manually 
		❌ Copying links from a source project
		❌ It´s not writen mannualy
		❌ Creating “artifacts” by uploading or creating Markdown files mannually
		❌ An artifact is not a link inside a .md
		```
		Artifacts exist to guarantee:
		
		. consistency
		. reproducibility
		. no manual corruption of the graph
		
		#### 🚀 Consuming an Artifact
		
		- For a consuming project, once the artifact has been obtained, run trudag import to add it into the local trustable graph. This command uses the following parameters:
		
		--artifact: (required) The path from which to read the artifact
		--needs-dir/-d: (required) The path to the directory where any items should be extracted.
		--graph-root/-R: The top-level item from which the score should be extracted.
		--namespace/-n: (required) Prefix for any extracted items. This helps resolve any name conflicts.
		
		```bash
		trudag import --artifact path/to/myartifact.tar.gz \
		  --needs-dir ./docs/needs \
		  --namespace upstream/SEA-ME
		```
		This creates “Needs” items in your project — placeholders that YOU must satisfy.
		
		Purpose:
		. enforce traceability
		. enforce modularity
		. ensure proof chains exist
		
		This will automatically:
		
		. Create “NEED” items the downstream project must satisfy
		. Link them under the specified namespace
		. Extend the .dotstop.dot graph and dependencies
		. It generates .md files to the user to fullfil
		
		The following series of operations will then occur:
		- Needs items will be created in the specified directory.
		   . The consuming project will need to add evidence items to prove how these needs are being satisfied for the needs to be scored.
		- A "root" item is created in the specified directory, which contains the score for the project being consumed, along with any additional metadata.
		   . The consuming project can incorporate this item into their graph, either by linking to it directly or by referencing it as an evidence artifact.
		
		The imported items should then be added to (or updated in) the consuming graph, treating these just like any other added or updated items.
		
		#### 📌 Why export/import exist
		
		Because TSF is meant for composable safety cases, automotive systems, and multi-team integration.
		
		That means you can:
		
		. Export validated evidence
		. Import requirements from upstream projects
		. Reuse trust chains
		
		This is especially useful when one team depends on another team’s software or ML component.
		
		#### 🧠 Why artifacts exist 
		
		1. Reuse requirements across multiple projects
		
		   Example:
		
		   - Project A defines security requirements requisitos de segurança
		   - Project B imports those requirements as an artifact
		   - Project B just need to fullfil evidences, and not to rewrite everything
		
		2. Freeze versions of traceability snapshots (we export a snapshot of the state of the graph in that particular moment)
		
		3. Reduce duplication
		
		4. Allow teams to integrate external safety/security requirements
		
		5. Enable multi-module TSF architectures (similar to ROS, AUTOSAR, Bazel)
		
		6. Artifacts are always created via trudag export and never by editing .md
		files or .dotstop.dot manually.
		
		7. Create "interfaces of dependencies", as in an API between software modules
		
		### 1.10. Validators
		
		#### Packaged Validator (Pre-built Validator)
		
		A **packaged validator** is a Python module/plugin that contains a scoring function for `trudag`.  
		It automates scoring of Evidence items in your TSF graph.
		
		##### Steps to use a packaged validator
		
		1. **Install a pre-built validator**
		
		```bash
		pip install trustable-myvalidator --index-url &lt;your-pypi-or-gitlab-url&gt;
		```
		
		2. **Check that it is available to trudag**
		
		```bash
		trudag manage list-validators
		```
		
		3. **Configure your project to use the validator**
		Edit your .trudag.yaml or project configuration and add:
		
		```bash
		validators:
		  - myvalidator
		```
		
		4. **Run scoring automatically**
		
		```bash
		trudag score
		```
		
		
		Alternatively — Local Plugin
		1 - Create the file .dotstop_extensions/validators.py in your project.
		2 - Define a validator function:
		```bash
		def my_validator(configuration: dict) -&gt; (float, list):
		    """
		    Returns a score (0.0–1.0) and a list of warnings/exceptions.
		    """
		    # Example logic
		    score = 1.0
		    issues = []
		    return score, issues
		```
		3 - trudag will automatically detect validators in .dotstop_extensions/validators.py
		```bash
		---
		
		# 🟦 **Question**
		
		&gt; Explain step by step how to implement automated scoring via validators (practical + doc).
		
		---
		
		# 🟩 **Answer — Using Validators for Evidence Scoring**
		
		```markdown
		## Why Use Validators for Evidence Scoring
		
		Evidence scores in TSF can come from:
		
		- Automated assessment of Artifacts
		- Calibrated SME assessment
		
		Validators automate the first part (automated assessment).
		
		### Step-by-Step
		
		1. **Create or install a validator**  
		   - Either use a pre-built validator (packaged plugin)  
		   - Or create a local validator in `.dotstop_extensions/validators.py`
		
		2. **Connect validator to Evidence items**  
		   - Each Evidence item (.md) can reference a validator in its frontmatter:
		
		```yaml
		validator: my_validator
		```
		
		3 - Run scoring automatically
		
		```bash
		trudag score
		```
		
		. trudag executes all validators linked to Evidence items
		. Returns a numeric score (Trustable Score) for each Evidence
		. Generates warnings for missing or suspect links
		
		4 - Publish results
		```bash
		trudag publish --output ./trustable_report
		```
		
		. Creates HTML / PDF / JSON reports
		. Can be used in CI pipelines
		. Supports tracking quality over time
		
		Reference
		"Evidence scores are based on either: Automated assessment of Artifacts or Calibrated SME assessment." — Codethink Labs, Trustable Software Framework Documentation
		
		### 1.11. Graphviz
		
		#### Graphviz in TSF
		
		- TSF stores the traceability graph in .dotstop.dot (DOT format).
		- Graphviz is used to render `.dot` files into graphical and visual formats (PDF, PNG, SVG).  
		- Graphviz is NOT required to compute the Trustable Score or publish reports.
		- Scoring (trudag score) and publishing (trudag publish) work without Graphviz.
		- Installing Graphviz is only needed if you want visual diagrams.
		- Example command:
		```bash
		dot -Tpdf output/trace.dot -o trace.pdf
		```
		. Command from Graphviz (external tool).
		. Converts DOT files into PDF.
		
		#### Graphviz usefull for
		
		- Usefulness:
		1. Visual inspection → See which evidence supports which expectations.
		2. Identify gaps or suspect links → Easier to debug traceability.
		3. Documentation / reports → Publish a clean PDF/diagram for stakeholders.
		
		Important: Like it was said, Graphviz is not required for scoring or publishing reports, only for visualization.
		
		Source: Graphviz official docs (https://graphviz.org
		)
		
		### 🔗 1.12. Remote Graph
		
		In a TSF graph, it helps to break work into smaller, verifiable projects that can be integrated into larger graphs. A Remote Graph is one of these building blocks. It is a finished artifact or snapshot that you can depend on but not change.
		
		![alt text](image-7.png)
		
		**What is a Remote Graph?**
		Think of a remote graph like a published library:
		. It is immutable: once published, it never changes.
		. You must use it as-is: you cannot edit its internals.
		. It defines clear needs: it tells you what you must resolve in order for you to use it safely.
		. It acts as an interface: connecting what the remote graph provides with what your local graph must supply.
		
		**Parts of a Remote Graph**
		A remote graph usually contains two complementary pieces: the Resolved Graph and the Needs Graph.
		
		**Resolved Graph**
		The resolved graph is a frozen snapshot of a graph at a specific point in time, everything that has already been computed (Validators) or processed (References).
		
		Key points:
		. Includes pre-computed scores and references
		. Read-only: you cannot attach new local items directly into it.
		. Exports content and references used to build it.
		   - Including needs from another remote graph.
		      . Does not expose transitive dependencies (you can’t automatically see what it depends on).
		
		**Needs Graph**
		The needs graph describes what is not yet resolved. This is what your local graph must provide in order to make use of the remote graph. Think of it as the “contract clauses” of the artifact.
		
		Key points:
		
		. Includes unresolved assumptions such as AoUs (Assumptions of Use).
		. Imported into your local graph under a namespace for clarity.
		. Must be scored locally, even if you choose to ignore parts of it.
		   - Important: ignored assumptions must still be documented.
		. Ensures your local graph complies with the requirements of the remote graph.
		
		### 1.13. Trustable Tenets and Assertions
		
		![alt text](image-6.png)
		
		This overview uses a generic "XYZ" software project/product as an example. For XYZ specifically, we need to collect and evaluate evidence for all of the following:
		
		Provenance: where it comes from, who produced it, and what claims they make about it
		Construction: how to construct, install and run it. Also, how to be sure that we constructed, installed and ran it correctly
		Change: how to update it, and be confident that it will not break or regress
		Expectations: what it is expected to do, and what it must not do
		Results: what it actually does, compared to our expectations
		Confidence: our confidence in the software, based on all of the above
		Software and systems complexity has increased to the extent that in most cases we cannot aspire to bug-free software, deterministic behaviours, or complete test coverage.
		
		In practice XYZ must be released into production regularly, to introduce planned updates and improvements, and irregularly to address severe problems and security vulnerabilities. Every release can and should be considered to involve risk. The Trustable Software framework aims to provide guidance for measuring, managing and minimising the risk associated with a software product/project, both in general and for each release.
		
		### 1.14. How the Components Fit Together
		
		📘 Source (Trustable GitLab):
		
		“TSF supports traceability and assurance evidence in compliance with certification standards.”
		👉 https://gitlab.com/trustable/trustable
		
		#### 🔧 TSF Component Overview (Unified Table)
		
		| Component / Tool    | Role / Purpose                                                   | Key Feature                                      | File Types Produced / Used |
		|---------------------|------------------------------------------------------------------|--------------------------------------------------|-----------------------------|
		| **TSF (Trustable)** | End-to-end traceability + assurance framework (uses Doorstop + trudag + other utilities) | Integrates requirements, tests, evidence, and confidence scoring | `.yaml`, `.dot` |
		| **Doorstop**        | Requirements management foundation                               | YAML-based version-controlled requirements        | `.yaml` |
		| **trudag**          | Diagram + report generator for TSF                                | Generates Graphviz diagrams and PDFs with DAGs    | `.dot`, `.pdf`, `.png` |
		
		---
		
		### 1.15. Applying TSF - theory
		
		. The Trustable Software Framework is a special kind of project that is purely formed of requirements. It is intended to be composed with large software projects like XYZ, enabling them to audit the quality, completeness and correctness of their own arguments.
		
		
		![alt text](image-4.png)
		
		A simple example Trustable Graph
		
		. The TSF and XYZ should be managed separately. We recommend that you first perform one iteration of the Methodology directly to XYZ, before applying TSF.
		
		. This means deciding on your Expectations and building an argument for them out of Statements. At this stage, its fine to have many broad Assumptions. The image on the right shows an example of how this may look: We have two Expectations, X1 and X2, supported by several Assertions which in turn are linked to Statements left as Assumptions, Zi.
		
		. Now we apply the TSF. Each TA in the TSF is in fact an Assumption that must be satisfied by XYZ. When we compose TSF with XYZ, we turn these Assumptions into Assertions by linking them in to both new and existing argumentation.
		
		. To turn each TA from an Assumption into an Assertion, consider the Statements and Artifacts from XYZ that can be used to support the TAs. Note that this may require treating some Statements as Artifact. For instance, in TA-BEHAVIOURS you will need to reference XYZ's Expectations. Similarly, TA-CONSTRAINTS requires you to reference XYZ's Assumptions. You may need to make new Artifacts to support TAs you have not considered before. For example, TA-ITERATIONS requires you to assemble and provide all source code with each constructed iteration of XYZ.
		
		. The image below illustrates what this may look like for a subset of the TSF. Intermediate Statements Ui are used to tie XYZ's Statements and Artifacts into XYZ.
		Note: The example below is incomplete and does not represent a sufficient argument for any TA.
		
		
		![alt text](image-5.png)
		
		TSF applied to a simple project
		
		. U1 makes a Statement about the source code XYZ provides, supporting TA-ITERATIONS
		. U2 makes a Statement about a property of XYZ's Expectations, supporting TA-BEHAVIOURS
		. U3 makes a Statement about a property of XYZ's Assumptions, supporting TA-CONSTRAINTS
		
		. The Expectation in the TSF, TRUSTABLE-SOFTWARE therefore provides a transparent and arms-length (though not truly independent) assessment of the trust we can place in XYZ's Expectations and their score. This structure, although optional, allows upstream and downstream projects to reuse the argumentation body independently of the TSF and to separately reevaluate their trustability.
		
		## 2. Tools: TruDAG ... installation
		
		**TruDAG** is the **official tool** for implementing TSF.  
		It allows you to **create, manage, and evaluate Trustable Graphs** directly in your Git repository.
		
		TruDAG traverses the graph, checks all links and evidence, and calculates how much we can trust the Expectation.
		The commands came from:
		. trustable/trudag reference documentation
		. trustable import/export examples
		. CI examples em repositórios públicos da Codethink
		
		📖 Oficial Source:
		GitLab — TSF / Trustable Software (trudag)
		CodethinkLabs/trustable
		Documentation → Artifacts, Import/Export
		https://gitlab.com/CodethinkLabs/trustable/trustable
		
		### 🔹 2.1. Installation
		
		```bash
		pipx install trustable --index-url https://gitlab.com/api/v4/projects/66600816/packages/pypi/simple
		```
		
		OR 
		
		```bash
		pip install trustable --index-url https://gitlab.com/api/v4/projects/66600816/packages/pypi/simple
		```
		
		### 2.2. Testing TSF Installation
		
		After installing TSF, check the commands:
		
		```bash
		trudag --version
		trustable --help
		doorstop --version
		```
		
		Output example:
		```bash 
		trudag 0.4.1
		trustable 0.4.1
		doorstop 2.2.0
		```
		
		Why this sequence?
		. trudag → main CLI tool
		. trustable → underlying library (CLI help available)
		. doorstop → requirements management backend
		
		---
		
		| Tool        | Role |
		|------------|------|
		| `trudag`   | CLI tool you use directly |
		| `trustable` | Core library (no direct CLI commands) |
		| `doorstop` | Auxiliary library for requirements management (used internally by TSF) |
		
		
		Reasoning from official docs:
		“trustable and trudag are distributed as Python packages. Installing with pipx ensures isolation from system Python packages.”
		— CodethinkLabs Trustable setup guide
		
		### 🔹 2.3. Basic Usage
		
		After installation, you can:
		
		- Create Statements (Expectations, Assertions, Evidence, Assumptions);
		
		- Define links between them;
		
		- Associate artifacts (files, tests, outputs);
		
		- Generate Confidence Scores and reports.
		
		🧩 Each Statement and its links are recorded in the Git repository, ensuring native traceability.
		
		### 🪜 2.4. Why We Use This Installation Method
		
		✔️ Technical reason: Using pipx keeps the TSF environment isolated, avoiding version conflicts with system-wide Python packages &lt;br&gt;
		✔️ Security reason: TSF is used in safety-critical domains where dependency control is essential.
		&lt;br&gt;
		✔️ Traceability reason: All requirements and evidence are version-controlled in GitHub as YAML files, supporting certification standards like DO-178C, ISO 26262, and EN 50128.
		&lt;br&gt;
		
		## ⚙️ 3. Simple Lab
		
		This is an example of a workflow for TSF.
		
		1. Create virtual env  
		    ```bash
		    python3.12 -m venv .venv
		    source .venv/bin/activate
		    ```
		
		2. Install trudag (official TSF tool)  
		    ```bash
		    git clone https://gitlab.com/CodethinkLabs/trustable/trustable.git /tmp/trustable
		    cd /tmp/trustable
		    git checkout v2025.09.16
		    pip install .
		    cd -
		    ```
		
		3. trudag --init & Create requirement  
		    ```bash
		    trudag --init
		    trudag manage create-item UR 001 reqs/
		    # Edit UR-001.md - change the template
		    # -&gt; do this for every requirement (UR, SYSR, SOFR, TC)
		    trudag manage lint
		    ```
		
		4. Link requirements  
		    ```bash
		    trudag manage create-link UR-001 SYSR-001
		    # -&gt; do this for every requirement
		    trudag manage lint
		    ```
		
		5. Review requirements  
		    ```bash
		    trudag manage set-item XX-001
		    # Review every requirement needed
		    ```
		
		6. Add references/evidence & configure scoring  
		    - Add reference files to the requirements/tests  
		    - The files must exist in the correct path  
		
		    ```bash
		    trudag manage lint
		    trudag score
		    ```
		
		---
		
		### Requirement template:
		
		```yaml
		---
		id: UR-001
		normative: true
		level: 1.1
		
		references:
		  - type: "file"
		    path: "ref1.md"
		---
		
		---
		id: UR-001
		normative: true
		level: 1.2
		
		references:
		  - type: "file"
		    path: "ref2.md"
		
		reviewers:
		  - name: David
		    email: david.fernandes@seame.pt
		    reviewed: "2025-11-12 - Approved by David david.fernandes@seame.pt"
		    review_status: true
		
		score:
		  David: 0.2
		  Joao: 0.4
		---
		```
		
		## ⚙️ 4. Detailed Implementation Example
		
		. Practical Implementation with TruDAG
		
		Assume a project XYZ aims to prove it is safe and trustworthy.
		
		#### 🪄 Step 1: Initialize Repository
		```bash
		git init XYZ
		cd XYZ
		```
		
		#### 📦 Step 2: Install TruDAG / Trustable
		```bash
		pipx install trustable --index-url https://gitlab.com/api/v4/projects/66600816/packages/pypi/simple
		```
		
		OR
		
		```bash
		pip install trustable --index-url https://gitlab.com/api/v4/projects/66600816/packages/pypi/simple
		```
		
		#### 🧱 Step 3: Init
		```bash
		trudag --init 
		```
		. Expectec result:
		
		INFO: Created new dot database at .dotstop.dot
		
		📄 This creates the .dotstop file, which will be used by trudag to plot, export or calcule the Trustable Score.
		
		#### Step 4: Create Statements
		```bash
		trudag manage create-item "Software XYZ is safe" ./XYZ/ --type Expectation
		trudag manage create-item "Automated security tests passed" ./XYZ/ --type Assertion
		trudag manage create-item "CI/CD logs show 0 failures" ./XYZ/ --type Evidence
		trudag manage create-item "Runs on Linux" ./XYZ/ --type Assumption
		```
		. Check suspect/unreviewed items and errors in the graph connections
		```bash
		trudag manage lint 
		```
		#### 🔗 Step 5: Link Statements
		```bash
		trudag manage create-link "Automated security tests passed" "Software XYZ is safe"
		trudag manage create-link "CI/CD logs show 0 failures" "Automated security tests passed"
		trudag manage create-link "Runs on Linux" "Automated security tests passed"
		```
		
		#### 🧩 Step 6: Validate Evidente
		```bash 
		trudag manage set-item "Software XYZ is safe" ./XYZ/ --type Expectation
		trudag manage set-item "Automated security tests passed" ./XYZ/ --type Assertion
		trudag manage set-item "CI/CD logs show 0 failures" ./XYZ/ --type Evidence
		trudag manage set-item "Runs on Linux" ./XYZ/ --type Assumption
		```
		. Check suspect/unreviewed items and errors in the graph connections
		```bash
		trudag manage lint 
		```
		
		#### Step 7: Evaluate Confidence
		. This calculates scores recursively using Evidence scores (SME + validators) and produces a resume. A doc describes the recursive calcule (W^i * t_E).
		```bash
		trudag score
		```
		
		#### Step 8: Publish
		```bash
		trudag publish
		```
		
		#### Step 9: Notes about Other commands
		. Inspect specific items or links 
		```bash
		trugad manage show-item 
		trudag manage show-link
		```
		.Reviewing items
		```bash
		trudag manage set-item #name of the item#
		trudag manage set item #name of the item1# #name of the item2#
		```
		.Clearing suspect items
		```bash
		trudag manage set link #name of the item#
		```
		. Linking a trustable graph
		```bash
		trudag manage lint 
		```
		. Get Score
		```bash
		trudag score           # executes validators and prints resume 
		trudag score --no-validate  # if we want only to propague SME scores
		```
		. Publish Report
		```bash
		trudag publish
		trudag publish --figures     # includes figures for temporal analysis - historical figures
		trudag publish --no-validate # it publish without execute the validators
		```
		. Creating an artifact
		```bash
		trudag export
		```
		. Consuming an artifact
		```bash
		trudag import
		```
		
		##### Trudag CLI — Commands Overview (version 2025.10.22)
		
		| Command | Subcommands / Functions | Description |
		|---------|------------------------|------------|
		| `init` | — | Creates a new `.dotstop.dot` repository (traceability database) |
		| `import` | — | Imports an artifact (requirements, code, tests) into the TSF graph |
		| `export` | — | Exports the resolved graph (DOT, YAML, PDF, etc.) |
		| `manage` | `lint` | Checks graph consistency, suspect links, missing reviews |
		|         | `show-item` | Display details of a specific item |
		|         | `show-link` | Display links of a specific item |
		|         | `set-item` | Modify attributes of a specific item |
		|         | `set-link` | Modify links between items |
		| `plot` | — | Generates visual traceability graph (like "build graph") |
		| `publish` | — | Publishes results/artifacts (for CI/CD or reports) |
		| `score` | — | Computes Trustable Score — traceability and completeness metric |
		
		###### 📖 References / Docs
		
		- `trudag manage`: https://gitlab.com/CodethinkLabs/trustable/trustable/-/blob/main/docs/reference/trudag/manage.md  
		- Full CLI reference: https://codethinklabs.gitlab.io/trustable/trustable/doorstop/TRUSTABLE.html
		
		#### Step 10: Notes about Other Recommended TSF Workflow
		
		##### ✅ What to Do Now
		1️⃣ **Ensure we are in your TSF project folder**
		```bash
		cd docs/TSF/tsf-demo
		```
		2️⃣ Generate the traceability graph (equivalent to "build")
		```bash
		trudag plot
		```
		Produces a .dot file under artifacts/ or output/
		
		3️⃣ (Optional) Export to PDF or YAML
		```bash
		trudag export --format dot
		```
		Or export everything in YAML for integration with other tools:
		
		```bash
		trudag export --format yaml
		```
		
		4️⃣ (Optional) Evaluate traceability
		
		After importing requirements and tests, measure project coverage:
		
		```bash
		trudag score
		```
		
		Generates a Trustable Score report showing how much of the project is verified.
		
		##### 💡 Why Use This Approach
		
		. plot → visual graph generation (architecture, visualization)
		. score → quantitative analysis (coverage, verification)
		. publish → automation for GitHub Actions / CI pipelines
		
		This model comes from the Trustable Software Initiative (2024–2025) in GitLab trustable/trustable, updating the classic Doorstop model for AI data, automotive software, and continuous traceability.
		
		“trudag plot replaces build as the command to render a complete traceability graph of the .dotstop database.” — Trustable Documentation, GitLab, CLI section, 2025.10 release
		
		##### TSF Real Project Examples
		
		Here are some open-source projects demonstrating TSF structure, folder naming, and file organization.
		
		```bash
		tsf-demo/
		├─ .dotstop.dot # Graph of traceability links
		├─ docs/
		│ ├─ TSF/
		│ │ ├─ statements/
		│ │ │ ├─ CAR-EXPECTED_LANE_KEEPING.md
		│ │ │ ├─ CAR-LANE_KEEPING_TESTS.md
		│ │ │ └─ CAR-ASSUMPTIONS.md
		│ │ ├─ output/
		│ │ │ └─ traceability.pdf
		│ │ └─ tsf-demo-config.yaml
		├─ artifacts/ # Auto-generated DOT/YAML/PDF
		├─ .trudag.yaml # Project configuration
		├─ requirements/
		│ └─ obstacle-detection.md
		└─ tests/
		└─ lane-keeping-tests.md
		```
		
		**Explanation:**
		
		- `statements/` → all human-editable Markdown files (Expectations, Evidence, Assumptions)  
		- `.dotstop.dot` → the directed graph defining dependencies  
		- `output/` → PDF/graph generated from `trudag plot`  
		- `artifacts/` → exported graphs, reports, or snapshots for versioning  
		- `.trudag.yaml` → configuration for validators, scoring, and publishing
		
		---
		
		**Example of a Validator Plugin**
		tsf-demo/.dotstop_extensions/
		└─ validators.py
		
		```python
		# Example validator
		def lane_keeping_validator(configuration: dict) -&gt; (float, list):
		    score = 1.0
		    issues = []
		    return score, issues
		```
		. trudag detects this validator automatically when running score
		. Validators automate scoring based on Evidence items in .md files
		
		**CI/CD Integration Example**
		
		.github/workflows/trustable.yml
		```bash
		name: TSF CI
		on: [push, pull_request]
		
		jobs:
		  trustable:
		    runs-on: ubuntu-latest
		    steps:
		      - uses: actions/checkout@v4
		      - name: Setup Python
		        uses: actions/setup-python@v4
		        with:
		          python-version: '3.10'
		      - run: pip install trustable --index-url https://gitlab.com/api/v4/projects/66600816/packages/pypi/simple
		      - run: trudag manage lint
		      - run: trudag score --validate
		      - run: trudag publish --output ./docs/TSF/output
		```
		
		. Automates linting, scoring, and report generation
		. Supports GitHub Pages / CI visibility
		
		References / Example Projects
		. CodethinkLabs TSF Demo: https://gitlab.com/CodethinkLabs/trustable/trustable
		. Trustable Software Framework Docs: https://codethinklabs.gitlab.io/trustable/trustable/doorstop/TRUSTABLE.html
		
		
		## 🧮 5. Relation to Traceability Matrix and Other Approaches
		
		TSF replaces (or generalizes) traditional traceability matrices, but in a live and automated form rather than static documents.
		
		| Method                  | Traditional Form                     | TSF / TruDAG                                     |
		| ----------------------- | ------------------------------------ | ------------------------------------------------ |
		| **Traceability Matrix** | Manually in Excel or dedicated tools | Represented as a **graph (DAG)** in Git          |
		| **Evidence**            | Linked via documents                 | Linked via **verifiable artifacts and metadata** |
		| **Updates**             | Manual, error-prone                  | **Automated** via CI/CD                          |
		| **Assessment**          | Subjective                           | **Quantifiable** with “Confidence Scores”        |
		| **Scalability**         | Limited                              | **High**, with composition across projects       |
		
		Thus, no traditional matrix is needed—the TSF graph replaces it, and TruDAG is the practical management tool.
		
		## 6. Github Integrations: CI / CD
		
		How can I integrate trudag into GitHub Actions so that:
		. each commit or merge automatically updates the .dot graph, and that graph is published as /docs/TSF/output/traceability.pdf on GitHub Pages.
		
		Here is a full, functional, ready-to-use workflow that:
		. Generates the .dot graph with trudag plot
		. Uses Graphviz to convert it into a PDF
		. Moves the PDF to /docs/TSF/output/traceability.pdf
		. Automatically commits it to the branch that powers GitHub Pages (main or gh-pages)
		
		### Workflow A
		It creates a workflow .github/workflows/trustable.yml that executes trudag score in each push/PR and publish the report as an  artefact or generates site (gh-pages):
		
		```bash
		name: Trustable score
		
		on:
		  push:
		    branches: [ main ]
		  pull_request:
		
		jobs:
		  trustable:
		    runs-on: ubuntu-latest
		    steps:
		      - uses: actions/checkout@v4
		        with:
		          fetch-depth: 0
		
		      - name: Set up Python
		        uses: actions/setup-python@v4
		        with:
		          python-version: '3.10'
		
		      - name: Install trudag (from GitLab package index)
		        run: |
		          python -m pip install --upgrade pip
		          pip install trustable --index-url https://gitlab.com/api/v4/projects/66600816/packages/pypi/simple
		
		      - name: Run trudag lint
		        run: trudag manage lint
		
		      - name: Run trudag score (validate)
		        run: |
		          trudag score --validate --dump trustable_scores.json
		
		      - name: Publish report
		        run: trudag publish --output ./trustable_report
		      - name: Upload report artifact
		        uses: actions/upload-artifact@v4
		        with:
		          name: trustable-report
		          path: trustable_report
		```
		
		### Workflow B
		
		Workflow B: It creates a basic workflow .github/workflows/trustable.yml
		
		```bash 
		name: Trustable CI
		on: [push, pull_request]
		jobs:
		  trustable:
		    runs-on: ubuntu-latest
		    steps:
		      - uses: actions/checkout@v4
		      - name: Setup Python
		        uses: actions/setup-python@v4
		        with:
		          python-version: '3.10'
		      - name: Install trudag
		        run: pip install trustable --index-url https://gitlab.com/api/v4/projects/66600816/packages/pypi/simple
		      - name: Lint trustable graph
		        run: trudag manage lint
		      - name: Run score
		        run: trudag score --validate --dump trustable_scores.json
		      - name: Upload report
		        run: trudag publish --output ./trustable_report
		      - uses: actions/upload-artifact@v4
		        with:
		          name: trustable-report
		          path: trustable_report
		```
		
		Workflow A — Full CI with lint + score + publish
		
		(The long one)
		
		Purpose:
		. Run trudag manage lint
		. Run trudag score
		. Validate the TSF graph
		. Generate HTML/JSON report
		. Upload report as GitHub Artifact
		
		Optional: publish to GitHub Pages
		
		This is the recommended production CI pipeline.
		
		Use when:
		
		✔ your project is active
		✔ you want automated score validation
		✔ you want CI to block bad PRs
		✔ you want to publish TSF reports
		
		Workflow B — Minimal CI (quick checks only)
		
		(The “smaller version”)
		
		Purpose:
		. Install trustable
		. Run lint
		. Run score
		. Upload report
		
		It is simpler but does not:
		. publish to GitHub Pages
		. generate full websites
		. enforce stricter CI failures
		
		Use when:
		
		✔ early development
		✔ experimentation
		✔ small PoCs
		
		Which one should YOU use?
		
		Use the full workflow (A) if your goal is:
		| Goal                    | Recommended workflow |
		| ----------------------- | -------------------- |
		| Enforce quality         | **Full workflow**    |
		| Generate HTML report    | **Full workflow**    |
		| Use GitHub Pages        | **Full workflow**    |
		| Only validate basic TSF | Minimal workflow     |
		
		
		### Workflow C - the one we will use
		
		. PDF of Traceability
		
		```bash
		name: Update Traceability Graph
		
		on:
		  push:
		    branches: [ main ]
		  pull_request:
		
		jobs:
		  build-graph:
		    runs-on: ubuntu-latest
		
		    steps:
		      - uses: actions/checkout@v4
		        with:
		          fetch-depth: 0
		
		      - name: Setup Python
		        uses: actions/setup-python@v4
		        with:
		          python-version: '3.10'
		
		      - name: Install trudag and trustable
		        run: |
		          pip install trustable --index-url https://gitlab.com/api/v4/projects/66600816/packages/pypi/simple
		
		      - name: Install Graphviz
		        run: sudo apt-get install -y graphviz
		
		      - name: Generate .dot graph
		        run: trudag plot --output output/trace.dot
		
		      - name: Convert .dot to PDF
		        run: dot -Tpdf output/trace.dot -o docs/TSF/output/traceability.pdf
		
		      - name: Commit and push PDF to repo
		        uses: stefanzweifel/git-auto-commit-action@v5
		        with:
		          commit_message: "Update traceability graph [CI]"
		          file_pattern: docs/TSF/output/traceability.pdf
		```
		
		## 🧭 7. Conclusion
		
		TSF provides a modern, formal approach to assess software trustability, replacing manual documents and matrices with a declarative, traceable structure integrated into the development workflow.
		
		### 🔑 Key Takeaways
		
		- Not a static Excel matrix — it is a graph of Statements in Git.
		
		- Each Statement is traceable and linked to concrete evidence.
		
		- TruDAG automates creation, linking, and analysis.
		
		- Confidence is calculated automatically, but human review remains essential.
		
		➡️ Outcome: a report with confidence scores, evidence, and traceable logical dependencies.
		
		### ✅ Practical steps:
		
		- Use TruDAG (official Python tool)
		
		- Model the project as a graph of Statements
		
		- Link concrete evidence and artifacts
		
		- Automate analysis via CI/CD
		
		- Discard traditional spreadsheets and traceability tools
		
		
		### Best Practices / Recommendations
		
		- Store Statements in human-editable files (Markdown) and use `trudag` to manage IDs and links. **Do not edit `.dotstop.dot` manually.**
		- Register Assumptions as Items without references — this is important for transparency.
		- Version artifacts (export) and keep a historical dump in a data store for temporal analysis. `trudag` supports integration with data stores via `.dotstop_extensions/data_store.py`.
		- Create reusable validators and package them (entry point: `trustable.validator.plugins`) for sharing between projects.
		- Automate scoring in CI and fail pipelines on PRs if `trudag manage lint` detects suspect links or items without review.
		
		### Common Pitfalls / Gotchas
		
		- Old Python version on your system (e.g., Python 2.7) — install Python 3.10+ and ensure `trudag` uses that interpreter.
		- If you install `trustable` from the GitLab package index, CI may require credentials if the index is private — use GitHub Actions secrets or install via wheel.
		- Statement identifiers cannot contain quotes (`"`) due to DOT parser limitations — avoid problematic characters.
		
		
		## 📚 8. References
		
		- Eclipse TSF Project Page: https://projects.eclipse.org/projects/technology.tsf
		
		- Codethink: https://www.codethink.co.uk/trustable-software-framework.html
		
		- Codethink TSF GitLab: https://gitlab.com/CodethinkLabs/trustable/trustable
		
		- Structure: https://gitlab.com/CodethinkLabs/safety-monitor/safety-monitor/-/tree/main/trustable
		
		- TruDAG Methodology & Documentation: https://codethinklabs.gitlab.io/trustable/trustable/index.html
		
		- TruDAG Installation Guide: https://codethinklabs.gitlab.io/trustable/trustable/trudag/install.html#user
		. https://gitlab.com/CodethinkLabs/trustable/trustable/-/tree/main/docs/trudag 
		
		- TSF Methodology Overview: https://codethinklabs.gitlab.io/trustable/trustable/methodology.html
		
		- https://score-json.github.io/json/main/generated/trustable_graph.html
		
		- Meetings: https://pad.codethink.co.uk/Bz7i6JRyRBSLC_Y7ID4lYQ
		
		- Question: https://gitlab.com/CodethinkLabs/trustable/trustable/-/issues/22
		. https://gitlab.com/CodethinkLabs/trustable/trustable/-/issues?sort=created_date&state=opened&first_page_size=100&page_after=eyJjcmVhdGVkX2F0IjoiMjAyNS0wNi0xMSAxMTo1MDoxNy4wNjQxNDMwMDAgKzAwMDAiLCJpZCI6IjE2ODczNjk4OCJ9
		
		## 💡 “Any consideration of trust must be based on evidence.”
		— Trustable Software Framework
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_15 | Reviewed: ✔ | Score: 1.0 ### {: #evidences-evid_l0_15 data-toc-label="EVIDENCES-EVID_L0_15" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_15](ASSERTIONS.md#assertions-assert_l0_15) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/dailys/dailys_warmups.md`

	??? "Click to view reference"

		````md
		# 🕒 Daily Scrum - Team 6
		
		**Scrum Master:** Joao Silva  
		**Participants:** Team
		1️⃣2️⃣3️⃣4️⃣5️⃣6️⃣7️⃣8️⃣9️⃣🔟
		1️⃣1️⃣ 1️⃣2️⃣ 1️⃣3️⃣ 1️⃣4️⃣ 1️⃣5️⃣ 1️⃣6️⃣ 1️⃣7️⃣ 1️⃣8️⃣ 1️⃣9️⃣ 2️⃣0️⃣
		---
		
		## 👤 Updates & Planning
		| **Date**       | **Owner / Master / Sprint Info**                   | **Progress** Goals of the Day / Pending Tasks                                                                 | What Was Done                                                                                             | **Blockers & Notes** Blockers / Questions / Doubts / Meeting Notes                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    |
		|:-----------|:------------------------|:---------------------------------------------------------------------------------------------------------|:----------------------------------------------------------------------------------------------------------|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
		| 06/10/2025 | João                    | SEA:ME presentation and team setup                                                                       | Goal achieved                                                                                             | No blockers                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |
		| 08/10/2025 | João (Pre-Sprint Planning) | First weekly meeting with pre-planning for the next sprint                                               | Goal achieved                                                                                             | No blockers                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |
		| 10/10/2025 | João                    | Creation of GitHub repo TEAM&_2025/2025 and project board on SEA:ME GitHub                                | In progress/To be done, no issues                                                                                    | Waiting for Victor’s authorization for GitHub Actions and PRs                                                                                                                                                                                                                                                                                                                                                                                                                            |
		| 13/10/2025 | João  **SPRINT PLANNING** | 1. Hardware setup&lt;br&gt;2. Software setup&lt;br&gt;3. GitHub Actions&lt;br&gt;4. TSF&lt;br&gt;5. Qt and cross-compilation — with repo setup and project board creation | In progress/To be done, no issues                                                                                    | Waiting for Victor’s authorization for GitHub Actions and PRs                                                                                                                                                                                                                                                                                                                                                                                                                            |
		| 14/10/2025 | João    **STANDUP PLANNING EXECUTION STARTING POINT**                 | Continued implementation of the items mentioned on the previous day                                     | In progress/To be done, no issues                                                                                    | Waiting for Victor’s authorization for GitHub Actions and PRs                                                                                                                                                                                                                                                                                                                                                                                                                            |
		| 15/10/2025 | João  **STANDUP MEETING**  | **Daily Goals** &lt;br&gt; 1. Setup&lt;br&gt;2. Install Hailo&lt;br&gt;3. Connect CSI&lt;br&gt;4. Connect DSI&lt;br&gt;5. Connect gamepad and install dependencies | **Done** &lt;br&gt; 1. Set almost ready and almost fully tested&lt;br&gt;2. Raspberry Pi Bookworm installed&lt;br&gt;3. Hailo Hat 8 installed [](https://github.com/hailo-ai/hailo-rpi5-examples)&lt;br&gt;5. DSI connected and working&lt;br&gt;6. Gamepad and dongle connected and working with dependencies installed&lt;br&gt;&lt;br&gt; **In Progress/To be done** &lt;br&gt;7. Missing: connect everything to the expansion board (need to buy headers) | **BLOCKERS** &lt;br&gt; Waiting for Victor’s authorization for GitHub Actions and PRs&lt;br&gt;&lt;br&gt;**Weekly Meeting Notes** Questions for David (with answers):&lt;br&gt;1. Things we need to buy for the car — should we buy them ourselves and present an expense note? (Yes, we can buy them: headers, original Raspberry charger €34 5V/5A, longer DSI cables, multiple microHDMI adapters, Raspberry Pi 5 heatsinks/fans)&lt;br&gt;2. I’m responsible for making an inventory of what all groups need and sending it to Filipe tomorrow. He’ll meet Maria and handle purchases.&lt;br&gt;3. Split big tasks across sprints (Epics). Tasks must be sprint-specific; otherwise, they’re too large and must be divided.&lt;br&gt;4. TSF requires a traceability matrix (PB1→L0→L1→L2→L3→Test Cases)? → Yes, that’s correct.&lt;br&gt;5. We’re on the right path; Filipe said Dana will prepare a session tomorrow.&lt;br&gt;6. Should we choose between AGL (Automotive Grade Linux) or Android Automotive OS? Or can we use others like Eclipse ThreadX? → We should use **AGL (for Raspberry)** and **ThreadX (for the microcontroller)**.&lt;br&gt;7. Pedro suggested connecting the motors to the microcontroller later. For now, keep them as-is, but plan for it later — the powertrain should stay with the microcontroller. Later, create a communication layer between the Raspberry and microcontroller (e.g., using uProtocol). It could be bidirectional.&lt;br&gt;8. Which warmup week are we in? → The previous one was week 0 (or -1); this is the first week of Sprint 1. There are two sprints in Module 0. We’re ahead (very good).&lt;br&gt;9. Power problems: we can use two separate battery sets or voltage regulators. Later, we can calculate total power usage theoretically using hardware specs.&lt;br&gt;10. Hardware has power specifications; we can measure via oscilloscope or theoretical calculation.&lt;br&gt;11. TSF nomenclature must be used (L0, L1, etc.).&lt;br&gt;12. Talk to Victor for permissions (PR rules, GitHub Actions workflows).&lt;br&gt;13. Miguel’s group can’t connect Wi-Fi (kernel config issue). If we use Raspberry Wi-Fi, we might face the same problem — needs solving.&lt;br&gt;14. Scrum: we can choose what works best for the group (daily reports, etc.) — keep it simple (KISS).&lt;br&gt;15. Asked if each task requires documentation — yes, as discussed, in the issue. Suggested extra traceability documentation backup (agreed). |
		| 16/10/2025 | João                    | **Daily Goals** 1. Integrate and test main components (Raspberry Pi, Hailo Hat, DSI, CSI, Gamepad)&lt;br&gt;2. Clarify logistic and technical doubts in the 16h meeting&lt;br&gt;3. Create Pull Request and Daily templates&lt;br&gt;4. Continue TSF implementation on GitHub&lt;br&gt;5. Continue GitHub Actions analysis&lt;br&gt;6. Proceed with Qt Application&lt;br&gt;7. Update board | **Done** 1. Set almost ready and tested:&lt;br&gt;• Raspberry Pi OS (Bookworm) installed&lt;br&gt;• Hailo Hat 8 working (rpicam-apps)&lt;br&gt;• CSI camera functional (hailo-rpi5-examples)&lt;br&gt;• DSI and gamepad working&lt;br&gt;2. PR, Daily, and Taskly templates created&lt;br&gt;3. TSF implementation started&lt;br&gt;**In Progress/To be done** 4. GitHub Actions on hold&lt;br&gt;5. Qt App and board update in progress | **BLOCKERS** Waiting for Victor’s authorization for GitHub Actions and PRs&lt;br&gt;&lt;br&gt;**REMINDERS:**&lt;br&gt;&lt;br&gt;• **Missing** Missing materials (headers, 5V/5A charger, DSI cables, HDMI adapters, fans)&lt;br&gt;&lt;br&gt;**Pending:** &lt;br&gt;• Possible Wi-Fi configuration issues&lt;br&gt;• Dependency on validation by David and Filipe&lt;br&gt; &lt;br&gt; Pending decision between AGL / ThreadX / FreeRTOS / Azure&lt;br&gt;• Need to split big tasks into subtasks/epics&lt;br&gt;• Confirm nomenclature and use of TSF traceability matrix&lt;br&gt;• Permissions for PRs and GitHub Actions still pending |
		| 17/10/2025 | João                    | **Daily Goals** &lt;br&gt; 1. Connect all components to expansion board / assemble car&lt;br&gt;2. Prepare component inventory and send to Filipe&lt;br&gt;3. Validate PR/Daily/Taskly templates (João & David)&lt;br&gt;4. Continue TSF implementation&lt;br&gt;5. Continue GitHub Actions and Qt work&lt;br&gt;6. Update board | Everything still to do                                                                                    | **BLOCKERS** Waiting for Victor’s authorization for GitHub Actions and PRs&lt;br&gt;&lt;br&gt;**REMINDERS:**&lt;br&gt;&lt;br&gt; **Missing** &lt;br&gt;• Dependent on inventory and template validation&lt;br&gt;&lt;br&gt; **Pending** &lt;br&gt; •TSF with implementation blocker on score •Qt crosscompilation blocker, making it not possible to compile&lt;br&gt;• GitHub Actions on standby |
		| 20/10/2025 | João                    | **Daily Goals** &lt;br&gt; 1. Connect all components and test new setup&lt;br&gt;2. Prepare inventory and send to Filipe&lt;br&gt;3. Validate templates&lt;br&gt;4. Continue TSF implementation&lt;br&gt;5. Continue GitHub Actions analysis&lt;br&gt;6. Continue Qt Application&lt;br&gt;7. Update board | Everything done except TSF and Qt                                                                         | **BLOCKERS** &lt;br&gt; Waiting for Victor’s authorization for GitHub Actions and PRs &lt;br&gt;&lt;br&gt;**REMINDERS:**&lt;br&gt;&lt;br&gt; **Missing** &lt;br&gt;• Dependent on inventory and template validation&lt;br&gt;&lt;br&gt; **Pending** &lt;br&gt; •TSF with implementation blocker on score •Qt crosscompilation blocker, making it not possible to compile&lt;br&gt;• GitHub Actions on standby |
		| 21/10/2025 | João                    | Continue implementation of previous unfinished points                                                    | In progress/To be done, no issues                                                                                    | **Blockers** &lt;br&gt; Waiting for Victor’s authorization for GitHub Actions and PRs &lt;br&gt;&lt;br&gt; **Missing** &lt;br&gt;• Dependent on inventory and template validation&lt;br&gt;&lt;br&gt; **Pending** &lt;br&gt; •TSF with implementation blocker on score •Qt crosscompilation blocker, making it not possible to compile&lt;br&gt;• GitHub Actions on standby |
		| 22/10/2025 | Rubén  **STANDUP MEETING**  | **Daily Goals** &lt;br&gt; 1. Continue ongoing work and prepare for weekly meeting                                                  | Everything done                                                                                           | **BLOCKERS** &lt;br&gt; Waiting for Victor’s authorization for GitHub Actions and PRs &lt;br&gt;&lt;br&gt;**REMINDERS:**&lt;br&gt;&lt;br&gt;**Missing** &lt;br&gt;• Dependent on inventory and template validation&lt;br&gt;&lt;br&gt; **Pending** &lt;br&gt; •TSF with implementation blocker on score •Qt crosscompilation blocker, making it not possible to compile&lt;br&gt;• GitHub Actions on standby &lt;br&gt;&lt;br&gt;**Weekly Meeting Notes:**&lt;br&gt;1. Git (reviews, rule sets, etc.): to be discussed and resolved with Victor (42)&lt;br&gt;2. Qt: only a basic “Hello World” is required for this module, but an early design version is also fine&lt;br&gt;3. Wireless DNS issues: being handled by Victor (42)&lt;br&gt;4. AGL: research what Yocto is (Linux distribution builder)&lt;br&gt;5. CAN communication: use CAN, not CAN-FD&lt;br&gt;6. CAN communication setup: RASP → MF2515 → CAN cable → MF2515 → STM32&lt;br&gt;7. I2C/SPI/GPIO libraries: research and use Eclipse uProtocol&lt;br&gt;8. Power management: research and plan system design (Battery Management System + Buck-Boost Regulator + Power Distribution Board with independent fuses)&lt;br&gt;9. Meetings: TSF → Nov 6; ThreadX → Nov 4/11/14 (TBD); Architecture Blueprints Eclipse → TBD; OpenHardware Workshop → TBD |
		| 23/10/2025 | João                    | **Daily Goals** &lt;br&gt; Study topics discussed in weekly meeting and continue themes:&lt;br&gt;1. Hardware setup&lt;br&gt;2. Software setup&lt;br&gt;3. GitHub Actions&lt;br&gt;4. TSF&lt;br&gt;5. Qt and cross-compilation | We´re studying the themes                                                                                  | **BLOCKERS** &lt;br&gt; Waiting for Victor’s authorization for GitHub Actions and PRs &lt;br&gt;&lt;br&gt;**REMINDERS:**&lt;br&gt;&lt;br&gt;**Missing** &lt;br&gt;• Dependent on inventory and template validation&lt;br&gt;&lt;br&gt; **Pending** &lt;br&gt; •TSF with implementation blocker on score •Qt crosscompilation blocker, making it not possible to compile&lt;br&gt;• GitHub Actions on standby |
		| 24/10/2025 | João  **SPRINT RETROSPECTIVE**| **Daily Goals** &lt;br&gt; Continue implementation of previous points and prepare sprint retrospective meeting                                    | In progress/To be done, no issues. Presentation done                                                                                    | **Blockers** &lt;br&gt; Waiting for Victor’s authorization for GitHub Actions and PRs &lt;br&gt;&lt;br&gt; **Missing** &lt;br&gt;• Dependent on inventory and template validation&lt;br&gt;&lt;br&gt; **Pending** &lt;br&gt; •TSF with implementation blocker on score •Qt crosscompilation blocker, making it not possible to compile&lt;br&gt;• GitHub Actions on standby |
		| 27/10/2025 | João  **SPRINT PLANNING**  | **Daily Goals** &lt;br&gt; 1. Update branches: make development and main identical&lt;br&gt;2. Define work methodology for next sprint (dailys, tasklys, etc.) and who´s going to make the dailys&lt;br&gt;3. Sprint planning and definition:&lt;br&gt; 1 - Architecture (software + hardware): AGL, ThreadX, CanBus, uProtocol&lt;br&gt; 2 - Remove expansion board and link motors directly to microcontroller&lt;br&gt; 3 - Implement TSF on GitHub&lt;br&gt; 4 - Qt and cross-compilation&lt;br&gt; 5 - Study next module and link to current one&lt;br&gt; 6 - Engineering process refinement | Branches updated, work methodology defined, sprint planning done and redefined as:&lt;br&gt;• Architecture (software + hardware): AGL, ThreadX, CanBus, uProtocol&lt;br&gt;• Remove expansion board and link motors directly to microcontroller&lt;br&gt;• Implement TSF on GitHub&lt;br&gt;• Qt and cross-compilation&lt;br&gt;• Study next module and link to current one&lt;br&gt;• Engineering process refinement&lt;br&gt;&lt;br&gt;**Sprint Planning Meeting Notes** &lt;br&gt; During the meeting we were advised to:&lt;br&gt;1. Replace “finalize past tasks” → “engineering process refinement” (Paulo)&lt;br&gt;2. Remove Epic numbers (Paulo)&lt;br&gt;3. Review difference between Epics and Spikes (Pedro)&lt;br&gt;4. Possible Epics: Instrument Cluster, UI Display, Communication Architecture, Motion Control&lt;br&gt;5. Study monolithic vs zonal architecture (zonal = divide car architecture into zones that communicate between each other for safety-critical isolation)&lt;br&gt;6. Dailys will be made by: 2ª (João), 3ª (Diogo), 4ª (Rubén), 5ª (David), 6ª (Vasco) | **BLOCKERS:**&lt;br&gt; 1. Waiting for Victor’s authorization for GitHub Actions and PRs&lt;br&gt; 2. Car doesn’t work: possible software problem&lt;br&gt;&lt;br&gt;**REMINDERS:**&lt;br&gt;&lt;br&gt;**Missing** &lt;br&gt; 1. Shop still missing: headers, original Raspberry charger €34 5V/5A, longer DSI cables, multiple microHDMI adapters, Raspberry Pi 5 heatsinks/fans&lt;br&gt;&lt;br&gt; **Pending** &lt;br&gt; •TSF with implementation blocker on score •Qt crosscompilation blocker, making it not possible to compile&lt;br&gt;• GitHub Actions on standby&lt;br&gt;&lt;br&gt;**Suggestions:**&lt;br&gt; 1. Removing expansion board&lt;br&gt; 2. Power management: research and plan system design (Battery Management System + Buck-Boost Regulator + Power Distribution Board with independent fuses)&lt;br&gt;• Powertrain should stay with the microcontroller. Later, create a communication layer between the Raspberry and microcontroller (e.g., using uProtocol). It could be bidirectional.&lt;br&gt; 3. Understanding Qt cross-compilation&lt;br&gt;• Integrating Qt within the DSI display&lt;br&gt;&lt;br&gt;**Next Module Issues:**&lt;br&gt; 1. AGL: research what Yocto is (Linux distribution builder)&lt;br&gt; 2. CAN communication: use CAN, not CAN-FD&lt;br&gt; 3. CAN communication setup: RASP → MF2515 → CAN cable → MF2515 → STM32&lt;br&gt; 4. I2C/SPI/GPIO libraries: research and use Eclipse uProtocol&lt;br&gt; 5. Meetings: TSF → Nov 6; ThreadX → Nov 4/11/14 (TBD); Architecture Blueprints Eclipse → TBD; OpenHardware Workshop → TBD |
		| 28/10/2025 | Rubén **SPRINT PLANNING EXECUTION STARTING POINT**                   | Continue with **Sprint planning execution**:&lt;br&gt; 1 - Architecture (software + hardware): AGL, ThreadX, CanBus, uProtocol&lt;br&gt; 2 - Remove expansion board and link motors directly to microcontroller&lt;br&gt; 3 - Implement TSF on GitHub&lt;br&gt; 4- Qt and cross-compilation&lt;br&gt; 5 - Study next module and link to current one&lt;br&gt; 6 - Engineering process refinement &lt;br&gt; &lt;br&gt; **Daily goals** &lt;br&gt; &lt;br&gt; 1 - Surpass Car blocker &lt;br&gt; 2 - Continue QT crosscompilation &lt;br&gt; 3 - Continue TSF implementation &lt;br&gt; 4 - Update Daily &lt;br&gt; | In progress/To be done, no issues                                                                                    | **BLOCKERS:**&lt;br&gt; 1. Waiting for Victor’s authorization for GitHub Actions and PRs&lt;br&gt; 2. Car doesn’t work: possible software problem&lt;br&gt;&lt;br&gt;**REMINDERS:**&lt;br&gt;&lt;br&gt;**Missing** &lt;br&gt; 1. Shop still missing: headers, original Raspberry charger €34 5V/5A, longer DSI cables, multiple microHDMI adapters, Raspberry Pi 5 heatsinks/fans&lt;br&gt;&lt;br&gt; **Pending** &lt;br&gt; •TSF with implementation blocker on score •Qt crosscompilation blocker, making it not possible to compile&lt;br&gt;• GitHub Actions on standby&lt;br&gt;&lt;br&gt;**Suggestions:**&lt;br&gt; 1. Removing expansion board&lt;br&gt; 2. Power management: research and plan system design (Battery Management System + Buck-Boost Regulator + Power Distribution Board with independent fuses)&lt;br&gt;• Powertrain should stay with the microcontroller. Later, create a communication layer between the Raspberry and microcontroller (e.g., using uProtocol). It could be bidirectional.&lt;br&gt; 3. Understanding Qt cross-compilation&lt;br&gt;• Integrating Qt within the DSI display&lt;br&gt;&lt;br&gt;**Next Module Issues:**&lt;br&gt; 1. AGL: research what Yocto is (Linux distribution builder)&lt;br&gt; 2. CAN communication: use CAN, not CAN-FD&lt;br&gt; 3. CAN communication setup: RASP → MF2515 → CAN cable → MF2515 → STM32&lt;br&gt; 4. I2C/SPI/GPIO libraries: research and use Eclipse uProtocol&lt;br&gt; 5. Meetings: TSF → Nov 6; ThreadX → Nov 4/11/14 (TBD); Architecture Blueprints Eclipse → TBD; OpenHardware Workshop → TBD |
		| 29/10/2025 | Diogo **STANDUP MEETING**  | Continue with **Sprint planning execution:**&lt;br&gt; 1 - Architecture (software + hardware): AGL, ThreadX, CanBus, uProtocol&lt;br&gt; 2 - Remove expansion board and link motors directly to microcontroller&lt;br&gt; 3 - Implement TSF on GitHub&lt;br&gt; 4 - Qt and cross-compilation&lt;br&gt; 5 - Study next module and link to current one&lt;br&gt; 6 - Engineering process refinement &lt;br&gt; &lt;br&gt; **Daily goals** &lt;br&gt; &lt;br&gt; 1 - Surpass Car blocker &lt;br&gt; 2 - Continue QT crosscompilation &lt;br&gt; 3 - Continue TSF implementation &lt;br&gt; 4 - Weekly Meeting Preparation &lt;br&gt; 5 - Update Daily &lt;br&gt; | In progress/To be done, no issues with ca blocker surpassed                                                                                   |  **BLOCKERS:**&lt;br&gt; 1. Waiting for Victor’s authorization for GitHub Actions and PRs&lt;br&gt;&lt;br&gt;**REMINDERS:**&lt;br&gt;&lt;br&gt;**Missing** &lt;br&gt; 1. Shop still missing: headers, original Raspberry charger €34 5V/5A, longer DSI cables, multiple microHDMI adapters, Raspberry Pi 5 heatsinks/fans&lt;br&gt;&lt;br&gt; **Pending** &lt;br&gt; •TSF with implementation blocker on score •Qt crosscompilation blocker, making it not possible to compile&lt;br&gt;• GitHub Actions on standby&lt;br&gt;&lt;br&gt;**Suggestions:**&lt;br&gt; 1. Removing expansion board&lt;br&gt; 2. Power management: research and plan system design (Battery Management System + Buck-Boost Regulator + Power Distribution Board with independent fuses)&lt;br&gt;• Powertrain should stay with the microcontroller. Later, create a communication layer between the Raspberry and microcontroller (e.g., using uProtocol). It could be bidirectional.&lt;br&gt; 3. Understanding Qt cross-compilation&lt;br&gt;• Integrating Qt within the DSI display&lt;br&gt;&lt;br&gt;**Weekly Meeting Notes:**&lt;br&gt;1. Git (reviews, rule sets, etc.): already discussed with Victor but still waiting for his availability&lt;br&gt;2. Delivery date of the items: there is no expected delivery date, they advised to find ways to workaround in the meantime&lt;br&gt;3. Presenting ideas: Before presenting an idea, it is extremely necessary to have everything well planned, studied and beeing convicted of what we are doing &lt;br&gt;4. STM and uProtocol: It was suggested by Pedro that when we start implementing those connections, to keep it simple with a basic interaction, only after that it is needed to implemente uProtocol (do not take this as an absolute truth, whatever we decide is ok)&lt;br&gt;&lt;br&gt;**Next Module Issues:**&lt;br&gt; 1. AGL: research what Yocto is (Linux distribution builder)&lt;br&gt; 2. CAN communication: use CAN, not CAN-FD&lt;br&gt; 3. CAN communication setup: RASP → MF2515 → CAN cable → MF2515 → STM32&lt;br&gt; 4. I2C/SPI/GPIO libraries: research and use Eclipse uProtocol&lt;br&gt; 5. Meetings: TSF → Nov 6; ThreadX → Nov 4/11/14 (TBD); Architecture Blueprints Eclipse → TBD; OpenHardware Workshop → TBD |
		| 30/10/2025 | David | &lt;br&gt; &gt;**Daily goals&gt;** &lt;br&gt; &lt;br&gt; 1 - Solve the car blocker &lt;br&gt; 2 - Continue Qt cross-compilation &lt;br&gt; 3 - Continue TSF implementation &lt;br&gt; 4 - Update daily &lt;br&gt;&lt;br&gt; | &gt;**Done:&gt;** &lt;br&gt; - Car blocker surpassed: main motors are running &lt;br&gt; - Qt app cross-compilation successful — window opens on PiRacer display&lt;br&gt;4 - Update daily &lt;br&gt;&lt;br&gt;&gt;**In progress:&gt;**&lt;br&gt; - Continuing architecture documentation and TSF traceability setup | **BLOCKERS:**&lt;br&gt; 1. Waiting for Victor’s authorization for GitHub Actions and PRs&lt;br&gt;&lt;br&gt;**REMINDERS:**&lt;br&gt;&lt;br&gt;**Missing** &lt;br&gt; 1. Shop still missing: headers, original Raspberry charger €34 5V/5A, longer DSI cables, multiple microHDMI adapters, Raspberry Pi 5 heatsinks/fans&lt;br&gt;&lt;br&gt; **Pending** &lt;br&gt; •TSF with implementation blocker on score •Qt crosscompilation blocker, making it not possible to compile&lt;br&gt;• GitHub Actions on standby&lt;br&gt;&lt;br&gt;**Meeting With Paulo Notes** &lt;br&gt; Feedback from Paulo for the proposal of removing the expansion board:&lt;br&gt;• The Raspberry Pi requires 5A, but the current power output only provides 2A.&lt;br&gt;• The Matek XT60 BAC input needs a step-down converter.&lt;br&gt;• To what extent is the BMS necessary? Perhaps only for charging the batteries without disconnecting them?&lt;br&gt;• The TB6612 also needs power supply, not only 12V — are we relying on STM power via I2C and current?&lt;br&gt;• It’s possible to power two components using the same step-down converter.&lt;br&gt;• The batteries are currently in an open circuit — review the wiring.&lt;br&gt;• Is the Matek XT60 actually beneficial?&lt;br&gt; - We might achieve the same result by feeding current directly from the batteries and using step-downs for each component.&lt;br&gt; - Since two components can share one step-down, study this further.&lt;br&gt; - Investigate connectors with multiple (e.g., five) outputs.&lt;br&gt;• Study the overall current distribution carefully.&lt;br&gt;&lt;br&gt;&gt;**Next Module Issues:&gt;**&lt;br&gt; 1. AGL: research what Yocto is (Linux distribution builder)&lt;br&gt; 2. CAN communication: use CAN, not CAN-FD&lt;br&gt; 3. CAN setup: Raspberry → MF2515 → CAN cable → MF2515 → STM32&lt;br&gt; 4. Research and use Eclipse uProtocol for I2C/SPI/GPIO abstraction&lt;br&gt; 5. Upcoming meetings: TSF → Nov 6; ThreadX → Nov 4/11/14 (TBD); Architecture Blueprints Eclipse → TBD; OpenHardware Workshop → TBD |
		| 31/10/2025 | Vasco | &lt;br&gt; **Daily goals** &lt;br&gt; 1 - Continue Qt cross-compilation &lt;br&gt; 2 - Continue TSF implementation &lt;br&gt;3 - study a CPP script to be able to implement it&lt;br&gt;4 - finalize consumption study of the car &lt;br&gt;5 - finalize 3D model of the car &lt;br&gt;6 - Study CanBus &lt;br&gt;8 - adapt cross compiler &lt;br&gt;9 - Change from QtWidgets to QtQuick&lt;br&gt; 10 - Update daily &lt;br&gt; | **Done:**1 - Qt cross-compilation done &lt;br&gt; 4 - finalized consumption study of the car &lt;br&gt; 8 - adapt cross compiler &lt;br&gt; 9 - Changed from QtWidgets to QtQuick&lt;br&gt; 10 - Update daily &lt;br&gt;**In progress/To be done:**&lt;br&gt; 2- Continuing architecture documentation and TSF traceability setup &lt;br&gt; 3 - study a CPP script to be able to implement it  - finalize 3D model of the car &lt;br&gt;6 - Study CanBus &lt;br&gt; | **BLOCKERS:**&lt;br&gt; 1. Waiting for Victor’s authorization for GitHub Actions and PRs &lt;br&gt;&lt;br&gt;**REMINDERS:**&lt;br&gt;&lt;br&gt; **Missing** &lt;br&gt; 1. Shop still missing: headers, original Raspberry charger €34 5V/5A, longer DSI cables, multiple microHDMI adapters, Raspberry Pi 5 heatsinks/fans&lt;br&gt;&lt;br&gt; **Pending** &lt;br&gt; •TSF with implementation blocker on score •Qt crosscompilation blocker, making it not possible to compile&lt;br&gt;• GitHub Actions on standby&lt;br&gt;&lt;br&gt;**Next Module Issues:**&lt;br&gt; 1. AGL: research what Yocto is (Linux distribution builder)&lt;br&gt; 2. CAN communication: use CAN, not CAN-FD&lt;br&gt; 3. CAN setup: Raspberry → MF2515 → CAN cable → MF2515 → STM32&lt;br&gt; 4. Research and use Eclipse uProtocol for I2C/SPI/GPIO abstraction&lt;br&gt; 5. Upcoming meetings: TSF → Nov 6&lt;br&gt; Daily goals &lt;br&gt;&lt;br&gt; 6 - Continue TSF implementation &lt;br&gt; 7 - Update daily &lt;br&gt;; ThreadX → Nov 11/11/14 (TBD); Architecture Blueprints Eclipse → TBD |
		| 03/11/2025 | Joao | Continue with **Sprint planning execution:**&lt;br&gt; 1 - Architecture (software + hardware): AGL, ThreadX, CanBus, uProtocol&lt;br&gt; 2 - Remove expansion board and link motors directly to microcontroller&lt;br&gt; 3 - Implement TSF on GitHub&lt;br&gt; 4 - Qt and cross-compilation&lt;br&gt; 5 - Study next module and link to current one&lt;br&gt; 6 - Engineering process refinement &lt;br&gt; &lt;br&gt; **Daily goals** &lt;br&gt; 1 - Finalize configuration of wireless DSI &lt;br&gt; 2 - finalize 3D model of the car &lt;br&gt;3 - Study a CPP script to be able to implement it&lt;br&gt; 4 - Finish Documentation about QT/QML and Cross Compiling, and UI Design, and Autostart &lt;br&gt; 5 - Study CanBus &lt;br&gt; &lt;br&gt;6 - Continue TSF implementation and Github automatations&lt;br&gt; 7 - Update daily &lt;br&gt; | **Done:** &lt;br&gt; 4 - Finish Documentation about QT/QML and Cross Compiling&lt;br&gt;7 - Update daily &lt;br&gt;**In progress/To be done:** 1 - Finalize configuration of wireless DSI &lt;br&gt; 2 - finalize 3D model of the car &lt;br&gt; 3-Study a CPP script to be able to implement it &lt;br&gt; 4 - UI Design, and Autostart &lt;br&gt; 6 - Continue TSF implementation and Github automatations&lt;br&gt;| **BLOCKERS:**&lt;br&gt; 1. Waiting for Victor’s authorization for GitHub Actions and PRs &lt;br&gt;&lt;br&gt;**REMINDERS:**&lt;br&gt;&lt;br&gt; **Missing** &lt;br&gt; 1. Shop still missing: headers, original Raspberry charger €34 5V/5A, longer DSI cables, multiple microHDMI adapters, Raspberry Pi 5 heatsinks/fans&lt;br&gt;&lt;br&gt; **Pending** &lt;br&gt; •TSF with implementation blocker on score •Qt crosscompilation blocker, making it not possible to compile&lt;br&gt;• GitHub Actions on standby&lt;br&gt;&lt;br&gt;**Next Module Issues:**&lt;br&gt; 1. AGL: research what Yocto is (Linux distribution builder)&lt;br&gt; 2. CAN communication: use CAN, not CAN-FD&lt;br&gt; 3. CAN setup: Raspberry → MF2515 → CAN cable → MF2515 → STM32&lt;br&gt; 4. Research and use Eclipse uProtocol for I2C/SPI/GPIO abstraction&lt;br&gt; 5. Upcoming meetings: TSF → Nov 6&lt;br&gt; Daily goals &lt;br&gt;&lt;br&gt; 6 - Continue TSF implementation &lt;br&gt; 7 - Update daily &lt;br&gt;; ThreadX → Nov 11/11/14 (TBD); Architecture Blueprints Eclipse → TBD |
		| 04/11/2025 | Joao | Continue with **Sprint planning execution:**&lt;br&gt; 1 - Architecture (software + hardware): AGL, ThreadX, CanBus, uProtocol&lt;br&gt; 2 - Remove expansion board and link motors directly to microcontroller&lt;br&gt; 3 - Implement TSF on GitHub&lt;br&gt; 4 - Qt and cross-compilation&lt;br&gt; 5 - Study next module and link to current one&lt;br&gt; 6 - Engineering process refinement &lt;br&gt; &lt;br&gt; **Daily goals** &lt;br&gt; 1 - Finalize configuration of wireless DSI &lt;br&gt; 2 - finalize 3D model of the car &lt;br&gt;3 - Study a CPP script to be able to implement it and make a presentation of threadX and AGL to next retrospective meeting at 7/11/2025 &lt;br&gt; 4 - Finish Documentation about QT/QML and Cross Compiling, and UI Design, and Autostart &lt;br&gt; 5 - Study CanBus  &lt;br&gt;6 - Continue TSF implementation and Github automatations&lt;br&gt; 7 - Update daily &lt;br&gt; | **Done:** 1 - Finalize configuration of wireless DSI &lt;br&gt; 4 - Finish Documentation about QT/QML and Cross Compiling&lt;br&gt; 5 - Study CanBus  &lt;br&gt;7 - Update daily &lt;br&gt;**In progress/To be done:** 2 - finalize 3D model of the car &lt;br&gt; 3-Study a CPP script to be able to implement it&lt;br&gt; 4 - UI Design, and Autostart &lt;br&gt; 6 - Continue TSF implementation and Github automatations&lt;br&gt;|  **BLOCKERS:**&lt;br&gt;&lt;br&gt; 1. Waiting for Victor’s authorization for GitHub Actions and PRs&lt;br&gt;&lt;br&gt;**REMINDERS:**&lt;br&gt;&lt;br&gt; **Missing** &lt;br&gt; 1. Shop still missing: headers, original Raspberry charger €34 5V/5A, longer DSI cables, multiple microHDMI adapters, Raspberry Pi 5 heatsinks/fans&lt;br&gt;&lt;br&gt; **Pending** &lt;br&gt; •TSF with implementation blocker on score •Qt crosscompilation blocker, making it not possible to compile&lt;br&gt;• GitHub Actions on standby&lt;br&gt;&lt;br&gt;**Next Module Issues:**&lt;br&gt; 1. AGL: research what Yocto is (Linux distribution builder)&lt;br&gt; 2. CAN communication: use CAN, not CAN-FD&lt;br&gt; 3. CAN setup: Raspberry → MF2515 → CAN cable → MF2515 → STM32&lt;br&gt; 4. Research and use Eclipse uProtocol for I2C/SPI/GPIO abstraction&lt;br&gt; 5. Upcoming meetings: TSF → Nov 6&lt;br&gt; Daily goals &lt;br&gt;&lt;br&gt; 6 - Continue TSF implementation &lt;br&gt; 7 - Update daily &lt;br&gt;; ThreadX → Nov 11/11/14 (TBD); Architecture Blueprints Eclipse → TBD |
		| 05/11/2025 | Joao **STANDUP MEETING** | **Daily goals** &lt;br&gt; 1 - Installation of AGL &lt;br&gt; 2 - Finalize 3D model of the car &lt;br&gt;3 - Implement a CPP script to be able to implement it &lt;br&gt; 4 Make documentation of threadX and AGL &lt;br&gt; 5 - Complete Documentation with the introduction of new documentation about QT/QML and Cross Compiling, and UI Design, and Autostart &lt;br&gt; 6 - Install Microcontroller IDE and try to put it working to link CanBus after it &lt;br&gt; 7 - Continue TSF implementation and Github automatations&lt;br&gt; 8 - Update daily &lt;br&gt; | **Done:** &lt;br&gt; 5 - Study CanBus&lt;br&gt; 6 - Daily Standup Meeting &lt;br&gt; 8 - Update daily&lt;br&gt;&lt;br&gt;**In progress/To be done:** Everything else still in Progress/To be done &lt;br&gt;|  **BLOCKERS:**&lt;br&gt; 1. Waiting for Victor’s authorization for GitHub Actions and PRs&lt;br&gt;&lt;br&gt;**REMINDERS:**&lt;br&gt;&lt;br&gt; **Missing** &lt;br&gt; 1. Shop still missing: headers, original Raspberry charger €34 5V/5A, longer DSI cables, multiple microHDMI adapters, Raspberry Pi 5 heatsinks/fans&lt;br&gt;&lt;br&gt; **Pending** &lt;br&gt; •TSF with implementation blocker on score •Qt crosscompilation blocker SURPASSED with docker solution, making it possible to compile&lt;br&gt;• GitHub Actions on standby&lt;br&gt; •AGL installation with some issues regarding versions and dependencies&lt;br&gt;&lt;br&gt;**Standup with Paulo Meeting Notes** &lt;br&gt; So, basically, in yesterday's meeting: - Paulo approved the scheme that Ruben made, as well as the other mentor - they liked the idea of using Docker for cross-compilation (they just told us not to build Qt because it would take too long) - when we look at AGL, we'll probably have a problem with the SDK keys for cross-compilation, I think - QTquick opens on Rasp5, so it's compatible - they said that the ideal would be to always get an executable to the Rasp of the cluster &lt;br&gt; **Next Module Issues:**&lt;br&gt; 1. AGL: research what Yocto is (Linux distribution builder)&lt;br&gt; 2. CAN communication: use CAN, not CAN-FD&lt;br&gt; 3. CAN setup: Raspberry → MF2515 → CAN cable → MF2515 → STM32&lt;br&gt; 4. Research and use Eclipse uProtocol for I2C/SPI/GPIO abstraction&lt;br&gt; 5. Upcoming meetings: TSF → Nov 6&lt;br&gt; Daily goals &lt;br&gt;&lt;br&gt; 6 - Continue TSF implementation &lt;br&gt; 7 - Update daily &lt;br&gt;; ThreadX → Nov 11/11/14 (TBD); Architecture Blueprints Eclipse → TBD |
		| 06/11/2025 | Joao **SPRINT RETROSPECTIVE** | **Daily goals** &lt;br&gt; 1 - Continue Installation of AGL &lt;br&gt; 2 - Finalize 3D model of the car &lt;br&gt;3 - Implement a CPP script to be able to implement it &lt;br&gt; 4 Make documentation of threadX and AGL &lt;br&gt;5 - Complete Documentation with the introduction of new documentation about QT/QML and Cross Compiling, and UI Design, and Autostart &lt;br&gt; 6 - Install Microcontroller IDE and try to put it working to link CanBus after it &lt;br&gt; &lt;br&gt; 7 - Continue TSF implementation and Github automatations&lt;br&gt;8 - Prepare Presentation on SPRINT RETROSPECTIVE and present it &lt;br&gt;9 - Update daily &lt;br&gt; | **Done:** &lt;br&gt; &lt;br&gt; 8 - Sprint Retrospective Done &lt;br&gt; 9 - Update daily&lt;br&gt;&lt;br&gt;**In progress/To be done:** 1 - Continue Installation of AGL &lt;br&gt; 2 - Finalize 3D model of the car &lt;br&gt; &lt;br&gt;3 - Implement a CPP script to be able to implement it &lt;br&gt; 4 Make documentation of threadX and AGL &lt;br&gt;5 - Complete Documentation with the introduction of new documentation about QT/QML and Cross Compiling, and UI Design, and Autostart &lt;br&gt; 6 - Install Microcontroller IDE and try to put it working to link CanBus after it &lt;br&gt; 7 - Continue TSF implementation and Github automatations&lt;br&gt;&lt;br&gt;|  **BLOCKERS:**&lt;br&gt;&lt;br&gt; 1. Waiting for Victor’s authorization for GitHub Actions and PRs&lt;br&gt;&lt;br&gt;**REMINDERS:**&lt;br&gt;&lt;br&gt;**Missing** &lt;br&gt; 1. Shop still missing: headers, original Raspberry charger €34 5V/5A, longer DSI cables, multiple microHDMI adapters, Raspberry Pi 5 heatsinks/fans&lt;br&gt;&lt;br&gt; **Pending** &lt;br&gt; •TSF with implementation blocker on score •Qt crosscompilation blocker SURPASSED with docker solution, making it possible to compile&lt;br&gt;• GitHub Actions on standby&lt;br&gt; •AGL installation with some issues regarding versions and dependencies&lt;br&gt;&lt;br&gt;**Next Module Issues:**&lt;br&gt; 1. AGL: research what Yocto is (Linux distribution builder)&lt;br&gt; 2. CAN communication: use CAN, not CAN-FD&lt;br&gt; 3. CAN setup: Raspberry → MF2515 → CAN cable → MF2515 → STM32&lt;br&gt; 4. Research and use Eclipse uProtocol for I2C/SPI/GPIO abstraction&lt;br&gt; 5. Upcoming meetings: TSF → Nov 6&lt;br&gt; Daily goals &lt;br&gt;&lt;br&gt; 6 - Continue TSF implementation &lt;br&gt; 7 - Update daily &lt;br&gt;; ThreadX → Nov 11/11/14 (TBD); Architecture Blueprints Eclipse → TBD |
		| 10/11/2025 | Joao **SPRINT PLANNING** | **Daily goals** &lt;br&gt; 1 - Continue Installation of AGL &lt;br&gt; 2 - Finalize 3D model of the car &lt;br&gt;3 - Implement a CPP script to be able to implement it &lt;br&gt; 4 Make documentation of threadX and AGL &lt;br&gt;5 - Complete Documentation with the introduction of new documentation about QT/QML and Cross Compiling, and UI Design, and Autostart &lt;br&gt; 6 - Install Microcontroller IDE and try to put it working to link CanBus after it &lt;br&gt; &lt;br&gt; 7 - Continue TSF implementation and Github automatations&lt;br&gt;8 - Prepare Sprint Planning Presentation and present it &lt;br&gt;9 - Update daily &lt;br&gt; | **Done:** &lt;br&gt; &lt;br&gt; 8 - Sprint Planning Done &lt;br&gt; 9 - Update daily&lt;br&gt;&lt;br&gt;**In progress/To be done:** 1 - Continue Installation of AGL &lt;br&gt; 2 - Finalize 3D model of the car &lt;br&gt;3 - Implement a CPP script to be able to implement it &lt;br&gt; 4 Make documentation of threadX and AGL &lt;br&gt;5 - Complete Documentation with the introduction of new documentation about QT/QML and Cross Compiling, and UI Design, and Autostart &lt;br&gt; 6 - Install Microcontroller IDE  andtry to connect CanBus and obtain output signal after it &lt;br&gt; 7 - Continue TSF implementation and Github automatations&lt;br&gt;&lt;br&gt;|  **BLOCKERS:**&lt;br&gt;&lt;br&gt; 1. Waiting for Victor’s authorization for GitHub Actions and PRs&lt;br&gt;&lt;br&gt;**REMINDERS:**&lt;br&gt;&lt;br&gt;**Missing** &lt;br&gt; 1. Shop still missing: headers, longer DSI cables, multiple microHDMI adapters, Raspberry Pi 5 heatsinks/fans&lt;br&gt;&lt;br&gt; **Pending** &lt;br&gt;&lt;br&gt;***Sprint Planning Meeting Notes:** &lt;br&gt; 1. Cross-compilation discussion (Pedro & Paulo) &lt;br&gt; 2. Hardware completeness discussion (Pedro & Paulo) &lt;br&gt; 3. Sprint considered ambitious (ThreadX + CAN + AGL) &lt;br&gt;&lt;br&gt;  **Next Module Issues:**&lt;br&gt; 1. AGL: research what Yocto is (Linux distribution builder)&lt;br&gt; 2. CAN communication: use CAN, not CAN-FD&lt;br&gt; 3. CAN setup: Raspberry → MF2515 → CAN cable → MF2515 → STM32&lt;br&gt; 4. Research and use Eclipse uProtocol for I2C/SPI/GPIO abstraction&lt;br&gt; 5. Upcoming meetings: TSF → Nov 6&lt;br&gt; Daily goals &lt;br&gt; 6 - Continue TSF implementation &lt;br&gt; 7 - Update daily &lt;br&gt;; ThreadX → Nov 11/11/14 (TBD); Architecture Blueprints Eclipse → TBD |
		| **11/11/2025** | **Joao — SPRINT PLANNING EXECUTION STARTING POINT** | &lt;br&gt;&lt;br&gt; **Sprint Goal:** Implement Automotive Grade Linux (AGL) and ThreadX RTOS environments, and establish CAN communication between the microcontroller, sensors, and the Raspberry Pi.&lt;br&gt;&lt;br&gt; **Epic — Car Software Architecture (#52)** &lt;br&gt; 1 - Install and configure AGL on the Raspberry Pi &lt;br&gt; 2 - Install and test ThreadX RTOS on the STM32 &lt;br&gt; 3 - Establish CANbus communication between microcontroller and Raspberry Pi &lt;br&gt; 4 - Configure auto-start of the Qt cluster app on AGL (migrated from Bookworm) &lt;br&gt;&lt;br&gt; **Epic — Car Hardware Architecture (#53)** &lt;br&gt; - Test the new hardware layout without the expansion board &lt;br&gt; - Validate power distribution and component behavior under load &lt;br&gt;&lt;br&gt; **Epic — Documentation (#54)** &lt;br&gt; - Approve and apply documentation tutorials &lt;br&gt; - Review and approve documentation templates and structure &lt;br&gt;&lt;br&gt; **Epic — Project Management & Traceability Refinement (#55)** &lt;br&gt; - Refine TSF implementation and traceability setup in GitHub &lt;br&gt; - Improve GitHub Actions (after granted permission) automations &lt;br&gt; - Continue study of GenAI and Hailo accelerator integration &lt;br&gt;&lt;br&gt; **Epic — Study and Integration of Core Technologies (#56)** &lt;br&gt; - Study COVESA uServices for future integration &lt;br&gt; - Study uProtocol for data exchange between ECUs &lt;br&gt;&lt;br&gt; **Daily Goals:** &lt;br&gt; 1. Finalize Installation of AGL &lt;br&gt; 2. Finalize 3D model of the car &lt;br&gt; 3. Implement a CPP script &lt;br&gt; 4. Complete Documentation (Qt/QML, Cross Compiling, UI Design, Autostart) &lt;br&gt; 5. Install Microcontroller IDE and link CanBus &lt;br&gt; 6. Continue TSF implementation and GitHub automations &lt;br&gt; 7. Turn repository public and test branch protection & workflows &lt;br&gt; 8. Start AI study (ML, DL, GenAI) &lt;br&gt; 9. Update daily | **Done:** &lt;br&gt; 1. Finalize Installation of AGL &lt;br&gt; 4. Document ThreadX and AGL &lt;br&gt; 9. Update daily &lt;br&gt;&lt;br&gt; **In Progress / To Be Done:** &lt;br&gt; 2. Finalize 3D model of the car &lt;br&gt; 3. Implement CPP script &lt;br&gt; 4. Complete Documentation (Qt/QML, Cross Compiling, UI Design, Autostart) &lt;br&gt; 5. Install Microcontroller IDE and link CanBus &lt;br&gt; 6. Continue TSF implementation and GitHub automations &lt;br&gt; 7. Turn repository public and test workflows &lt;br&gt; 8. Start AI study (ML, DL, GenAI) | **BLOCKERS:** &lt;br&gt; 1. Check if we need  Victor’s authorization for GitHub Actions and PRs &lt;br&gt;&lt;br&gt; **REMINDERS**&lt;br&gt;&lt;br&gt; **Missing:** &lt;br&gt; - Headers &lt;br&gt; - Longer DSI cables &lt;br&gt; - Multiple microHDMI adapters &lt;br&gt; - Raspberry Pi 5 heatsinks/fans &lt;br&gt;&lt;br&gt; **Pending:** &lt;br&gt;&lt;br&gt; **Next Module Issues:** &lt;br&gt; 1. AGL: research what Yocto is (Linux distribution builder) &lt;br&gt; 2. CAN communication: use CAN, not CAN-FD &lt;br&gt; 3. CAN setup: Raspberry → MF2515 → CAN cable → MF2515 → STM32 &lt;br&gt; 4. Research Eclipse uProtocol for I2C/SPI/GPIO abstraction &lt;br&gt; 5. Upcoming meetings: TSF → Nov 6; ThreadX → Nov 11/14 (TBD); Architecture Blueprints Eclipse → TBD |
		| **12/11/2025** | **Joao** &lt;br&gt;&lt;br&gt; | **Daily Goals:** &lt;br&gt; 1. Test AGL installation &lt;br&gt; 2. Finalize 3D model of the car &lt;br&gt; 3. Implement a CPP script &lt;br&gt; 4. Complete Documentation (Qt/QML, Cross Compiling, UI Design, Autostart) &lt;br&gt; 5. Confirm Microcontroller ↔ CanBus communication &lt;br&gt; 6. Continue TSF implementation and GitHub automations &lt;br&gt; 7. Turn repository public and test branch protection & workflows &lt;br&gt; 8. Start AI study (ML, DL, GenAI) &lt;br&gt; 9. Update daily | **Done:** &lt;br&gt; 9. Update daily &lt;br&gt;&lt;br&gt; **In Progress / To Be Done:** &lt;br&gt; Everything else | **BLOCKERS:** &lt;br&gt; Check if we need  Victor’s authorization for GitHub Actions and PRs  &lt;br&gt;&lt;br&gt; **REMINDERS**&lt;br&gt;&lt;br&gt;**Missing:** &lt;br&gt; - Headers &lt;br&gt; - Longer DSI cables &lt;br&gt; - Multiple microHDMI adapters &lt;br&gt; - Raspberry Pi 5 heatsinks/fans &lt;br&gt;&lt;br&gt; **Pending:** &lt;br&gt;&lt;br&gt;  **Standup with Paulo Meeting Notes** 1 - ALG didn´t boot. The answer was to disable uboot; 2 - Can Controller question about using Can controller or Can hat. Answer was Can Hat is better because its thinner, the cable are and we don´t need to make cable connections once it connects directly using the pins that connect directly with the pins of the Rasp5; 3 - They recommended to make dailys  **Next Module Issues:** &lt;br&gt; 1. AGL: research what Yocto is (Linux distribution builder) &lt;br&gt; 2. CAN communication: use CAN, not CAN-FD &lt;br&gt; 3. CAN setup: Raspberry → MF2515 → CAN cable → MF2515 → STM32 &lt;br&gt; 4. Research and use Eclipse uProtocol for I2C/SPI/GPIO abstraction &lt;br&gt; 5. Upcoming meetings: TSF → Nov 6; ThreadX → Nov 11/14 (TBD); Architecture Blueprints Eclipse → TBD |
		| **13/11/2025** | **Joao** &lt;br&gt;&lt;br&gt; | Continued implementation of the items mentioned on the previous day | In Progress / To be Done | Equal to 12/11/2025 |
		| **14/11/2025** | **Joao** &lt;br&gt;&lt;br&gt; | Continued implementation of the items mentioned on the previous day | In Progress / To be Done | Equal to 12/11/2025 |
		| **17/11/2025** | **Joao** &lt;br&gt;&lt;br&gt; | **Daily Goals:** &lt;br&gt; 1. Test AGL installation &lt;br&gt; 2. Finalize 3D model of the car &lt;br&gt; 3. Implement a CPP script &lt;br&gt; 4. Complete Documentation (Qt/QML, Cross Compiling, UI Design, Autostart) &lt;br&gt; 5. Confirm Microcontroller ↔ CanBus communication &lt;br&gt; 6. Continue TSF implementation and GitHub automations &lt;br&gt; 7. Turn repository public and test branch protection & workflows &lt;br&gt; 8. Start AI study (ML, DL, GenAI) &lt;br&gt; 9. Meeting regarding TSF questions - "ECLIPSE TSF Q&A questions" &lt;br&gt; 10. Thread X Installations and test &lt;br&gt; 11. Update daily | **Done:** &lt;br&gt; 1. Test AGL installation &lt;br&gt; 2. Finalize 3D model of the car &lt;br&gt; 11. Update daily &lt;br&gt;&lt;br&gt; **In Progress / To Be Done:** &lt;br&gt; Everything else | **BLOCKERS:** &lt;br&gt; Check if we need  Victor’s authorization for GitHub Actions and PRs  &lt;br&gt;&lt;br&gt; **REMINDERS**&lt;br&gt;&lt;br&gt;**Missing:** &lt;br&gt; - Headers &lt;br&gt; - Longer DSI cables &lt;br&gt; - Multiple microHDMI adapters &lt;br&gt; - Raspberry Pi 5 heatsinks/fans &lt;br&gt;&lt;br&gt; **Pending:** &lt;br&gt;&lt;br&gt;  **Next Module Issues:** &lt;br&gt; 1. AGL: research what Yocto is (Linux distribution builder) &lt;br&gt; 2. CAN communication: use CAN, not CAN-FD &lt;br&gt; 3. CAN setup: Raspberry → MF2515 → CAN cable → MF2515 → STM32 &lt;br&gt; 4. Research and use Eclipse uProtocol for I2C/SPI/GPIO abstraction &lt;br&gt; 5. Upcoming meetings: TSF → Nov 6; ThreadX → Nov 11/14 (TBD); Architecture Blueprints Eclipse → TBD |
		| **18/11/2025** | **Joao** &lt;br&gt;&lt;br&gt; | Continued implementation of the items mentioned on the previous day **Victory of the day** - 10. Thread X Installations and test DONE| In Progress / To be Done | Equal to 17/11/2025 |
		| **19/11/2025** | **Joao** | Continued implementation of the items mentioned on the previous day&lt;br&gt;**Victory of today** - unidirectional communication between STM32 and Raspberry Pi | In Progress / To be Done | **Standup Meeting (Paulo) Notes**&lt;br&gt;**Team Updates:**&lt;br&gt;- Group 2: blockage in wheel connection via I²C — unable to establish communication.&lt;br&gt;- DrivaPi group: difficulties booting from SSD; Filipe will assist; if we have the same problem, coordinate with them.&lt;br&gt;**Workflow & GitHub:** suggestion to run a linter (e.g., clang-tidy) only on changed code in PRs via GitHub Actions.&lt;br&gt;Keep issues well-described and linked; tag Filipe, Paulo or Pedro in issues.&lt;br&gt;Keep the GitHub board updated: To Do → In Progress → Done. Suggest changing views to iterations.&lt;br&gt;**Technical Development:** ThreadX must ensure control of motor, servos, sensor and CAN; some teams facing difficulties.&lt;br&gt;Filipe will ask Pete A. Zouk for another example of a TSF implementation 🎉 |
		| **20/11/2025** | **Joao** &lt;br&gt;&lt;br&gt; | Continued implementation of the items mentioned on the previous day **Victory of today** - bidirectional comunication betwen STM 32 e Rasp | In Progress / To be Done | Equal to 17/11/2025 |
		| **21/11/2025** | **Joao** &lt;br&gt;&lt;br&gt; | Continued implementation of the items mentioned on the previous day **Victory of today** - QT opens on display without  | In Progress / To be Done | Equal to 17/11/2025 |
		| **24/11/2025** | **Joao — SPRINT PLANNING** | &lt;br&gt;&lt;br&gt; **Sprint Goal:** Assemble and validate the new system's hardware & Update team's knowledge documentation foundations.&lt;br&gt;&lt;br&gt; **Epic — Team Knowledge Update/Status Point (#54)** &lt;br&gt; - Approve and apply documentation tutorials &lt;br&gt; - Review and approve documentation templates and structure &lt;br&gt; - Spread knowledge among team &lt;br&gt;&lt;br&gt; **Epic — Project Management & Traceability Refinement (#55)** &lt;br&gt; - Project Management Overview &lt;br&gt;&lt;br&gt; **Epic — Car Hardware Architecture (#53)** &lt;br&gt; - Assemble Vehicle Layout &lt;br&gt; - Test New Power Distribution and Validate It &lt;br&gt; - Component Testing: Servo motors ↔ Microcontroller, DC motor ↔ Microcontroller &lt;br&gt;&lt;br&gt; **Epic — Car Software Architecture (#52)** &lt;br&gt; - Remote display auto-start Qt app &lt;br&gt; - Test and validate motors with script &lt;br&gt;&lt;br&gt; **Daily Goals:** &lt;br&gt; 1. Remote display auto-start Qt app: Establish communication of remote display of the Qt app &lt;br&gt; 2. TSF github improvement &lt;br&gt; 3. ThreadX spike: Research and investigate ThreadX RTOS capabilities &lt;br&gt; 4. QT design improvements: Enhance cluster interface design | **Done:** &lt;br&gt; 1. Remote display auto-start Qt app: Establish communication of remote display of the Qt app &lt;br&gt; 2. TSF github improvement &lt;br&gt; 3. ThreadX spike: Research and investigate ThreadX RTOS capabilities &lt;br&gt; 4. QT design improvements: Enhance cluster interface design &lt;br&gt;&lt;br&gt; **In Progress / To Be Done:** &lt;br&gt; Continue with Sprint planning execution | **BLOCKERS:** &lt;br&gt; Weak/intermittent internet connection &lt;br&gt;&lt;br&gt; **REMINDERS**&lt;br&gt;&lt;br&gt; **Missing:** &lt;br&gt; - Headers &lt;br&gt; - Longer DSI cables &lt;br&gt; - Multiple microHDMI adapters &lt;br&gt; - Raspberry Pi 5 heatsinks/fans &lt;br&gt;&lt;br&gt; **Pending:** |
		| **25/11/2025** | **Joao — SPRINT PLANNING EXECUTION STARTING POINT** | **Daily Goals:** &lt;br&gt; 1. Assemble Vehicle Layout: Assemble and document the complete hardware and software system architecture. Verify the correct physical connections, module placement, and data flow between Raspberry Pi, STM32, and other components &lt;br&gt; 2. TSF github improvement &lt;br&gt; 3. ThreadX spike continuation: Continue researching ThreadX RTOS implementation &lt;br&gt; 4. QT design improvements: Continue enhancing cluster interface | **Done:** &lt;br&gt; 1. Assemble Vehicle Layout: Assemble and document the complete hardware and software system architecture. Verify the correct physical connections, module placement, and data flow between Raspberry Pi, STM32, and other components &lt;br&gt; 2. TSF github improvement &lt;br&gt; 3. ThreadX spike continuation: Continue researching ThreadX RTOS implementation &lt;br&gt; 4. QT design improvements: Continue enhancing cluster interface &lt;br&gt;&lt;br&gt; **In Progress / To Be Done:** &lt;br&gt; Continue with Sprint planning execution | **BLOCKERS:** &lt;br&gt; Weak/intermittent internet connection &lt;br&gt;&lt;br&gt; **REMINDERS**&lt;br&gt;&lt;br&gt; **Missing:** &lt;br&gt; - Headers &lt;br&gt; - Longer DSI cables &lt;br&gt; - Multiple microHDMI adapters &lt;br&gt; - Raspberry Pi 5 heatsinks/fans &lt;br&gt;&lt;br&gt; **Pending:** |
		| **26/11/2025** | **Joao — STANDUP MEETING** | **Daily Goals:** &lt;br&gt; 1. Component Testing with script: DC and servo motors, checking and validating control signal and response and testing speed and direction control through PWM or driver module &lt;br&gt; 2. Improvement in QT cluster design &lt;br&gt; 3. TSF github improvement &lt;br&gt; 4. ThreadX spike continuation: Continue researching ThreadX RTOS implementation | **Done:** &lt;br&gt; 1. Component Testing with script: DC and servo motors, checking and validating control signal and response and testing speed and direction control through PWM or driver module &lt;br&gt; 2. Improvement in QT cluster design &lt;br&gt; 3. TSF github improvement &lt;br&gt; 4. ThreadX spike continuation: Continue researching ThreadX RTOS implementation &lt;br&gt;&lt;br&gt; **Victory of today:** First motor tests executed successfully &lt;br&gt;&lt;br&gt; **In Progress / To Be Done:** &lt;br&gt; Continue with Sprint planning execution | **BLOCKERS:** &lt;br&gt; Weak/intermittent internet connection &lt;br&gt;&lt;br&gt; **REMINDERS**&lt;br&gt;&lt;br&gt; **Missing:** &lt;br&gt; - Headers &lt;br&gt; - Longer DSI cables &lt;br&gt; - Multiple microHDMI adapters &lt;br&gt; - Raspberry Pi 5 heatsinks/fans &lt;br&gt;&lt;br&gt; **Pending:** &lt;br&gt;&lt;br&gt; **Standup Meeting Notes:** &lt;br&gt; **Team Updates:** &lt;br&gt; - Team 6: Electrical system is assembled but without any tests yet &lt;br&gt; - Team 1: Reported to Filipe that only 2 members were working in the group &lt;br&gt;&lt;br&gt; **Workflow & GitHub:** &lt;br&gt; - We were told that we need to do Unit tests (no performance or scalability tests) &lt;br&gt; - Also implement automatic and manual tests for integration testing &lt;br&gt;&lt;br&gt; **Technical Development:** &lt;br&gt; - Paulo suggested to start looking at unit tests as they will start asking questions about it in meetings &lt;br&gt; - Paulo also asked if we had already done any kind of tests with code on our new electrical system, to which I said no |
		| **27/11/2025** | **Joao** | **Daily Goals:** &lt;br&gt; 1. Approve and apply documentation tutorials: Follow the existing setup guides step by step to validate their accuracy and clarity, updating them if needed &lt;br&gt; 2. TSF github improvement &lt;br&gt; 3. ThreadX spike continuation: Continue researching ThreadX RTOS implementation &lt;br&gt; 4. QT design improvements: Continue enhancing cluster interface | **Done:** &lt;br&gt; 1. Approve and apply documentation tutorials: Follow the existing setup guides step by step to validate their accuracy and clarity, updating them if needed &lt;br&gt; 2. TSF github improvement &lt;br&gt; 3. ThreadX spike continuation: Continue researching ThreadX RTOS implementation &lt;br&gt; 4. QT design improvements: Continue enhancing cluster interface &lt;br&gt;&lt;br&gt; **In Progress / To Be Done:** &lt;br&gt; Continue with Sprint planning execution | **BLOCKERS:** &lt;br&gt; Weak/intermittent internet connection &lt;br&gt;&lt;br&gt; **REMINDERS**&lt;br&gt;&lt;br&gt; **Missing:** &lt;br&gt; - Headers &lt;br&gt; - Longer DSI cables &lt;br&gt; - Multiple microHDMI adapters &lt;br&gt; - Raspberry Pi 5 heatsinks/fans &lt;br&gt;&lt;br&gt; **Pending:** |
		| **28/11/2025** | **Joao** | **Daily Goals:** &lt;br&gt; 1. Project Management Overview: Perform an overview of project organization, including branch strategy, GitHub Actions setup, and task management practices. Ensure alignment between sprint planning, documentation, and TSF traceability &lt;br&gt; 2. TSF github improvement | **Done:** &lt;br&gt; 1. Project Management Overview: Perform an overview of project organization, including branch strategy, GitHub Actions setup, and task management practices. Ensure alignment between sprint planning, documentation, and TSF traceability &lt;br&gt; 2. TSF github improvement &lt;br&gt;&lt;br&gt; **In Progress / To Be Done:** &lt;br&gt; Continue with Sprint planning execution | **BLOCKERS:** &lt;br&gt; Weak/intermittent internet connection &lt;br&gt;&lt;br&gt; **REMINDERS**&lt;br&gt;&lt;br&gt; **Missing:** &lt;br&gt; - Headers &lt;br&gt; - Longer DSI cables &lt;br&gt; - Multiple microHDMI adapters &lt;br&gt; - Raspberry Pi 5 heatsinks/fans &lt;br&gt;&lt;br&gt; **Pending:** |
		| **02/12/2025** | **Joao** | **Daily Goals:** &lt;br&gt; 1. First motor tests execution &lt;br&gt; 2. Continue with Sprint execution &lt;br&gt; 3. ThreadX spike continuation: Continue researching ThreadX RTOS implementation &lt;br&gt; 4. QT design improvements: Continue enhancing cluster interface | **Done:** &lt;br&gt; 1. First motor tests - motors worked initially but DC motors stopped without understanding why. Tried to identify the issue &lt;br&gt; 2. ThreadX spike continuation: Continue researching ThreadX RTOS implementation &lt;br&gt; 3. QT design improvements: Continue enhancing cluster interface &lt;br&gt;&lt;br&gt; **In Progress / To Be Done:** &lt;br&gt; Debug DC motor blocker and continue with Sprint execution | **BLOCKERS:** &lt;br&gt; DC motors stopped working without clear reason. Investigating the cause &lt;br&gt;&lt;br&gt; **REMINDERS**&lt;br&gt;&lt;br&gt; **Missing:** &lt;br&gt; - Headers &lt;br&gt; - Longer DSI cables &lt;br&gt; - Multiple microHDMI adapters &lt;br&gt; - Raspberry Pi 5 heatsinks/fans &lt;br&gt;&lt;br&gt; **Pending:** |
		| **03/12/2025** | **Joao — STANDUP MEETING** | **Daily Goals:** &lt;br&gt; 1. Resolve DC motor blocker &lt;br&gt; 2. Continue with Sprint execution &lt;br&gt; 3. ThreadX spike continuation: Continue researching ThreadX RTOS implementation &lt;br&gt; 4. QT design improvements: Continue enhancing cluster interface | **Done:** &lt;br&gt; 1. Standup meeting &lt;br&gt; 2. ThreadX spike continuation: Continue researching ThreadX RTOS implementation &lt;br&gt; 3. QT design improvements: Continue enhancing cluster interface &lt;br&gt;&lt;br&gt; **In Progress / To Be Done:** &lt;br&gt; Continue debugging DC motor issue | **BLOCKERS:** &lt;br&gt; DC motors not working - confirmed I2C communication with STM32 is working but DC motors don't move. Believed to be a hardware issue, not code &lt;br&gt;&lt;br&gt; **REMINDERS**&lt;br&gt;&lt;br&gt; **Missing:** &lt;br&gt; - Headers &lt;br&gt; - Longer DSI cables &lt;br&gt; - Multiple microHDMI adapters &lt;br&gt; - Raspberry Pi 5 heatsinks/fans &lt;br&gt;&lt;br&gt; **Pending:** &lt;br&gt;&lt;br&gt; **Standup Meeting Notes:** &lt;br&gt; **Team Updates:** &lt;br&gt; - Electrical system assembled and servos tested (working). DC motors have a blocker. Connected via I2C with STM32 but DC doesn't move. We believe it's not a code issue &lt;br&gt; - Paulo questioned: Need to confirm we're using correct communication in ACDC, and confirm we're using the correct pins. ACDC is conversion from alternating current to direct current. Plugs have alternating current. Lamps have direct current &lt;br&gt;&lt;br&gt; **Workflow & GitHub:** &lt;br&gt; - Paulo recommended: Make a PR when one of us finishes documentation. A PR that all of us must approve (except the one who made it) after testing it. This way we do documentation review, besides the weekly day (suggested stopping all Wednesdays and not working. Only doing this. Even gave an example of last year's group, which only wanted to move fast and then had to adjust. And the way they found was this) &lt;br&gt;&lt;br&gt; **Technical Development:** &lt;br&gt; - No need to use / there's no advantage in AGL instead of Bookworm RaspOS on Rasp4 for wireless display. QT opens automatically on AGL and the remote monitor (which has a Rasp 4) connects automatically and opens the remote screen in fullscreen too &lt;br&gt; - Paulo commented: Regarding AGL, we can try creating an image in AGL after putting it in Rasp 4, but only if we have time. Otherwise, it's fine as it is &lt;br&gt;&lt;br&gt; Filipe asked about requirements and if we had blockers at that level. Said we didn't and showed him GitHub and TSF requirements |
		| **04/12/2025** | **Joao** | **Daily Goals:** &lt;br&gt; 1. Review and approve documentation templates and structure: Standardize Markdown templates and folder organization to ensure consistent documentation across the project &lt;br&gt; 2. Continue car testing &lt;br&gt; 3. Battery stress test: After having the script, place both motors at maximum (servo turning at maximum angle left and right and DC motor rotating at maximum speed between -255 and 255) | **Done:** &lt;br&gt; 1. Review and approve documentation templates and structure: Standardize Markdown templates and folder organization to ensure consistent documentation across the project &lt;br&gt; 2. Continue car testing &lt;br&gt; 3. Battery stress test completed &lt;br&gt;&lt;br&gt; **Victory of today:** Battery stress test worked! Batteries held for 1 hour running motors non-stop &lt;br&gt;&lt;br&gt; **In Progress / To Be Done:** &lt;br&gt; Continue with Sprint execution | **BLOCKERS:** &lt;br&gt; None &lt;br&gt;&lt;br&gt; **REMINDERS**&lt;br&gt;&lt;br&gt; **Missing:** &lt;br&gt; - Headers &lt;br&gt; - Longer DSI cables &lt;br&gt; - Multiple microHDMI adapters &lt;br&gt; - Raspberry Pi 5 heatsinks/fans &lt;br&gt;&lt;br&gt; **Pending:** |
		| **05/12/2025** | **Joao — SPRINT RETROSPECTIVE** | **Daily Goals:** &lt;br&gt; 1. Continue Review and approve documentation templates and structure &lt;br&gt; 2. Prepare Sprint Retrospective presentation | **Done:** &lt;br&gt; 1. Continue Review and approve documentation templates and structure &lt;br&gt; 2. Sprint Retrospective presentation completed &lt;br&gt;&lt;br&gt; **In Progress / To Be Done:** &lt;br&gt; Finalize documentation review | **BLOCKERS:** &lt;br&gt; None &lt;br&gt;&lt;br&gt; **REMINDERS**&lt;br&gt;&lt;br&gt; **Missing:** &lt;br&gt; - Headers &lt;br&gt; - Longer DSI cables &lt;br&gt; - Multiple microHDMI adapters &lt;br&gt; - Raspberry Pi 5 heatsinks/fans &lt;br&gt;&lt;br&gt; **Pending:** &lt;br&gt;&lt;br&gt; **Sprint Retrospective Notes:** &lt;br&gt; - Paulo commented that we already did "spread knowledge" when we were having difficulties with that &lt;br&gt; - Pedro challenged us to have 2 displays instead of only 1 (one will be the instrument cluster and the other the infotainment) &lt;br&gt; - In Sprint 4 the objective was achieved and all tasks completed (see sprint4.md doc for more info) |
		| **09/12/2025** | **Joao — SPRINT PLANNING** | **Daily Goals:** &lt;br&gt; 1. Sprint 5 Planning: Execute Sprint 5 planning session &lt;br&gt; 2. Research test frameworks: Investigate testing frameworks appropriate for the project &lt;br&gt; 3. Evaluate speedometer inclusion in project scope | **Done:** &lt;br&gt; 1. Sprint 5 Planning completed &lt;br&gt; 2. Research test frameworks initiated &lt;br&gt; 3. Speedometer inclusion evaluated and approved for Sprint 5 &lt;br&gt;&lt;br&gt; **Sprint 5 Goal:** "Establish and Validate our RDAF (Real Data Architecture Flow)" &lt;br&gt;&lt;br&gt; **In Progress / To Be Done:** &lt;br&gt; Begin Sprint 5 execution tasks | **BLOCKERS:** &lt;br&gt; None &lt;br&gt;&lt;br&gt; **REMINDERS**&lt;br&gt;&lt;br&gt; **Missing:** &lt;br&gt; - Headers &lt;br&gt; - Longer DSI cables &lt;br&gt; - Multiple microHDMI adapters &lt;br&gt; - Raspberry Pi 5 heatsinks/fans &lt;br&gt;&lt;br&gt; **Pending:** &lt;br&gt;&lt;br&gt; **Next Module Issues:** &lt;br&gt; Research spikes on COVESA, Kuksa, uProtocol, MQTT, and testing frameworks |
		| **10/12/2025** | **Joao — SPRINT PLANNING EXECUTION STARTING POINT + STANDUP MEETING** | **Daily Goals:** &lt;br&gt; 1. Sprint 5 execution begins: Start implementing Sprint 5 tasks &lt;br&gt; 2. Reinstall AGL and STM32 IDE on new setup &lt;br&gt; 3. Standup meeting with Filipe | **Done:** &lt;br&gt; 1. Sprint 5 execution initiated &lt;br&gt; 2. AGL and STM32 IDE reinstallation completed &lt;br&gt; 3. Standup meeting completed &lt;br&gt;&lt;br&gt; **In Progress / To Be Done:** &lt;br&gt; Continue with Sprint 5 task execution | **BLOCKERS:** &lt;br&gt; None &lt;br&gt;&lt;br&gt; **REMINDERS**&lt;br&gt;&lt;br&gt; **Missing:** &lt;br&gt; - Headers &lt;br&gt; - Longer DSI cables &lt;br&gt; - Multiple microHDMI adapters &lt;br&gt; - Raspberry Pi 5 heatsinks/fans &lt;br&gt;&lt;br&gt; **Pending:** &lt;br&gt;&lt;br&gt; **Standup Meeting Notes with Filipe:** &lt;br&gt; - Discussed Requirements and shown the current state &lt;br&gt; - Testing frameworks: Filipe suggested Google Test (C/C++), Pytest (Python), JUnit (Java), NUnit (C#), RSpec (Ruby) &lt;br&gt; - TraceX: Filipe explained it's a tool for visualizing ThreadX RTOS events. Helps debug and analyze system behavior in real-time &lt;br&gt; - STM32 VSCode Extension: Filipe recommended using STM32 extension for VSCode instead of separate IDE &lt;br&gt;&lt;br&gt; **Technical concepts learned:** &lt;br&gt; - TraceX: Real-time visualization tool for ThreadX RTOS debugging &lt;br&gt; - Testing frameworks overview: Different frameworks for different languages &lt;br&gt; - ST-Link Flash: Programming interface for STM32 microcontrollers &lt;br&gt; - TSN (Time-Sensitive Networking): Ethernet extension for deterministic real-time communication &lt;br&gt; - RT-Linux: Real-time variant of Linux kernel &lt;br&gt; - QEMU: Machine emulator and virtualizer &lt;br&gt; - Cortex-R vs Cortex-M33: Different ARM processor cores (R for real-time, M for microcontrollers) &lt;br&gt; - Safety critical primitives: Essential components for safety-critical systems &lt;br&gt; - Eclipse SDV Hackathon: Software-Defined Vehicle development initiative &lt;br&gt; - Ferrosystems: Company specializing in embedded safety-critical systems &lt;br&gt; - uProtocol layers: Architecture layers in Eclipse uProtocol specification &lt;br&gt;&lt;br&gt; **Next Module Issues:** &lt;br&gt; Research spikes on COVESA, Kuksa, uProtocol, MQTT, and testing frameworks |
		| **11/12/2025** | **Joao** | **Daily Goals:** &lt;br&gt; 1. ThreadX tests with new STM project &lt;br&gt; 2. CAN communication establishment between Raspberry Pi and STM32 &lt;br&gt; 3. Research spikes on COVESA, Kuksa, uProtocol, MQTT, and testing frameworks | **Done:** &lt;br&gt; 1. ThreadX tests with new STM project initiated &lt;br&gt; 2. CAN communication establishment work started &lt;br&gt;&lt;br&gt; **In Progress / To Be Done:** &lt;br&gt; Continue CAN communication integration and ThreadX implementation &lt;br&gt; Continue research spikes on COVESA, Kuksa, uProtocol, MQTT, and testing frameworks | **BLOCKERS:** &lt;br&gt; None &lt;br&gt;&lt;br&gt; **REMINDERS**&lt;br&gt;&lt;br&gt; **Missing:** &lt;br&gt; - Headers &lt;br&gt; - Longer DSI cables &lt;br&gt; - Multiple microHDMI adapters &lt;br&gt; - Raspberry Pi 5 heatsinks/fans &lt;br&gt;&lt;br&gt; **Pending:** &lt;br&gt;&lt;br&gt; **Next Module Issues:** &lt;br&gt; Research spikes on COVESA, Kuksa, uProtocol, MQTT, and testing frameworks |
		| **12/12/2025** | **Joao** | **Daily Goals:** &lt;br&gt; 1. Continue ThreadX and CAN communication work &lt;br&gt; 2. Continue testing framework research &lt;br&gt; 3. Complete research spikes on COVESA, Kuksa, uProtocol, MQTT, and testing frameworks | **Done:** &lt;br&gt; 1. Continued ThreadX implementation and CAN communication debugging &lt;br&gt; 2. Continued testing framework research &lt;br&gt; 3. Research spikes completed on COVESA, Kuksa, uProtocol, MQTT, and testing frameworks &lt;br&gt;&lt;br&gt; **In Progress / To Be Done:** &lt;br&gt; Finalize CAN communication integration and complete testing framework evaluation | **BLOCKERS:** &lt;br&gt; None &lt;br&gt;&lt;br&gt; **REMINDERS**&lt;br&gt;&lt;br&gt; **Missing:** &lt;br&gt; - Headers &lt;br&gt; - Longer DSI cables &lt;br&gt; - Multiple microHDMI adapters &lt;br&gt; - Raspberry Pi 5 heatsinks/fans &lt;br&gt;&lt;br&gt; **Pending:** &lt;br&gt;&lt;br&gt; **Next Module Issues:** &lt;br&gt; None - Research spikes completed |
		| **15/12/2025** | **Joao** | **Daily Goals:** &lt;br&gt; 1. Spread Knowledge presentations: Present research findings on COVESA/Kuksa/uProtocol/MQTT &lt;br&gt; 2. Spread Knowledge presentations: Present research findings on testing types and frameworks | **Done:** &lt;br&gt; 1. Spread Knowledge presentation on COVESA/Kuksa/uProtocol/MQTT completed &lt;br&gt; 2. Spread Knowledge presentation on testing types and frameworks completed &lt;br&gt;&lt;br&gt; **Victory of today:** Successfully shared knowledge with team on automotive software architecture and testing strategies &lt;br&gt;&lt;br&gt; **In Progress / To Be Done:** &lt;br&gt; Continue with Sprint 5 execution | **BLOCKERS:** &lt;br&gt; None &lt;br&gt;&lt;br&gt; **REMINDERS**&lt;br&gt;&lt;br&gt; **Missing:** &lt;br&gt; - Headers &lt;br&gt; - Longer DSI cables &lt;br&gt; - Multiple microHDMI adapters &lt;br&gt; - Raspberry Pi 5 heatsinks/fans &lt;br&gt;&lt;br&gt; **Pending:** &lt;br&gt;&lt;br&gt; **Presentations Summary:** &lt;br&gt; - COVESA/Kuksa/uProtocol/MQTT: Covered Eclipse Kuksa databroker, uProtocol communication specification, MQTT pub/sub patterns, and COVESA VSS (Vehicle Signal Specification) &lt;br&gt; - Testing types/frameworks: Covered unit testing, integration testing, system testing, acceptance testing, and framework comparisons (Google Test, Pytest, JUnit, etc.) &lt;br&gt;&lt;br&gt; **Next Module Issues:** &lt;br&gt; None - Research spikes completed |
		````


- `docs/sprints/sprint1.md`

	??? "Click to view reference"

		````md
		# 🏁 Sprint 1 — (13/10/2025 → 24/10/2025)
		
		&gt; **Brief summary**: By the end of this sprint, we hope to know the components of the PiRacer and have it assembled, create documentation and implement Agile methodologies and TSF, and create a Qt App including a mockup design of the cluster.
		
		---
		
		## 📌 Epics
		- [x] **Epic 1** — Hardware Assembly - [Epic 1 - #2](https://github.com/orgs/SEAME-pt/projects/89/views/1?filterQuery=label%3A%22Type%3A+Epic%22&pane=issue&itemId=133634028&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C2)
		- [x] **Epic 2** — Software Setup - [Epic 2 - #6](https://github.com/orgs/SEAME-pt/projects/89/views/1?filterQuery=label%3A%22Type%3A+Epic%22&pane=issue&itemId=133636567&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C6)
		- [ ] **Epic 3** — Development of Qt App - [Epic 3 - #10](https://github.com/orgs/SEAME-pt/projects/89/views/1?filterQuery=label%3A%22Type%3A+Epic%22&pane=issue&itemId=133637985&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C10)
		- [ ] **Epic 4** — Project & GitHub setup - [Epic 4 - #13](https://github.com/orgs/SEAME-pt/projects/89/views/1?filterQuery=label%3A%22Type%3A+Epic%22&pane=issue&itemId=133639290&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C13)
		- [x] **Epic 5** — TSF & Documentation - [Epic 5 - #18](https://github.com/orgs/SEAME-pt/projects/89/views/1?filterQuery=label%3A%22Type%3A+Epic%22&pane=issue&itemId=133640653&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C18)
		
		---
		
		## 📈 Actual Progress
		- **Completed tasks**: `14 / 16` (**87.5%**) 
		
		- ✅ Done: #3, #4, #5, #7, #8, #9, #11, #14, #15, #17, #19, #20, #21, #27  
		- ⏳ Blocked: #2, #12
		
		---
		
		| Date       | Daily Facilitator | Daily Objectives                                                                                                                                                                                                                                                                                                                                              | What was done                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         | Blockers                                                                                                                                                                                                                                                                                                                                                                                                         | Next steps / What is missing                                                                                                                                                                                                                                                                                                                                                   |
		| :--------- | :---------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
		| 2025-10-16 | **Team**          | 1️⃣ Integrate and test main hardware components (Raspberry Pi, Hailo Hat, DSI, CSI, Gamepad)&lt;br&gt;2️⃣ Clarify logistics and technical doubts during 16h meeting&lt;br&gt;3️⃣ Create Pull Request and Daily templates&lt;br&gt;4️⃣ Continue TSF implementation on GitHub&lt;br&gt;5️⃣ Analyze GitHub Actions&lt;br&gt;6️⃣ Continue Qt Application development&lt;br&gt;7️⃣ Update sprint board | ✅ Set almost fully assembled and tested:&lt;br&gt;• Raspberry Pi OS (Bookworm) installed&lt;br&gt;• Hailo Hat 8 functional ([rpicam-apps](https://github.com/raspberrypi/rpicam-apps))&lt;br&gt;• CSI camera working with Python script ([hailo-rpi5-examples](https://github.com/hailo-ai/hailo-rpi5-examples))&lt;br&gt;• DSI display functional&lt;br&gt;• Gamepad and dongle connected and working&lt;br&gt;✅ PR, Daily & Taskly templates created&lt;br&gt;🧩 TSF implementation started&lt;br&gt;⏸ GitHub Actions in standby&lt;br&gt;🚧 Qt Application and board updates in progress | - Missing components (headers, 5V/5A charger, DSI cables, HDMI adapters, heatsinks)&lt;br&gt;- Possible Wi-Fi config issues on Raspberry Pi&lt;br&gt;- Validation dependency with David & Filipe&lt;br&gt;- Pending decision: AGL vs ThreadX vs FreeRTOS vs Azure RTOS&lt;br&gt;- Large tasks need splitting into subtasks/epics&lt;br&gt;- Confirm TSF traceability matrix naming and usage&lt;br&gt;- Permissions for PR and Actions still pending | 1️⃣ Connect all components to expansion board and test full set&lt;br&gt;2️⃣ Prepare inventory of missing components → send to Filipe&lt;br&gt;3️⃣ Validate PR/Daily/Taskly templates (@joaocp @daviduartecf)&lt;br&gt;4️⃣ Continue TSF implementation on GitHub&lt;br&gt;5️⃣ Continue GitHub Actions analysis&lt;br&gt;6️⃣ Continue Qt Application development&lt;br&gt;7️⃣ Update board with current task status |
		| 2025-10-17 | **Team**          | 1️⃣ Assemble car (set connection on expansion board)&lt;br&gt;2️⃣ Prepare inventory and send to Filipe&lt;br&gt;3️⃣ Validate PR/Daily/Taskly templates (@joaocp @daviduartecf)&lt;br&gt;4️⃣ Continue TSF implementation&lt;br&gt;5️⃣ Analyze GitHub Actions&lt;br&gt;6️⃣ Develop Qt Application&lt;br&gt;7️⃣ Update board                                                                         | Work pending from previous day — assembly and validation not yet done                                                                                                                                                                                                                                                                                                                                                                                                                                                                 | - Hardware integration incomplete&lt;br&gt;- Pending inventory validation&lt;br&gt;- TSF and Qt in development&lt;br&gt;- GitHub Actions on hold                                                                                                                                                                                                                                                                                   | 1️⃣ Complete hardware integration and testing&lt;br&gt;2️⃣ Submit inventory to Filipe&lt;br&gt;3️⃣ Validate templates&lt;br&gt;4️⃣ Continue TSF development&lt;br&gt;5️⃣ Continue Qt Application&lt;br&gt;6️⃣ Resume GitHub Actions testing&lt;br&gt;7️⃣ Update board                                                                                                                                              |
		| 2025-10-20 | **Team**          | 1️⃣ Connect Raspberry Pi + Hailo + Camera + Display&lt;br&gt;2️⃣ Create GitHub guidelines and Actions&lt;br&gt;3️⃣ Study cluster design&lt;br&gt;4️⃣ Implement Action to download Taskly                                                                                                                                                                                        | ⚙️ Verified camera + display setup&lt;br&gt;✅ Cluster mockup completed&lt;br&gt;✅ GitHub and Actions guides created                                                                                                                                                                                                                                                                                                                                                                                                                               | - Unable to run JetRacer script on current OS                                                                                                                                                                                                                                                                                                                                                                    | 1️⃣ TSF requirements + traceability matrix (@jpjpcs)&lt;br&gt;2️⃣ Continue cluster design study (@souzitaaaa)&lt;br&gt;3️⃣ Test JetRacer script on Trixie SD (@rcosta-c @vasferre)&lt;br&gt;4️⃣ Create Sprint Summary + Daily Docs (@daviduartecf)                                                                                                                                               |
		
		---
		
		## ✅ Outcomes
		- **Delivered**:
		EXPECT-L0-3 - "Software Setup capable of running Qt applications":
		EXPECT-L0-5 - "Launch basic Qt on built-in display":
		    - Qt app code in [Qt app](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/src/cluster)
		
		    
		- **Demos**:
		  
		**ARCHITECTURE**:
		EXPECT-L0-1 - "System architecture is defined and documented
		      - [Assembled car](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/assembled-car1.jpeg)
		    - [Assembled car2](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/assembled-car2.jpeg)
		EXPECT-L0-4 - "Driveable Car with Remote Control"
		    - [Car running](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Remote%20control%201%20evidence_EXPECT_L0_4.mp4)
		    - [Car running](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Remote%20control%202%20evidence_EXPECT_L0_4.mp4)
		
		
		**QT**:
		  EXPECT-L0-5 - "Launch basic Qt on built-in display":
		    - [QtApp](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/first_qt_app.jpeg)
		  EXPECT-L0-6 - "The Qt GUI shall display static graphical elements";
		  - [mockup1](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup1.jpeg)
		    - [mockup2](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup2.jpeg)
		    - [mockup3](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup3.jpeg)
		    - [mockup4](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup4.jpeg)
		    - [mockup5](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup5.jpeg)
		    - [mockup6](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup6.jpeg)
		               
		      
		- **Docs/Guides updated**:
		
		
		**GITHUB**
		
		  EXPECT-L0-13 - "Project & GitHub setup":
		- [Github guidelines](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/github-guidelines.md)
		- [Github guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/github-actions-guide.md)
		- [CI/CD Workflow](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/.github/workflows/validate_items_formatation.yml)
		
		**AGILE SCRUM**
		EXPECT-L0-15 - "Agile and Scrum":
		- [Scrum Implementation: Dailys](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/dailys/dailys_warmups.md)
		
		- [Scrum Implementation: Project Board](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/projects?query=is%3Aopen)
		
		---
		
		## 🔎 Retrospective
		- **Went well**:
		  - We were able to do **14** of the **16** tasks of this sprint! (**87.5%**)
		  - We did research about the next steps
		  - The team communicated well and was able to solve problems together
		- **To improve**:
		  - Improved Agile task managment:
		      - Implement Priority level in tasks
		      - Add Time estimates to tasks for better planning
		- **What we didn't do**:
		  - Auto-start of Qt app on boot
		  - Complete GitHub setup (branch protection - reviewers)
		 
		## 🧭 **Action Items for Next Sprint**
		- Implement auto-start for Qt app  
		- Improve sprint planning templates (add estimated time & priority)
		
		
		---
		
		## 🔗 Useful Links
		- Sprint board/view: [Sprint 1](https://github.com/orgs/SEAME-pt/projects/89/views/1)
		
		## TSF Useful: 
		  EXPECT-L0-1 - "System architecture is defined and documented":
		  EXPECT-L0-2 - "Hardware Assembly: Fully assemble the PiRacer robot":
		  EXPECT-L0-3 - "Software Setup capable of running Qt applications":
		  EXPECT-L0-4 - "Driveable Car with Remote Control":
		  EXPECT-L0-5 - "Launch basic Qt on built-in display":
		  EXPECT-L0-6 - "The Qt GUI shall display static graphical elements":
		  EXPECT-L0-7 - "QT Crosscompilation should be achieved":
		  EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)":
		  EXPECT-L0-9 - "ThreadX running on the Microcontroller (STM32) as a RTOS";
		  EXPECT-L0-10 - "Implement new architecture.":
		  EXPECT-L0-11 - "QT cluster UI **auto-start on boot** on display":
		  EXPECT-L0-12 - "Wireless DCI display":
		  EXPECT-L0-13 - "Project & GitHub setup":
		  EXPECT-L0-14 - "TSF & Documentation":
		  EXPECT-L0-15 - "Agile and Scrum":
		  EXPECT-L0-16 - "CI/CD":
		  EXPECT-L0-17 - "Introduce AI in the project":
		
		````


- `docs/sprints/sprint2.md`

	??? "Click to view reference"

		````md
		# 🏁 Sprint 2 — (27/10/2025 → 07/11/2025)
		
		&gt; **Sprint Goal**: Refine the project setup - Hardware layout + Software architecture - in preparation of the implementation of automotive software 
		
		---
		
		## 📌 Epics
		- [x] **Epic** — Definition of Car Architecture - Software & Hardware - [Epic 7 - #34](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/34)
		- [x] **Epic** — Study Technologies - [Epic 8 - #35](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/35)
		- [x] **Epic** — Qt App - Cluster Instrument - [Epic 9 - #36](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/36)
		- [ ] **Epic** — Implement TSF in Github - [Epic 6 - #33](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/33)
		- [x] **Epic** — Engineering Process Refinement - [Epic 10 - #37](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/37)
		
		---
		
		## 📈 Actual Progress
		
		- ✅ Done:
		  - **Hardware layout defined** — 3D model of the car + energy consumption scheme (expansion board removed).
		  - **Software architecture defined** — studies & documentation on AGL, ThreadX, and CANbus.
		  - **Cluster Instrument** — Qt app cross-compiled and **auto-starts on boot** (PiRacer).
		  - **TSF** — deep study of Trustable Software Framework; baseline structure prepared.
		  - **Documentation** — team produced thorough docs covering steps, decisions, and processes.
		
		- ⏳ Blocked:
		  - GitHub privileges / protection rules (PRs, Actions) pending confirmation.
		  - Waiting for hardware items (on the way) to fullfill our hardware layout. (Update - arrived today!)
		
		---
		
		## ✅ Outcomes
		- **Delivered**:
		  - Car **hardware layout** and **3D model**.
		  - **Energy/power scheme** with expansion board removal plan.
		  - **Architecture baseline** (AGL + ThreadX + CAN).
		  - **Qt cluster app** artifact that **boots automatically** on the display.
		  - TSF groundwork (structure + studies).
		
		- **Demos**:
		
		**ARCHITECTURE**
		
		EXPECT-L0-1 - "System architecture is defined and documented":
		  - Energy consumption plan
		  - ![Eletric-Scheme](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/eletric-scheme.png)
		
		**HARDWARE**
		
		EXPECT-L0-2 - "Hardware Assembly: Fully assemble the PiRacer robot":
		  - 3D model of the car
		  - ![3d-car](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/3d-car.jpg)
		  - ![3d-car2](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/3d-car2.jpg)
		  - ![3d-car3](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/3d-car3.jpg)
		
		**QT:** 
		
		EXPECT-L0-5 - "Launch basic Qt on built-in display"
		  - Qt app showing on display
		  - ![Qt_app](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/first_qt_app.jpeg)
		
		EXPECT-L0-7 - "QT Crosscompilation should be achieved":
		  - Crosscompilation scheme
		  - ![Crosscompilation](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/system-architecture-cross.png)
		
		**CAN:** 
		
		EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)":
		  - CANbus communication scheme
		  - ![CANScheme](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/CANbus%20Scheme%20Updated.png)
		
		
		- **Docs/Guides updated**:
		
		
		**ARCHITECTURE**
		
		EXPECT-L0-1 - "System architecture is defined and documented":
		  - Power consuption analysis - [power-analysis-doc](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Power%20Consumption%20Analysis.md)
		
		**SOFTWARE**
		
		EXPECT-L0-3 - "Software Setup capable of running Qt applications":
		  - AGL guide - [agl-guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/AGL_Installation_Guide.md)
		  
		**QT:**
		
		EXPECT-L0-7 - "QT Crosscompilation should be achieved":
		  - Crosscompilation guide - [crosscompiling-guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/src/cross-compiler)
		
		EXPECT-L0-3 - "Software Setup capable of running Qt applications":
		EXPECT-L0-5 - "Launch basic Qt on built-in display":
		  - Cluster (Qt app) guide - [cluster-guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/src/cluster)
		
		**CAN:** 
		
		EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)":
		- CAN guide - [can-guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/CAN-overview.md)
		  
		**TSF** (Trustable software framework): 
		
		EXPECT-L0-14 - "TSF & Documentation":
		  - TSF guide - [tsf-guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/WhatsTSF.md)
		
		
		
		---
		
		## 🔎 Retrospective
		- **Went well**:
		  - Clear division of work; solid documentation culture.
		  - All of the **high priority** goals completed ✅
		  - Sprint goal completed ✅
		  - AGL installed on Raspberry Pi - **bonus objective!**
		
		- **To improve**:
		  - Spikes showcase - presenting the study done to the team.
		  - Enhance our working methodology - **continuous work**.
		
		## 🔗 Useful Links
		- Sprint board/view: [Sprint 2](https://github.com/orgs/SEAME-pt/projects/89/views/7)
		
		
		## TSF Useful: 
		  EXPECT-L0-1 - "System architecture is defined and documented":
		  EXPECT-L0-2 - "Hardware Assembly: Fully assemble the PiRacer robot":
		  EXPECT-L0-3 - "Software Setup capable of running Qt applications":
		  EXPECT-L0-4 - "Driveable Car with Remote Control":
		  EXPECT-L0-5 - "Launch basic Qt on built-in display":
		  EXPECT-L0-6 - "The Qt GUI shall display static graphical elements":
		  EXPECT-L0-7 - "QT Crosscompilation should be achieved":
		  EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)":
		  EXPECT-L0-9 - "ThreadX running on the Microcontroller (STM32) as a RTOS";
		  EXPECT-L0-10 - "Implement new architecture.":
		  EXPECT-L0-11 - "QT cluster UI **auto-start on boot** on display":
		  EXPECT-L0-12 - "Wireless DCI display":
		  EXPECT-L0-13 - "Project & GitHub setup":
		  EXPECT-L0-14 - "TSF & Documentation":
		  EXPECT-L0-15 - "Agile and Scrum":
		  EXPECT-L0-16 - "CI/CD":
		  EXPECT-L0-17 - "Introduce AI in the project":
		
		````


- `docs/sprints/sprint3.md`

	??? "Click to view reference"

		````md
		# 🏁 Sprint 3 — (10/11/2025 → 21/11/2025)
		
		&gt; ## **Sprint Goal**: Implement Automotive Grade Linux (AGL) and ThreadX RTOS environments, and establish CAN communication between the microcontroller, sensors, and the Raspberry Pi.
		
		---
		
		## 📌 Epics
		- #### **Epic** — Car Software Architecture - [#52](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315398&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C52)
		  - [x] Install and configure **AGL** on the Raspberry Pi 
		  - [x] Install and test **ThreadX RTOS** on the STM32
		  - [x] Establish **CANbus communication** between microcontroller and Raspberry Pi 
		  - [x] Configure **auto-start of the Qt cluster app** on AGL (migrated from Bookworm)
		- #### **Epic** — Car Hardware Architecture - [#53](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315665&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C53)
		  - [x] Test the **new hardware layout** without the expansion board 
		  - [x] Validate power distribution and component behavior under load
		- #### **Epic** — Documentation - [#54](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315799&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C54)
		  - [ ] Approve and apply **documentation tutorials** 
		  - [ ] Review and approve **documentation templates and structure**
		- #### **Epic** — Project Management & Traceability Refinement - [#55](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315926&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C55)
		  - [x] Refine **TSF implementation** and traceability setup in GitHub 
		  - [ ] Continue the **study of GenAI** and its potential application with the Hailo accelerator
		- #### **Epic** — Study and Integration of Core Technologies - [#56](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138316020&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C56)
		  - [x] Study **COVESA** uServices for future integration
		  - [x] Study **uProtocol** for data exchange between ECUs 
		
		
		---
		
		## 📈 Actual Progress
		
		- ✅ Done: Sprint goal achieved! 🎉
		
		- ❌ Undone: Some spikes and review documentation tutorials.
		
		---
		
		## ✅ Outcomes
		- **Delivered**:
		  - Hardware layout validated (component test - not "under load" test) ✅
		  - Running Qt app on AGL ✅
		  - CAN communication validated ✅
		  - ThreadX running on STM32 
		
		  
		- **Demos**:
		
		**ARCHITECTURE**
		
		EXPECT-L0-1 - "System architecture is defined and documented":
		EXPECT-L0-10 - "Implement new architecture.":
		  - Validation of energy supply for Raspberry Pi 5
		  - ![Rasp energy validation sprint1](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint1.jpeg)
		  - ![Rasp energy validation sprint2](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint2.jpeg)
		  - ![Rasp energy validation](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint3.jpeg)
		
		**QT**
		
		EXPECT-L0-6 - "The Qt GUI shall display static graphical elements":
		  - Cluster updates
		  - ![Atual state cluster](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Cluster-display-sprint3.jpeg)
		  - ![Cluster mockup](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Cluster-mockup-sprint3.jpeg)
		  - ![Cluster study](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Cluster-study.jpeg)
		  - [UI Design README](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/ui_design/README.md)
		
		  **CAN**
		  
		EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)":
		- CAN layout
		    - ![CAN layout](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/CAN-layout-sprint3.jpeg) 
		
		
		
		      
		- **Docs/Guides updated**:
		
		  **SOFTWARE**
		  
		EXPECT-L0-3 - "Software Setup capable of running Qt applications":
		  - [AGL Guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/AGL_Installation_Guide.md)
		EXPECT-L0-9 - "ThreadX running on the Microcontroller (STM32) as a RTOS";
		  - [ThreadX Guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/ThreadX_Installation_Guide.md
		EXPECT-L0-3 - "Software Setup capable of running Qt applications":
		EXPECT-L0-9 - "ThreadX running on the Microcontroller (STM32) as a RTOS";
		  - [AGL VS ThreadX ]https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/AGL%20and%20ThreadX%20benefits%20Guide.md
		
		**QT**
		
		EXPECT-L0-5 - "Launch basic Qt on built-in display":
		  - [QML guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/QML-guide.md)
		  - [Cluster Instrument](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/cluster/README.md)
		  
		**CAN**
		
		EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)":
		  - [CAN implementation](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/CAN-test-guide.md)
		- [CAN explanation](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/CAN-explanation.md)
		
		**TSF**
		
		EXPECT-L0-14 - "TSF & Documentation":
		  - [TSF Guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/WhatsTSF.md)
		
		**JOYSTICK**
		
		EXPECT-L0-4 - "Driveable Car with Remote Control":
		  - [Joystick on AGL](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Install%20and%20Test%20Joystick%20in%20AGL.md)
		
		
		- **Images**:
		
		**CAN**
		
		  EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)":
		  - [CANFrame image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/images/CANFrame.png) 
		- [CAN Signal](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/images/CANsignal.png)
		- [CAN Topology](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/images/CANtopology.png)
		- [CAN Topology 2](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/images/CANtopology2.png)
		- [CAN half-duplex-communication](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/images/Half-duplex-communication.png)
		
		
		---
		
		## 🔎 Retrospective
		- **Went well**:
		  - Team is focused in the **sprint goal**
		
		- **To improve**: 
		  - Reserve at least **1 full day** in the sprint only for documentation review and approval.
		
		## 🔗 Useful Links
		- Sprint board/view: [Sprint 3](https://github.com/orgs/SEAME-pt/projects/89/views/8?visibleFields=%5B%22Title%22%2C%22Assignees%22%2C%22Status%22%2C%22Linked+pull+requests%22%2C%22Sub-issues+progress%22%5D&sortedBy%5Bdirection%5D=&sortedBy%5BcolumnId%5D=)
		
		## TSF Useful: 
		  EXPECT-L0-1 - "System architecture is defined and documented":
		  EXPECT-L0-2 - "Hardware Assembly: Fully assemble the PiRacer robot":
		  EXPECT-L0-3 - "Software Setup capable of running Qt applications":
		  EXPECT-L0-4 - "Driveable Car with Remote Control":
		  EXPECT-L0-5 - "Launch basic Qt on built-in display":
		  EXPECT-L0-6 - "The Qt GUI shall display static graphical elements":
		  EXPECT-L0-7 - "QT Crosscompilation should be achieved":
		  EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)":
		  EXPECT-L0-9 - "ThreadX running on the Microcontroller (STM32) as a RTOS";
		  EXPECT-L0-10 - "Implement new architecture.":
		  EXPECT-L0-11 - "QT cluster UI **auto-start on boot** on display":
		  EXPECT-L0-12 - "Wireless DCI display":
		  EXPECT-L0-13 - "Project & GitHub setup":
		  EXPECT-L0-14 - "TSF & Documentation":
		  EXPECT-L0-15 - "Agile and Scrum":
		  EXPECT-L0-16 - "CI/CD":
		  EXPECT-L0-17 - "Introduce AI in the project":
		  
		
		````


- `docs/sprints/sprint4.md`

	??? "Click to view reference"

		````md
		# 🏁 Sprint 4 — (24/11/2025 → 05/12/2025)
		
		&gt; ## **Sprint Goal**: Assemble and validate the new system’s hardware & Update team's knowledge documentation foundations.
		
		---
		
		## 📌 Epics
		- #### **Epic** — Team Knowlegde Update/Status Point - [#54](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315799&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C54)
		  - [x] Approve and apply **documentation tutorials** 
		  - [x] Review and approve **documentation templates and structure**
		  - [x] Spread knowledge among team
		- #### **Epic** — Project Management & Traceability Refinement - [#55](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315926&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C55)
		  - [x] Project Management Overview 
		- #### **Epic** — Car Hardware Architecture - [#53](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315665&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C53)
		  - [x] Assemble Vehicle Layout
		  - [x] Test New Power Distribution and Validate It
		  - [x] Component Testing: 
		  - [x]  **Servo motors ↔ Microcontroller**
		  - [x]  **DC motor ↔ Microcontroller**
		- #### **Epic** — Car Software Architecture - [#52](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315398&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C52)
		  - [x] Remote display auto-start Qt app
		  - [x] Test and validate motors with script
		
		---
		
		## 📈 Actual Progress
		
		- ## ✅ Done: Sprint goal completed!! New layout architecture implemented successfully!! 🎉
		- ## ✅ Sprint 4 points: 44/44
		
		- ❌ Undone: 
		---
		
		## ✅ Outcomes
		
		- **Delivered**:
		  - Hardware architecture validated! (Under stress - all components on)
		  - CAN communication test done!
		  - Display with auto-start running.
		 
		- **Demos**:
		  - EXPECT-L0-10 - "Implement new architecture.":
		  - EXPECT-L0-11 - "QT cluster UI **auto-start on boot** on display":
		  - EXPECT-L0-12 - "Wireless DCI display":
		  - https://github.com/user-attachments/assets/e47dfee6-8d20-4a2b-88ba-0ed2be615f46
		  - EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)":
		  - ![can_communication_test](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/CAN-test.gif)
		  
		  - EXPECT-L0-9 - "ThreadX running on the Microcontroller (STM32) as a RTOS";
		  - ![ThreadX test](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/ThreadX_Running.gif)
		
		- **Docs updated**:
		
		  - EXPECT-L0-6 - "The Qt GUI shall display static graphical elements":
		  - EXPECT-L0-11 - "QT cluster UI **auto-start on boot** on display":
		  - ![Cluster flow](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/images/Flow.jpg)
		  - ![Cluster current-layout](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Cluster-display-sprint4.jpeg)
		
		  - EXPECT-L0-17 - "Introduce AI in the project":
		    - [Generative AI PDF](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Generative-AI-and-LLMs-for-Dummies.pdf)
		    - [External AI Reference](https://github.com/fprezado/generative-ai-for-beginners)
		
		- **Code/Tests**:
		  - EXPECT-L0-10 - "Implement new architecture.":
		  - EXPECT-L0-11 - "QT cluster UI **auto-start on boot** on display":
		  - EXPECT-L0-12 - "Wireless DCI display":
		  - [Motors stress test](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Combined_test.md)
		  - EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)":
		  - [CAN test](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/CAN/CAN_test/README.md)
		
		  - EXPECT-L0-19 - "Test TSF automation":
		    - [TSF Manager Script](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/sync_tsf_manager.py)
		    - [AI Generator](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/ai_generator.py)
		
		  - EXPECT-L0-20 - "AI integration for TSF":
		    - [AI Generator](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/ai_generator.py)
		    - [GenAI on TSF Doc](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/genAI_on_tsf.md)
		
		---
		
		# 🔎 Retrospective
		- ## **Went well**:
		  - ### All points of the sprint were achieved! 44/44
		  - ### Sprint goal achieved!
		
		- ## **To improve**: 
		  - ### Create a robust tests for the project!
		
		## 🔗 Useful Links
		- Sprint board/view: [Sprint 4](https://github.com/orgs/SEAME-pt/projects/89/views/10)
		
		## TSF Useful: 
		  EXPECT-L0-1 - "System architecture is defined and documented":
		  EXPECT-L0-2 - "Hardware Assembly: Fully assemble the PiRacer robot":
		  EXPECT-L0-3 - "Software Setup capable of running Qt applications":
		  EXPECT-L0-4 - "Driveable Car with Remote Control":
		  EXPECT-L0-5 - "Launch basic Qt on built-in display":
		  EXPECT-L0-6 - "The Qt GUI shall display static graphical elements":
		  EXPECT-L0-7 - "QT Crosscompilation should be achieved":
		  EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)":
		  EXPECT-L0-9 - "ThreadX running on the Microcontroller (STM32) as a RTOS";
		  EXPECT-L0-10 - "Implement new architecture.":
		  EXPECT-L0-11 - "QT cluster UI **auto-start on boot** on display":
		  EXPECT-L0-12 - "Wireless DCI display":
		  EXPECT-L0-13 - "Project & GitHub setup":
		  EXPECT-L0-14 - "TSF & Documentation":
		  EXPECT-L0-15 - "Agile and Scrum":
		  EXPECT-L0-16 - "CI/CD":
		  EXPECT-L0-17 - "Introduce AI in the project":
		
		````


- `docs/sprints/sprint5.md`

	??? "Click to view reference"

		````md
		# 🏁 Sprint 5 — (09/12/2025 → 19/12/2025)
		
		&gt; ## **Sprint Goal**:  Establish and Validate our RDAF(Real Data Architecture Flow)
		---
		
		## 📌 Epics
		
		- #### **Epic** — Documentation - [#54](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315799&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C54)
		  - [ ] Team Knowlegde Update/Status Point (5)
		- #### **Epic** — Project Management & Traceability Refinement - [#55](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315926&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C55)
		  - [ ] Update Project Board (3)
		- #### **Epic** — Car Hardware Architecture - [#53](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315665&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C53)
		  - [ ] Integrate Speedometer with Hardware (3)
		  - [ ] Integrate Relay for Motor Driver (3)
		  - [ ] Mount and test SSD through USB device (3)
		  - [ ] Integrate Recharge Terminal (3)
		- #### **Epic** — Car Software Architecture - [#52](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315398&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C52)
		  - [ ] Test real data displaying in QT (8)
		  - [ ] Design and Establish CAN Protocol (13)
		  - [ ] Initialize Data Processing with ThreadX (13)
		  - [ ] Integrate Data from Multiple Sensors (5)
		- #### **Epic** — Study and Integration of Core Technologies - [#56](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138316020&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C56)
		  - [ ] UPROTOCOL, COVESA and KUKSA documentation (13)
		- #### **Epic** - Testing and Validate Software - [103](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/103)
		  - [ ] Test Knowledge (Integration and Unit) (21) 
		---
		
		## 📈 Actual Progress
		
		- ## ✅ Running... 🎉
		- ## ✅ Sprint 5 points: 0/93 (🔥🔥Ambitious!!🔥🔥)
		
		- ❌ Undone: 
		---
		
		## ✅ Outcomes
		
		- **Delivered**:
		 
		- **Demos**:
		
		
		- **Docs updated**:
		
		
		- **Code**:
		
		- **Tests**:
		  
		---
		
		# 🔎 Retrospective
		- ## **Went well**:
		  - ###
		  - ### 
		
		- ## **To improve**: 
		  - ### 
		
		## 🔗 Useful Links
		- 
		
		## TSF Useful: 
		  EXPECT-L0-1 - "System architecture is defined and documented":
		  EXPECT-L0-2 - "Hardware Assembly: Fully assemble the PiRacer robot":
		  EXPECT-L0-3 - "Software Setup capable of running Qt applications":
		  EXPECT-L0-4 - "Driveable Car with Remote Control":
		  EXPECT-L0-5 - "Launch basic Qt on built-in display":
		  EXPECT-L0-6 - "The Qt GUI shall display static graphical elements":
		  EXPECT-L0-7 - "QT Crosscompilation should be achieved":
		  EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)":
		  EXPECT-L0-9 - "ThreadX running on the Microcontroller (STM32) as a RTOS";
		  EXPECT-L0-10 - "Implement new architecture.":
		  EXPECT-L0-11 - "QT cluster UI **auto-start on boot** on display":
		  EXPECT-L0-12 - "Wireless DCI display":
		  EXPECT-L0-13 - "Project & GitHub setup":
		  EXPECT-L0-14 - "TSF & Documentation":
		  EXPECT-L0-15 - "Agile and Scrum":
		  EXPECT-L0-16 - "CI/CD":
		  EXPECT-L0-17 - "Introduce AI in the project":
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_16 | Reviewed: ✔ | Score: 0.0 ### {: #evidences-evid_l0_16 data-toc-label="EVIDENCES-EVID_L0_16" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.

**Evidence Status:** Pending - no evidence has been documented in sprint files yet.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_16](ASSERTIONS.md#assertions-assert_l0_16) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `.github/workflows/daily-meeting.yml`

	??? "Click to view reference"

		````yaml
		name: Build Daily Doc
		
		on:
		  issues:
		    types: [opened, edited, labeled, reopened]
		
		permissions:
		  issues: write
		  contents: read
		
		jobs:
		  build:
		    runs-on: ubuntu-latest
		    if: contains(github.event.issue.labels.*.name, 'ceremony:daily')
		
		    steps:
		      - name: Checkout (read-only)
		        uses: actions/checkout@v4
		        with:
		          fetch-depth: 0
		
		      - name: Extract issue data
		        id: info
		        uses: actions/github-script@v7
		        with:
		          script: |
		            const issue = context.payload.issue
		            const body = issue.body || ''
		            const url = issue.html_url
		            const author = issue.user?.login || 'unknown'
		            const number = issue.number
		
		            // Parse YYYY-MM-DD from "### Date" (emoji tolerant)
		            let date = null
		            const m = body.match(/###\s*📅?\s*Date\s*\n+([0-9]{4}-[0-9]{2}-[0-9]{2})/i)
		            if (m && m[1]) date = m[1]
		            if (!date) date = (issue.created_at || '').slice(0,10) || '1970-01-01'
		
		            core.setOutput('date', date)
		            core.setOutput('body', body)
		            core.setOutput('url', url)
		            core.setOutput('author', author)
		            core.setOutput('number', number.toString())
		
		      - name: Create single markdown file
		        env:
		          DATE: ${{ steps.info.outputs.date }}
		          BODY: ${{ steps.info.outputs.body }}
		          ISSUE_URL: ${{ steps.info.outputs.url }}
		          AUTHOR: ${{ steps.info.outputs.author }}
		        run: |
		          set -euo pipefail
		          FILE="${DATE}-daily.md"
		          {
		            echo "---"
		            echo "date: ${DATE}"
		            echo "issue: ${ISSUE_URL}"
		            echo "author: ${AUTHOR}"
		            echo "---"
		            echo
		            echo "# 🗓️ Daily Standup — ${DATE}"
		            echo
		            echo "**Source issue:** ${ISSUE_URL}  "
		            echo "**Author:** @${AUTHOR}"
		            echo
		            echo "---"
		            echo
		            printf "%s\n" "${BODY}"
		          } &gt; "${FILE}"
		          echo "Created ${FILE}"
		
		      - name: Upload artifact (single file)
		        uses: actions/upload-artifact@v4
		        with:
		          name: daily-${{ steps.info.outputs.date }}
		          path: ${{ steps.info.outputs.date }}-daily.md
		          retention-days: 30
		
		      - name: Comment instructions on the issue
		        uses: actions/github-script@v7
		        with:
		          script: |
		            const date = '${{ steps.info.outputs.date }}'
		            const msg = [
		              `The daily doc for **${date}** is ready ✅`,
		              ``,
		              `**How to add it to the repo:**`,
		              `1) Go to this workflow run (GitHub Actions) → **Artifacts** → download **daily-${date}**`,
		              `2) Unzip and place \`${date}-daily.md\` where you want (e.g., \`docs/meetings/dailies/\`)`,
		              `3) Commit in your branch and open a PR`,
		            ].join('\n')
		            await github.rest.issues.createComment({
		              ...context.repo,
		              issue_number: context.payload.issue.number,
		              body: msg
		            })
		
		````


- `docs/guides/github-actions-guide.md`

	??? "Click to view reference"

		````md
		# 🚀 How to Use GitHub Actions
		
		## 📚 Index
		- [Intro](#sec-intro)
		- [Core Ideas](#sec-core-ideas)
		- [Summary](#sec-summary)
		- [Example: “Taskly”](#sec-example-taskly)
		- [Links & tutorials](#sec-links)
		
		---
		
		&lt;a id="sec-intro"&gt;&lt;/a&gt;
		## Intro
		
		GitHub Actions lets you **automate** tasks in your repository.  
		You describe **workflows** in YAML files inside `.github/workflows/`.  
		When specific **events** happen (example, a comment is posted, code is pushed), Actions **run jobs** made of **steps**.
		
		---
		
		&lt;a id="sec-core-ideas"&gt;&lt;/a&gt;
		## 🧠 Core Ideas
		
		- **Workflow**: a YAML file describing when and what to run.
		- **on**: the event that triggers the workflow (example, `push`, `pull_request`, `issue_comment`).
		- **permissions**: minimal access required (example, `issues: write` to post comments).
		- **jobs**: units of work (each job runs on a fresh virtual machine).
		- **runs-on**: which machine image to use (example, `ubuntu-latest`).
		- **steps**: actions or shell commands run in order.
		- **actions**: reusable steps (example, `actions/checkout`, `actions/github-script`).
		
		---
		
		&lt;a id="sec-summary"&gt;&lt;/a&gt;
		## 🧩 Summary
		
		1. **Event happens** → (example, someone comments on an issue).  
		2. GitHub starts your **workflow** → runs your **job** on a **runner**.  
		3. The job executes **steps** (reusable actions or shell commands).  
		4. Your workflow can **comment**, **build**, **test**, **upload artifacts**, etc.
		
		---
		
		&lt;a id="sec-example-taskly"&gt;&lt;/a&gt;
		## 📄 Example: “Taskly” — Post a Closure Template via Comment
		
		This workflow listens to **issue comments**.  
		If someone writes `/taskly` on an **Issue** (not a PR), the bot replies with a **closure summary template** so you can fill it and close the issue.
		
		&gt; File path: `.github/workflows/close-issue.yml`
		
		```yaml
		# Workflow name (shows in Actions tab)
		name: Taskly Template
		
		# When should this run? On new issue comments.
		on:
		  issue_comment:
		    types: [created]
		
		# Permissions needed:
		# - issues: write -&gt; to post a comment
		# - contents: read -&gt; safe default
		permissions:
		  issues: write
		  contents: read
		
		jobs:
		  post-template:
		    # Run this job on a GitHub-hosted Ubuntu VM
		    runs-on: ubuntu-latest
		
		    # Guard: only run if
		    # 1) the comment belongs to an Issue (not a PR)
		    # 2) the comment contains our trigger "/taskly"
		    if: ${{ github.event.issue.pull_request == null && contains(github.event.comment.body, '/taskly') }}
		
		    steps:
		      # Use the github-script action to call the API with small JS
		      - name: Post closure template
		        uses: actions/github-script@v7
		        with:
		          script: |
		            // Build the comment body line by line
		            const lines = [
		              '## ✅ Closure Summary',
		              '',
		              '**What was done**',
		              '- ',
		              '',
		              '**How it was done (approach / steps)**',
		              '- ',
		              '',
		              '**References**',
		              '- Docs: ',
		              '- Links: ',
		            ];
		            const body = lines.join('\n');
		
		            // Post the comment to the same issue
		            await github.rest.issues.createComment({
		              ...context.repo,
		              issue_number: context.payload.issue.number,
		              body
		            });
		```
		---
		
		## 📝 What this workflow does
		
		- on → issue_comment: triggers when a new comment is created.
		
		- permissions: allows the workflow to write comments on issues.
		
		- jobs.post-template:
		
		- runs-on: uses an Ubuntu runner.
		
		- if: only continues if the comment is on an Issue and includes /taskly.
		
		- steps: runs actions/github-script to post a template comment.
		
		---
		
		## ▶️ How to use it
		
		On any Issue, add a comment containing /taskly.
		The bot will reply with the closure template → you fill it → close the issue.
		
		---
		
		&lt;a id="sec-links"&gt;&lt;/a&gt;
		
		## Links
		
		https://docs.github.com/en/actions/get-started/quickstart
		
		https://www.freecodecamp.org/news/learn-to-use-github-actions-step-by-step-guide/#heading-github-actions-syntax
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_17 | Reviewed: ✔ | Score: 1.0 ### {: #evidences-evid_l0_17 data-toc-label="EVIDENCES-EVID_L0_17" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_17](ASSERTIONS.md#assertions-assert_l0_17) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/guides/github-guidelines.md`

	??? "Click to view reference"

		````md
		# Contributing Guide – SEA:ME Team 6 (2025/26)
		
		This document explains **how we work** in this repo: branches, commits, PRs, reviews, CI, documentation, and our sprint strategy.
		
		---
		
		## 📚 Index
		
		- [Branching](#branching)
		- [Working Flow](#working-flow)
		- [TSF Traceability (always in PR)](#tsf-traceability-always-in-pr)
		- [Commit Style (Conventional Commits)](#commit-style-conventional-commits)
		- [Issues](#issues)
		- [Pull Requests](#pull-requests)
		- [CI & Automation](#ci--automation)
		- [Sprints](#sprints)
		
		---
		
		&lt;a id="branching"&gt;&lt;/a&gt;
		## 🔀 Branching
		
		- **main** → protected and release-ready.
		- **dev** → integration branch; all features merge here first.
		- **branch number**-&lt;epic&gt;/\&lt;name&gt; → working branches off **dev**.
		  - Example:
		    - `#12-QtApp-mockup`
		
		---
		&lt;a id="working-flow"&gt;&lt;/a&gt;
		## 🧭 Working Flow
		
		```mermaid 
		  sequenceDiagram
		  participant Dev as Developer
		  participant FeatureBranch as feat branch
		  participant Deve as dev branch
		  participant Main as main
		  participant Rev as reviewer
		
		  Dev-&gt;&gt;FeatureBranch: Create #number-Epic-name from dev
		  Dev-&gt;&gt;FeatureBranch: Push commits to feature branch
		  FeatureBranch-&gt;&gt;Deve: Open PR feature -&gt; dev
		  Dev--&gt;&gt;Deve: Run checks (build, test, lint)
		  Deve-&gt;&gt;Main: Open PR dev -&gt; main
		  Dev--&gt;&gt;Deve: Report checks status (pass)
		  Rev--&gt;&gt;Main: Approve PR
		  Deve-&gt;&gt;Main: Merge PR into main
		```
		&lt;a id="tsf-traceability-always-in-pr"&gt;&lt;/a&gt;
		## 🧱 TSF Traceability (always in PR)
		
		** Still working on this part (will update when me and @jpjpcs finish this part) **
		
		Keep the TSF table updated in the PR body:
		
		| Requirement ID | Description | Status |
		|---|---|---|
		| REQ-001 | Display speed on screen | ✅ Implemented |
		| REQ-002 | Remote control capability | ⚙️ In Progress |
		
		---
		&lt;a id="commit-style-conventional-commits"&gt;&lt;/a&gt;
		## 🧠 Commit Style (Conventional Commits)
		
		We should write clear commits, nothing to long.
		Keep it simple for clarity.
		
		- `feat(ui): add speedometer widget`
		- `fix(rt): correct timer overflow`
		- `docs(workflow): add daily meeting explainer`
		
		Small commits are easier to review.
		
		---
		&lt;a id="issues"&gt;&lt;/a&gt;
		## 🔃 Issues
		
		-&gt;The issues are normally open in the begginning of each sprint -- planning.
		
		-&gt;Branches are created from the issue.
		
		-&gt;When the task of the issue is done -&gt; go to the issue -&gt; comment /taskly -&gt; fill the comment -&gt; close the issue ✅
		
		&lt;a id="pull-requests"&gt;&lt;/a&gt;
		## 🔃 Pull Requests
		
		Open PRs from `feature/*` → **dev**. Later, open **dev → main** for releases.
		
		**PR requirements**
		- Use the repo **PR Template**.
		- Clear **Summary**, **Testing steps**, **TSF table**.
		- Link Issues/Epics (`Relates to Epic: ...`).
		
		**Reviews**
		- 1 approval required for **main**.
		
		**Merging**
		- **Merge** into **dev** and **main**.
		- Delete merged branches.
		
		---
		&lt;a id="ci--automation"&gt;&lt;/a&gt;
		## 🔧 CI & Automation
		
		**(Need to work more on this part, later I will create a document to explain GitHub actions)**
		
		- **GitHub Actions** run on PRs/Issues (build/test/lint, docs helpers).
		- **Daily Standup**: Issue form creates an artifact with `YYYY-MM-DD-daily.md` and comments instructions to commit it via normal PR.
		
		---
		&lt;a id="sprints"&gt;&lt;/a&gt;
		## 🗺️ Sprints
		
		- Sprint lives in the **Project board** (Sprint view).  
		- Use labels to filter and track (examples):  
		  `Sprint 1`, `Type: Epic`, `Daily Meeting`, `Blocked`.
		- Link tasks to Epics for traceability.
		
		````


- `docs/guides/genAI-pair-programming-guidelines.md`

	??? "Click to view reference"

		````md
		https://github.com/fprezado/generative-ai-for-beginners
		
		# **GenAI Pair Programming Guidelines**
		
		### **Purpose**
		
		This document defines the techniques, best practices, and integration strategies for effective collaboration with Generative AI tools (e.g., ChatGPT, GitHub Copilot, Codeium).  
		It ensures that AI-assisted development aligns with the **Trustable Software Framework (TSF)** principles of traceability, quality, and accountability.
		
		---
		
		## **1. Techniques for Effective Pair Programming with Generative AI**
		
		| **Technique** | **Description** | **Example Application** |
		|----------------|-----------------|---------------------------|
		| **Turn-taking collaboration** | Alternate between writing code manually and prompting the AI for suggestions, improvements, or documentation. | The developer implements a class structure; the AI suggests complementary test cases. |
		| **Prompt refinement** | Provide detailed and contextual prompts for more accurate results. | “Write a C++ function to control a servo motor on a Raspberry Pi 5 using WiringPi” instead of “write servo code.” |
		| **Explain-first approach** | Explain the project’s objective and environment before requesting code. | “We’re developing a Qt GUI for the PiRacer’s screen that displays static elements only.” |
		| **Collaborative debugging** | Use the AI to interpret compiler errors, logs, or stack traces. | “Here’s the CMake error when building the Qt app — what might be missing?” |
		| **Code walkthroughs** | Request detailed explanations of generated code to strengthen understanding. | “Explain how this Qt signal-slot connection works.” |
		
		---
		
		## **2. Best Practices for Reviewing AI-Generated Code**
		
		All AI-generated content must undergo **human validation** before being committed to the repository.
		
		| **Best Practice** | **Objective** | **Application** |
		|--------------------|----------------|-----------------|
		| **Human validation first** | Ensure that AI output is logical, correct, and contextually appropriate. | Review all AI code manually before merging. |
		| **Security & privacy check** | Prevent leaks of sensitive data or insecure API usage. | Review for exposed tokens, hardcoded credentials, or unsafe URLs. |
		| **Style consistency** | Maintain consistent coding style and formatting. | Use automated formatters (e.g., `clang-format`, `black`) to standardize. |
		| **Performance and safety review** | Validate that AI-generated code does not introduce performance or safety risks. | Check loops, memory usage, and thread safety. |
		| **Source attribution** | Ensure compliance with open-source licensing. | Verify that the AI did not reproduce GPL or proprietary code snippets. |
		| **Traceability** | Document the use of AI in commits and design documentation. | Mention AI usage in commit messages or PR descriptions (e.g., “Code generated with AI assistance, reviewed by human developer”). |
		
		---
		
		## **3. Integration Strategies for Seamless AI Assistance**
		
		| **Strategy** | **Goal** | **Implementation** |
		|---------------|----------|--------------------|
		| **IDE integration** | Embed AI tools directly in the development environment. | Use extensions such as *GitHub Copilot* or *ChatGPT for VSCode*. |
		| **Prompt templates** | Standardize prompts for recurring tasks. | “Generate unit tests for function X using GoogleTest.” |
		| **Review loop integration** | Incorporate AI-assisted review in Pull Request workflow. | Use AI suggestions before human code review. |
		| **Documentation generation** | Automate comment and documentation creation. | “Write Doxygen comments for this C++ class.” |
		| **AI-assisted test generation** | Automatically generate test cases based on the implementation. | “Generate Python tests for the `display_controller` module.” |
		| **Knowledge transfer** | Use AI to explain new frameworks or libraries. | “Explain the difference between `QMainWindow` and `QWidget` in Qt.” |
		| **Ethical usage policy** | Define clear rules for when and how AI can be used. | Require peer review for all AI-generated contributions. |
		
		---
		
		## **4. Ethical and Educational Considerations**
		
		- AI is a **support tool**, not a substitute for critical thinking.  
		- The developer retains **full responsibility** for code correctness, safety, and maintainability.  
		- AI-generated code must **not** contain or replicate copyrighted material.  
		- All team members should maintain transparency about AI usage.  
		- The objective is to **learn and improve human skill**, not to offload reasoning to AI.
		
		---
		
		## **5. Expected Outcomes**
		
		By following these guidelines, the development team will:
		
		- Improve productivity through structured AI collaboration.  
		- Maintain software integrity and compliance with TSF principles.  
		- Enhance learning and understanding of both AI-assisted and manual development.  
		- Ensure reproducibility and accountability in all AI-supported work.
		
		---
		
		## **6. References**
		
		- *ISO/IEC 5338:2022* — Guidelines for the Use of AI in Software Development  
		- *Trustable Software Framework (TSF)* — Documentation and Traceability Principles  
		- *GitHub Copilot Guidelines* — Responsible AI Usage Policy  
		- *ChatGPT Code Interpreter Best Practices*, OpenAI (2024)
		
		---
		
		**Document version:** 1.0  
		**Last updated:** *October 2025*  
		**Author(s):** *PiRacer Warm-Up Team*  
		**Repository:** [GitHub Repository Link]
		
		````


- `docs/guides/generative-ai-for-beginners.md`

	??? "Click to view reference"

		````md
		# Generative AI for Beginners
		
		Reference to external repository:
		https://github.com/fprezado/generative-ai-for-beginners
		
		This file is a local pointer for the external resource used as evidence.
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_18 | Reviewed: ✔ | Score: 0.0 ### {: #evidences-evid_l0_18 data-toc-label="EVIDENCES-EVID_L0_18" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.

**Evidence Status:** Pending - no evidence has been documented in sprint files yet.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_18](ASSERTIONS.md#assertions-assert_l0_18) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/ThreadXGuide.md`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/ThreadXGuide.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/ThreadXGuide.md)


- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/CAN-test-guide.md`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/CAN-test-guide.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/CAN-test-guide.md)



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_19 | Reviewed: ✔ | Score: 1.0 ### {: #evidences-evid_l0_19 data-toc-label="EVIDENCES-EVID_L0_19" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_19](ASSERTIONS.md#assertions-assert_l0_19) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/tsf_implementation/README.md`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/tsf_implementation/README.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/tsf_implementation/README.md)


- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/.github/workflows/validate_items_formatation.yml`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/.github/workflows/validate_items_formatation.yml](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/.github/workflows/validate_items_formatation.yml)



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_20 | Reviewed: ✔ | Score: 1.0 ### {: #evidences-evid_l0_20 data-toc-label="EVIDENCES-EVID_L0_20" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_20](ASSERTIONS.md#assertions-assert_l0_20) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/genAI-pair-programming-guidelines.md`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/genAI-pair-programming-guidelines.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/genAI-pair-programming-guidelines.md)


- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/tsf_implementation/TSF_docs/genAI_on_tsf.md`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/tsf_implementation/TSF_docs/genAI_on_tsf.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/tsf_implementation/TSF_docs/genAI_on_tsf.md)



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_21 | Reviewed: ✔ | Score: 0.0 ### {: #evidences-evid_l0_21 data-toc-label="EVIDENCES-EVID_L0_21" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.

**Evidence Status:** Pending - no evidence has been documented in sprint files yet.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_21](ASSERTIONS.md#assertions-assert_l0_21) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py)



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_22 | Reviewed: ✔ | Score: 0.0 ### {: #evidences-evid_l0_22 data-toc-label="EVIDENCES-EVID_L0_22" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
This evidence item collects repository artifacts, sprint reports and timing measurements that demonstrate the STM32 ThreadX startup requirement is met.

**Evidence Status:** Pending - timing measurements to be collected during hardware integration testing.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_22](ASSERTIONS.md#assertions-assert_l0_22) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md)



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_23 | Reviewed: ✔ | Score: 0.0 ### {: #evidences-evid_l0_23 data-toc-label="EVIDENCES-EVID_L0_23" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
This evidence item collects boot logs, timing measurements, and video recordings that demonstrate the Raspberry Pi 5 AGL boot time requirement is met.

**Evidence Status:** Pending - timing measurements to be collected during hardware integration testing.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_23](ASSERTIONS.md#assertions-assert_l0_23) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md)



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_24 | Reviewed: ✔ | Score: 0.0 ### {: #evidences-evid_l0_24 data-toc-label="EVIDENCES-EVID_L0_24" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
This evidence item collects integration test results, CAN traffic logs, and timing correlation data that demonstrate the combined startup requirement is met.

**Evidence Status:** Pending - integration test to be performed during system integration phase.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_24](ASSERTIONS.md#assertions-assert_l0_24) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md)



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_25 | Reviewed: ✔ | Score: 0.0 ### {: #evidences-evid_l0_25 data-toc-label="EVIDENCES-EVID_L0_25" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
This evidence item collects video recordings, startup logs, and timing measurements that demonstrate the instrument cluster UI availability requirement is met.

**Evidence Status:** Pending - timing measurements to be collected during hardware integration testing.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_25](ASSERTIONS.md#assertions-assert_l0_25) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md)



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_26 | Reviewed: ✔ | Score: 0.0 ### {: #evidences-evid_l0_26 data-toc-label="EVIDENCES-EVID_L0_26" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
This evidence item collects end-to-end integration test results, video recordings, and system logs that demonstrate all startup time requirements are met.

**Evidence Status:** Pending - end-to-end integration test to be performed during final system integration phase.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_26](ASSERTIONS.md#assertions-assert_l0_26) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md)



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_27 | Reviewed: ✔ | Score: 0.0 ### {: #evidences-evid_l0_27 data-toc-label="EVIDENCES-EVID_L0_27" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
This evidence item collects video recordings, distance measurements, and test logs that demonstrate the emergency braking stopping distance requirement is met.

**Evidence Status:** Pending - integration test to be performed during ADAS testing phase.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_27](ASSERTIONS.md#assertions-assert_l0_27) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md)



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_28 | Reviewed: ✔ | Score: 0.0 ### {: #evidences-evid_l0_28 data-toc-label="EVIDENCES-EVID_L0_28" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
This evidence item collects measurement comparisons, photos, and logs that demonstrate the temperature measurement accuracy requirement is met.

**Evidence Status:** Pending - temperature comparison test to be performed during sensor integration testing.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_28](ASSERTIONS.md#assertions-assert_l0_28) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md)



{% endraw %}

**Fallacies:**

_None_


---

### EVIDENCES-EVID_L0_29 | Reviewed: ✔ | Score: 0.0 ### {: #evidences-evid_l0_29 data-toc-label="EVIDENCES-EVID_L0_29" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
This evidence item collects video recordings and logs that demonstrate the driver presence condition requirement is met.

**Evidence Status:** Pending - integration test to be performed during driver presence validation testing.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_29](ASSERTIONS.md#assertions-assert_l0_29) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/README.md)



{% endraw %}

**Fallacies:**

_None_
