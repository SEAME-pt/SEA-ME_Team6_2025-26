---
id: EVID-L0-29
header: Driver Presence Condition Evidence
text: 'Evidence demonstrating that the vehicle only accepts driving or autonomous
  commands when a driver (Diogo''s duck) is detected as present inside the vehicle.


  **Verification Method:** Integration test. Execute command attempts with and without
  the driver present. Video recording showing system behavior in both cases.


  **Expected Artifacts:**

  - Video recording of test with driver (duck) absent - showing command rejection

  - Video recording of test with driver (duck) present - showing command acceptance

  - Driver presence sensor logs

  - Command rejection/acceptance logs

  - Photos of test setup with symbolic driver (duck)

  '
level: '1.29'
normative: true
references:
- type: file
  path: docs/demos/assembled-car2.jpeg
- type: file
  path: docs/demos/3d-car.jpg
- type: file
  path: docs/demos/car_architecture_new_2 - sprint6.jpg
- type: file
  path: docs/demos/car_architecture_old - sprint6.jpeg
- type: file
  path: docs/demos/OTA-Action.png
- type: file
  path: docs/demos/CI-CD-tests-sprint7.png
- type: file
  path: docs/demos/car_architecture_new_1 - sprint6.jpg
- type: file
  path: docs/demos/3d-car2.jpg
- type: file
  path: docs/demos/3d-car3.jpg
- type: file
  path: docs/demos/assembled-car1.jpeg
- type: file
  path: docs/guides/github-actions-guide.md
- type: file
  path: docs/guides/Testing-Framework-Decision.md
- type: file
  path: docs/guides/Car_Architecture/Circuit Diagram.png
- type: file
  path: docs/guides/Car_Architecture/Circuit Documentation.md
- type: file
  path: docs/presentations/threadX/2025.11 Eclipse ThreadX_ A recipe for success with
    four simple ingredients.pdf
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
This evidence item collects video recordings and logs that demonstrate the driver presence condition requirement is met.

**Evidence Status:** Pending - integration test to be performed during driver presence validation testing.
