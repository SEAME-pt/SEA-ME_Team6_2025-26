---
id: EXPECT_L0_22
header: "STM32 ThreadX Deterministic Startup"
text: |
  The STM32-based control system running ThreadX shall initialize and start publishing speed data over CAN within a deterministic and bounded time after power-on. The STM32 system must start speed data acquisition and CAN transmission within ≤ 100 ms after power-on.

  **Justification:** AUTOSAR Classic–like ECUs and RTOS-based MCUs typically achieve startup times in the 10–100 ms range. Tier-1 suppliers (Bosch, Continental) commonly target <100 ms for availability of safety-relevant signals. ThreadX is designed for deterministic startup on MCUs.

  **Acceptance Criteria:** The STM32 system starts speed data acquisition and CAN transmission within ≤ 100 ms after power-on.
level: '1.22'
normative: true
references:
  - type: file
    path: docs/TSF/tsf_implementation/.trudag_items/ASSERTIONS/ASSERT_L0_22/ASSERTIONS-ASSERT_L0_22.md
reviewers:
  - name: Joao Jesus Silva
    email: joao.silva@seame.pt
review_status: accepted
---
