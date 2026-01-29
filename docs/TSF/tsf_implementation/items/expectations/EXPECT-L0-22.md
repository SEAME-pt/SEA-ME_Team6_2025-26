---
id: EXPECT-L0-22
header: "STM32 ThreadX safety-related startup: The STM32-based system shall perform a safe startup sequence using ThreadX RTOS"
text: |
  The STM32 microcontroller shall execute a safety-related startup procedure utilizing the ThreadX Real-Time Operating System (RTOS), ensuring that all critical components, threads, and peripherals are initialized in the correct order to prevent unsafe states and guarantee system reliability.
level: "1.1"
normative: true
references:
  - id: ASSERT-L0-22
    type: file
    path: ../assertions/ASSERT-L0-22.md
---
