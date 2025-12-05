---
id: ASSUMP_L0_11
header: 'Assumption: The Qt cluster UI shall **auto-start on boot** and display...'
level: '1.11'
normative: true
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
score: 0.6
text: 'Both Raspberry Pi 5 (in-car Qt cluster host) and Raspberry Pi 4 (wireless display
  receiver) use systemd-based init systems that support service auto-start configuration
  before Qt application boot automation testing begins.'
---
Both Raspberry Pi 5 (in-car Qt cluster host) and Raspberry Pi 4 (wireless display receiver) use systemd-based init systems that support service auto-start configuration before Qt application boot automation testing begins.

Acceptance criteria / notes:
- Systemd is installed and operational on both Raspberry Pi 5 and Raspberry Pi 4.
- Service file creation and enabling mechanisms are documented for both systems.
- Display managers are configured for auto-login on both Rasp5 and Rasp4.

Rationale: Auto-start verification for `EXPECT-L0-11` assumes systemd availability on both Raspberry Pi 5 (Qt cluster host) and Raspberry Pi 4 (wireless display receiver). This assumption makes the dual-system init configuration dependency explicit.
