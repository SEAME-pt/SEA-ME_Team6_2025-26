---
id: ASSERT-L0-2
header: Hardware components assembled
text: The demonstrator car hardware assembly is complete with all components physically
  installed and connected according to the documented architecture.
level: 1.2
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-2.md
- type: file
  path: ../evidences/EVID-L0-2.md
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
