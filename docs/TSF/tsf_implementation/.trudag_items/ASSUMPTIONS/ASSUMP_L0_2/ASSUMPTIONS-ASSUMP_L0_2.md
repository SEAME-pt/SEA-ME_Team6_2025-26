---
id: ASSUMP_L0_2
header: 'Assumption: Hardware Assembly'
level: '1.2'
normative: true
references:
- id: EXPECT_L0_2
  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_2/EXPECTATIONS-EXPECT_L0_2.md
  type: file
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "Expansion Board"
      - "Raspberry Pi 5"
      - "Raspberry Pi 4"
      - "DC motors"
      - "servo motor"
      - "chassis"
text: 'The required hardware components (Expansion Board, Raspberry Pi 5, Raspberry
  Pi 4, DC motors, servo motor, display interfaces, chassis) are procured and available
  before hardware assembly activities begin.'
---
The required hardware components (Expansion Board, Raspberry Pi 5, Raspberry Pi 4, DC motors, servo motor, display interfaces, chassis) are procured and available before hardware assembly activities begin.

Acceptance criteria / notes:
- All hardware components have been purchased or provided by the organization.
- Components are received and physically available in the project workspace.
- Component specifications match the system architecture requirements.

Rationale: Hardware assembly for `EXPECT-L0-2` depends on external procurement of components. This assumption makes explicit that hardware availability is a prerequisite for assembly work.
