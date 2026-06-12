# 🏁 Sprint 15 — (01/06/2026 → 12/06/2026)

> ## **Sprint Goal**: Familiarization with Mobility Scenarios: Vehicle-to-Infrastructure (V2I) + Emergency Vehicle Priority
> ## **Bonus Goal**: Wrap up the loose tasks
---

## 📌 Epics

- #### **Epic** — Car Software Architecture - [#52](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315398&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C52)
  - [x] Vehicle-to-Infrastructure: communication with barriers. (21)
  - [x] Emergency Vehicle Priority: communication with barriers and traffic lights (13)
  - [x] Implement OTA pipeline to deploy GitHub releases directly to the car. (13)
  - [x] ADAS Manager Tuning (13)
  ---
  
- #### **Epic** — Car Hardware Architecture - [#53](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315665&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C53)
  - [x] Car hardware remodeling: physical restructuring of the vehicle. (13)
  ---

- #### **Epic** — AI Models Implementation - [#309](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/309)
  - [x] Retrain YOLO model - cars and obstacles. (8)
  ---

- #### **Epic** - Documentation - [#54](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315799&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C54)
  - [ ] Status point: update the documentation to reflect current implementation. (8)
 
- ### Tasks from the previous sprint:
  - [ ] Integration of Traffic Sign Recognition into ADAS Manager (8)
  - [x] Full Integration of ADAS into Cluster (8)
---
## **Total Sprint Points: 105 (89 + 16)**

## 📊 Sprint Points Summary

| Member    | Tasks | Points | Achieved |
|-----------|-------|--------|----------|
| **João**  | Vehicle-to-Infrastructure (21) + Emergency Vehicle Priority (13) | 34 | 34 |
| **David** | ADAS Manager Tuning (13) + Deploy GitHub releases directly to the car (13) | 26 | 26 |
| **Vasco** | Emergency Vehicle Priority (13) + Retrain YOLO model (8) | 21 | 21 |
| **Diogo** | Integration of Traffic Sign Recognition into ADAS Manager (8) + Full Integration of ADAS into Cluster (8) + Deploy GitHub releases directly to the car (13) | 29 | 21 |
| **Ruben** | Car hardware remodeling (13) + Deploy GitHub releases directly to the car (13) | 26 | 26 |
> **Note:** Individual point totals may not sum to the overall sprint total, as points for collaborative tasks are counted in full for each contributing member

---

## 🎯 ADAS Module Goals

| Module Goal | Achieved |
|-------------|----------------------|
| **Goal 1**: Choice of a Mobility Scenario | |
| **Goal 2**: Document architecture and technology | |
| **Goal 3**: Establish requirements | |
| **Goal 4**: Test coverage | |

---

## 📈 Actual Progress
----

- ### Sprint 15 points: **89 / 105**
 
## ❌ Undone:
  - Integration of Traffic Sign Recognition into ADAS Manager
  - Status point: update the documentation to reflect current implementation.
------
  
## ✅ Outcomes:
  - Achieved Radio communication between 2 microbits ✅
  - OTA pipeline working ✅
  - Car remodeling ✅
------

- **Demos**:
- <img width="1536" height="2048" alt="car-remodeling" src="https://github.com/user-attachments/assets/37f6cb10-24dd-4a02-94b8-795212afc16e" />
- <img width="1536" height="2048" alt="car-remodeling-2" src="https://github.com/user-attachments/assets/0f8f1ff2-d06b-4232-bc63-afc07dd1926f" />
- <img width="2048" height="1536" alt="car-remodeling-3" src="https://github.com/user-attachments/assets/8b535f9b-6764-4fb2-a786-28a9dd504c50" />
- <img width="1536" height="1024" alt="ota_diagram" src="https://github.com/user-attachments/assets/8f22cfc0-0d77-452f-970c-6e208837e89f" />


- **Docs updated**:
- https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/guides/OTA/RAUC_OTA_GUIDE.md

# 🔎 Retrospective
- ## **Went well**:
- Goal reached: Familiarization with Vehicle-to-Infrastructure Scenario 🎯
- OTA pipeline for software (ADAS-Manager, Inference, Cluster) 🎯

- ## **To improve**:
- Everything went well this sprint 🫡

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
