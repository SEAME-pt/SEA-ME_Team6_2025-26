---
id: ASSERT-L0-16
header: CI/CD pipeline operational
text: The GitHub Actions CI/CD pipeline successfully runs automated builds and tests
  on code commits according to the documented workflow configuration.
level: '1.16'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-16.md
- type: file
  path: ../evidences/EVID-L0-16.md
review_status: accepted
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
evidence:
  type: validate_hardware_availability
  configuration:
    components:
    - STM32
    - CAN
    - Raspberry Pi
---
