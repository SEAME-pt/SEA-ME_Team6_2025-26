---
id: EVID-L0-28
header: Temperature Measurement Accuracy Evidence
text: 'Evidence demonstrating that the system measures the exterior temperature with
  a maximum deviation of ±2 °C from the reference temperature.


  **Verification Method:** Integration test. Measure the actual exterior temperature
  using a calibrated thermometer. Compare the reference value with the system-reported
  temperature. Video or image evidence of the measurement setup.


  **Expected Artifacts:**

  - Photos of measurement setup with calibrated thermometer

  - Comparison table of reference vs. system-reported temperatures

  - Temperature sensor readings log

  - Calibration certificate or specifications of reference thermometer

  - Multiple measurement results at different temperatures

  '
level: '1.28'
normative: true
references:
- type: file
  path: docs/demos/assembled-car2.jpeg
- type: file
  path: docs/demos/Cluster-display-sprint4.jpeg
- type: file
  path: docs/demos/Cluster-display-sprint3.jpeg
- type: file
  path: docs/demos/Cluster-state-sprint3.jpeg
- type: file
  path: docs/demos/3d-car.jpg
- type: file
  path: docs/demos/car_architecture_new_2 - sprint6.jpg
- type: file
  path: docs/demos/car_architecture_old - sprint6.jpeg
- type: file
  path: docs/demos/mockup2.jpeg
- type: file
  path: docs/demos/OTA-Action.png
- type: file
  path: docs/demos/first_qt_app.jpeg
- type: file
  path: docs/demos/CI-CD-tests-sprint7.png
- type: file
  path: docs/demos/mockup5.jpeg
- type: file
  path: docs/demos/Cluster-study.jpeg
- type: file
  path: docs/demos/mockup6.jpeg
- type: file
  path: docs/demos/car_architecture_new_1 - sprint6.jpg
- type: file
  path: docs/demos/mockup4.jpeg
- type: file
  path: docs/demos/LCOV_coverage_report_QT - sprint 6.jpeg
- type: file
  path: docs/demos/mockup1.jpeg
- type: file
  path: docs/demos/3d-car2.jpg
- type: file
  path: docs/demos/mockup3.jpeg
- type: file
  path: docs/demos/3d-car3.jpg
- type: file
  path: docs/demos/assembled-car1.jpeg
- type: file
  path: docs/demos/Cluster-mockup-sprint3.jpeg
- type: file
  path: docs/guides/MQTT_spike.md
- type: file
  path: docs/guides/QML-guide.md
- type: file
  path: docs/guides/github-actions-guide.md
- type: file
  path: docs/guides/Testing-Framework-Decision.md
- type: file
  path: docs/guides/Car_Architecture/circuit_image.png
- type: file
  path: docs/guides/Car_Architecture/Circuit_Connection_Diagram.md
- type: file
  path: docs/presentations/threadX/2025.11 Eclipse ThreadX_ A recipe for success with
    four simple ingredients.pdf
- type: file
  path: src/setup/scripts/r5-qt6-build.sh
- type: file
  path: src/setup/scripts/qt6-build.sh
- type: file
  path: src/hmi/assets/icons/car-inactive.svg
- type: file
  path: src/hmi/assets/icons/car-active.svg
- type: file
  path: src/shared/backend/providers/vehicleprovider.hpp
- type: file
  path: src/shared/backend/providers/vehicleprovider.cpp
- type: file
  path: src/stm/Core/Inc/vehicle_state.h
- type: file
  path: src/stm/Core/Inc/lcd1602.h
- type: file
  path: src/stm/Core/Src/lcd1602.c
- type: file
  path: src/stm/Drivers/CMSIS/Include/cmsis_iccarm.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pcd_ex.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pcd.h
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pcd_ex.c
- type: file
  path: src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pcd.c
score: 1.0
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: pending
---
This evidence item collects measurement comparisons, photos, and logs that demonstrate the temperature measurement accuracy requirement is met.

**Evidence Status:** Pending - temperature comparison test to be performed during sensor integration testing.
