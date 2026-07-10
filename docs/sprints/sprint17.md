# 🏁 Sprint 17 — (29/06/2026 → 10/07/2026)

> ## **Sprint Goal**: `Requisites review and tests. Car fix. `
---

## 📌 Epics

- #### **Epic** — Car Software Architecture - [#52](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315398&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C52)
  - [X] ADAS manager restructured (21)
  - [X] Tests (TSR) (21)
  - [X] Tests (Emergency Vehicle Priority) (8)
  - [X] Requirements Review and Creation (5)
  - [X] TSF Script adaptation to OS (MAC, Linux, Windows) (8)

- #### Tasks from the previous sprint:
  - [X] HMI features implementation (OTA confirmation, ADAS features, CC speed, and on/off features) (21)
  - [ ] Status point: update the documentation to reflect current implementation (8)

- #### Bonus
  - [X] Debug and fix the car (21)
---

## **Total Sprint Points: `92` + `21`**

## 📊 Sprint Points Summary

| Member    | Tasks | Points | Achieved |
|-----------|-------|--------|----------|
| **João**  | Tests (Emergency Vehicle Priority) + Requirements Review and Creation + TSF Script adaptation to OS (MAC, Linux, Windows) + Status point: update the documentation to reflect current implementation | 29 | 21 |
| **David** | ADAS manager restructured + Status point: update the documentation to reflect current implementation | 29 | 21 |
| **Vasco** | Tests (TSR / Emergency Vehicle Priority / ADAS Manager features) + Status point: update the documentation to reflect current implementation | 29 | 21 |
| **Diogo** | HMI features implementation (OTA confirmation, ADAS features, CC speed, and on/off features) + Status point: update the documentation to reflect current implementation | 29 | 21 |
| **Ruben** | Car fixed | 21 | 21 |
> **Note:** Individual point totals may not sum to the overall sprint total, as points for collaborative tasks are counted in full for each contributing member

---

## 🎯 ADAS Module Goals

| Module Goal | Achieved |
|-------------|----------------------|
| **Goal 1**: Choice of a Mobility Scenario | X |
| **Goal 2**: Document architecture and technology | |
| **Goal 3**: Establish requirements | |
| **Goal 4**: Test coverage | |

---

## 📈 Actual Progress
----

- ### Sprint 16 points: **`105/92`**

## ❌ Undone:
  - We left the final documentation to the end of the module.
  
------

## ✅ Outcomes:
  - We have effectively accomplish/delivered what we purposed to do in this sprint - mainly, **fixing the car**!!
  - Car can do a full lap again!
------

- **Demos**:
- https://github.com/user-attachments/assets/0f8182d9-582c-4f29-9664-d5bd070b1077

- **Docs updated**:


**EMERGENY PRIORITY TESTS**
- https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/feature/mobility_scenarios/V2I_and_emergencypriority/docs/guides/mobility_scenarios/TEST_RESULTS_EMERGENCY_VEHICLE_PRIORITY.md
- https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/feature/mobility_scenarios/V2I_and_emergencypriority/src/mobility_scenarios_src
- https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/feature/mobility_scenarios/V2I_and_emergencypriority/src/mobility_scenarios_src/emergency_priority/tests

**TSF NEW REQUIREMENTS**
- https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/feature/TSF/new_requirements/docs/TSF/requirements/tsf-requirements-table.md
- https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/feature/TSF/new_requirements/docs/TSF/tsf_implementation/items/expectations/EXPECT-L0-31.md
- https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/feature/TSF/new_requirements/docs/TSF/tsf_implementation/items/expectations/EXPECT-L0-32.md

# 🔎 Retrospective
- ## **Went well**:
- Our major worry was eliminated by fixing the car.
- We have effectively achieved what we purposed to do this sprint.

- ## **To improve/Still to be done**:
- We still have to effectively test on the car.
- Close the documentation topic.
- Finalize the module.

## 🔗 Useful Links

## TSF Useful:
  - EXPECT-L0-1 - "System architecture is defined and documented";
  - EXPECT-L0-2 - "Hardware Assembly: Fully assemble the PiRacer robot"
  - EXPECT-L0-3 - "Software Setup capable of running Qt applications"
  - EXPECT-L0-4 - "Driveable Car with Remote Control"
  - EXPECT-L0-5 - "Launch basic Qt on built-in display"
  - EXPECT-L0-6 - "The Qt GUI shall display static graphical elements"
  - EXPECT-L0-7 - "QT Crosscompilation should be achieved"
  - EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)"
  - EXPECT-L0-9 - "ThreadX running on the Microcontroller (STM32) as a RTOS"
  - EXPECT-L0-10 - "Implement new architecture"
  - EXPECT-L0-11 - "QT cluster UI auto-start on boot on display"
  - EXPECT-L0-12 - "Wireless DCI display"
  - EXPECT-L0-13 - "Project & GitHub setup"
  - EXPECT-L0-14 - "TSF & Documentation"
  - EXPECT-L0-15 - "Agile and Scrum"
  - EXPECT-L0-16 - "CI/CD"
  - EXPECT-L0-17 - "Introduce AI in the project"
  - EXPECT-L0-18 - "The vehicle shall communicate via CAN bus also using ThreadX"
  - EXPECT-L0-19 - "Test requirement for TSF automation"
  - EXPECT-L0-20 - "Integrate AI for content generation"
  - EXPECT-L0-21 - "Test the complete TSF automation workflow"
  - EXPECT-L0-22 - "STM32 ThreadX safety-related startup"
  - EXPECT-L0-23 - "Raspberry Pi 5 AGL boot time"
  - EXPECT-L0-24 - "Combined startup (ThreadX + AGL)"
  - EXPECT-L0-25 - "Instrument Cluster availability (Qt on Raspberry Pi 4)"
  - EXPECT-L0-26 - "End-to-end system startup time (ThreadX + AGL + Qt)"
  - EXPECT-L0-27 - "Emergency braking stopping distance"
  - EXPECT-L0-28 - "Exterior temperature measurement accuracy"
  - EXPECT-L0-29 - "Driver presence condition"
  - EXPECT-L0-30 - "Car ready to next module 2"
  - EXPECT-L0-31 - "Unit Tests"
