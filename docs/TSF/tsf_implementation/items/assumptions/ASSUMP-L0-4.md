---
id: ASSUMP-L0-4
header: 'Assumption: Driveable Car with Remote Control/Driveable Car Model...'
level: '1.4'
normative: true
references:
- id: EXPECT-L0-4
  path: ../expectations/EXPECT-L0-4.md
  type: file
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "joystick"
      - "gamepad"
      - "controller"
text: 'The joystick or remote control hardware is available, functional, and compatible
  with the target platform input subsystem before remote control integration testing
  begins.'
---
The joystick or remote control hardware is available, functional, and compatible with the target platform input subsystem before remote control integration testing begins.

Acceptance criteria / notes:
- Joystick device is recognized by the Linux input subsystem (e.g., /dev/input/js0).
- Device drivers are installed and documented.
- Input mapping configuration files are available or will be provided before testing.

Rationale: Remote control verification for `EXPECT-L0-4` depends on external input hardware availability. This assumption clarifies that hardware procurement and driver availability are prerequisites.
