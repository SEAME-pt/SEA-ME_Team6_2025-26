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
