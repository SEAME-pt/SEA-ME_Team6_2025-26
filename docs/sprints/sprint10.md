# 🏁 Sprint 10 — (09/03/2026 → 20/03/2026)

> ## **Sprint Goal**: Integrate AI models into the vehicle's ADAS pipeline, enabling real-time perception and lane detection on Hailo-8.
---

## 📌 Epics

- #### **Epic** — Car Software Architecture - [#52](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315398&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C52)
  - [X] Hailo-8 Runtime Integration on AGL (13)
      - Deploy pre-compiled YOLOv8m HEF from Hailo Model Zoo (object detection)
      - Validate inference on live camera feed (IMX708 → libcamera → Hailo pipeline)
      - Document setup and performance benchmarks (FPS, latency)
  - [X] AI/ML Model Validation - Object Detection Model Testing & Benchmarking (13)
      - Test YOLOv8m on real driving scenarios (corridors, objects, people)
      - Compare pre-compiled HEFs: YOLOv8n vs YOLOv8m vs YOLOv8s (FPS vs accuracy trade-off)
      - Document performance benchmarks (FPS, latency, accuracy)
  - [ ] Lane detection algorithm implementation (8)
      - Lane lines detected reliably in test footage
      - Works under varied lighting conditions
      - Outputs lane center offset and heading error
  - [ ] Lane Departure Warning — Computer Vision Pipeline (13)
      - Integrate UFLDv2 (Ultra Fast Lane Detection v2) pre-compiled HEF on Hailo-8
      - Implement camera frame capture pipeline (libcamera → frame buffer → Hailo inference)
      - Define lane position thresholds for departure warning
  - [X] AEB — Complete implementation and tuning (13)
      - Complete safety thresholds and response times (carryover from Sprint 9)
      - Tune progressive braking: SRF08 + IMU fusion with TTC-based state machine
      - Integrate AEB status reporting via CAN → Kuksa.val
      - Camera and sensor positioning optimization for ADAS (carryover from Sprint 9)
  - [ ] AEB → LDW Integration Layer (5)
      - Define ADAS manager architecture (priority-based feature arbitration)
      - CAN message protocol for ADAS events (STM32 ↔ RPi5)
      - Implement basic ADAS event bus on ThreadX side
  - [ ] ADAS Visualization on Qt HMI Cluster (13)
      - New cluster display elements: LDW indicator, AEB status, AI inference overlay
      - ADAS warning indicators (visual + state-based color changes)
      - Wire Kuksa.val ADAS signals to Qt UI
  - [X] CARLA simulation environment setup (8)
      - CARLA installed and running on dev machine(s)
      - Vehicle spawned with camera sensor configured
      - Basic manual drive confirmed working
  - [X] VSS Path Definition & Databroker Audit (8)
      - New VSS paths defined for ADAS signals
      - Paths validated against VSS spec and registered in vss.json / overlay
      - Existing databroker entries audited (naming, types, units)
      - Inconsistencies or deprecated paths documented and flagged
      - Databroker tested with updated paths (publish + subscribe confirmed working)

- #### **Bonus**
  - [X] TSF Presentation (13)
  - [ ] AI Models Type Conversion (13)

---

## 📊 Sprint Points Summary

| Member    | Tasks | Points | Achieved |
|-----------|-------|--------|----------|
| **João**  | CARLA simulation environment setup (8) + TSF Presentation (13) + AI Models Type Conversion (13) | **34** | **21** |
| **Ruben** | Hailo-8 Runtime Integration on AGL (13) + AEB — Complete implementation and tuning (13) + AEB → LDW Integration Layer (5) | **31** | **26** |
| **David** | Lane detection algorithm implementation (8) + Lane Departure Warning — Computer Vision Pipeline (13) + AEB — Complete implementation and tuning (13) + AEB → LDW Integration Layer (5) + VSS Path Definition & Databroker Audit (8) | **47** | **21** |
| **Vasco** | Hailo-8 Runtime Integration on AGL (13) + AI/ML Model Validation - Object Detection Model Testing & Benchmarking (13) + CARLA simulation environment setup (8) | **34** | **34** |
| **Diogo** | Lane detection algorithm implementation (8) + AEB → LDW Integration Layer (5) + ADAS Visualization on Qt HMI Cluster (13) +  VSS Path Definition & Databroker Audit (8) | **34** | **8** |
> **Note:** Individual point totals may not sum to the overall sprint total, as points for collaborative tasks are counted in full for each contributing member

**Total Sprint Points: 94 + 26 (Bonus)**

---

## 🗺️ ADAS Module Roadmap (12 weeks — 6 sprints)

| Sprint | Weeks | Theme | Key Deliverables |
|--------|-------|-------|------------------|
| **Sprint 9** ✅ | 1-2 | Foundations & Spikes | Car v3.1, AEB start, AI/model research, OTA w/RAUC |
| **Sprint 10** ✅ | 3-4 | Perception & Control | Object detection on Hailo-8, AEB functional, LDW pipeline |
| Sprint 11 | 5-6 | Lane Keeping & Cruise Control | LKA implementation, basic CC, CARLA simulation setup |\
| Sprint 12 | 7-8 | Integration & Autonomous Mode | Autonomous driving mode, sensor fusion, ACC (bonus) |
| Sprint 13 | 9-10 | Testing & Validation | Comprehensive ADAS testing, CARLA validation, benchmarks |
| Sprint 14 | 11-12 | Final Polish & Demo | System stabilization, documentation, demo preparation |

---

## 🎯 ADAS Module Goals Mapping

| Module Goal | Sprint 10 Contribution |
|-------------|----------------------|
| **Goal 1**: ADAS features (AEB, LDW, ACC, TSR) | AEB tuning + LDW pipeline implementation |
| **Goal 2**: Perception, Control, Localization | Hailo-8 runtime integration + lane detection |
| **Goal 3**: CARLA simulation | CARLA environment setup |
| **Goal 4**: AI/ML/Deep Learning for ADAS | YOLOv8 benchmarking + UFLDv2 on Hailo-8 |
| **Goal 5**: GenAI applied to ADAS | Not this sprint |
| **Goal 6**: OTA updates for ADAS | Not this sprint |

---

## 📈 Actual Progress

- Sprint 10 points: **55 + 13 (Bonus) / 94 + 26 (Bonus)**

- ❌ Undone:
   - Lane detection algorithm implementation (8)
   - Lane Departure Warning — Computer Vision Pipeline (13)
   - AEB → LDW Integration Layer (5)
   - ADAS Visualization on Qt HMI Cluster (13)

---

## ✅ Outcomes

- **Delivered**:
  - AEB tunning
  - Yolo models benchmark
  - Lane Detection Algorithm comprehension
  - Creation of masks usin Computer Vision

- **Demos**:
<img width="830" height="198" alt="yolov8s-table" src="https://github.com/user-attachments/assets/ae1ed875-410c-4fb6-9adf-aa757e1acb38" />
<img width="830" height="198" alt="yolov26s-table" src="https://github.com/user-attachments/assets/a4f17aba-0886-41d0-83b2-fc43022c5d1c" />
<img width="705" height="756" alt="yolov8s-results" src="https://github.com/user-attachments/assets/9535bf28-2432-4766-8622-ee809e58a129" />
<img width="705" height="502" alt="yolov26s-results" src="https://github.com/user-attachments/assets/192d6ff5-c342-442c-8e4f-683aab55e762" />

![segformer](https://github.com/user-attachments/assets/37ec6319-0237-4523-978a-b662a0480620)  

![lane-detection-algorithm-2](https://github.com/user-attachments/assets/1d770a0e-2b91-4f31-96b4-e35e26165ea0)  

![lane-detection-algorithm-1](https://github.com/user-attachments/assets/73fa2875-ded7-466e-8008-9d6a24f87d15)  


- **Docs updated**:
    - [Yolo_benchmark](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Yolo_benchmark.md)
    - [RAUC - AGL Integration](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/OTA/Rauc%20-%20AGL%20integration.md)
    - [Hailo 8 Integration on AGL](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Hailo%208%20Integration%20on%20AGL.md)
    - [Spike_AEB.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Spike_AEB.md)
    - [Spike LKA.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Spike%20LKA.md)
    - [Classical_Lane_Detection.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Classical_Lane_Detection.md)
    - [Lane-Detection-Progress.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Lane-Detection-Progress.md)

---

# 🔎 Retrospective
- ## **Went well**:
  - Understanding of AI Models
  - Finished the first feature of ADAS (AEB)
  - We tested the car working with the hallo running with a yolo model and the car moving full speed at the same time and the power supply worked as expected

- ## **To improve**:
  - Task attribution and organization
  - We did the inference of the Perception Model on the Hallo and Camera but not the Lane Assistance

## 🔗 Useful Links
- Sprint board/view: [Sprint 10](https://github.com/orgs/SEAME-pt/projects/89/views/17)
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
