---
id: EVID-L0-23
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
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint0.jpeg
  description: 'Evidence from docs/demos/Validation-of-energy-layout-sprint0.jpeg:
    Validation of energy layout sprint0'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/rpi5-thermal-stress-test-.png
  description: 'Evidence from docs/demos/rpi5-thermal-stress-test-.png: rpi5 thermal
    stress test '
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint3.jpeg
  description: 'Evidence from docs/demos/Validation-of-energy-layout-sprint3.jpeg:
    Validation of energy layout sprint3'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint2.jpeg
  description: 'Evidence from docs/demos/Validation-of-energy-layout-sprint2.jpeg:
    Validation of energy layout sprint2'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint1.jpeg
  description: 'Evidence from docs/demos/Validation-of-energy-layout-sprint1.jpeg:
    Validation of energy layout sprint1'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/AGL_and_ThreadX_benefits_Guide.md
  description: 'Evidence from docs/guides/AGL_and_ThreadX_benefits_Guide.md: AGL and
    ThreadX benefits Guide'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/AGL_Installation_Guide.md
  description: 'Evidence from docs/guides/AGL_Installation_Guide.md: AGL Installation
    Guide'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/AGL_
    Joystick_installation_and_test.md
  description: 'Evidence from docs/guides/AGL_ Joystick_installation_and_test.md:
    AGL  Joystick installation and test'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/AGL_Configuration_Guide.md
  description: 'Evidence from docs/guides/AGL_Configuration_Guide.md: AGL Configuration
    Guide'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Power
    Consumption Analysis.md
  description: 'Evidence from docs/guides/Power Consumption Analysis.md: Power Consumption
    Analysis'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/setup/toolchains/agl-toolchain.cmake
  description: 'Evidence from src/setup/toolchains/agl-toolchain.cmake: agl toolchain'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/setup/toolchains/r5-agl-toolchain.cmake
  description: 'Evidence from src/setup/toolchains/r5-agl-toolchain.cmake: r5 agl
    toolchain'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/ota/rauc/bootloader-custom-backend.sh
  description: 'Evidence from src/ota/rauc/bootloader-custom-backend.sh: bootloader
    custom backend'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/ota/rauc/post-reboot-verify.sh
  description: 'Evidence from src/ota/rauc/post-reboot-verify.sh: post reboot verify'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/shared/backend/providers/powertrainprovider.hpp
  description: 'Evidence from src/shared/backend/providers/powertrainprovider.hpp:
    powertrainprovider'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/shared/backend/providers/powertrainprovider.cpp
  description: 'Evidence from src/shared/backend/providers/powertrainprovider.cpp:
    powertrainprovider'
score: 1.0
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: pending
---
This evidence item collects boot logs, timing measurements, and video recordings that demonstrate the Raspberry Pi 5 AGL boot time requirement is met.

**Evidence Status:** Pending - timing measurements to be collected during hardware integration testing.
