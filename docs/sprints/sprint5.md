# 🏁 Sprint 5 — (09/12/2025 → 19/12/2025)

> ## **Sprint Goal**:  Establish and Validate our RDAF(Real Data Architecture Flow)
---

## 📌 Epics

- #### **Epic** — Documentation - [#54](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315799&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C54)
  - [ ] Team Knowlegde Update/Status Point (5)
- #### **Epic** — Project Management & Traceability Refinement - [#55](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315926&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C55)
  - [x] Update Project Board (3)
- #### **Epic** — Car Hardware Architecture - [#53](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315665&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C53)
  - [x] Integrate Speedometer with Hardware (3)
  - [x] Integrate Relay for Motor Driver (3)
  - [x] Mount and test SSD through USB device (3)
  - [x] Integrate Recharge Terminal (3)
- #### **Epic** — Car Software Architecture - [#52](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315398&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C52)
  - [x] Test real data displaying in QT (8)
  - [x] Design and Establish CAN Protocol (13)
  - [x] Initialize Data Processing with ThreadX (13)
  - [x] Integrate Data from Multiple Sensors (5)
- #### **Epic** — Study and Integration of Core Technologies - [#56](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138316020&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C56)
  - [x] UPROTOCOL, COVESA and KUKSA documentation (13)
- #### **Epic** - Testing and Validate Software - [103](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/103)
  - [x] Test Knowledge (Integration and Unit) (21) 
---

## 📈 Actual Progress

- ## ✅ Goal Completed! Implemention of end-to-end data flow
- `STM32 (ThreadX) <-> CAN Bus <-> Raspberry Pi 5 (AGL) - (CAN decoder + KUKSA publisher) <-> KUKSA Databroker -> Qt App`
- ## ✅ Sprint 5 points: 88/93 

- ❌ Undone: Team Knowlegde Update/Status Point (5)
---

## ✅ Outcomes

- **Delivered**:
  - All hardware additions/changes completed. (Speedometer; Relay; SSD; Recharge terminal)
  - Testing Spike/Small implementation of unit testing.
  - End-to-end communication via CAN with KUKSA implementation.
  - Processing CAN messages in ThreadX.
 
- **Demos**:
  - Coverage report - Testing
  ![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/first-code-coverage.png)
  - Qt app receiving real values from CAN messages
  ![Image](https://github.com/user-attachments/assets/3f2fdbde-a6dc-48fb-baea-6ff7a0ed3223)
  - End-to-end communication with KUKSA
  ![Image](https://github.com/user-attachments/assets/93c0be02-b795-4251-90d5-cee29f532622)
  - CAN messages benchmark
  ![Image](https://github.com/user-attachments/assets/504d738f-c1b2-4f7c-8e9c-e903a6c60e4e)
  - Demo of starting Relay 
  ![Image](https://github.com/user-attachments/assets/51083775-6852-4c18-8703-7d6342156b04)

- **Docs updated**:
  - KUKSA integration in here -> [README.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/feature/kuksa-implementation/src/can_to_kuksa/README.md)
  - Cross-compile for KUKSA implementation in here -> [README.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/docs/cross-compile-stubs/src/cross-compiler/protobuf_gRPC_crosscompiling/README.md)
  - Software Tests Guide in here -> [Software-tests-guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Software-Tests-guide.md)
  - Testing framework decision in here -> [Testing-Framework-Decision](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Testing-Framework-Decision.md)

- **Tests**:
  - Speed provider tests -> [here](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/docs/test-knowledge/src/tests/unit/speedprovider_test.cpp)
  
---

# 🔎 Retrospective
- ## **Went well**:
  - ### Objetive completed - End-to-end communication with real Data!
  - ### Bonus tasks:
      - #### KUKSA implemented
      - #### CAN benchmark test - 1000 packages/s without losses (estimated needs - 100-150 packages/s)

## TSF Useful: 
  EXPECT-L0-1 - "System architecture is defined and documented":
  EXPECT-L0-2 - "Hardware Assembly: Fully assemble the PiRacer robot":
  EXPECT-L0-3 - "Software Setup capable of running Qt applications":
  EXPECT-L0-4 - "Driveable Car with Remote Control":
  EXPECT-L0-5 - "Launch basic Qt on built-in display":
  EXPECT-L0-6 - "The Qt GUI shall display static graphical elements":
  EXPECT-L0-7 - "QT Crosscompilation should be achieved":
  EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)":
  EXPECT-L0-9 - "ThreadX running on the Microcontroller (STM32) as a RTOS";
  EXPECT-L0-10 - "Implement new architecture.":
  EXPECT-L0-11 - "QT cluster UI **auto-start on boot** on display":
  EXPECT-L0-12 - "Wireless DCI display":
  EXPECT-L0-13 - "Project & GitHub setup":
  EXPECT-L0-14 - "TSF & Documentation":
  EXPECT-L0-15 - "Agile and Scrum":
  EXPECT-L0-16 - "CI/CD":
  EXPECT-L0-17 - "Introduce AI in the project":
