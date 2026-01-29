---
id: EXPECT-L0-28
header: "Exterior Temperature Measurement Accuracy"
text: |
  The system shall measure the exterior temperature with an accuracy suitable for vehicle status display.

  **Justification:** Automotive exterior temperature sensors typically allow deviations between ±1 °C and ±3 °C, depending on sensor placement and environmental conditions. A tolerance of ±2 °C is realistic and appropriate for a demonstrator system.

  **Acceptance Criteria:** The measured exterior temperature shall correspond to the reference temperature with a maximum deviation of ±2 °C.
level: '1.28'
normative: true
references:
- type: file
  path: ../assertions/ASSERT-L0-28.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
---
