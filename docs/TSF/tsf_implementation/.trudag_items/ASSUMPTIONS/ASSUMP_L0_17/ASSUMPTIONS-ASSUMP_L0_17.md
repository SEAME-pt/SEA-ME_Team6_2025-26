---
id: ASSUMP_L0_17
header: 'Assumption: Introduce AI in the project'
level: '1.17'
normative: true
references:
- path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_17/EXPECTATIONS-EXPECT_L0_17.md
  type: file
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
evidence:
  type: validate_hardware_availability
  configuration:
    components:
    - Hailo Hat
    - Raspberry Pi 5
    - AI
text: The Hailo AI Hat hardware is available, functional, and compatible with the
  Raspberry Pi 5 before AI model deployment testing begins.
review_status: accepted
---
The Hailo AI Hat hardware is available, functional, and compatible with the Raspberry Pi 5 before AI model deployment testing begins.

Acceptance criteria / notes:
- Hailo AI Hat is physically installed on the Raspberry Pi 5.
- Hailo runtime and drivers are installed and operational.
- Pre-trained AI models (if required) are available or documented.

Rationale: AI integration verification for `EXPECT-L0-17` depends on Hailo AI Hat hardware availability and driver installation, which are external dependencies.
