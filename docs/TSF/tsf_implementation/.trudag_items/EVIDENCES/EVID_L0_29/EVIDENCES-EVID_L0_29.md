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
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/OTA-Action.png
  description: 'Evidence from docs/demos/OTA-Action.png: OTA Action'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/CI-CD-tests-sprint7.png
  description: 'Evidence from docs/demos/CI-CD-tests-sprint7.png: CI CD tests sprint7'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/car_architecture_new_1
    - sprint6.jpg
  description: 'Evidence from docs/demos/car_architecture_new_1 - sprint6.jpg: car
    architecture new 1   sprint6'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/3d-car2.jpg
  description: 'Evidence from docs/demos/3d-car2.jpg: 3d car2'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/3d-car3.jpg
  description: 'Evidence from docs/demos/3d-car3.jpg: 3d car3'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/assembled-car1.jpeg
  description: 'Evidence from docs/demos/assembled-car1.jpeg: assembled car1'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/github-actions-guide.md
  description: 'Evidence from docs/guides/github-actions-guide.md: github actions
    guide'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Testing-Framework-Decision.md
  description: 'Evidence from docs/guides/Testing-Framework-Decision.md: Testing Framework
    Decision'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Car_Architecture/circuit_image.png
  description: 'Evidence from docs/guides/Car_Architecture/circuit_image.png: circuit
    image'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Car_Architecture/Circuit_Connection_Diagram.md
  description: 'Evidence from docs/guides/Car_Architecture/Circuit_Connection_Diagram.md:
    Circuit Connection Diagram'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/presentations/threadX/2025.11
    Eclipse ThreadX_ A recipe for success with four simple ingredients.pdf
  description: 'Evidence from docs/presentations/threadX/2025.11 Eclipse ThreadX_
    A recipe for success with four simple ingredients.pdf: 2025.11 Eclipse ThreadX  A
    recipe for success with four simple ingredients'
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
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Core/Inc/vehicle_state.h
  description: 'Evidence from src/stm/Core/Inc/vehicle_state.h: vehicle state'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Core/Inc/lcd1602.h
  description: 'Evidence from src/stm/Core/Inc/lcd1602.h: lcd1602'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Core/Src/lcd1602.c
  description: 'Evidence from src/stm/Core/Src/lcd1602.c: lcd1602'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/CMSIS/Include/cmsis_iccarm.h
  description: 'Evidence from src/stm/Drivers/CMSIS/Include/cmsis_iccarm.h: cmsis
    iccarm'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pcd_ex.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pcd_ex.h:
    stm32u5xx hal pcd ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pcd.h
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal_pcd.h:
    stm32u5xx hal pcd'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pcd_ex.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pcd_ex.c:
    stm32u5xx hal pcd ex'
- type: url
  url: https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pcd.c
  description: 'Evidence from src/stm/Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pcd.c:
    stm32u5xx hal pcd'
score: 1.0
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: pending
---
This evidence item collects video recordings and logs that demonstrate the driver presence condition requirement is met.

**Evidence Status:** Pending - integration test to be performed during driver presence validation testing.
