---
id: ASSUMP_L0_8
header: 'Assumption: Connect the Rasp5 to the STM 32 (bidirectional) using Can...'
level: '1.8'
normative: true
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
score: 0.6
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
