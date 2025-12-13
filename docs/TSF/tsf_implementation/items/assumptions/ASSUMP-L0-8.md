---
id: ASSUMP-L0-8
header: 'Assumption: Connect the Rasp5 to the STM 32 (bidirectional) using Can...'
level: '1.8'
normative: true
references:
- id: EXPECT-L0-8
  path: ../expectations/EXPECT-L0-8.md
  type: file
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "CAN"
      - "STM32"
      - "MCP2515"
text: 'CAN bus hardware interfaces (CAN controllers, transceivers, wiring) are installed
  and functional on both Raspberry Pi 5 and STM32 before CAN communication testing
  begins.'
---
CAN bus hardware interfaces (CAN controllers, transceivers, wiring) are installed and functional on both Raspberry Pi 5 and STM32 before CAN communication testing begins.

Acceptance criteria / notes:
- CAN interface is recognized by Linux (e.g., can0 device exists).
- CAN bus wiring is correctly connected between Rasp5 and STM32.
- CAN bus termination resistors are properly installed.

Rationale: CAN communication verification for `EXPECT-L0-8` depends on physical CAN hardware availability and correct installation. This assumption makes hardware prerequisites explicit.
