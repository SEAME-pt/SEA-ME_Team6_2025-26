---
id: ASSUMP_L0_1
header: 'Assumption: Availability of hardware and demonstrator components for system
  architecture validation'
level: '1.1'
normative: true
references:
- type: file
  path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_1/EXPECTATIONS-EXPECT_L0_1.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
evidence:
  type: validate_hardware_availability
  configuration:
    components:
    - Expansion Board
    - Raspberry Pi 5
    - Hailo Hat
    - DC motors
    - servo motor
    - Qt
    - cluster
    - DSI
text: 'Assumption: The project team will provide the required hardware and demonstrator
  components listed in the system architecture (Expansion Board, Raspberry Pi 5, Hailo
  Hat, DC motors, servo motor, Qt display/cluster via DSI interface) and these components
  will be operational in the integration environment during verification activities.'
review_status: accepted
---
Assumption: The project team will provide the required hardware and demonstrator components listed in the system architecture (Expansion Board, Raspberry Pi 5, Hailo Hat, DC motors, servo motor, Qt display/cluster via DSI interface) and these components will be operational in the integration environment during verification activities.

Acceptance criteria / notes:
- Hardware units and connectors are available and powered in the test environment when required by integration tests.
- Device-specific drivers or firmware are available or will be provided by the component owner prior to verification.
- If a component is unavailable, a documented mitigation plan (substitute hardware or simulator) will be provided and linked in the corresponding EVID item.

Rationale: verification activities and evidence collection for `EXPECT-L0-1` depend on access to the stated hardware; recording this as an assumption highlights a test prerequisite that can be tracked and validated.
