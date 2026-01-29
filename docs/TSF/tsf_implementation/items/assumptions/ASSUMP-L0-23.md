---
id: ASSUMP-L0-23
header: "Assumption: AGL Environment Ready"
text: |
  Assumption: The Raspberry Pi 5 development environment with Automotive Grade Linux (AGL) is properly configured and includes a working KUKSA middleware installation. The AGL image has been optimized for fast boot according to AGL fast boot guidelines. Power supply is stable and boot timing measurements can be accurately captured through system logs and external recording.
level: '1.23'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-23.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_software_dependencies
  configuration:
    components:
      - "Raspberry Pi 5"
      - "AGL image"
      - "KUKSA middleware"
      - "Boot timing tools"
---
