---
id: EVID_L0_27
header: Emergency Braking Distance Evidence
text: 'Evidence demonstrating that the emergency braking system actuates correctly
  and stops the vehicle at a safe distance from detected obstacles, with a minimum
  remaining distance of ≥ 50 mm when an obstacle is detected at 300 mm.


  **Verification Method:** Integration test. Measure the distance between the vehicle
  and the obstacle after full stop. Video recording of the test execution.


  **Expected Artifacts:**

  - Video recording of emergency braking test execution

  - Distance measurements (before and after braking)

  - Obstacle detection sensor logs

  - Braking actuation timing logs

  - Multiple test run results for repeatability

  '
level: '1.27'
normative: true
references:
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
  path: src/stm/Drivers/CMSIS/Include/cmsis_iccarm.h
score: 1.0
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: pending
---
This evidence item collects video recordings, distance measurements, and test logs that demonstrate the emergency braking stopping distance requirement is met.

**Evidence Status:** Pending - integration test to be performed during ADAS testing phase.
