---
id: ASSERT_L0_32
header: "End-to-End AI Inference Performance (Dual Model Ex"
text: |
  The assertion verifies that the requirement is satisfied.
level: '1.32'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_32/EXPECTATIONS-EXPECT_L0_32.md
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_32/EVIDENCES-EVID_L0_32.md
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
