---
id: ASSERT_L0_9
header: ThreadX operates on STM32
text: The ThreadX RTOS successfully runs on the STM32 microcontroller and executes
  scheduled tasks according to the documented configuration.
level: '1.9'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_9/EXPECTATIONS-EXPECT_L0_9.md
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EVIDENCES/EVID_L0_9/EVIDENCES-EVID_L0_9.md
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
