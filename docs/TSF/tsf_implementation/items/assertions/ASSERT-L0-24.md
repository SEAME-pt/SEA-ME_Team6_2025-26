---
id: ASSERT-L0-24
header: "Combined ThreadX AGL Startup Verification"
text: |
  The combined STM32 (ThreadX) and Raspberry Pi 5 (AGL) system has been verified to provide integrated startup behavior where safety-relevant data is available independently of AGL boot completion. Testing confirms that speed data publication from STM32 is available within ≤ 100 ms, AGL becomes operational within ≤ 10 s, and no startup conflicts occur between the ThreadX and AGL domains.

  **Verification Method:** Integration test. CAN traffic analysis during AGL boot. Correlation of STM32 CAN timestamps with AGL service startup logs.
level: '1.24'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-24.md
- type: file
  path: ../evidences/EVID-L0-24.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: accepted
evidence:
  type: validate_hardware_availability
  configuration:
    components:
      - "STM32"
      - "ThreadX"
      - "Raspberry Pi 5"
      - "AGL"
      - "CAN"
---
