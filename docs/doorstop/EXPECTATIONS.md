

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
		text: The repository contains system architecture documentation that describes the
		  hardware components and software modules of the demonstrator car.
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
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
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
		text: The demonstrator car hardware assembly is complete with all components physically
		  installed and connected according to the documented architecture.
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
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
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
		text: The software development environment is set up on the Raspberry Pi 5 with all
		  required dependencies and tools installed according to the documented procedures.
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
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
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
		text: The demonstrator car responds to remote control commands for steering and throttle
		  through the implemented joystick interface.
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
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
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
		text: A basic Qt QML application successfully launches and displays on the Raspberry
		  Pi 5 using the documented Qt 6 installation.
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
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
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
		text: The Qt cluster application displays static visual elements including speedometer,
		  RPM gauge, and status indicators as defined in the QML implementation.
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
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
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
		text: The Qt cluster application successfully builds using the cross-compilation toolchain
		  and runs on the Raspberry Pi 5 target hardware.
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
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
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
		text: The Raspberry Pi 5 successfully sends and receives CAN messages to and from
		  the STM32 microcontroller using the configured CAN interface.
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
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
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
		text: The ThreadX RTOS successfully runs on the STM32 microcontroller and executes
		  scheduled tasks according to the documented configuration.
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
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
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
		text: The system operates with Raspberry Pi 5 running computational tasks and Raspberry
		  Pi 4 running the display cluster, with wireless communication between them.
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
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
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
		text: The Qt cluster application automatically launches on both Raspberry Pi 5 and
		  Raspberry Pi 4 during system boot without manual intervention.
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
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
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
		text: The Qt cluster display content from Raspberry Pi 5 successfully streams to Raspberry
		  Pi 4 wirelessly using VNC protocol with acceptable latency.
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
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
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
		text: The project GitHub repository is configured with appropriate structure, branch
		  protection, and team access permissions as documented in the setup guide.
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
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
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
		text: The Trustable Safety Framework structure is implemented in the repository with
		  documented requirements, evidences, and traceability links.
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
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
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
		text: The team follows Agile development practices including sprint planning, daily
		  standups, and retrospectives as documented in the sprint records.
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
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_16 | Reviewed: ✔ | Score: 0.5 ### {: #expectations-expect_l0_16 data-toc-label="EXPECTATIONS-EXPECT_L0_16" .item-element .item-section class="tsf-score" style="background-color:hsl(60.0, 100%, 47%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_16](ASSERTIONS.md#assertions-assert_l0_16) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_16](ASSUMPTIONS.md#assumptions-assump_l0_16) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | GitHub Actions runner resources and execution quotas are available and sufficient for CI/CD pipeline execution before CI/CD verification begins. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_16/ASSERTIONS-ASSERT_L0_16.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_16
		header: CI/CD pipeline operational
		text: The GitHub Actions CI/CD pipeline successfully runs automated builds and tests
		  on code commits according to the documented workflow configuration.
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
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
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
		text: The team uses AI-assisted development tools including GitHub Copilot and follows
		  the documented guidelines for AI pair programming practices.
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
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_18 | Reviewed: ✔ | Score: 0.5 ### {: #expectations-expect_l0_18 data-toc-label="EXPECTATIONS-EXPECT_L0_18" .item-element .item-section class="tsf-score" style="background-color:hsl(60.0, 100%, 47%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_18](ASSERTIONS.md#assertions-assert_l0_18) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_18](ASSUMPTIONS.md#assumptions-assump_l0_18) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | The development environment meets all prerequisites for implementing and verifying this requirement. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_18/ASSERTIONS-ASSERT_L0_18.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_18
		header: CAN-ThreadX Integration Verified
		text: 'The CAN bus communication layer is successfully integrated with ThreadX RTOS
		  on the STM32 microcontroller. CAN messages are handled by dedicated ThreadX threads
		  with proper priority scheduling, ensuring real-time response for automotive communication
		  requirements.
		
		  '
		level: '1.18'
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
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
		---
		
		
		````



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
		header: TSF Validation Automated
		text: 'Automated TSF validation is implemented and running in the CI/CD pipeline.
		  The validation script checks YAML frontmatter structure, required fields, level
		  format, and cross-references between TSF items. Validation results are reported
		  in GitHub Actions workflow runs.
		
		  '
		level: '1.19'
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
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
		---
		
		
		````



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
		header: AI Content Generation Integrated
		text: 'AI-assisted content generation is integrated into the TSF workflow. The system
		  supports both semi-automated mode (VSCode/Copilot Chat with human confirmation)
		  and fully automated mode (GitHub Copilot CLI). Generated content follows TSF item
		  structure requirements and project documentation standards.
		
		  '
		level: '1.20'
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
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
		---
		
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_21 | Reviewed: ✔ | Score: 0.5 ### {: #expectations-expect_l0_21 data-toc-label="EXPECTATIONS-EXPECT_L0_21" .item-element .item-section class="tsf-score" style="background-color:hsl(60.0, 100%, 47%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_21](ASSERTIONS.md#assertions-assert_l0_21) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_21](ASSUMPTIONS.md#assumptions-assump_l0_21) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} | The development environment meets all prerequisites for implementing and verifying this requirement. | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_21/ASSERTIONS-ASSERT_L0_21.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_21
		header: Complete TSF Workflow Operational
		text: 'The unified TSF automation script (open_check_sync_update_validate_run_publish_tsfrequirements.py)
		  is operational and executes the complete workflow: open_check (validation), sync_update
		  (AI-assisted content generation), and validate_run_publish (TruDAG execution). All
		  three phases complete successfully.
		
		  '
		level: '1.21'
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
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		    - STM32
		    - CAN
		    - Raspberry Pi
		---
		
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_22 | Reviewed: ✔ | Score: 0.5 ### {: #expectations-expect_l0_22 data-toc-label="EXPECTATIONS-EXPECT_L0_22" .item-element .item-section class="tsf-score" style="background-color:hsl(60.0, 100%, 47%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_22](ASSERTIONS.md#assertions-assert_l0_22) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_22](ASSUMPTIONS.md#assumptions-assump_l0_22) {class="tsf-score" style="background-color:hsl(120.0, 100%, 30%)"} |  | 1.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_22/ASSERTIONS-ASSERT_L0_22.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_22
		header: "STM32 ThreadX Deterministic Startup Verification"
		text: |
		  The STM32-based control system running ThreadX has been verified to initialize and start publishing speed data over CAN within the specified deterministic time bound. Startup timing measurements confirm that speed data acquisition and CAN transmission begin within ≤ 100 ms after power-on, meeting AUTOSAR-like ECU startup requirements.
		level: '1.22'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_22/EXPECTATIONS-EXPECT_L0_22.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_22/EVIDENCES-EVID_L0_22.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		      - "STM32"
		      - "CAN"
		      - "Raspberry Pi"
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_23 | Reviewed: ✔ | Score: 0.0 ### {: #expectations-expect_l0_23 data-toc-label="EXPECTATIONS-EXPECT_L0_23" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_23](ASSERTIONS.md#assertions-assert_l0_23) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_23](ASSUMPTIONS.md#assumptions-assump_l0_23) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_23/ASSERTIONS-ASSERT_L0_23.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_23
		header: "Raspberry Pi 5 AGL Boot Time Verification"
		text: |
		  The Raspberry Pi 5–based system running Automotive Grade Linux (AGL) has been verified to complete its operating system initialization and reach middleware operational state within the specified time bound. Boot timing measurements confirm that KUKSA and middleware services become operational within ≤ 10 s after power-on, meeting AGL fast boot requirements for automotive Linux platforms.
		
		  **Verification Method:** Timestamp logging from bootloader, kernel, systemd, and KUKSA service startup. AGL boot logs analysis. External time measurement (video recording from power-on to service availability).
		level: '1.23'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_23/EXPECTATIONS-EXPECT_L0_23.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_23/EVIDENCES-EVID_L0_23.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		      - "Raspberry Pi 5"
		      - "AGL"
		      - "KUKSA"
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_24 | Reviewed: ✔ | Score: 0.0 ### {: #expectations-expect_l0_24 data-toc-label="EXPECTATIONS-EXPECT_L0_24" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_24](ASSERTIONS.md#assertions-assert_l0_24) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_24](ASSUMPTIONS.md#assumptions-assump_l0_24) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_24/ASSERTIONS-ASSERT_L0_24.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_24
		header: "Combined ThreadX AGL Startup Verification"
		text: |
		  The combined STM32 (ThreadX) and Raspberry Pi 5 (AGL) system has been verified to provide integrated startup behavior where safety-relevant data is available independently of AGL boot completion. Testing confirms that speed data publication from STM32 is available within ≤ 100 ms, AGL becomes operational within ≤ 10 s, and no startup conflicts occur between the ThreadX and AGL domains.
		
		  **Verification Method:** Integration test. CAN traffic analysis during AGL boot. Correlation of STM32 CAN timestamps with AGL service startup logs.
		level: '1.24'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_24/EXPECTATIONS-EXPECT_L0_24.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_24/EVIDENCES-EVID_L0_24.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		      - "STM32"
		      - "ThreadX"
		      - "Raspberry Pi 5"
		      - "AGL"
		      - "CAN"
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_25 | Reviewed: ✔ | Score: 0.0 ### {: #expectations-expect_l0_25 data-toc-label="EXPECTATIONS-EXPECT_L0_25" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_25](ASSERTIONS.md#assertions-assert_l0_25) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_25](ASSUMPTIONS.md#assumptions-assump_l0_25) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_25/ASSERTIONS-ASSERT_L0_25.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_25
		header: "Instrument Cluster Qt UI Verification"
		text: |
		  The instrument cluster system running a Qt-based application on Raspberry Pi 4 has been verified to display an initial usable UI within the specified time bound. Testing confirms that the initial instrument cluster image is displayed within ≤ 2.0 s after power-on, the Qt application operates independently of backend data availability, and vehicle data values are displayed once published by KUKSA.
		
		  **Verification Method:** Integration test. External time measurement using video recording. Qt application startup logs. Framebuffer / display initialization logs.
		level: '1.25'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_25/EXPECTATIONS-EXPECT_L0_25.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_25/EVIDENCES-EVID_L0_25.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		      - "Raspberry Pi 4"
		      - "Qt"
		      - "Instrument Cluster"
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_26 | Reviewed: ✔ | Score: 0.0 ### {: #expectations-expect_l0_26 data-toc-label="EXPECTATIONS-EXPECT_L0_26" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_26](ASSERTIONS.md#assertions-assert_l0_26) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_26](ASSUMPTIONS.md#assumptions-assump_l0_26) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_26/ASSERTIONS-ASSERT_L0_26.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_26
		header: "End-to-End Startup Time Verification"
		text: |
		  The complete system has been verified to provide end-to-end availability of safety-relevant data and a usable instrument cluster UI within the specified time bounds. Testing confirms that safety-relevant speed data is available within ≤ 100 ms, instrument cluster UI is displayed within ≤ 2.0 s, and backend services (AGL + KUKSA) become fully operational within ≤ 10 s without impacting safety or UI availability.
		
		  **Verification Method:** End-to-end integration test. External time measurement (video recording from power-on to UI display). System logs from STM32, AGL, and Qt. CAN traffic correlation with UI updates.
		level: '1.26'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_26/EXPECTATIONS-EXPECT_L0_26.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_26/EVIDENCES-EVID_L0_26.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		      - "STM32"
		      - "ThreadX"
		      - "Raspberry Pi 5"
		      - "AGL"
		      - "Raspberry Pi 4"
		      - "Qt"
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_27 | Reviewed: ✔ | Score: 0.0 ### {: #expectations-expect_l0_27 data-toc-label="EXPECTATIONS-EXPECT_L0_27" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_27](ASSERTIONS.md#assertions-assert_l0_27) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_27](ASSUMPTIONS.md#assumptions-assump_l0_27) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_27/ASSERTIONS-ASSERT_L0_27.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_27
		header: "Emergency Braking Distance Verification"
		text: |
		  The emergency braking system has been verified to actuate correctly and stop the vehicle at a safe distance from detected obstacles. Testing confirms that given an obstacle detected at 300 mm from the sensor, the vehicle stops with a minimum remaining distance of ≥ 50 mm, and braking action is triggered automatically once the obstacle detection threshold is reached.
		
		  **Verification Method:** Integration test. Measure the distance between the vehicle and the obstacle after full stop. Video recording of the test execution.
		level: '1.27'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_27/EXPECTATIONS-EXPECT_L0_27.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_27/EVIDENCES-EVID_L0_27.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		      - "Emergency Braking System"
		      - "Obstacle Detection Sensor"
		      - "Vehicle Platform"
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_28 | Reviewed: ✔ | Score: 0.0 ### {: #expectations-expect_l0_28 data-toc-label="EXPECTATIONS-EXPECT_L0_28" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_28](ASSERTIONS.md#assertions-assert_l0_28) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_28](ASSUMPTIONS.md#assumptions-assump_l0_28) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_28/ASSERTIONS-ASSERT_L0_28.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_28
		header: "Temperature Measurement Accuracy Verification"
		text: |
		  The system has been verified to measure the exterior temperature with accuracy suitable for vehicle status display. Testing confirms that the measured exterior temperature corresponds to the reference temperature with a maximum deviation of ±2 °C, meeting automotive ambient temperature sensing requirements.
		
		  **Verification Method:** Integration test. Measure the actual exterior temperature using a calibrated thermometer. Compare the reference value with the system-reported temperature. Video or image evidence of the measurement setup.
		level: '1.28'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_28/EXPECTATIONS-EXPECT_L0_28.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_28/EVIDENCES-EVID_L0_28.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		      - "Temperature Sensor"
		      - "STM32"
		      - "Calibrated Thermometer"
		---
		
		````



{% endraw %}

**Fallacies:**

_None_


---

### EXPECTATIONS-EXPECT_L0_29 | Reviewed: ✔ | Score: 0.0 ### {: #expectations-expect_l0_29 data-toc-label="EXPECTATIONS-EXPECT_L0_29" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}

{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [ASSERTIONS-ASSERT_L0_29](ASSERTIONS.md#assertions-assert_l0_29) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |
| [ASSUMPTIONS-ASSUMP_L0_29](ASSUMPTIONS.md#assumptions-assump_l0_29) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

{% raw %}

**References:**

- `docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_29/ASSERTIONS-ASSERT_L0_29.md`

	??? "Click to view reference"

		````md
		---
		id: ASSERT_L0_29
		header: "Driver Presence Condition Verification"
		text: |
		  The vehicle has been verified to only accept driving or autonomous commands when a driver is detected as present inside the vehicle. Testing confirms that the vehicle rejects all driving or autonomous commands when no driver is detected, and accepts commands when the driver (Diogo's duck) is detected, meeting functional safety principles for prevention of unintended actuation.
		
		  **Verification Method:** Integration test. Execute command attempts with and without the driver present. Video recording showing system behavior in both cases.
		level: '1.29'
		normative: true
		references:
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_29/EXPECTATIONS-EXPECT_L0_29.md
		- type: file
		  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_29/EVIDENCES-EVID_L0_29.md
		reviewers:
		- name: Joao Jesus Silva
		  email: joao.silva@seame.pt
		review_status: accepted
		evidence:
		  type: validate_hardware_availability
		  configuration:
		    components:
		      - "Driver Presence Sensor"
		      - "Vehicle Control System"
		      - "Diogo's Duck"
		---
		
		````



{% endraw %}

**Fallacies:**

_None_
