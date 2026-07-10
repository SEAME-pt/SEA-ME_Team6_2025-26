---
id: EXPECT_L0_29
header: "Driver Presence Condition"
text: |
  The vehicle shall only accept driving or autonomous commands when a driver is detected as present inside the vehicle.

  **Justification:** Driver presence detection is a common prerequisite in automotive systems to prevent unintended operation. In this project, a symbolic driver (the duck) is used to validate the logical gating of vehicle operation.

  **Acceptance Criteria:**
  - If no driver is detected, the vehicle shall reject all driving or autonomous commands.
  - If a driver is detected (Diogo's duck), the vehicle shall accept driving or autonomous commands.
level: '1.29'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_29/ASSERTIONS-ASSERT_L0_29.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
---
