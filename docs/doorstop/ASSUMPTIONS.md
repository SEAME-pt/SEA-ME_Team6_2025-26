

---

### ASSUMPTIONS-ASSUMP_L0_1 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_1 data-toc-label="ASSUMPTIONS-ASSUMP_L0_1" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
Assumption: The project team will provide the required hardware and demonstrator components listed in the system architecture (Expansion Board, Raspberry Pi 5, Hailo Hat, DC motors, servo motor, Qt display/cluster via DSI interface) and these components will be operational in the integration environment during verification activities.

Acceptance criteria / notes:
- Hardware units and connectors are available and powered in the test environment when required by integration tests.
- Device-specific drivers or firmware are available or will be provided by the component owner prior to verification.
- If a component is unavailable, a documented mitigation plan (substitute hardware or simulator) will be provided and linked in the corresponding EVID item.

Rationale: verification activities and evidence collection for `EXPECT-L0-1` depend on access to the stated hardware; recording this as an assumption highlights a test prerequisite that can be tracked and validated.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_1](EXPECTATIONS.md#expectations-expect_l0_1) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_1/EXPECTATIONS-EXPECT_L0_1.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_1
		header: System architecture defined
		text: The demonstrator car system has a documented architecture that describes all
		  hardware and software components and their interactions.
		level: 1.1
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_1/ASSERTIONS-ASSERT_L0_1.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_2 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_2 data-toc-label="ASSUMPTIONS-ASSUMP_L0_2" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
The required hardware components (Expansion Board, Raspberry Pi 5, Raspberry Pi 4, DC motors, servo motor, display interfaces, chassis) are procured and available before hardware assembly activities begin.

Acceptance criteria / notes:
- All hardware components have been purchased or provided by the organization.
- Components are received and physically available in the project workspace.
- Component specifications match the system architecture requirements.

Rationale: Hardware assembly for `EXPECT-L0-2` depends on external procurement of components. This assumption makes explicit that hardware availability is a prerequisite for assembly work.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_2](EXPECTATIONS.md#expectations-expect_l0_2) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_2/EXPECTATIONS-EXPECT_L0_2.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_2
		header: Hardware assembly complete
		text: The demonstrator car hardware is fully assembled with all components correctly
		  connected and operational.
		level: '1.2'
		normative: true
		references:
		- path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_2/ASSERTIONS-ASSERT_L0_2.md
		  type: file
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_3 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_3 data-toc-label="ASSUMPTIONS-ASSUMP_L0_3" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
Assumption: The system and development tooling target a Linux-based integration environment (Ubuntu 22.04 LTS or compatible). Device drivers and kernel modules required for platform-specific components (e.g., motor controllers, Hailo AI Hat, camera interfaces) are available or will be provided by component owners prior to verification.

Acceptance criteria / notes:
- A VM or physical host running Ubuntu 22.04 is available for integration tests.
- Required kernel modules and device drivers are installed and documented in the corresponding EVID items.
- If a driver is unavailable, an alternative (simulator or stub) will be provided and documented before verification begins.

Rationale: Many verification steps and evidence artifacts assume POSIX/Linux tooling (bash, apt, systemd, udev rules). Making this explicit avoids ambiguous environment dependency during testing.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_3](EXPECTATIONS.md#expectations-expect_l0_3) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_3/EXPECTATIONS-EXPECT_L0_3.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_3
		header: Software environment operational
		text: The Raspberry Pi 5 runs a Linux environment capable of executing Qt applications.
		level: '1.3'
		normative: true
		references:
		- path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_3/ASSERTIONS-ASSERT_L0_3.md
		  type: file
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_4 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_4 data-toc-label="ASSUMPTIONS-ASSUMP_L0_4" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
The joystick or remote control hardware is available, functional, and compatible with the target platform input subsystem before remote control integration testing begins.

Acceptance criteria / notes:
- Joystick device is recognized by the Linux input subsystem (e.g., /dev/input/js0).
- Device drivers are installed and documented.
- Input mapping configuration files are available or will be provided before testing.

Rationale: Remote control verification for `EXPECT-L0-4` depends on external input hardware availability. This assumption clarifies that hardware procurement and driver availability are prerequisites.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_4](EXPECTATIONS.md#expectations-expect_l0_4) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_4/EXPECTATIONS-EXPECT_L0_4.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_4
		header: Remote control functional
		text: The demonstrator car responds to remote control inputs for steering and throttle
		  control.
		level: '1.4'
		normative: true
		references:
		- path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_4/ASSERTIONS-ASSERT_L0_4.md
		  type: file
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_5 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_5 data-toc-label="ASSUMPTIONS-ASSUMP_L0_5" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
Qt runtime libraries and dependencies are installed and available on the target platform before Qt application deployment begins.

Acceptance criteria / notes:
- Qt6 runtime is installed (e.g., via apt or compiled from source).
- Required Qt modules (QtCore, QtGui, QtWidgets, QtQml) are present.
- Display server (Wayland or X11) is configured and operational.

Rationale: Qt application verification for `EXPECT-L0-5` assumes that the Qt framework and runtime environment are already provisioned on the target system.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_5](EXPECTATIONS.md#expectations-expect_l0_5) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_5/EXPECTATIONS-EXPECT_L0_5.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_5
		header: Qt application launches
		text: A basic Qt application successfully launches and displays on the system.
		level: '1.5'
		normative: true
		references:
		- path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_5/ASSERTIONS-ASSERT_L0_5.md
		  type: file
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_6 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_6 data-toc-label="ASSUMPTIONS-ASSUMP_L0_6" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
Third-party font libraries and icon sets required by the Qt GUI are available under compatible licenses before UI implementation begins.

Acceptance criteria / notes:
- External font files (e.g., TrueType, OpenType) are downloaded or accessible.
- Icon sets (e.g., Font Awesome, Material Icons) are available with compatible licenses.
- License terms allow redistribution and use in the project context.

Rationale: Qt GUI implementation for `EXPECT-L0-6` depends on external font and icon libraries being legally available. This assumption clarifies that third-party asset procurement and licensing are prerequisites.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_6](EXPECTATIONS.md#expectations-expect_l0_6) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_6/EXPECTATIONS-EXPECT_L0_6.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_6
		header: GUI displays static elements
		text: The Qt graphical user interface displays static visual elements that form a
		  simple cluster dashboard.
		level: '1.6'
		normative: true
		references:
		- path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_6/ASSERTIONS-ASSERT_L0_6.md
		  type: file
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_7 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_7 data-toc-label="ASSUMPTIONS-ASSUMP_L0_7" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
GCC ARM64 cross-compiler, CMake, and Qt6 cross-compilation libraries are available and configured on the build host before cross-compilation activities begin.

Acceptance criteria / notes:
- GCC/Clang ARM64 cross-compiler is installed and accessible.
- CMake toolchain file is configured with correct sysroot and compiler paths.
- Qt6 cross-compiled libraries are available or will be built as part of setup.

Rationale: Cross-compilation for `EXPECT-L0-7` depends on external toolchain components (GCC, CMake) being available on the build host. This assumption makes toolchain provisioning prerequisites explicit.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_7](EXPECTATIONS.md#expectations-expect_l0_7) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_7/EXPECTATIONS-EXPECT_L0_7.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_7
		header: Cross-compilation capability exists
		text: The Qt application can be cross-compiled for the target hardware architecture.
		level: '1.7'
		normative: true
		references:
		- path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_7/ASSERTIONS-ASSERT_L0_7.md
		  type: file
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_8 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_8 data-toc-label="ASSUMPTIONS-ASSUMP_L0_8" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
CAN bus hardware interfaces (CAN controllers, transceivers, wiring) are installed and functional on both Raspberry Pi 5 and STM32 before CAN communication testing begins.

Acceptance criteria / notes:
- CAN interface is recognized by Linux (e.g., can0 device exists).
- CAN bus wiring is correctly connected between Rasp5 and STM32.
- CAN bus termination resistors are properly installed.

Rationale: CAN communication verification for `EXPECT-L0-8` depends on physical CAN hardware availability and correct installation. This assumption makes hardware prerequisites explicit.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_8](EXPECTATIONS.md#expectations-expect_l0_8) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_8/EXPECTATIONS-EXPECT_L0_8.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_8
		header: CAN communication established
		text: The Raspberry Pi 5 and STM32 microcontroller communicate bidirectionally using
		  the CAN protocol.
		level: '1.8'
		normative: true
		references:
		- path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_8/ASSERTIONS-ASSERT_L0_8.md
		  type: file
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_9 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_9 data-toc-label="ASSUMPTIONS-ASSUMP_L0_9" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
The STM32 microcontroller is programmed and accessible via debug interface (ST-LINK or JTAG) before ThreadX firmware verification begins.

Acceptance criteria / notes:
- ST-LINK debugger hardware is available and connected to the STM32.
- STM32CubeProgrammer or OpenOCD is installed on the development host.
- Firmware flashing and debugging procedures are documented.

Rationale: ThreadX verification for `EXPECT-L0-9` assumes that STM32 programming infrastructure (debugger, flashing tools) is already available and operational.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_9](EXPECTATIONS.md#expectations-expect_l0_9) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_9/EXPECTATIONS-EXPECT_L0_9.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_9
		header: Real-time OS operational
		text: The STM32 microcontroller runs the ThreadX real-time operating system for control
		  tasks.
		level: '1.9'
		normative: true
		references:
		- path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_9/ASSERTIONS-ASSERT_L0_9.md
		  type: file
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_10 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_10 data-toc-label="ASSUMPTIONS-ASSUMP_L0_10" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
Hardware components (Raspberry Pi 5, Raspberry Pi 4, STM32, Hailo AI Hat, motors, sensors) are procured and available before system architecture implementation begins.

Acceptance criteria / notes:
- All hardware components listed in the architecture have been purchased or provided.
- Components are received and physically available in the project workspace.
- Component specifications match the planned architecture requirements.

Rationale: Architecture implementation for `EXPECT-L0-10` depends on hardware availability. This assumption makes explicit that hardware procurement is a prerequisite for architecture realization.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_10](EXPECTATIONS.md#expectations-expect_l0_10) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_10/EXPECTATIONS-EXPECT_L0_10.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_10
		header: Dual-device architecture implemented
		text: The system operates using a dual Raspberry Pi architecture with Raspberry Pi
		  5 for computation and Raspberry Pi 4 for display.
		level: '1.10'
		normative: true
		references:
		- path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_10/ASSERTIONS-ASSERT_L0_10.md
		  type: file
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_11 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_11 data-toc-label="ASSUMPTIONS-ASSUMP_L0_11" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
Both Raspberry Pi 5 (in-car Qt cluster host) and Raspberry Pi 4 (wireless display receiver) use systemd-based init systems that support service auto-start configuration before Qt application boot automation testing begins.

Acceptance criteria / notes:
- Systemd is installed and operational on both Raspberry Pi 5 and Raspberry Pi 4.
- Service file creation and enabling mechanisms are documented for both systems.
- Display managers are configured for auto-login on both Rasp5 and Rasp4.

Rationale: Auto-start verification for `EXPECT-L0-11` assumes systemd availability on both Raspberry Pi 5 (Qt cluster host) and Raspberry Pi 4 (wireless display receiver). This assumption makes the dual-system init configuration dependency explicit.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_11](EXPECTATIONS.md#expectations-expect_l0_11) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_11/EXPECTATIONS-EXPECT_L0_11.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_11
		header: Automatic startup configured
		text: The Qt cluster application automatically starts on system boot and displays
		  fullscreen.
		level: '1.11'
		normative: true
		references:
		- path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_11/ASSERTIONS-ASSERT_L0_11.md
		  type: file
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_12 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_12 data-toc-label="ASSUMPTIONS-ASSUMP_L0_12" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
Raspberry Pi 4 wireless display receiver hardware with WiFi capability and VNC server software (Wayland/VNC stack) are available and functional before wireless display integration testing begins.

Acceptance criteria / notes:
- Raspberry Pi 4 with WiFi module is procured and configured.
- Wayland display server and VNC server (e.g., wayvnc) are installed on Raspberry Pi 5.
- VNC client software is installed on Raspberry Pi 4 for receiving the stream.
- Network configuration allows Rasp5-to-Rasp4 VNC communication over WiFi.

Rationale: Wireless display verification for `EXPECT-L0-12` depends on Raspberry Pi 4 hardware availability and VNC/Wayland software stack being operational. This assumption clarifies the Rasp5-to-Rasp4 streaming architecture.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_12](EXPECTATIONS.md#expectations-expect_l0_12) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_12/EXPECTATIONS-EXPECT_L0_12.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_12
		header: Wireless display operational
		text: The cluster display content streams wirelessly from the computation device to
		  the display device.
		level: '1.12'
		normative: true
		references:
		- path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_12/ASSERTIONS-ASSERT_L0_12.md
		  type: file
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````


- `docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_12/EVIDENCES-EVID_L0_12.md`

	??? "Click to view reference"

		````md
		---
		id: EVID_L0_12
		header: Wireless display demonstrated
		text: The repository contains VNC configuration guides and demonstration images showing wireless display streaming.
		normative: true
		level: '1.12'
		references:
		- type: file
		  path: docs/guides/Power Consumption Analysis.md
		- type: file
		  path: docs/guides/AGL_Installation_Guide.md
		score: 1.0
		reviewers:
		  - name: Joao Jesus Silva
		    email: joao.silva@seame.pt
		review_status: accepted
		---
		This evidence item collects repository artifacts, sprint reports and demo images that demonstrate the requirement is met.
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_13 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_13 data-toc-label="ASSUMPTIONS-ASSUMP_L0_13" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
GitHub repository access and permissions are granted to all project team members before project setup verification begins.

Acceptance criteria / notes:
- All team members have GitHub accounts.
- Repository permissions (read, write, admin) are configured according to roles.
- Branch protection rules are documented.

Rationale: Project setup verification for `EXPECT-L0-13` assumes that access control and team onboarding are handled externally to the technical verification process.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_13](EXPECTATIONS.md#expectations-expect_l0_13) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_13/EXPECTATIONS-EXPECT_L0_13.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_13
		header: Version control established
		text: The project uses a GitHub repository with proper structure and version control
		  practices.
		level: '1.13'
		normative: true
		references:
		- path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_13/ASSERTIONS-ASSERT_L0_13.md
		  type: file
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_14 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_14 data-toc-label="ASSUMPTIONS-ASSUMP_L0_14" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
The Trustable Software Framework (TSF) methodology documentation and reference implementations are publicly accessible online before TSF implementation begins.

Acceptance criteria / notes:
- TruDAG documentation: https://codethinklabs.gitlab.io/trustable/trustable/
- TSF methodology: https://codethinklabs.gitlab.io/trustable/trustable/methodology.html
- Reference implementation: https://gitlab.com/CodethinkLabs/safety-monitor/safety-monitor/-/tree/main/trustable
- Installation guide: https://codethinklabs.gitlab.io/trustable/trustable/trudag/install.html
- Eclipse TSF project: https://projects.eclipse.org/projects/technology.tsf
- Codethink TSF GitLab: https://gitlab.com/CodethinkLabs/trustable/trustable
- TruDAG source docs: https://gitlab.com/CodethinkLabs/trustable/trustable/-/tree/main/docs/trudag
- SCORE-JSON schema: https://score-json.github.io/json/main/generated/trustable_graph.html

Rationale: TSF implementation for `EXPECT-L0-14` depends on external documentation and examples being available for consultation and self-training. This assumption provides explicit URLs for rastreability and transparency.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_14](EXPECTATIONS.md#expectations-expect_l0_14) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_14/EXPECTATIONS-EXPECT_L0_14.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_14
		header: Requirements traceability established
		text: The project implements the Trustable Safety Framework for requirements definition
		  and traceability.
		level: '1.14'
		normative: true
		references:
		- path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_14/ASSERTIONS-ASSERT_L0_14.md
		  type: file
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_15 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_15 data-toc-label="ASSUMPTIONS-ASSUMP_L0_15" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
GitHub Projects and Agile/Scrum methodology documentation are accessible for self-training before agile process adoption begins.

Acceptance criteria / notes:
- GitHub Projects is enabled and accessible for the repository.
- Agile and Scrum training materials (online resources, books, documentation) are accessible to team members.
- Team members complete individual self-training on Agile/Scrum methodologies.
- Sprint schedules and ceremonies (standup, review, retrospective) are defined by the team.

Rationale: Agile process adoption for `EXPECT-L0-15` depends on GitHub Projects tooling availability and individual self-training on Agile/Scrum methodologies. This assumption clarifies that the project uses GitHub Projects exclusively and relies on self-directed learning.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_15](EXPECTATIONS.md#expectations-expect_l0_15) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_15/EXPECTATIONS-EXPECT_L0_15.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_15
		header: Agile methodology adopted
		text: The team follows Agile Scrum practices for iterative development and project
		  management.
		level: '1.15'
		normative: true
		references:
		- path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_15/ASSERTIONS-ASSERT_L0_15.md
		  type: file
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_16 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_16 data-toc-label="ASSUMPTIONS-ASSUMP_L0_16" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
GitHub Actions runner resources and execution quotas are available and sufficient for CI/CD pipeline execution before CI/CD verification begins.

Acceptance criteria / notes:
- GitHub Actions is enabled for the repository.
- Execution minutes quota is sufficient for planned build/test workloads.
- Self-hosted runners (if used) are configured and operational.

Rationale: CI/CD verification for `EXPECT-L0-16` assumes that GitHub Actions infrastructure and quota allocation are provided externally to the project.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_16](EXPECTATIONS.md#expectations-expect_l0_16) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_16/EXPECTATIONS-EXPECT_L0_16.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_16
		header: Continuous integration active
		text: The project repository includes automated CI/CD pipelines for building and testing
		  code changes.
		level: '1.16'
		normative: true
		references:
		- path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_16/ASSERTIONS-ASSERT_L0_16.md
		  type: file
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_17 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_17 data-toc-label="ASSUMPTIONS-ASSUMP_L0_17" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
The Hailo AI Hat hardware is available, functional, and compatible with the Raspberry Pi 5 before AI model deployment testing begins.

Acceptance criteria / notes:
- Hailo AI Hat is physically installed on the Raspberry Pi 5.
- Hailo runtime and drivers are installed and operational.
- Pre-trained AI models (if required) are available or documented.

Rationale: AI integration verification for `EXPECT-L0-17` depends on Hailo AI Hat hardware availability and driver installation, which are external dependencies.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_17](EXPECTATIONS.md#expectations-expect_l0_17) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_17/EXPECTATIONS-EXPECT_L0_17.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_17
		header: AI tools integrated
		text: The development process incorporates AI-assisted tools for code development
		  and documentation.
		level: '1.17'
		normative: true
		references:
		- path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_17/ASSERTIONS-ASSERT_L0_17.md
		  type: file
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_18 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_18 data-toc-label="ASSUMPTIONS-ASSUMP_L0_18" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
The development environment meets all prerequisites for implementing and verifying this requirement.

**Acceptance criteria / notes:**
- Required components are available and configured in the test environment.
- If a component is unavailable, a documented mitigation plan will be provided.
- This assumption is validated by the `validate_linux_environment` validator.

**Rationale:** Verification activities for `EXPECT-L0-18` depend on these prerequisites being met.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_18](EXPECTATIONS.md#expectations-expect_l0_18) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_18/EXPECTATIONS-EXPECT_L0_18.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_18
		header: "CAN Bus Communication with ThreadX RTOS"
		text: |
		  The vehicle shall communicate via CAN bus using ThreadX as the real-time operating system on the STM32 microcontroller. The CAN communication layer shall be integrated with ThreadX threads to ensure deterministic message handling and proper prioritization of automotive communication tasks.
		level: '1.18'
		normative: true
		references:
		  - type: file
		    path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_18/ASSERTIONS-ASSERT_L0_18.md
		reviewers:
		  - name: Joao Jesus Silva
		    email: joao.silva@seame.pt
		review_status: accepted
		---
		
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_19 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_19 data-toc-label="ASSUMPTIONS-ASSUMP_L0_19" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
The development environment meets all prerequisites for implementing and verifying this requirement.

**Acceptance criteria / notes:**
- Required components are available and configured in the test environment.
- If a component is unavailable, a documented mitigation plan will be provided.
- This assumption is validated by the `validate_linux_environment` validator.

**Rationale:** Verification activities for `EXPECT-L0-19` depend on these prerequisites being met.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_19](EXPECTATIONS.md#expectations-expect_l0_19) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_19/EXPECTATIONS-EXPECT_L0_19.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_19
		header: "TSF Automation Testing"
		text: |
		  The project shall implement automated testing for TSF (Technical Software Framework) requirements. This includes validation of item formats, YAML frontmatter structure, and cross-references between EXPECT, ASSERT, EVID, and ASSUMP items. Automated validation shall run in CI/CD pipeline.
		level: '1.19'
		normative: true
		references:
		  - type: file
		    path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_19/ASSERTIONS-ASSERT_L0_19.md
		reviewers:
		  - name: Joao Jesus Silva
		    email: joao.silva@seame.pt
		review_status: accepted
		---
		
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_20 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_20 data-toc-label="ASSUMPTIONS-ASSUMP_L0_20" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
The development environment meets all prerequisites for implementing and verifying this requirement.

**Acceptance criteria / notes:**
- Required components are available and configured in the test environment.
- If a component is unavailable, a documented mitigation plan will be provided.
- This assumption is validated by the `validate_linux_environment` validator.

**Rationale:** Verification activities for `EXPECT-L0-20` depend on these prerequisites being met.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_20](EXPECTATIONS.md#expectations-expect_l0_20) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_20/EXPECTATIONS-EXPECT_L0_20.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_20
		header: "AI-Assisted Content Generation Integration"
		text: |
		  The project shall integrate AI-assisted tools for automated content generation in the TSF workflow. This includes using GitHub Copilot CLI and/or VSCode Copilot Chat to generate TSF item content (EXPECT, ASSERT, EVID, ASSUMP) based on requirements and acceptance criteria. The AI integration shall follow documented guidelines for pair programming with generative AI.
		level: '1.20'
		normative: true
		references:
		  - type: file
		    path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_20/ASSERTIONS-ASSERT_L0_20.md
		reviewers:
		  - name: Joao Jesus Silva
		    email: joao.silva@seame.pt
		review_status: accepted
		---
		
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_21 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_21 data-toc-label="ASSUMPTIONS-ASSUMP_L0_21" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}
The development environment meets all prerequisites for implementing and verifying this requirement.

**Acceptance criteria / notes:**
- Required components are available and configured in the test environment.
- If a component is unavailable, a documented mitigation plan will be provided.
- This assumption is validated by the `validate_linux_environment` validator.

**Rationale:** Verification activities for `EXPECT-L0-21` depend on these prerequisites being met.
{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_21](EXPECTATIONS.md#expectations-expect_l0_21) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_21/EXPECTATIONS-EXPECT_L0_21.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_21
		header: "Complete TSF Automation Workflow"
		text: |
		  The project shall implement a complete TSF automation workflow that includes: detection of new requirements, automatic generation of TSF items (EXPECT, ASSERT, EVID, ASSUMP), validation of item structure, evidence synchronization from sprint files, and TruDAG graph generation. The workflow shall be executable via a single unified script.
		level: '1.21'
		normative: true
		references:
		  - type: file
		    path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_21/ASSERTIONS-ASSERT_L0_21.md
		reviewers:
		  - name: Joao Jesus Silva
		    email: joao.silva@seame.pt
		review_status: accepted
		---
		
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_22 | Reviewed: ✔ | Score: 1.0 ### {: #assumptions-assump_l0_22 data-toc-label="ASSUMPTIONS-ASSUMP_L0_22" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_22/EXPECTATIONS-EXPECT_L0_22.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_22
		header: "STM32 ThreadX Deterministic Startup"
		text: |
		  The STM32-based control system running ThreadX shall initialize and start publishing speed data over CAN within a deterministic and bounded time after power-on. The STM32 system must start speed data acquisition and CAN transmission within ≤ 100 ms after power-on.
		
		  **Justification:** AUTOSAR Classic–like ECUs and RTOS-based MCUs typically achieve startup times in the 10–100 ms range. Tier-1 suppliers (Bosch, Continental) commonly target &lt;100 ms for availability of safety-relevant signals. ThreadX is designed for deterministic startup on MCUs.
		
		  **Acceptance Criteria:** The STM32 system starts speed data acquisition and CAN transmission within ≤ 100 ms after power-on.
		level: '1.22'
		normative: true
		references:
		  - type: file
		    path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_22/ASSERTIONS-ASSERT_L0_22.md
		reviewers:
		  - name: Joao Jesus Silva
		    email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_23 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_23 data-toc-label="ASSUMPTIONS-ASSUMP_L0_23" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}

{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_23](EXPECTATIONS.md#expectations-expect_l0_23) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_23/EXPECTATIONS-EXPECT_L0_23.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_23
		header: "Raspberry Pi 5 AGL Boot Time"
		text: |
		  The Raspberry Pi 5–based system running Automotive Grade Linux (AGL) shall complete its operating system initialization and reach a state where middleware services (e.g. KUKSA) are operational within a bounded time after power-on.
		
		  **Justification:** Linux-based automotive platforms (AGL, QNX, Adaptive AUTOSAR) typically boot in 5–15 s. AGL documentation describes fast boot paths but does not mandate strict limits. This timing does not affect safety, as safety-relevant data is handled by the MCU.
		
		  **Acceptance Criteria:** The RASP5/AGL system reaches middleware operational state within ≤ 10 s after power-on.
		level: '1.23'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_23/ASSERTIONS-ASSERT_L0_23.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_24 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_24 data-toc-label="ASSUMPTIONS-ASSUMP_L0_24" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}

{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_24](EXPECTATIONS.md#expectations-expect_l0_24) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_24/EXPECTATIONS-EXPECT_L0_24.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_24
		header: "Combined Startup ThreadX and AGL"
		text: |
		  The combined STM32 (ThreadX) and Raspberry Pi 5 (AGL) system shall provide an integrated startup behavior where safety-relevant data is available before or independently of the completion of the AGL boot process.
		
		  **Justification:** Common automotive architectural practice separates safety-critical MCUs from Linux-based systems. The slower boot domain (AGL) must not block faster safety-relevant functions.
		
		  **Acceptance Criteria:**
		  - Speed data publication from the STM32 is available within ≤ 100 ms after power-on.
		  - AGL system becomes operational within ≤ 10 s, without blocking or delaying data publication.
		  - No startup conflict occurs between ThreadX and AGL domains.
		level: '1.24'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_24/ASSERTIONS-ASSERT_L0_24.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_25 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_25 data-toc-label="ASSUMPTIONS-ASSUMP_L0_25" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}

{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_25](EXPECTATIONS.md#expectations-expect_l0_25) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_25/EXPECTATIONS-EXPECT_L0_25.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_25
		header: "Instrument Cluster Qt UI Availability"
		text: |
		  The instrument cluster system running a Qt-based application on Raspberry Pi 4 shall display an initial usable UI within a bounded time after power-on.
		
		  **Justification:** OEM instrument cluster guidelines commonly target 1–2 s for first visual feedback after ignition on. AGL and Tier-1 cluster architectures emphasize early visual availability, even with partial data.
		
		  **Acceptance Criteria:**
		  - The initial instrument cluster image is displayed within ≤ 2.0 s after power-on.
		  - The Qt application operates independently of backend data availability.
		  - Vehicle data values are displayed once published by KUKSA.
		level: '1.25'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_25/ASSERTIONS-ASSERT_L0_25.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_26 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_26 data-toc-label="ASSUMPTIONS-ASSUMP_L0_26" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}

{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_26](EXPECTATIONS.md#expectations-expect_l0_26) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_26/EXPECTATIONS-EXPECT_L0_26.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_26
		header: "End-to-End System Startup Time"
		text: |
		  The complete system shall provide end-to-end availability of safety-relevant data and a usable instrument cluster UI within time bounds consistent with common automotive architectural practices.
		
		  **Justification:** Automotive systems are architected to ensure fast availability of safety-relevant data and early driver feedback, while allowing complex systems to boot in parallel.
		
		  **Acceptance Criteria:**
		  - Safety-relevant speed data is available within ≤ 100 ms after power-on.
		  - A usable instrument cluster UI is displayed within ≤ 2.0 s after power-on.
		  - Backend services (AGL + KUKSA) become fully operational within ≤ 10 s, without impacting safety or UI availability.
		level: '1.26'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_26/ASSERTIONS-ASSERT_L0_26.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_27 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_27 data-toc-label="ASSUMPTIONS-ASSUMP_L0_27" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}

{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_27](EXPECTATIONS.md#expectations-expect_l0_27) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_27/EXPECTATIONS-EXPECT_L0_27.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_27
		header: "Emergency Braking Stopping Distance"
		text: |
		  The emergency braking system shall actuate correctly and stop the vehicle at a safe distance from a detected obstacle.
		
		  **Justification:** Short-range obstacle detection and emergency braking demonstrators commonly use distances in the 200–500 mm range for low-speed platforms. A minimum remaining distance of 50 mm provides a clear safety margin while remaining realistic for a small-scale vehicle.
		
		  **Acceptance Criteria:**
		  - Given an obstacle detected at 300 mm from the sensor, the vehicle shall stop with a minimum remaining distance of ≥ 50 mm from the obstacle.
		  - The braking action shall be triggered automatically once the obstacle detection threshold is reached.
		level: '1.27'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_27/ASSERTIONS-ASSERT_L0_27.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_28 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_28 data-toc-label="ASSUMPTIONS-ASSUMP_L0_28" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}

{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_28](EXPECTATIONS.md#expectations-expect_l0_28) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_28/EXPECTATIONS-EXPECT_L0_28.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_28
		header: "Exterior Temperature Measurement Accuracy"
		text: |
		  The system shall measure the exterior temperature with an accuracy suitable for vehicle status display.
		
		  **Justification:** Automotive exterior temperature sensors typically allow deviations between ±1 °C and ±3 °C, depending on sensor placement and environmental conditions. A tolerance of ±2 °C is realistic and appropriate for a demonstrator system.
		
		  **Acceptance Criteria:** The measured exterior temperature shall correspond to the reference temperature with a maximum deviation of ±2 °C.
		level: '1.28'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_28/ASSERTIONS-ASSERT_L0_28.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_29 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_29 data-toc-label="ASSUMPTIONS-ASSUMP_L0_29" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}

{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_29](EXPECTATIONS.md#expectations-expect_l0_29) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_29/EXPECTATIONS-EXPECT_L0_29.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_29
		header: "Driver Presence Condition"
		text: |
		  The vehicle shall only accept driving or autonomous commands when a driver is detected as present inside the vehicle.
		
		  **Justification:** Driver presence detection is a common prerequisite in automotive systems to prevent unintended operation. In this project, a symbolic driver (the duck) is used to validate the logical gating of vehicle operation.
		
		  **Acceptance Criteria:**
		  - If no driver is detected, the vehicle shall reject all driving or autonomous commands.
		  - If a driver is detected (Diogo's duck), the vehicle shall accept driving or autonomous commands.
		level: '1.29'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_29/ASSERTIONS-ASSERT_L0_29.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### ASSUMPTIONS-ASSUMP_L0_30 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_30 data-toc-label="ASSUMPTIONS-ASSUMP_L0_30" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}

{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_30](EXPECTATIONS.md#expectations-expect_l0_30) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_30/EXPECTATIONS-EXPECT_L0_30.md`

	??? "Click to view reference"

		````md
		---
		id: EXPECT_L0_30
		header: "Car ready to next module: The vehicle shall be finished to module 2"
		text: |
		  The vehicle shall be finished to module 2.
		
		  **Justification:** Architecture finished with software and hardware ready to next module: AGL, ThreadX, CAN, QT, Camera, Sensors, etc.
		
		  **References:** Car ready to ADAS and Machine Learning and OTA module 2.
		
		  **Acceptance Criteria:** Car should be completely finalized.
		
		  **Verification Method:** Visual, Unity, Functional, Integration tests.
		level: '1.30'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_30/ASSERTIONS-ASSERT_L0_30.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}

**Fallacies:**

_None_
