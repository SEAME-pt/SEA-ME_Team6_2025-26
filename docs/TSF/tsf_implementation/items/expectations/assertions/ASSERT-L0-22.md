---
id: ASSERT-L0-22
header: "STM32 ThreadX Deterministic Startup Verification"
text: |
  The STM32-based control system running ThreadX has been verified to initialize and start publishing speed data over CAN within the specified deterministic time bound. Startup timing measurements confirm that speed data acquisition and CAN transmission begin within ≤ 100 ms after power-on, meeting AUTOSAR-like ECU startup requirements.
level: '1.22'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-22.md
- type: file
  path: ../evidences/EVID-L0-22.md
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
