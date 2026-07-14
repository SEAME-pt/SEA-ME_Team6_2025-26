---
id: ASSUMP_L0_5
header: 'Assumption: The system shall be capable of launching a basic Qt...'
level: '1.5'
normative: true
references:
- path: docs/TSF/tsf_implementation/.trudag_items/EXPECTATIONS/EXPECT_L0_5/EXPECTATIONS-EXPECT_L0_5.md
  type: file
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
evidence:
  type: validate_software_dependencies
  configuration:
    packages:
    - Qt
    - Qt6
    - QtCore
    - QtGui
text: Qt runtime libraries and dependencies are installed and available on the target
  platform before Qt application deployment begins.
review_status: accepted
---
Qt runtime libraries and dependencies are installed and available on the target platform before Qt application deployment begins.

Acceptance criteria / notes:
- Qt6 runtime is installed (e.g., via apt or compiled from source).
- Required Qt modules (QtCore, QtGui, QtWidgets, QtQml) are present.
- Display server (Wayland or X11) is configured and operational.

Rationale: Qt application verification for `EXPECT-L0-5` assumes that the Qt framework and runtime environment are already provisioned on the target system.
