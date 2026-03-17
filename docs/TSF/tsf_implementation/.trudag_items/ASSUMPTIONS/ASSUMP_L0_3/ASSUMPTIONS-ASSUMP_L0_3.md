---
id: ASSUMP_L0_3
header: 'Assumption: Target platform and drivers for software integration'
level: '1.3'
normative: true
references:
- path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_3/EXPECTATIONS-EXPECT_L0_3.md
  type: file
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
evidence:
  type: validate_linux_environment
  configuration:
    required_tools:
    - Ubuntu
    - Linux
    - kernel
    - driver
text: 'Assumption: The system and development tooling target a Linux-based integration
  environment (Ubuntu 22.04 LTS or compatible). Device drivers and kernel modules
  required for platform-specific components (e.g., motor controllers, Hailo AI Hat,
  camera interfaces) are available or will be provided by component owners prior to
  verification.'
review_status: accepted
---
Assumption: The system and development tooling target a Linux-based integration environment (Ubuntu 22.04 LTS or compatible). Device drivers and kernel modules required for platform-specific components (e.g., motor controllers, Hailo AI Hat, camera interfaces) are available or will be provided by component owners prior to verification.

Acceptance criteria / notes:
- A VM or physical host running Ubuntu 22.04 is available for integration tests.
- Required kernel modules and device drivers are installed and documented in the corresponding EVID items.
- If a driver is unavailable, an alternative (simulator or stub) will be provided and documented before verification begins.

Rationale: Many verification steps and evidence artifacts assume POSIX/Linux tooling (bash, apt, systemd, udev rules). Making this explicit avoids ambiguous environment dependency during testing.
