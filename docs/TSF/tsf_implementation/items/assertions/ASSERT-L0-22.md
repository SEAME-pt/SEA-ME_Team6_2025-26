---
id: ASSERT-L0-22
header: "STM32 ThreadX safety-related startup: The STM32-based system shall perform a safe startup sequence using ThreadX RTOS"
text: |
  The STM32 microcontroller implements a safety-related startup sequence utilizing the ThreadX Real-Time Operating System (RTOS), where all critical components, threads, and peripherals are initialized in the correct order, preventing unsafe states and ensuring system reliability during boot-up.
level: "1.1"
normative: true
references:
  - type: file
    path: ../expectations/EXPECT-L0-22.md
  - type: file
    path: ../evidences/EVID-L0-22.md
---
