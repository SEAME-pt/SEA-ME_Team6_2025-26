---
id: ASSERT-L0-14
header: TSF framework implemented
text: The Trustable Safety Framework structure is implemented in the repository with
  documented requirements, evidences, and traceability links.
level: '1.14'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-14.md
- type: file
  path: ../evidences/EVID-L0-14.md
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
