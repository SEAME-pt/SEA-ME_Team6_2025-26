---
id: ASSUMP-L0-22
header: 'Assumption: STM32 ThreadX safety-related startup capabilities'
level: '1.22'
normative: true
references:
- id: EXPECT-L0-22
  type: file
  path: ../expectations/EXPECT-L0-22.md
evidence:
  type: validate_stm32_threadx_startup
  configuration:
    components:
      - "STM32 microcontroller"
      - "ThreadX RTOS"
      - "Safety-related startup code"
      - "Critical component initialization"
text: 'Assumption: The STM32 microcontroller and ThreadX RTOS are capable of performing a safety-related startup sequence, and the development team has the necessary expertise and tools to implement and verify this functionality.'
---
Assumption: The STM32 microcontroller and ThreadX RTOS are capable of performing a safety-related startup sequence, and the development team has the necessary expertise and tools to implement and verify this functionality.

Acceptance criteria / notes:
- The STM32 hardware supports ThreadX RTOS with safety-critical features.
- Development tools for STM32 and ThreadX are available and functional.
- Team members have access to documentation and training for safety-related programming on STM32 with ThreadX.
- If any capabilities are limited, a mitigation plan (alternative RTOS or simplified startup) will be documented.

Rationale: The safety-related startup requirement assumes that the chosen hardware and software stack can meet safety standards; this assumption ensures prerequisites are met for implementing EXPECT-L0-22.