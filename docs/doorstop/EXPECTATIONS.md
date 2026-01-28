

---

### EXPECTATIONS-EXPECT_L0_1 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_1 data-toc-label="EXPECTATIONS-EXPECT_L0_1" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_1](ASSERTIONS.md#assertions-assert_l0_1) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_1](ASSUMPTIONS.md#assumptions-assump_l0_1) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | Assumption: The project team will provide the required hardware and demonstrator components listed in the system architecture (Expansion Board, Raspberry Pi 5, Hailo Hat, DC motors, servo motor, Qt display/cluster via DSI interface) and these components will be operational in the integration environment during verification activities. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_1/ASSERTIONS-ASSERT_L0_1.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_1
		header: Architecture documentation exists
		text: The repository contains system architecture documentation that describes the hardware components and software modules of the demonstrator car.
		level: '1.1'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_1/EXPECTATIONS-EXPECT_L0_1.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_1/EVIDENCES-EVID_L0_1.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_2 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_2 data-toc-label="EXPECTATIONS-EXPECT_L0_2" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_2](ASSERTIONS.md#assertions-assert_l0_2) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_2](ASSUMPTIONS.md#assumptions-assump_l0_2) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | The required hardware components (Expansion Board, Raspberry Pi 5, Raspberry Pi 4, DC motors, servo motor, display interfaces, chassis) are procured and available before hardware assembly activities begin. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_2/ASSERTIONS-ASSERT_L0_2.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_2
		header: Hardware components assembled
		text: The demonstrator car hardware assembly is complete with all components physically installed and connected according to the documented architecture.
		level: 1.2
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_2/EXPECTATIONS-EXPECT_L0_2.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_2/EVIDENCES-EVID_L0_2.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_3 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_3 data-toc-label="EXPECTATIONS-EXPECT_L0_3" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_3](ASSERTIONS.md#assertions-assert_l0_3) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_3](ASSUMPTIONS.md#assumptions-assump_l0_3) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | Assumption: The system and development tooling target a Linux-based integration environment (Ubuntu 22.04 LTS or compatible). Device drivers and kernel modules required for platform-specific components (e.g., motor controllers, Hailo AI Hat, camera interfaces) are available or will be provided by component owners prior to verification. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_3/ASSERTIONS-ASSERT_L0_3.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_3
		header: Software environment configured
		text: The software development environment is set up on the Raspberry Pi 5 with all required dependencies and tools installed according to the documented procedures.
		level: '1.3'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_3/EXPECTATIONS-EXPECT_L0_3.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_3/EVIDENCES-EVID_L0_3.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_4 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_4 data-toc-label="EXPECTATIONS-EXPECT_L0_4" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_4](ASSERTIONS.md#assertions-assert_l0_4) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_4](ASSUMPTIONS.md#assumptions-assump_l0_4) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | The joystick or remote control hardware is available, functional, and compatible with the target platform input subsystem before remote control integration testing begins. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_4/ASSERTIONS-ASSERT_L0_4.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_4
		header: Remote control operational
		text: The demonstrator car responds to remote control commands for steering and throttle through the implemented joystick interface.
		level: '1.4'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_4/EXPECTATIONS-EXPECT_L0_4.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_4/EVIDENCES-EVID_L0_4.md
		review_status: accepted
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_5 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_5 data-toc-label="EXPECTATIONS-EXPECT_L0_5" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_5](ASSERTIONS.md#assertions-assert_l0_5) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_5](ASSUMPTIONS.md#assumptions-assump_l0_5) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | Qt runtime libraries and dependencies are installed and available on the target platform before Qt application deployment begins. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_5/ASSERTIONS-ASSERT_L0_5.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_5
		header: Basic Qt application launches
		text: A basic Qt QML application successfully launches and displays on the Raspberry Pi 5 using the documented Qt 6 installation.
		level: '1.5'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_5/EXPECTATIONS-EXPECT_L0_5.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_5/EVIDENCES-EVID_L0_5.md
		review_status: accepted
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_6 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_6 data-toc-label="EXPECTATIONS-EXPECT_L0_6" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_6](ASSERTIONS.md#assertions-assert_l0_6) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_6](ASSUMPTIONS.md#assumptions-assump_l0_6) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | Third-party font libraries and icon sets required by the Qt GUI are available under compatible licenses before UI implementation begins. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_6/ASSERTIONS-ASSERT_L0_6.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_6
		header: Static GUI elements displayed
		text: The Qt cluster application displays static visual elements including speedometer, RPM gauge, and status indicators as defined in the QML implementation.
		level: '1.6'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_6/EXPECTATIONS-EXPECT_L0_6.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_6/EVIDENCES-EVID_L0_6.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_7 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_7 data-toc-label="EXPECTATIONS-EXPECT_L0_7" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_7](ASSERTIONS.md#assertions-assert_l0_7) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_7](ASSUMPTIONS.md#assumptions-assump_l0_7) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | GCC ARM64 cross-compiler, CMake, and Qt6 cross-compilation libraries are available and configured on the build host before cross-compilation activities begin. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_7/ASSERTIONS-ASSERT_L0_7.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_7
		header: Cross-compilation builds Qt app
		text: The Qt cluster application successfully builds using the cross-compilation toolchain and runs on the Raspberry Pi 5 target hardware.
		level: '1.7'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_7/EXPECTATIONS-EXPECT_L0_7.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_7/EVIDENCES-EVID_L0_7.md
		review_status: accepted
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_8 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_8 data-toc-label="EXPECTATIONS-EXPECT_L0_8" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_8](ASSERTIONS.md#assertions-assert_l0_8) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_8](ASSUMPTIONS.md#assumptions-assump_l0_8) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | CAN bus hardware interfaces (CAN controllers, transceivers, wiring) are installed and functional on both Raspberry Pi 5 and STM32 before CAN communication testing begins. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_8/ASSERTIONS-ASSERT_L0_8.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_8
		header: CAN messages exchanged
		text: The Raspberry Pi 5 successfully sends and receives CAN messages to and from the STM32 microcontroller using the configured CAN interface.
		level: '1.8'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_8/EXPECTATIONS-EXPECT_L0_8.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_8/EVIDENCES-EVID_L0_8.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_9 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_9 data-toc-label="EXPECTATIONS-EXPECT_L0_9" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_9](ASSERTIONS.md#assertions-assert_l0_9) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_9](ASSUMPTIONS.md#assumptions-assump_l0_9) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | The STM32 microcontroller is programmed and accessible via debug interface (ST-LINK or JTAG) before ThreadX firmware verification begins. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_9/ASSERTIONS-ASSERT_L0_9.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_9
		header: ThreadX operates on STM32
		text: The ThreadX RTOS successfully runs on the STM32 microcontroller and executes scheduled tasks according to the documented configuration.
		level: '1.9'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_9/EXPECTATIONS-EXPECT_L0_9.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_9/EVIDENCES-EVID_L0_9.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_10 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_10 data-toc-label="EXPECTATIONS-EXPECT_L0_10" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_10](ASSERTIONS.md#assertions-assert_l0_10) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_10](ASSUMPTIONS.md#assumptions-assump_l0_10) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | Hardware components (Raspberry Pi 5, Raspberry Pi 4, STM32, Hailo AI Hat, motors, sensors) are procured and available before system architecture implementation begins. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_10/ASSERTIONS-ASSERT_L0_10.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_10
		header: Dual Raspberry Pi architecture operational
		text: The system operates with Raspberry Pi 5 running computational tasks and Raspberry Pi 4 running the display cluster, with wireless communication between them.
		level: '1.10'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_10/EXPECTATIONS-EXPECT_L0_10.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_10/EVIDENCES-EVID_L0_10.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_11 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_11 data-toc-label="EXPECTATIONS-EXPECT_L0_11" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_11](ASSERTIONS.md#assertions-assert_l0_11) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_11](ASSUMPTIONS.md#assumptions-assump_l0_11) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | Both Raspberry Pi 5 (in-car Qt cluster host) and Raspberry Pi 4 (wireless display receiver) use systemd-based init systems that support service auto-start configuration before Qt application boot automation testing begins. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_11/ASSERTIONS-ASSERT_L0_11.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_11
		header: Qt launches automatically
		text: The Qt cluster application automatically launches on both Raspberry Pi 5 and Raspberry Pi 4 during system boot without manual intervention.
		level: '1.11'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_11/EXPECTATIONS-EXPECT_L0_11.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_11/EVIDENCES-EVID_L0_11.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_12 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_12 data-toc-label="EXPECTATIONS-EXPECT_L0_12" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_12](ASSERTIONS.md#assertions-assert_l0_12) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_12](ASSUMPTIONS.md#assumptions-assump_l0_12) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | Raspberry Pi 4 wireless display receiver hardware with WiFi capability and VNC server software (Wayland/VNC stack) are available and functional before wireless display integration testing begins. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_12/ASSERTIONS-ASSERT_L0_12.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_12
		header: Display streams wirelessly
		text: The Qt cluster display content from Raspberry Pi 5 successfully streams to Raspberry Pi 4 wirelessly using VNC protocol with acceptable latency.
		level: '1.12'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_12/EXPECTATIONS-EXPECT_L0_12.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_12/EVIDENCES-EVID_L0_12.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_13 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_13 data-toc-label="EXPECTATIONS-EXPECT_L0_13" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_13](ASSERTIONS.md#assertions-assert_l0_13) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_13](ASSUMPTIONS.md#assumptions-assump_l0_13) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | GitHub repository access and permissions are granted to all project team members before project setup verification begins. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_13/ASSERTIONS-ASSERT_L0_13.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_13
		header: GitHub repository configured
		text: The project GitHub repository is configured with appropriate structure, branch protection, and team access permissions as documented in the setup guide.
		level: '1.13'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_13/EXPECTATIONS-EXPECT_L0_13.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_13/EVIDENCES-EVID_L0_13.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_14 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_14 data-toc-label="EXPECTATIONS-EXPECT_L0_14" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_14](ASSERTIONS.md#assertions-assert_l0_14) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_14](ASSUMPTIONS.md#assumptions-assump_l0_14) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | The Trustable Software Framework (TSF) methodology documentation and reference implementations are publicly accessible online before TSF implementation begins. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_14/ASSERTIONS-ASSERT_L0_14.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_14
		header: TSF framework implemented
		text: The Trustable Safety Framework structure is implemented in the repository with documented requirements, evidences, and traceability links.
		level: '1.14'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_14/EXPECTATIONS-EXPECT_L0_14.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_14/EVIDENCES-EVID_L0_14.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_15 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_15 data-toc-label="EXPECTATIONS-EXPECT_L0_15" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_15](ASSERTIONS.md#assertions-assert_l0_15) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_15](ASSUMPTIONS.md#assumptions-assump_l0_15) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | GitHub Projects and Agile/Scrum methodology documentation are accessible for self-training before agile process adoption begins. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_15/ASSERTIONS-ASSERT_L0_15.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_15
		header: Agile processes active
		text: The team follows Agile development practices including sprint planning, daily standups, and retrospectives as documented in the sprint records.
		level: '1.15'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_15/EXPECTATIONS-EXPECT_L0_15.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_15/EVIDENCES-EVID_L0_15.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_16 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_16 data-toc-label="EXPECTATIONS-EXPECT_L0_16" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_16](ASSERTIONS.md#assertions-assert_l0_16) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_16](ASSUMPTIONS.md#assumptions-assump_l0_16) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | GitHub Actions runner resources and execution quotas are available and sufficient for CI/CD pipeline execution before CI/CD verification begins. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_16/ASSERTIONS-ASSERT_L0_16.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_16
		header: CI/CD pipeline operational
		text: The GitHub Actions CI/CD pipeline successfully runs automated builds and tests on code commits according to the documented workflow configuration.
		level: '1.16'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_16/EXPECTATIONS-EXPECT_L0_16.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_16/EVIDENCES-EVID_L0_16.md
		review_status: accepted
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_17 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_17 data-toc-label="EXPECTATIONS-EXPECT_L0_17" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_17](ASSERTIONS.md#assertions-assert_l0_17) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_17](ASSUMPTIONS.md#assumptions-assump_l0_17) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | The Hailo AI Hat hardware is available, functional, and compatible with the Raspberry Pi 5 before AI model deployment testing begins. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_17/ASSERTIONS-ASSERT_L0_17.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_17
		header: AI tools integrated
		text: The team uses AI-assisted development tools including GitHub Copilot and follows the documented guidelines for AI pair programming practices.
		level: '1.17'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_17/EXPECTATIONS-EXPECT_L0_17.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_17/EVIDENCES-EVID_L0_17.md
		review_status: accepted
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_18 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_18 data-toc-label="EXPECTATIONS-EXPECT_L0_18" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_18](ASSERTIONS.md#assertions-assert_l0_18) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_18](ASSUMPTIONS.md#assumptions-assump_l0_18) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | The development environment meets all prerequisites for implementing and verifying this requirement. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_18/ASSERTIONS-ASSERT_L0_18.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_18
		header: "CAN-ThreadX Integration Verified"
		text: |
		  The CAN bus communication layer is successfully integrated with ThreadX RTOS on the STM32 microcontroller. CAN messages are handled by dedicated ThreadX threads with proper priority scheduling, ensuring real-time response for automotive communication requirements.
		level: "1.18"
		normative: true
		references:
		  - type: file
		    path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_18/EXPECTATIONS-EXPECT_L0_18.md
		  - type: file
		    path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_18/EVIDENCES-EVID_L0_18.md
		reviewers:
		  - name: Joao Jesus Silva
		    email: joao.silva@seame.pt
		review_status: accepted
		---
		
		
		````


- `docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_18/EVIDENCES-EVID_L0_18.md`

	??? "Click to view reference"

		````md
		---
		id: EVID_L0_18
		header: "Evidence: CAN Bus Communication with ThreadX"
		text: |
		  Evidence demonstrating CAN bus communication integrated with ThreadX RTOS on STM32. Includes CAN message traces, ThreadX thread scheduling logs, and integration test results.
		level: "1.18"
		normative: true
		references:
		  - type: file
		    path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_18/EXPECTATIONS-EXPECT_L0_18.md
		  - type: url
		    url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/ThreadXGuide.md
		  - type: url
		    url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/CAN-test-guide.md
		score: 1.0
		reviewers:
		  - name: Joao Jesus Silva
		    email: joao.silva@seame.pt
		review_status: accepted
		---
		
		
		````


- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/ThreadXGuide.md`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/ThreadXGuide.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/ThreadXGuide.md)



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_19 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_19 data-toc-label="EXPECTATIONS-EXPECT_L0_19" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_19](ASSERTIONS.md#assertions-assert_l0_19) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_19](ASSUMPTIONS.md#assumptions-assump_l0_19) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | The development environment meets all prerequisites for implementing and verifying this requirement. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_19/ASSERTIONS-ASSERT_L0_19.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_19
		header: "TSF Validation Automated"
		text: |
		  Automated TSF validation is implemented and running in the CI/CD pipeline. The validation script checks YAML frontmatter structure, required fields, level format, and cross-references between TSF items. Validation results are reported in GitHub Actions workflow runs.
		level: "1.19"
		normative: true
		references:
		  - type: file
		    path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_19/EXPECTATIONS-EXPECT_L0_19.md
		  - type: file
		    path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_19/EVIDENCES-EVID_L0_19.md
		reviewers:
		  - name: Joao Jesus Silva
		    email: joao.silva@seame.pt
		review_status: accepted
		---
		
		
		````


- `docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_19/EVIDENCES-EVID_L0_19.md`

	??? "Click to view reference"

		````md
		---
		id: EVID_L0_19
		header: "Evidence: TSF Automation Validation"
		text: |
		  Evidence demonstrating TSF automation testing functionality. Includes CI/CD workflow execution logs, validation script output, and GitHub Actions run results showing successful TSF item validation.
		level: "1.19"
		normative: true
		references:
		  - type: file
		    path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_19/EXPECTATIONS-EXPECT_L0_19.md
		  - type: url
		    url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/tsf_implementation/README.md
		  - type: url
		    url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/.github/workflows/validate_items_formatation.yml
		score: 1.0
		reviewers:
		  - name: Joao Jesus Silva
		    email: joao.silva@seame.pt
		review_status: accepted
		---
		
		
		````


- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/.github/workflows/tsf-validate.yml`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/.github/workflows/tsf-validate.yml](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/.github/workflows/tsf-validate.yml)



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_20 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_20 data-toc-label="EXPECTATIONS-EXPECT_L0_20" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_20](ASSERTIONS.md#assertions-assert_l0_20) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_20](ASSUMPTIONS.md#assumptions-assump_l0_20) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | The development environment meets all prerequisites for implementing and verifying this requirement. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_20/ASSERTIONS-ASSERT_L0_20.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_20
		header: "AI Content Generation Integrated"
		text: |
		  AI-assisted content generation is integrated into the TSF workflow. The system supports both semi-automated mode (VSCode/Copilot Chat with human confirmation) and fully automated mode (GitHub Copilot CLI). Generated content follows TSF item structure requirements and project documentation standards.
		level: "1.20"
		normative: true
		references:
		  - type: file
		    path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_20/EXPECTATIONS-EXPECT_L0_20.md
		  - type: file
		    path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_20/EVIDENCES-EVID_L0_20.md
		reviewers:
		  - name: Joao Jesus Silva
		    email: joao.silva@seame.pt
		review_status: accepted
		---
		
		
		````


- `docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_20/EVIDENCES-EVID_L0_20.md`

	??? "Click to view reference"

		````md
		---
		id: EVID_L0_20
		header: "Evidence: AI Content Generation Integration"
		text: |
		  Evidence demonstrating AI-assisted content generation in TSF workflow. Includes script execution logs showing Option G (VSCode/Claude) and Option C (gh copilot CLI) integration, generated TSF item samples, and documentation of the AI generation process.
		level: "1.20"
		normative: true
		references:
		  - type: file
		    path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_20/EXPECTATIONS-EXPECT_L0_20.md
		  - type: url
		    url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/genAI-pair-programming-guidelines.md
		  - type: url
		    url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/tsf_implementation/TSF_docs/genAI_on_tsf.md
		score: 1.0
		reviewers:
		  - name: Joao Jesus Silva
		    email: joao.silva@seame.pt
		review_status: accepted
		---
		
		
		````


- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/genAI-pair-programming-guidelines.md`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/genAI-pair-programming-guidelines.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/genAI-pair-programming-guidelines.md)



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_21 | Reviewed: ✔ | Score: 1.0 ### {: #expectations-expect_l0_21 data-toc-label="EXPECTATIONS-EXPECT_L0_21" .item-element .item-section class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_21](ASSERTIONS.md#assertions-assert_l0_21) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_21](ASSUMPTIONS.md#assumptions-assump_l0_21) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | The development environment meets all prerequisites for implementing and verifying this requirement. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_21/ASSERTIONS-ASSERT_L0_21.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_21
		header: "Complete TSF Workflow Operational"
		text: |
		  The unified TSF automation script (open_check_sync_update_validate_run_publish_tsfrequirements.py) is operational and executes the complete workflow: open_check (validation), sync_update (AI-assisted content generation), and validate_run_publish (TruDAG execution). All three phases complete successfully.
		level: "1.21"
		normative: true
		references:
		  - type: file
		    path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_21/EXPECTATIONS-EXPECT_L0_21.md
		  - type: file
		    path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_21/EVIDENCES-EVID_L0_21.md
		reviewers:
		  - name: Joao Jesus Silva
		    email: joao.silva@seame.pt
		review_status: accepted
		---
		
		
		````


- `docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_21/EVIDENCES-EVID_L0_21.md`

	??? "Click to view reference"

		````md
		---
		id: EVID_L0_21
		header: "Evidence: Complete TSF Workflow Execution"
		text: |
		  Evidence demonstrating successful execution of the complete TSF automation workflow. Includes execution logs of open_check_sync_update_validate_run_publish_tsfrequirements.py with --all flag, showing all three phases (check, sync, validate) completing successfully.
		level: "1.21"
		normative: true
		references:
		  - type: file
		    path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_21/EXPECTATIONS-EXPECT_L0_21.md
		  - type: url
		    url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py
		score: 1.0
		reviewers:
		  - name: Joao Jesus Silva
		    email: joao.silva@seame.pt
		review_status: accepted
		---
		
		
		````


- `https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py`

	??? "Click to view reference"

		[https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py)



{% endraw %}

**Fallacies:**

_None_
