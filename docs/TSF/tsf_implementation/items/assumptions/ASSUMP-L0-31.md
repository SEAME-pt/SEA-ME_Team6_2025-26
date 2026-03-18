---
id: ASSUMP-L0-31
header: "Assumption: OTA Update System Implementation using SOTA, COTA, FOTA, and RAUC"
text: |
  Assumption: The vehicle platform supports the necessary infrastructure for comprehensive OTA updates, including:
  
  - Network connectivity (WiFi/Ethernet) for receiving update packages
  - Sufficient storage for A/B partition scheme on AGL system
  - RAUC framework available and compatible with AGL distribution
  - CAN bus interface available for FOTA updates to STM32/ECU
  - Persistent /data partition for user data preservation across updates
  - Bootloader (U-Boot) configured to support slot switching
  - System services can be safely stopped/restarted during updates
  
  The development environment includes tools for:
  - RAUC bundle creation and signing
  - OTA script development and testing
  - Remote deployment and monitoring capabilities
level: '1.31'
normative: true
references:
- type: file
  path: ../expectations/EXPECT-L0-31.md
- type: file
  path: docs/guides/OTA/OTA_Implementation_Guide.md
reviewers:
- name: Joao Jesus Silva
  email: joao.silva@seame.pt
review_status: pending
evidence:
  type: validate_software_dependencies
  configuration:
    dependencies:
      - "RAUC"
      - "AGL"
      - "U-Boot"
      - "Network connectivity"
      - "A/B partitions"
      - "CAN bus interface"
      - "STM32"
      - "Persistent storage"
      - "OTA scripts"
---
