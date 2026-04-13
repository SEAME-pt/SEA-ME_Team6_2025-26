# 🏁 Sprint 12 — (13/04/2026 → 24/04/2026)

> ## **Sprint Goal**: Continue ADAS features implementation
---

## 📌 Epics

- #### **Epic** — Car Software Architecture - [#52](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315398&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C52)
  - [ ] ADAS manager architecture (13)
      - Define ADAS manager architecture (priority-based feature arbitration)
      - CAN message protocol for ADAS events (STM32 ↔ RPi5)
      - Implement basic ADAS event bus on ThreadX side
  - [ ] ADAS Features (13)
      - Cruise Control PID finetuning
      - Implement Lane Keep Assistant Control Task
  - [ ] ADAS Visualization on Qt HMI Cluster (13)
      - New cluster display elements: LDW indicator, AEB status, AI inference overlay
      - ADAS warning indicators (visual + state-based color changes)
      - Wire Kuksa.val ADAS signals to Qt UI
      - KUKSA Integration — Lane and Object Detection
      
  - #### **Epic** — AI Models Implementation - [#309](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/309)
  - [ ] Test real E2E (decode→preprocess→infer→postprocess→render) using UFLv2 (postprocess improvement) + Yolo8s (w/boxs in horizontal sinalization) in the race track (choosing between latency & precision) (5)
  - [ ] Computer Vision Pipeline (13)
      - Migration of post-processing to C++
      - Fine-tuning UFLDv2
      - C++ post-migration benchmark
  - [ ] Dataset Labelling (5)
      - Dataset finetuning
  - [ ] YoloV8 compilation and post-processing (8)
  - [ ] Benchmarks and tests (26)
      - Test YOLO26n-seg w/ split hybrid (decode→preprocess→infer→postprocess→render) and YOLO26s-seg w/ full INT8 (fallback to YOLO26s-seg hybrid) (decode→preprocess→infer)
      - Test YOLO8n-seg vs YOLO26n-seg
       
- #### **Epic** — Project Management & Traceability Refinement - [#55](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315926&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C55)
  - [ ] TSF presentation (5)    

- #### **Epic** - Car Hardware Architecture - [#53](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315665&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C53)
  - [ ] Headlights implementation (8)
      - Implement functional headlights on the vehicle
       
- #### **Epic** - Documentation - [#54](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315799&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C54)
  - [ ] Reorganize documentation (5)
      - Reorganize documentation on the repository

---

  
## 📊 Sprint Points Summary

| Member    | Tasks | Points | Achieved |
|-----------|-------|--------|----------|
| **João**  | **Benchmarks and Tests (26)** + **TSF Presentation (5)** + **Reorganize documentation (5)**| **36** | **-** |
| **Ruben** | **ADAS manager architecture (13)** + **ADAS Features (13)** + **Headlights implementation (8)** + **Reorganize documentation (5)**| **39** | **-** |
| **David** | **ADAS manager architecture (13) + Test real E2E using UFLv2 + Yolo8s (5) + Computer Vision Pipeline (13)** + **Reorganize documentation (5)**| **36** | **-** |
| **Vasco** | **Dataset Labelling (5) + Test real E2E using UFLv2 + Yolo8s (5) + YoloV8 compilation and post-processing (8) + Computer Vision Pipeline (13)** | **31** | **-** |
| **Diogo** | **ADAS manager architecture (13)** + **ADAS Visualization on Qt HMI Cluster (13) + Computer Vision Pipeline (13)** | **39** | **-** |
> **Note:** Individual point totals may not sum to the overall sprint total, as points for collaborative tasks are counted in full for each contributing member

**Total Sprint Points: 114**

---

## 🗺️ ADAS Module Roadmap (12 weeks — 6 sprints)

| Sprint | Weeks | Theme | Key Deliverables |
|--------|-------|-------|------------------|
| **Sprint 9** ✅ | 1-2 | Foundations & Spikes | Car v3.1, AEB start, AI/model research, OTA w/RAUC |
| **Sprint 10** ✅ | 3-4 | Perception & Control | Object detection on Hailo-8, AEB functional, LDW pipeline |
| **Sprint 11** ✅| 5-6 | Lane Keeping & Cruise Control | LKA implementation, basic CC, CARLA simulation setup |\
| Sprint 12 ⚙| 7-8 | Integration & Autonomous Mode | Autonomous driving mode, sensor fusion, ACC (bonus) |
| Sprint 13 | 9-10 | Testing & Validation | Comprehensive ADAS testing, CARLA validation, benchmarks |
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


## ✅ Outcomes
------


- **Demos**:

 
- **Docs updated**:


# 🔎 Retrospective
- ## **Went well**:

- ## **To improve**:

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
