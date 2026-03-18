---
id: EVID_L0_23
header: Raspberry Pi 5 AGL Boot Time Evidence
text: 'Evidence demonstrating that the Raspberry Pi 5–based system running Automotive
  Grade Linux (AGL) completes operating system initialization and reaches middleware
  operational state within ≤ 10 s after power-on.


  **Verification Method:** Timestamp logging from bootloader, kernel, systemd, and
  KUKSA service startup. AGL boot logs analysis. External time measurement (video
  recording from power-on to service availability).


  **Expected Artifacts:**

  - AGL boot logs with timestamps

  - Systemd service startup timing analysis

  - KUKSA service availability timestamps

  - Video recording from power-on to middleware availability

  '
level: '1.23'
normative: true
references:
- type: file
  path: docs/guides/AGL_and_ThreadX_benefits_Guide.md
- type: file
  path: docs/guides/AGL_Installation_Guide.md
- type: file
  path: docs/guides/AGL_ Joystick_installation_and_test.md
- type: file
  path: docs/guides/AGL_Configuration_Guide.md
- type: file
  path: docs/guides/Power Consumption Analysis.md
- type: file
  path: src/setup/toolchains/agl-toolchain.cmake
- type: file
  path: src/setup/toolchains/r5-agl-toolchain.cmake
- type: file
  path: src/ota/rauc/bootloader-custom-backend.sh
- type: file
  path: src/ota/rauc/post-reboot-verify.sh
- type: file
  path: src/shared/backend/providers/powertrainprovider.hpp
- type: file
  path: src/shared/backend/providers/powertrainprovider.cpp
score: 1.0
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: pending
---
This evidence item collects boot logs, timing measurements, and video recordings that demonstrate the Raspberry Pi 5 AGL boot time requirement is met.

**Evidence Status:** Pending - timing measurements to be collected during hardware integration testing.
