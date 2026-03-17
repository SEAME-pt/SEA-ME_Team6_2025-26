---
id: EVID-L0-31
header: OTA Update System Implementation - Evidence (SOTA, COTA, FOTA, RAUC)
text: "Evidence demonstrating that the comprehensive OTA update system is implemented\
  \ and functional, covering SOTA, COTA, FOTA, and RAUC OS updates.\n\n**Verification\
  \ Method:** \n- Integration test: SOTA application updates\n- Integration test:\
  \ COTA configuration changes\n- Integration test: FOTA firmware updates\n- Integration\
  \ test: RAUC bundle validation, installation, slot switching\n- Health check execution\
  \ verification\n- Rollback verification on simulated failure\n- Logs from OTA scripts,\
  \ RAUC, bootloader, and health check scripts\n\n**Expected Artifacts:**\n\n**SOTA\
  \ (Software OTA):**\n- Application update scripts\n- Package management configuration\n\
  - Update verification logs\n\n**COTA (Configuration OTA):**\n- Configuration management\
  \ scripts\n- Parameter files and templates\n- Configuration persistence validation\n\
  \n**FOTA (Firmware OTA):**\n- STM32/ECU firmware update scripts\n- CAN-based firmware\
  \ flash utilities\n- Firmware version verification logs\n\n**RAUC OS Updates:**\n\
  - RAUC configuration files (system.conf)\n- RAUC bundle creation scripts and signed\
  \ bundles (.raucb)\n- Installation script (install-bundle.sh)\n- Post-reboot health\
  \ check script (post-reboot-verify.sh)\n- Test logs showing successful bundle installation\n\
  - Test logs showing automatic slot switching\n- Test logs showing rollback on failure\
  \ scenario\n- Documentation of A/B partition layout\n- Evidence of data persistence\
  \ across updates\n"
level: '1.31'
normative: true
references:
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/src/ota/rauc
  description: RAUC implementation scripts
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/ota/rauc/install-bundle.sh
  description: Bundle installation script
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/ota/rauc/post-reboot-verify.sh
  description: Post-reboot health check script
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/OTA/RAUC-implementation.md
  description: RAUC implementation documentation
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/OTA-Action.png
  description: OTA GitHub Action Workflow screenshot
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/OTA-Tags.png
  description: OTA Release Tags screenshot
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/OTA-Release.png
  description: OTA Release Page screenshot
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/OTA-r4-script.jpeg
  description: OTA Script Running on Raspberry Pi 4
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/OTA-r5-script.jpeg
  description: OTA Script Running on Raspberry Pi 5
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/assembled-car2.jpeg
  description: 'Evidence from docs/demos/assembled-car2.jpeg: assembled car2'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/3d-car.jpg
  description: 'Evidence from docs/demos/3d-car.jpg: 3d car'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/car_architecture_new_2
    - sprint6.jpg
  description: 'Evidence from docs/demos/car_architecture_new_2 - sprint6.jpg: car
    architecture new 2   sprint6'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/car_architecture_old
    - sprint6.jpeg
  description: 'Evidence from docs/demos/car_architecture_old - sprint6.jpeg: car
    architecture old   sprint6'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/CAN Scheme
    - New update.png
  description: 'Evidence from docs/demos/CAN Scheme - New update.png: CAN Scheme   New
    update'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/crosscompiling_scheme-sprint6.jpeg
  description: 'Evidence from docs/demos/crosscompiling_scheme-sprint6.jpeg: crosscompiling
    scheme sprint6'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/car_architecture_new_1
    - sprint6.jpg
  description: 'Evidence from docs/demos/car_architecture_new_1 - sprint6.jpg: car
    architecture new 1   sprint6'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint0.jpeg
  description: 'Evidence from docs/demos/Validation-of-energy-layout-sprint0.jpeg:
    Validation of energy layout sprint0'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/3d-car2.jpg
  description: 'Evidence from docs/demos/3d-car2.jpg: 3d car2'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/system-architecture-cross.png
  description: 'Evidence from docs/demos/system-architecture-cross.png: system architecture
    cross'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint3.jpeg
  description: 'Evidence from docs/demos/Validation-of-energy-layout-sprint3.jpeg:
    Validation of energy layout sprint3'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/3d-car3.jpg
  description: 'Evidence from docs/demos/3d-car3.jpg: 3d car3'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/CAN-layout-sprint3.jpeg
  description: 'Evidence from docs/demos/CAN-layout-sprint3.jpeg: CAN layout sprint3'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/assembled-car1.jpeg
  description: 'Evidence from docs/demos/assembled-car1.jpeg: assembled car1'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/eletric-scheme.png
  description: 'Evidence from docs/demos/eletric-scheme.png: eletric scheme'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint2.jpeg
  description: 'Evidence from docs/demos/Validation-of-energy-layout-sprint2.jpeg:
    Validation of energy layout sprint2'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/CANbus
    Scheme Updated.png
  description: 'Evidence from docs/demos/CANbus Scheme Updated.png: CANbus Scheme
    Updated'
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
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/OTA/OTA_Presentation-sprint8.md
  description: 'Evidence from docs/guides/OTA/OTA_Presentation-sprint8.md: OTA Presentation
    sprint8'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/OTA/OTA_Comparison_Tests.md
  description: 'Evidence from docs/guides/OTA/OTA_Comparison_Tests.md: OTA Comparison
    Tests'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/OTA/OTA_Implementation_Guide.md
  description: 'Evidence from docs/guides/OTA/OTA_Implementation_Guide.md: OTA Implementation
    Guide'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/OTA/OTA_multiversion-arch-sprint8.md
  description: 'Evidence from docs/guides/OTA/OTA_multiversion-arch-sprint8.md: OTA
    multiversion arch sprint8'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Car_Architecture/Circuit_Connection_Diagram.md
  description: 'Evidence from docs/guides/Car_Architecture/Circuit_Connection_Diagram.md:
    Circuit Connection Diagram'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/images/CanFeederArchitecture.jpg
  description: 'Evidence from docs/images/CanFeederArchitecture.jpg: CanFeederArchitecture'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/setup/toolchains/agl-toolchain.cmake
  description: 'Evidence from src/setup/toolchains/agl-toolchain.cmake: agl toolchain'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/setup/toolchains/r5-agl-toolchain.cmake
  description: 'Evidence from src/setup/toolchains/r5-agl-toolchain.cmake: r5 agl
    toolchain'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/ota/rauc/create-bundle.sh
  description: 'Evidence from src/ota/rauc/create-bundle.sh: create bundle'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/ota/rauc/bootloader-custom-backend.sh
  description: 'Evidence from src/ota/rauc/bootloader-custom-backend.sh: bootloader
    custom backend'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/ota/rauc/setup-rauc.sh
  description: 'Evidence from src/ota/rauc/setup-rauc.sh: setup rauc'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/ota/scripts/ota-update.sh
  description: 'Evidence from src/ota/scripts/ota-update.sh: ota update'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/ota/scripts/ota-check.sh
  description: 'Evidence from src/ota/scripts/ota-check.sh: ota check'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/ota/scripts/setup-ota-device.sh
  description: 'Evidence from src/ota/scripts/setup-ota-device.sh: setup ota device'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/ota/scripts/benchmark-ota.sh
  description: 'Evidence from src/ota/scripts/benchmark-ota.sh: benchmark ota'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/hmi/assets/icons/car-inactive.svg
  description: 'Evidence from src/hmi/assets/icons/car-inactive.svg: car inactive'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/hmi/assets/icons/car-active.svg
  description: 'Evidence from src/hmi/assets/icons/car-active.svg: car active'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/shared/backend/providers/vehicleprovider.hpp
  description: 'Evidence from src/shared/backend/providers/vehicleprovider.hpp: vehicleprovider'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/shared/backend/providers/vehicleprovider.cpp
  description: 'Evidence from src/shared/backend/providers/vehicleprovider.cpp: vehicleprovider'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Middlewares/ST/threadx/common/src/txe_byte_release.c
  description: 'Evidence from src/stm/Middlewares/ST/threadx/common/src/txe_byte_release.c:
    txe byte release'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Middlewares/ST/threadx/common/src/tx_byte_release.c
  description: 'Evidence from src/stm/Middlewares/ST/threadx/common/src/tx_byte_release.c:
    tx byte release'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Middlewares/ST/threadx/common/src/txe_block_release.c
  description: 'Evidence from src/stm/Middlewares/ST/threadx/common/src/txe_block_release.c:
    txe block release'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Middlewares/ST/threadx/common/src/tx_block_release.c
  description: 'Evidence from src/stm/Middlewares/ST/threadx/common/src/tx_block_release.c:
    tx block release'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Core/Inc/vehicle_state.h
  description: 'Evidence from src/stm/Core/Inc/vehicle_state.h: vehicle state'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/CMSIS/Include/cmsis_iccarm.h
  description: 'Evidence from src/stm/Drivers/CMSIS/Include/cmsis_iccarm.h: cmsis
    iccarm'
score: 1.0
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: pending
---
This evidence item collects test reports, logs, scripts, and documentation demonstrating the comprehensive OTA update system implementation.

## Evidence Collection Status

### SOTA (Software Over-The-Air) 🔄
- [ ] Application update scripts created
- [ ] Package management configured
- [ ] SOTA update verification tested

### COTA (Configuration Over-The-Air) 🔄
- [ ] Configuration push scripts created
- [ ] Parameter templates defined
- [ ] Configuration persistence validated

### FOTA (Firmware Over-The-Air) 🔄
- [ ] STM32 firmware update scripts created
- [ ] CAN-based flash utilities implemented
- [ ] Firmware version verification tested

### RAUC OS Updates

#### Phase A: RAUC Configuration ✅
- [x] RAUC installed on AGL (v1.15.1)
- [x] System configuration defined
- [x] A/B partition scheme configured

#### Phase B: Bundle Creation 🔄
- [x] Bundle creation process documented
- [ ] Signed bundle (.raucb) generated
- [ ] Bundle manifest validated

#### Phase C: Installation Workflow ✅
- [x] install-bundle.sh script created
- [x] Pre-install validation implemented
- [x] Config backup mechanism implemented
- [x] Logging to /var/log/rauc-install.log

#### Phase D: Post-Reboot Verification ✅
- [x] post-reboot-verify.sh script created
- [x] 7 health checks implemented:
  1. Network connectivity
  2. Disk space availability
  3. RAUC service status
  4. Boot slot verification
  5. Critical services check
  6. System log analysis
  7. Time synchronization
- [x] Mark-good / rollback logic implemented

#### Phase E: Integration Testing 🔄
- [ ] End-to-end RAUC update test
- [ ] Rollback test on simulated failure
- [ ] Data persistence verification
- [ ] Multi-update cycle test

**Evidence Status:** In Progress - RAUC scripts implemented, SOTA/COTA/FOTA scripts pending, awaiting bundle creation and integration testing.
