---
id: ASSERT_L0_31
header: "Unit tests – Ensure all functions operate as expected"
text: |
  This assertion confirms that the requirement for unit testing
  has been fulfilled.

  It verifies that all implemented functions have been properly
  validated through unit tests and that supporting evidence
  is available to demonstrate compliance.
level: '1.31'
normative: true
references:
  - type: file
    path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_31/EXPECTATIONS-EXPECT_L0_31.md
  - type: file
    path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_31/EVIDENCES-EVID_L0_31.md
reviewers:
  - name: Joao Jesus Silva
    email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "STM32"
      - "CAN"
      - "Raspberry Pi"
---
