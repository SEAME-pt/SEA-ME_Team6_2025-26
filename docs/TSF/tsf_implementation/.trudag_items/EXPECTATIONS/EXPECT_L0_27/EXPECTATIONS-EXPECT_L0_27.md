---
id: EXPECT_L0_27
header: "Emergency Braking Stopping Distance"
text: |
  The emergency braking system shall actuate correctly and stop the vehicle at a safe distance from a detected obstacle.

  **Justification:** Short-range obstacle detection and emergency braking demonstrators commonly use distances in the 200–500 mm range for low-speed platforms. A minimum remaining distance of 50 mm provides a clear safety margin while remaining realistic for a small-scale vehicle.

  **Acceptance Criteria:**
  - Given an obstacle detected at 300 mm from the sensor, the vehicle shall stop with a minimum remaining distance of ≥ 50 mm from the obstacle.
  - The braking action shall be triggered automatically once the obstacle detection threshold is reached.
level: '1.27'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_27/ASSERTIONS-ASSERT_L0_27.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
---
