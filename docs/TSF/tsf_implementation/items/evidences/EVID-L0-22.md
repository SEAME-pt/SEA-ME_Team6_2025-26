---
id: EVID-L0-22
header: "STM32 ThreadX safety-related startup: The STM32-based system shall perform a safe startup sequence using ThreadX RTOS"
text: |
  The STM32 project in the repository includes ThreadX RTOS integration with safety-related startup code, configuration files, and test logs demonstrating proper initialization of critical components.
level: '1.22'
normative: true
references:
  - type: file
    path: src/stm/AZURE_RTOS
  - type: file
    path: src/stm/Core
  - type: file
    path: src/stm/Combined_test.md
  - type: file
    path: docs/guides/ThreadX_Installation_Guide.md
  - type: file
    path: docs/guides/ThreadXGuide.md
score: 1.0
---
