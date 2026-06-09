# 🏁 Sprint 11 — (23/03/2026 → 02/04/2026)

> ## **Sprint Goal**: Continue ADAS features implementation 
---

## 📌 Epics

- #### **Epic** — Car Software Architecture - [#52](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315398&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C52)
  - [x] Lane detection algorithm implementation (8)
      - Lane lines detected reliably in test footage
      - Works under varied lighting conditions
      - Outputs lane center offset and heading error
  - [x] Computer Vision Pipeline (13)
      - Integrate UFLDv2 (Ultra Fast Lane Detection v2) compiled HEF on Hailo-8
      - Implement camera frame capture pipeline (libcamera → frame buffer → Hailo inference)
      - Define lane position thresholds for departure warning
  - [ ] ADAS manager architecture (13)
      - Define ADAS manager architecture (priority-based feature arbitration)
      - CAN message protocol for ADAS events (STM32 ↔ RPi5)
      - Implement basic ADAS event bus on ThreadX side
  - [x] Measure end-to-end pipeline latency (8)
      - Timestamp from frame capture to inference output
      - Verify real-time viability for ADAS
  - [x] Dataset labelling (8)
      - Label images from shared dataset
  - [x] YOLOv8 model training (13)
      - Train object detection model
  - [x] Basic cruise control (21)
      - Implement a basic cruise control that maintains a set vehicle speed without driver input
  - [x] Architectural Decision Record about Yolov8-s decision (8)
      - Document the architectural decision for choosing YOLOv8-s as the object detection model, justifying the choice against alternatives
  - [x] Logging system in the car (13)
      - Implement an on-board logging system that records vehicle data and events when the car is not connected to an external computer
  - [ ] Team knowledge (8)
      - Document and share technical knowledge across the team, ensuring all members are aligned on architecture, tools and decisions made during the sprint
       
- ### **Epic** - Car Hardware Architecture - [#53](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315665&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C53)
  - [ ] Headlights implementation (8)
      - Implement functional headlights on the vehicle
  - [x] Second camera integration (8)
      - Integrate a second camera into the perception pipeline, defining its position and role alongside the primary camera
       
- ### **Epic** - Documentation - [#54](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315799&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C54)
  - [ ] Reorganize documentation (5)
      - Reorganize documentation on the repository
      - Expectations updated in sprints    
---

## 📊 Sprint Points Summary

| Member    | Tasks | Points | Achieved |
|-----------|-------|--------|----------|
| **João**  | **Lane detection algorithm implementation (8) + Lane Departure Warning — Computer Vision Pipeline (13) + Measure end-to-end pipeline latency (8)** | **29** | **29** |
| **Ruben** | **Basic cruise control (21) + Headlights implementation (8) + Second camera integration (8) + Logging system in the car (13) + Extra Task(8)**| **58** | **50** |
| **David** | **Lane detection algorithm implementation (8) + Computer Vision Pipeline (13) + ADAS manager architecture (13) + Measure end-to-end pipeline latency (8)** | **42** | **29** |
| **Vasco** | **YOLOv8 model training (13) + Dataset labelling (13) + Architectural Decision Record about Yolov8-s decision (8) + Extra Task(13)** | **40** | **37** |
| **Diogo** | **Lane detection algorithm implementation (8) + Computer Vision Pipeline (13) + Measure end-to-end pipeline latency (8) + Extra Task(13)** | **42** | **42** |
> **Note:** Individual point totals may not sum to the overall sprint total, as points for collaborative tasks are counted in full for each contributing member

**Total Sprint Points: 155**

---

## 🗺️ ADAS Module Roadmap (12 weeks — 6 sprints)

| Sprint | Weeks | Theme | Key Deliverables |
|--------|-------|-------|------------------|
| **Sprint 9** ✅ | 1-2 | Foundations & Spikes | Car v3.1, AEB start, AI/model research, OTA w/RAUC |
| **Sprint 10** ✅ | 3-4 | Perception & Control | Object detection on Hailo-8, AEB functional, LDW pipeline |
| Sprint 11 ⚙| 5-6 | Lane Keeping & Cruise Control | LKA implementation, basic CC, CARLA simulation setup |\
| Sprint 12 | 7-8 | Integration & Autonomous Mode | Autonomous driving mode, sensor fusion, ACC (bonus) |
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

- ### Sprint 11 points: **121 / 155**

- ### Bonus:
  - Object detection labell dataset (13) ✅
  - Creation of documentation for team knowledge (sensor pipeline + datalogger + car connections) (8) ✅  

- ❌ Undone:
  - ADAS manager architecture (13)
  - Team knowledge (8)
  - Headlights implementation (8)
  - Reorganize documentation (5)

---

## ✅ Outcomes

- **Delivered**:
  - Progress on lane detection with viable latency for ADAS
  - Reasearch and implementation of PID in cruise control
  - Creation of "blackbox" - system datalogger
  - Creation of new documentation for team knowledge
 

- **Demos**:

  

- https://github.com/user-attachments/assets/28d68665-9391-4ccd-a853-7bb43c93a3df
- <img width="1004" height="842" alt="inferencia_camera_1600x320" src="https://github.com/user-attachments/assets/9b04bc4a-e517-42d0-aaa7-08095109594a" />
- 8-9 FPS
- <img width="1122" height="839" alt="inferencia_camera_800x160_2" src="https://github.com/user-attachments/assets/24e39f24-b5a3-4b58-9802-93ba0d7e708b" />
- 19-20 FPS
- ![yolo_inf](https://github.com/user-attachments/assets/5104ca6d-faa2-43af-8823-9afec4731ae9)
- ![system-logger](https://github.com/user-attachments/assets/146da718-0c8b-40f3-ab8e-734c786edd2c)
- ![system-logger-2](https://github.com/user-attachments/assets/580d5248-edca-4c78-9706-dfa288dcea3a)



- **Docs updated**:
  - https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/guides/Datalogger_guide.md
  - https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/guides/Car_Architecture/Car_Conections.md
  - https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/guides/Car_Architecture/Sensor_Pipeline.md

---

# 🔎 Retrospective
- ## **Went well**:
- Better expertise of AI models and machine learning team-wise

- ## **To improve**:
- Team-building activities

## 🔗 Useful Links
- Sprint board/view: [Sprint 11](https://github.com/orgs/SEAME-pt/projects/89/views/17)
- ADAS Module Goals: [02_ADAS](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/docs/contents_2025/02_ADAS)

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
