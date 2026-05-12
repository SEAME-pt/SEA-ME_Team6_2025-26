# 🏁 Sprint 14 — (12/05/2026 → 22/05/2026)

> ## **Sprint Goal**: Documentation (Module closing) + Full ADAS implementation (features & manager) + MPC
---

## 📌 Epics

- #### **Epic** — Car Software Architecture - [#52](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315398&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C52)
  - [ ] ADAS manager architecture (13)
      - Define ADAS manager architecture (priority-based feature arbitration)
      - ADAS manager state machine 
      - CAN message protocol for ADAS events (STM32 ↔ RPi5)
      - Implement basic ADAS event bus on ThreadX side
   - [ ] ADAS Features (21)
      - Parking assistance (PA) 
      - Adaptative Cruise Control (ACC)
      - Truck Platooning (TP - Bonus)

- NOTE: Bonus tasks that we won´t be doing: Blind-spot detection

- #### **Epic** — AI Models Implementation - [#309](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/309)
  - [ ] Yolo8s integration in the car and ADAS manager (13)
   - Object detection and swerve
   - Stop using emergency braking
   - Reduces speed if it detects 50 or 80 sign
   - Slows down at a pedestrian crossing, priority sign, or danger sign
   - Stops at a stop sign and at a pedestrian crossing.
  - [ ] Yolo 26 combined test (5)
      - Test Yolo26s with UFLv2 on hailo 8
  - [ ] MPC (Model Predictive Control) (13)
      - Practical Spike

- #### **Epic** - Documentation - [#54](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315799&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C54)
  - [ ] Reorganize documentation (5)
      - Reorganize documentation on the repository
  - [ ] Update documentation (40)
      - Update all the documentation regarding benchmark (Joao), Yolo8s (Vasco), UFLV2(David), ADAS features (Ruben & Diogo) 
  - [ ] ADR (8)
      - Create Architecture Decision Record (ADR) that explains the research work behind the selection of the models to be implemented in the car.
  
---

  
## 📊 Sprint Points Summary

| Member    | Tasks | Points | Achieved |
|-----------|-------|--------|----------|
| **João**  | **Update Documentation (8)** + **Reorganize documentation (5)** + **MPC (Model Predictive Control) (13)** + **ADR (8)** + **Yolo 26 combined test (5)** | **39** | **-** |
| **Ruben** | **Update Documentation (8)** + **Reorganize documentation (5)** + **ADAS manager architecture (13)** + **ADAS Features (21)** | **47** | **-** |
| **David** | **Update Documentation (8)** + **Reorganize documentation (5)** + **ADAS manager architecture (13)** + **ADAS Features (21)** | **39** | **-** |
| **Vasco** | **Update Documentation (8)** + **Reorganize documentation (5)** + **ADAS manager architecture (13)** + **Yolo8s integration in the car (13)** | **39** | **-** |
| **Diogo** | **Update Documentation (8)** + **Reorganize documentation (5)** + **ADAS manager architecture (13)** + **ADAS Features (21)** | **47** | **-** |
> **Note:** Individual point totals may not sum to the overall sprint total, as points for collaborative tasks are counted in full for each contributing member

**Total Sprint Points: 118**

---

## 🗺️ ADAS Module Roadmap (12 weeks — 6 sprints)

| Sprint | Weeks | Theme | Key Deliverables |
|--------|-------|-------|------------------|
| **Sprint 9** ✅ | 1-2 | Foundations & Spikes | Car v3.1, AEB start, AI/model research, OTA w/RAUC |
| **Sprint 10** ✅ | 3-4 | Perception & Control | Object detection on Hailo-8, AEB functional, LDW pipeline |
| **Sprint 11** ✅| 5-6 | Lane Keeping & Cruise Control | LKA implementation, basic CC, CARLA simulation setup |\
| **Sprint 12** ✅| 7-8 | Integration & Autonomous Mode | Autonomous driving mode, sensor fusion, ACC (bonus) |
| **Sprint 13** ✅| 9-10 | Testing & Validation | Comprehensive ADAS testing, CARLA validation, benchmarks |
| Sprint 14 | 11-12 | Final Polish & Demo | System stabilization, documentation, demo preparation |

---

## 🎯 ADAS Module Goals Mapping

| Module Goal | Sprint 10 Contribution |
|-------------|----------------------|
| **Goal 1**: ADAS features (AEB, LDW, ACC, TSR) | LDW — lane departure warning via UFLDv2 inference + threshold detection; ACC — basic cruise control implementation |
| **Goal 2**: Perception, Control, Localization | Camera capture pipeline in AGL; Hailo-8 inference integration; YOLOv8-s training for object detection; second camera integration |
| **Goal 3**: CARLA simulation | Not this sprint |
| **Goal 4**: AI/ML/Deep Learning for ADAS | UFLDv2 HEF deployment on Hailo-8; YOLOv8-s model training; lane detection algorithm; end-to-end latency benchmarking |
| **Goal 5**: GenAI applied to ADAS | Not this sprint |
| **Goal 6**: OTA updates for ADAS | Not this sprint |

---

## 📈 Actual Progress
----

- ### Sprint 14 points: ** / 107** (without bonus points)

- ### Bonus:

  
 ❌ Undone:

  
## ✅ Outcomes
------

- **Delivered**:
  
  
- **Demos**:







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
