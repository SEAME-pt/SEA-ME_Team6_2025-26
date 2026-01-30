---
id: ASSERT_L0_14
header: TSF framework implemented
text: The Trustable Safety Framework structure is implemented in the repository with
  documented requirements, evidences, and traceability links.
level: '1.14'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_14/EXPECTATIONS-EXPECT_L0_14.md
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_14/EVIDENCES-EVID_L0_14.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_hardware_availability
  configuration:
    components:
    - STM32
    - CAN
    - Raspberry Pi
---
