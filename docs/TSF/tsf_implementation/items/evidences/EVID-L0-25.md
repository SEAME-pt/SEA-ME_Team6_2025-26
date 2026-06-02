---
id: EVID-L0-25
header: Instrument Cluster Qt UI Availability Evidence
text: 'Evidence demonstrating that the instrument cluster system running a Qt-based
  application on Raspberry Pi 4 displays an initial usable UI within ≤ 2.0 s after
  power-on.


  **Verification Method:** Integration test. External time measurement using video
  recording. Qt application startup logs. Framebuffer / display initialization logs.


  **Expected Artifacts:**

  - Video recording from power-on to UI display

  - Qt application startup logs with timestamps

  - Framebuffer initialization timing logs

  - Display driver startup measurements

  - Screenshots of initial UI state

  '
level: '1.25'
normative: true
references:
- type: file
  path: docs/demos/Cluster-display-sprint4.jpeg
- type: file
  path: docs/demos/Cluster-display-sprint3.jpeg
- type: file
  path: docs/demos/Cluster-state-sprint3.jpeg
- type: file
  path: docs/demos/car_architecture_new_2 - sprint6.jpg
- type: file
  path: docs/demos/car_architecture_old - sprint6.jpeg
- type: file
  path: docs/demos/mockup2.jpeg
- type: file
  path: docs/demos/first_qt_app.jpeg
- type: file
  path: docs/demos/CAN Scheme - New update.png
- type: file
  path: docs/demos/mockup5.jpeg
- type: file
  path: docs/demos/Cluster-study.jpeg
- type: file
  path: docs/demos/mockup6.jpeg
- type: file
  path: docs/demos/crosscompiling_scheme-sprint6.jpeg
- type: file
  path: docs/demos/car_architecture_new_1 - sprint6.jpg
- type: file
  path: docs/demos/Validation-of-energy-layout-sprint0.jpeg
- type: file
  path: docs/demos/mockup4.jpeg
- type: file
  path: docs/demos/LCOV_coverage_report_QT - sprint 6.jpeg
- type: file
  path: docs/demos/mockup1.jpeg
- type: file
  path: docs/demos/rpi5-thermal-stress-test-.png
- type: file
  path: docs/demos/system-architecture-cross.png
- type: file
  path: docs/demos/Validation-of-energy-layout-sprint3.jpeg
- type: file
  path: docs/demos/mockup3.jpeg
- type: file
  path: docs/demos/CAN-layout-sprint3.jpeg
- type: file
  path: docs/demos/Cluster-mockup-sprint3.jpeg
- type: file
  path: docs/demos/eletric-scheme.png
- type: file
  path: docs/demos/Validation-of-energy-layout-sprint2.jpeg
- type: file
  path: docs/demos/CANbus Scheme Updated.png
- type: file
  path: docs/demos/Validation-of-energy-layout-sprint1.jpeg
- type: file
  path: docs/guides/MQTT_spike.md
- type: file
  path: docs/guides/QML-guide.md
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
  path: docs/guides/Car_Architecture/Circuit Documentation.md
- type: file
  path: docs/images/CanFeederArchitecture.jpg
- type: file
  path: src/setup/scripts/r5-qt6-build.sh
- type: file
  path: src/setup/scripts/qt6-build.sh
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
This evidence item collects video recordings, startup logs, and timing measurements that demonstrate the instrument cluster UI availability requirement is met.

**Evidence Status:** Pending - timing measurements to be collected during hardware integration testing.
