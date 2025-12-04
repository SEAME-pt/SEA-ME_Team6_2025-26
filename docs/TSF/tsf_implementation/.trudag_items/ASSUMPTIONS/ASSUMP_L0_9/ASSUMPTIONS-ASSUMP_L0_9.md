---
id: ASSUMP_L0_9
header: 'Assumption: ThreadX should be used on the Microcontroller (STM32) as a...'
level: '1.9'
normative: true
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
score: 0.6
text: 'The STM32 microcontroller is programmed and accessible via debug interface (ST-LINK
  or JTAG) before ThreadX firmware verification begins.'
---
The STM32 microcontroller is programmed and accessible via debug interface (ST-LINK or JTAG) before ThreadX firmware verification begins.

Acceptance criteria / notes:
- ST-LINK debugger hardware is available and connected to the STM32.
- STM32CubeProgrammer or OpenOCD is installed on the development host.
- Firmware flashing and debugging procedures are documented.

Rationale: ThreadX verification for `EXPECT-L0-9` assumes that STM32 programming infrastructure (debugger, flashing tools) is already available and operational.
