---
id: ASSUMP-L0-12
header: "Assumption: Wireless DCI Display Hardware Available"
level: "1.12"
normative: true
references:
  - type: file
    path: ../expectations/EXPECT-L0-12.md
  - type: file
    path: ../evidences/EVID-L0-12.md
reviewers:
  - name: Joao Jesus Silva
    email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "Raspberry Pi 4"
      - "WiFi"
      - "VNC"
      - "wireless display"
text: |
  Raspberry Pi 4 wireless display receiver hardware with WiFi capability and VNC server software (Wayland/VNC stack) are available and functional before wireless display integration testing begins.
---
Raspberry Pi 4 wireless display receiver hardware with WiFi capability and VNC server software (Wayland/VNC stack) are available and functional before wireless display integration testing begins.

Acceptance criteria / notes:
- Raspberry Pi 4 with WiFi module is procured and configured.
- Wayland display server and VNC server (e.g., wayvnc) are installed on Raspberry Pi 5.
- VNC client software is installed on Raspberry Pi 4 for receiving the stream.
- Network configuration allows Rasp5-to-Rasp4 VNC communication over WiFi.

Rationale: Wireless display verification for `EXPECT-L0-12` depends on Raspberry Pi 4 hardware availability and VNC/Wayland software stack being operational. This assumption clarifies the Rasp5-to-Rasp4 streaming architecture.
