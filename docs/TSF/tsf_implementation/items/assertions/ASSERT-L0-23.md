---
id: ASSERT-L0-23
header: "Raspberry Pi 5 AGL Boot Time Verification"
text: |
  The Raspberry Pi 5–based system running Automotive Grade Linux (AGL) has been verified to complete its operating system initialization and reach middleware operational state within the specified time bound. Boot timing measurements confirm that KUKSA and middleware services become operational within ≤ 10 s after power-on, meeting AGL fast boot requirements for automotive Linux platforms.

  **Verification Method:** Timestamp logging from bootloader, kernel, systemd, and KUKSA service startup. AGL boot logs analysis. External time measurement (video recording from power-on to service availability).
level: '1.23'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-23.md
- type: file
  path: ../evidences/EVID-L0-23.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "Raspberry Pi 5"
      - "AGL"
      - "KUKSA"
---
