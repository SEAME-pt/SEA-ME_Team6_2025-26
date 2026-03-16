

---

### ASSUMPTIONS-ASSUMP_L0_1 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_1 data-toc-label="ASSUMPTIONS-ASSUMP_L0_1" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
Assumption: The project team will provide the required hardware and demonstrator components listed in the system architecture (Expansion Board, Raspberry Pi 5, Hailo Hat, DC motors, servo motor, Qt display/cluster via DSI interface) and these components will be operational in the integration environment during verification activities.

Acceptance criteria / notes:
- Hardware units and connectors are available and powered in the test environment when required by integration tests.
- Device-specific drivers or firmware are available or will be provided by the component owner prior to verification.
- If a component is unavailable, a documented mitigation plan (substitute hardware or simulator) will be provided and linked in the corresponding EVID item.

Rationale: verification activities and evidence collection for `EXPECT-L0-1` depend on access to the stated hardware; recording this as an assumption highlights a test prerequisite that can be tracked and validated.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_1.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-1.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_2 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_2 data-toc-label="ASSUMPTIONS-ASSUMP_L0_2" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
The required hardware components (Expansion Board, Raspberry Pi 5, Raspberry Pi 4, DC motors, servo motor, display interfaces, chassis) are procured and available before hardware assembly activities begin.

Acceptance criteria / notes:
- All hardware components have been purchased or provided by the organization.
- Components are received and physically available in the project workspace.
- Component specifications match the system architecture requirements.

Rationale: Hardware assembly for `EXPECT-L0-2` depends on external procurement of components. This assumption makes explicit that hardware availability is a prerequisite for assembly work.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_2.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-2.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_3 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_3 data-toc-label="ASSUMPTIONS-ASSUMP_L0_3" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
Assumption: The system and development tooling target a Linux-based integration environment (Ubuntu 22.04 LTS or compatible). Device drivers and kernel modules required for platform-specific components (e.g., motor controllers, Hailo AI Hat, camera interfaces) are available or will be provided by component owners prior to verification.

Acceptance criteria / notes:
- A VM or physical host running Ubuntu 22.04 is available for integration tests.
- Required kernel modules and device drivers are installed and documented in the corresponding EVID items.
- If a driver is unavailable, an alternative (simulator or stub) will be provided and documented before verification begins.

Rationale: Many verification steps and evidence artifacts assume POSIX/Linux tooling (bash, apt, systemd, udev rules). Making this explicit avoids ambiguous environment dependency during testing.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_3.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-3.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_4 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_4 data-toc-label="ASSUMPTIONS-ASSUMP_L0_4" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
The joystick or remote control hardware is available, functional, and compatible with the target platform input subsystem before remote control integration testing begins.

Acceptance criteria / notes:
- Joystick device is recognized by the Linux input subsystem (e.g., /dev/input/js0).
- Device drivers are installed and documented.
- Input mapping configuration files are available or will be provided before testing.

Rationale: Remote control verification for `EXPECT-L0-4` depends on external input hardware availability. This assumption clarifies that hardware procurement and driver availability are prerequisites.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_4.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-4.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_5 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_5 data-toc-label="ASSUMPTIONS-ASSUMP_L0_5" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
Qt runtime libraries and dependencies are installed and available on the target platform before Qt application deployment begins.

Acceptance criteria / notes:
- Qt6 runtime is installed (e.g., via apt or compiled from source).
- Required Qt modules (QtCore, QtGui, QtWidgets, QtQml) are present.
- Display server (Wayland or X11) is configured and operational.

Rationale: Qt application verification for `EXPECT-L0-5` assumes that the Qt framework and runtime environment are already provisioned on the target system.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_5.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-5.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_6 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_6 data-toc-label="ASSUMPTIONS-ASSUMP_L0_6" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
Third-party font libraries and icon sets required by the Qt GUI are available under compatible licenses before UI implementation begins.

Acceptance criteria / notes:
- External font files (e.g., TrueType, OpenType) are downloaded or accessible.
- Icon sets (e.g., Font Awesome, Material Icons) are available with compatible licenses.
- License terms allow redistribution and use in the project context.

Rationale: Qt GUI implementation for `EXPECT-L0-6` depends on external font and icon libraries being legally available. This assumption clarifies that third-party asset procurement and licensing are prerequisites.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_6.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-6.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_7 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_7 data-toc-label="ASSUMPTIONS-ASSUMP_L0_7" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
GCC ARM64 cross-compiler, CMake, and Qt6 cross-compilation libraries are available and configured on the build host before cross-compilation activities begin.

Acceptance criteria / notes:
- GCC/Clang ARM64 cross-compiler is installed and accessible.
- CMake toolchain file is configured with correct sysroot and compiler paths.
- Qt6 cross-compiled libraries are available or will be built as part of setup.

Rationale: Cross-compilation for `EXPECT-L0-7` depends on external toolchain components (GCC, CMake) being available on the build host. This assumption makes toolchain provisioning prerequisites explicit.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_7.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-7.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_8 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_8 data-toc-label="ASSUMPTIONS-ASSUMP_L0_8" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
CAN bus hardware interfaces (CAN controllers, transceivers, wiring) are installed and functional on both Raspberry Pi 5 and STM32 before CAN communication testing begins.

Acceptance criteria / notes:
- CAN interface is recognized by Linux (e.g., can0 device exists).
- CAN bus wiring is correctly connected between Rasp5 and STM32.
- CAN bus termination resistors are properly installed.

Rationale: CAN communication verification for `EXPECT-L0-8` depends on physical CAN hardware availability and correct installation. This assumption makes hardware prerequisites explicit.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_8.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-8.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_9 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_9 data-toc-label="ASSUMPTIONS-ASSUMP_L0_9" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
The STM32 microcontroller is programmed and accessible via debug interface (ST-LINK or JTAG) before ThreadX firmware verification begins.

Acceptance criteria / notes:
- ST-LINK debugger hardware is available and connected to the STM32.
- STM32CubeProgrammer or OpenOCD is installed on the development host.
- Firmware flashing and debugging procedures are documented.

Rationale: ThreadX verification for `EXPECT-L0-9` assumes that STM32 programming infrastructure (debugger, flashing tools) is already available and operational.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_9.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-9.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_10 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_10 data-toc-label="ASSUMPTIONS-ASSUMP_L0_10" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
Hardware components (Raspberry Pi 5, Raspberry Pi 4, STM32, Hailo AI Hat, motors, sensors) are procured and available before system architecture implementation begins.

Acceptance criteria / notes:
- All hardware components listed in the architecture have been purchased or provided.
- Components are received and physically available in the project workspace.
- Component specifications match the planned architecture requirements.

Rationale: Architecture implementation for `EXPECT-L0-10` depends on hardware availability. This assumption makes explicit that hardware procurement is a prerequisite for architecture realization.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_10.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-10.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_11 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_11 data-toc-label="ASSUMPTIONS-ASSUMP_L0_11" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
Both Raspberry Pi 5 (in-car Qt cluster host) and Raspberry Pi 4 (wireless display receiver) use systemd-based init systems that support service auto-start configuration before Qt application boot automation testing begins.

Acceptance criteria / notes:
- Systemd is installed and operational on both Raspberry Pi 5 and Raspberry Pi 4.
- Service file creation and enabling mechanisms are documented for both systems.
- Display managers are configured for auto-login on both Rasp5 and Rasp4.

Rationale: Auto-start verification for `EXPECT-L0-11` assumes systemd availability on both Raspberry Pi 5 (Qt cluster host) and Raspberry Pi 4 (wireless display receiver). This assumption makes the dual-system init configuration dependency explicit.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_11.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-11.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_12 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_12 data-toc-label="ASSUMPTIONS-ASSUMP_L0_12" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
Raspberry Pi 4 wireless display receiver hardware with WiFi capability and VNC server software (Wayland/VNC stack) are available and functional before wireless display integration testing begins.

Acceptance criteria / notes:
- Raspberry Pi 4 with WiFi module is procured and configured.
- Wayland display server and VNC server (e.g., wayvnc) are installed on Raspberry Pi 5.
- VNC client software is installed on Raspberry Pi 4 for receiving the stream.
- Network configuration allows Rasp5-to-Rasp4 VNC communication over WiFi.

Rationale: Wireless display verification for `EXPECT-L0-12` depends on Raspberry Pi 4 hardware availability and VNC/Wayland software stack being operational. This assumption clarifies the Rasp5-to-Rasp4 streaming architecture.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_12.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-12.md
    ````



??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_12.md' of reference type 'file': Cannot get non-existent or non-regular file ../evidences/EVID-L0-12.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_13 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_13 data-toc-label="ASSUMPTIONS-ASSUMP_L0_13" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
GitHub repository access and permissions are granted to all project team members before project setup verification begins.

Acceptance criteria / notes:
- All team members have GitHub accounts.
- Repository permissions (read, write, admin) are configured according to roles.
- Branch protection rules are documented.

Rationale: Project setup verification for `EXPECT-L0-13` assumes that access control and team onboarding are handled externally to the technical verification process.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_13.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-13.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_14 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_14 data-toc-label="ASSUMPTIONS-ASSUMP_L0_14" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
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

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_14.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-14.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_15 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_15 data-toc-label="ASSUMPTIONS-ASSUMP_L0_15" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
GitHub Projects and Agile/Scrum methodology documentation are accessible for self-training before agile process adoption begins.

Acceptance criteria / notes:
- GitHub Projects is enabled and accessible for the repository.
- Agile and Scrum training materials (online resources, books, documentation) are accessible to team members.
- Team members complete individual self-training on Agile/Scrum methodologies.
- Sprint schedules and ceremonies (standup, review, retrospective) are defined by the team.

Rationale: Agile process adoption for `EXPECT-L0-15` depends on GitHub Projects tooling availability and individual self-training on Agile/Scrum methodologies. This assumption clarifies that the project uses GitHub Projects exclusively and relies on self-directed learning.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_15.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-15.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_16 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_16 data-toc-label="ASSUMPTIONS-ASSUMP_L0_16" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
GitHub Actions runner resources and execution quotas are available and sufficient for CI/CD pipeline execution before CI/CD verification begins.

Acceptance criteria / notes:
- GitHub Actions is enabled for the repository.
- Execution minutes quota is sufficient for planned build/test workloads.
- Self-hosted runners (if used) are configured and operational.

Rationale: CI/CD verification for `EXPECT-L0-16` assumes that GitHub Actions infrastructure and quota allocation are provided externally to the project.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_16.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-16.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_17 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_17 data-toc-label="ASSUMPTIONS-ASSUMP_L0_17" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
The Hailo AI Hat hardware is available, functional, and compatible with the Raspberry Pi 5 before AI model deployment testing begins.

Acceptance criteria / notes:
- Hailo AI Hat is physically installed on the Raspberry Pi 5.
- Hailo runtime and drivers are installed and operational.
- Pre-trained AI models (if required) are available or documented.

Rationale: AI integration verification for `EXPECT-L0-17` depends on Hailo AI Hat hardware availability and driver installation, which are external dependencies.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_17.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-17.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_18 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_18 data-toc-label="ASSUMPTIONS-ASSUMP_L0_18" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
The development environment meets all prerequisites for implementing and verifying this requirement.

**Acceptance criteria / notes:**
- Required components are available and configured in the test environment.
- If a component is unavailable, a documented mitigation plan will be provided.
- This assumption is validated by the `validate_linux_environment` validator.

**Rationale:** Verification activities for `EXPECT-L0-18` depend on these prerequisites being met.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_18.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-18.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_19 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_19 data-toc-label="ASSUMPTIONS-ASSUMP_L0_19" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
The development environment meets all prerequisites for implementing and verifying this requirement.

**Acceptance criteria / notes:**
- Required components are available and configured in the test environment.
- If a component is unavailable, a documented mitigation plan will be provided.
- This assumption is validated by the `validate_linux_environment` validator.

**Rationale:** Verification activities for `EXPECT-L0-19` depend on these prerequisites being met.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_19.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-19.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_20 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_20 data-toc-label="ASSUMPTIONS-ASSUMP_L0_20" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
The development environment meets all prerequisites for implementing and verifying this requirement.

**Acceptance criteria / notes:**
- Required components are available and configured in the test environment.
- If a component is unavailable, a documented mitigation plan will be provided.
- This assumption is validated by the `validate_linux_environment` validator.

**Rationale:** Verification activities for `EXPECT-L0-20` depend on these prerequisites being met.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_20.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-20.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_21 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_21 data-toc-label="ASSUMPTIONS-ASSUMP_L0_21" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}
The development environment meets all prerequisites for implementing and verifying this requirement.

**Acceptance criteria / notes:**
- Required components are available and configured in the test environment.
- If a component is unavailable, a documented mitigation plan will be provided.
- This assumption is validated by the `validate_linux_environment` validator.

**Rationale:** Verification activities for `EXPECT-L0-21` depend on these prerequisites being met.
{: .expanded-item-element }

**Supported Requests:**

_None_

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_21.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-21.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_22 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_22 data-toc-label="ASSUMPTIONS-ASSUMP_L0_22" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}

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
		    path: ../assertions/ASSERT-L0-22.md
		reviewers:
		  - name: Joao Jesus Silva
		    email: joao.silva@seame.pt
		review_status: accepted
		---
		
		````



{% endraw %}


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

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_23.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-23.md
    ````



{% endraw %}


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

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_24.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-24.md
    ````



{% endraw %}


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

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_25.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-25.md
    ````



{% endraw %}


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

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_26.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-26.md
    ````



{% endraw %}


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

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_27.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-27.md
    ````



{% endraw %}


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

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_28.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-28.md
    ````



{% endraw %}


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

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_29.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-29.md
    ````



{% endraw %}


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

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_30.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-30.md
    ````



{% endraw %}


---

### ASSUMPTIONS-ASSUMP_L0_31 | Reviewed: ✔ | Score: 0.0 ### {: #assumptions-assump_l0_31 data-toc-label="ASSUMPTIONS-ASSUMP_L0_31" .item-element .item-section class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"}

{: .expanded-item-element }

**Supported Requests:**

| Item {style="width:25%"} | Summary {style="width:50%"} | Score {style="width:0%"} | Status {style="width:25%"} |
| --- | --- | --- | --- |
| [EXPECTATIONS-EXPECT_L0_31](EXPECTATIONS.md#expectations-expect_l0_31) {class="tsf-score" style="background-color:hsl(0.0, 100%, 65%)"} |  | 0.00 | ✔ Item Reviewed<br>✔ Link Reviewed |

**Supporting Items:**

_None_

{% raw %}

**References:**

??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_31.md' of reference type 'file': Cannot get non-existent or non-regular file ../expectations/EXPECT-L0-31.md
    ````



??? failure "Click to view reference logs"
    ````md
    Error in item 'ASSUMPTIONS-ASSUMP_L0_31.md' of reference type 'url': Could not fetch URL https://rauc.readthedocs.io/: <urlopen error [SSL: CERTIFICATE_VERIFY_FAILED] certificate verify failed: unable to get local issuer certificate (_ssl.c:1016)>
    ````



{% endraw %}
