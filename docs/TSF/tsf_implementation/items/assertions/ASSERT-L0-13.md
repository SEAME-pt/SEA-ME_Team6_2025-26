---
id: ASSERT-L0-13
header: GitHub repository configured
text: The project GitHub repository is configured with appropriate structure, branch
  protection, and team access permissions as documented in the setup guide.
level: '1.13'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-13.md
- type: file
  path: ../evidences/EVID-L0-13.md
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
