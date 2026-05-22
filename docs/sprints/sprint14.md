# 🏁 Sprint 14 — (12/05/2026 → 22/05/2026)

> ## **Sprint Goal**: Integration of ADAS features into ADAS Manager
---

## 📌 Epics

- #### **Epic** — Car Software Architecture - [#52](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315398&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C52)
  - [ ] Integration of Traffic Sign Recognition into ADAS Manager (13)
  - [X] Integration of ADAS Manager logic into Firmware - STM32 (13)
  - [X] Integration of Object Detection into Emergency Brake Logic (13)
  - [X] Transformation of Basic Cruise Control into Adaptive Cruise Control (13)
  - [ ] Full Integration of ADAS into Cluster (13)
  ---


- #### **Epic** — AI Models Implementation - [#309](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/309)
  - [X] Object Detection Distance Measurement (13)
  - [X] Object Avoidance Implementation (13)
  ---

- #### **Epic** - Documentation - [#54](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315799&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C54)
  - [ ] Reorganize documentation (5)
  - [X] ADR (8)
      - Create Architecture Decision Record (ADR) that explains the research work behind the selection of the models to be implemented in the car.
  
---

  
## 📊 Sprint Points Summary

| Member    | Tasks | Points | Achieved |
|-----------|-------|--------|----------|
| **João**  | **Reorganize documentation (5)** + **Integration of ADAS Manager logic into Firmware — STM32** (13) + **Object Avoidance Implementation** (13) | **31** | **26** |
| **Ruben** | **Reorganize documentation (5)** + **Integration of Object Detection into Emergency Brake Logic** (13) + **Transformation of Basic Cruise Control into Adaptive Cruise Control** (13) | **31** | **26** |
| **David** | **Reorganize documentation (5)** + **Integration of ADAS Manager logic into Firmware — STM32** (13) + **Object Avoidance Implementation** (13) | **31** | **26** |
| **Vasco** | **Reorganize documentation (5)** + **Object Detection Distance Measurement** (13) + **Object Avoidance Implementation** (13) + **ADR** (8) | **39** | **34** |
| **Diogo** | **Reorganize documentation (5)** + **Integration of Traffic Sign Recognition into ADAS Manager** (13) + **Full Integration of ADAS into Cluster** (13) | **31** | **-** |
> **Note:** Individual point totals may not sum to the overall sprint total, as points for collaborative tasks are counted in full for each contributing member

**Total Sprint Points: 91**

---

## 🗺️ ADAS Module Roadmap (12 weeks — 6 sprints)

| Sprint | Weeks | Theme | Key Deliverables |
|--------|-------|-------|------------------|
| **Sprint 9** ✅ | 1-2 | Foundations & Spikes | Car v3.1, AEB start, AI/model research, OTA w/RAUC |
| **Sprint 10** ✅ | 3-4 | Perception & Control | Object detection on Hailo-8, AEB functional, LDW pipeline |
| **Sprint 11** ✅| 5-6 | Lane Keeping & Cruise Control | LKA implementation, basic CC, CARLA simulation setup |
| **Sprint 12** ✅| 7-8 | Integration & Autonomous Mode | Autonomous driving mode, sensor fusion, ACC (bonus) |
| **Sprint 13** ✅| 9-10 | Testing & Validation | Comprehensive ADAS testing, CARLA validation, benchmarks |
| **Sprint 14** ✅| 11-12 | Final Polish & Demo | System stabilization, documentation, demo preparation |

---

## 🎯 ADAS Module Goals

| Module Goal | Achieved |
|-------------|----------------------|
| **Goal 1**: ADAS features (AEB, LDW, ACC, TSR) | AEB, LDW, ACC, TSR on the making |
| **Goal 2**: Perception, Control, Localization | Perception, Control, Simulation |
| **Goal 3**: CARLA simulation | Yes |
| **Goal 4**: AI/ML/Deep Learning for ADAS | Yolov8-s for Object Detection and Avoidance and UltraFastLaneV2 for Lane Detection |
| **Goal 5**: GenAI applied to ADAS | Familiarized |
| **Goal 6**: OTA updates for ADAS | Not achieved |

---

## 📈 Actual Progress
----

- ### Sprint 14 points: **60 / 91**
 
 ❌ Undone:
- Integration of Traffic Sign Recognition into ADAS Manager (13)
- Full Integration of ADAS into Cluster (13)
- Reorganize documentation (5)

  
## ✅ Outcomes
------

- **Delivered**:
  
  
- **Demos**:

<img width="1600" height="1203" alt="WhatsApp Image 2026-05-22 at 15 08 39" src="https://github.com/user-attachments/assets/e82e5260-c6dc-4472-a532-97e346d57b63" />

<img width="1600" height="1203" alt="WhatsApp Image 2026-05-22 at 15 08 43" src="https://github.com/user-attachments/assets/162ee4f1-8eb4-4143-b225-c7a004668930" />

<img width="1600" height="1203" alt="WhatsApp Image 2026-05-22 at 15 08 48" src="https://github.com/user-attachments/assets/03151873-48ae-4e35-993e-e90872af348f" />

<img width="396" height="357" alt="WhatsApp Image 2026-05-22 at 15 14 54" src="https://github.com/user-attachments/assets/2684c30a-44ac-4bdc-afc1-d085b2167956" />

<img width="396" height="357" alt="WhatsApp Image 2026-05-22 at 15 14 54(1)" src="https://github.com/user-attachments/assets/ecac5392-d357-49a5-9c37-34a663262db6" />

https://www.youtube.com/shorts/WfnoQ8bGTzY




- **Docs updated**:

# 🔎 Retrospective
- ## **Went well**:
- 

- ## **To improve**:
- 

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
