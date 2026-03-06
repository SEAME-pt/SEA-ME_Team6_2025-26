# 🏁 Sprint 9 — (23/02/2026 → 06/03/2026)

> ## **Sprint Goal**: Kickstart the ADAS module
---

## 📌 Epics

- #### **Epic** — Car Hardware Architecture - [#53](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315665&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C53)
  - [x] Reformulation of physical car architecture — Version 3.1 (13)
      - Final physical layout design
      - Camera and sensor positioning optimization for ADAS

- #### **Epic** — Car Software Architecture - [#52](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315398&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C52)
  - [x] OTA implementation with RAUC (continuation) (13)
      - Continue RAUC integration for A/B partition management
      - Validate OTA update flow end-to-end
      - Multi-version deployment support
  - [x] AEB — Automatic Emergency Braking initial implementation (13)
      - Design AEB architecture
      - Implement progressive braking algorithm on ThreadX
      - Define safety thresholds and response times
  - [x] Qt HMI & Cluster continued development (21)
      - HMI design improvements and new ADAS-related displays
      - Cluster integration with new data signals
      - Continuous work throughout the sprint
  - [x] Qt Backend refactoring (8)
      - Code restructuring for scalability
      - Improve data model and KUKSA integration
  - [x] CAN Bus Simulation - Signal Emulation Layer (8)
      - Set up virtual CAN interface
      - Implement signals and realistic CAN frames

- #### **Epic** — Study and Integration of Core Technologies - [#56](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138316020&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C56)
  - [x] ADAS Spike — Perception, Control & Path Planning (13)
      - Study AEB, LDW, ACC, TSR concepts and feasibility
  - [x] AI/ML Model Survey (13)
      - Research open-source models for object detection and obstacle avoidance
  - [x] AI Spike — GenAI applied to ADAS (13)
      - Study GenAI tools for data augmentation and scenario generation
      - Evaluate GANs, diffusion models, transformers for ADAS context
  
- #### **Epic** — Project Management & Traceability Refinement - [#55](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315926&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C55)
  - [x] TSF refinement — complete pending tasks from previous module (8)
      - Review and close open TSF requirements
      - Refine TSF automation algorithm
---

## 📊 Sprint Points Summary

| Member | Tasks | Points |
|--------|-------|--------|
| **João** | OTA with RAUC (13) + TSF refinement (8) + AI Spike (13) | **34** |
| **Ruben** | Car Architecture v3.1 (13) + AEB implementation (13) | **26** |
| **David** | AEB implementation (shared with Ruben) + CAN Bus Simulation | **21** |
| **Vasco** | ADAS Spike (13) + AI/ML Model Survey (13) | **26** |
| **Diogo** | Qt HMI & Cluster (21) + Qt Backend refactoring (8) | **37** |

**Total Sprint Points: 123**

---




## 🗺️ ADAS Module Roadmap (12 weeks — 6 sprints)

> Sprint 9 is the **first of 6 sprints** in the ADAS module. Below is a high-level roadmap to guide our planning across the full module.

| Sprint | Weeks | Theme | Key Deliverables |
|--------|-------|-------|------------------|
| **Sprint 9** (current) | 1-2 | Foundations & Spikes | Car v3.1, AEB start, AI/model research, OTA w/RAUC |
| **Sprint 10** | 3-4 | Perception & Control | Object detection model deployed on Hailo-8, AEB functional |
| **Sprint 11** | 5-6 | Lane Keeping & Cruise Control | LKA implementation, basic CC, CARLA simulation setup |
| **Sprint 12** | 7-8 | Integration & Autonomous Mode | Autonomous driving mode, sensor fusion, ACC (bonus) |
| **Sprint 13** | 9-10 | Testing & Validation | Comprehensive ADAS testing, CARLA validation, benchmarks |
| **Sprint 14** | 11-12 | Final Polish & Demo | System stabilization, documentation, demo preparation |

---

## 🎯 ADAS Module Goals Mapping

| Module Goal | Sprint 9 Contribution |
|-------------|----------------------|
| **Goal 1**: ADAS features (AEB, LDW, ACC, TSR) | AEB initial implementation + ADAS spike study |
| **Goal 2**: Perception, Control, Localization | ADAS spike covers perception/control theory |
| **Goal 3**: CARLA simulation | Not this sprint — planned for Sprint 11 |
| **Goal 4**: AI/ML/Deep Learning for ADAS | Model survey + AI spike (Hailo-8 evaluation) |
| **Goal 5**: GenAI applied to ADAS | AI spike — GenAI for data augmentation |
| **Goal 6**: OTA updates for ADAS | OTA with RAUC continuation |

---

## 📈 Actual Progress

- Sprint 9 points: **113 / 123**

- ❌ Undone:
   - Camera and sensor positioning optimization for ADAS
   - Define safety thresholds and response times (AEB)
---

## ✅ Outcomes

- **Delivered**:
  - ✅ Research on ADAS
  - ✅ OTA implementation with RAUC
  - ✅ AEB implemented
  - ✅ New cluster and HMI
  - ✅ Car Architecture v3.1

- **Demos**:
  -  https://github.com/user-attachments/assets/ba84e182-e34c-4a16-aa46-636038c26ac3
  -  https://github.com/user-attachments/assets/45c0ee6c-a703-49ad-913b-5d854b98bc17

  - <img width="584" height="269" alt="image" src="https://github.com/user-attachments/assets/0a8686de-59d5-4ffb-8299-dfbf5ee977f4" />
  - <img width="702" height="397" alt="image" src="https://github.com/user-attachments/assets/6f4d6248-5f2a-453a-8a61-0c96fd4b8e8d" />
  - <img width="1001" height="385" alt="image" src="https://github.com/user-attachments/assets/40b252c5-c5bb-40a0-b345-af96520a91c9" />
  - <img width="372" height="117" alt="image" src="https://github.com/user-attachments/assets/0eaaf71a-b81c-45b8-9a33-c45d5651b1c8" />
  - <img width="599" height="511" alt="image" src="https://github.com/user-attachments/assets/e28d31d1-3d56-4164-afad-2dacaff175c7" />
  - <img width="564" height="481" alt="image" src="https://github.com/user-attachments/assets/f51b2165-cc86-412b-8476-5a253d81be6a" />





- **Docs updated**:
  - [RAUC Documentation](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/ota/rauc/README.md)
  - [OTA Documentation](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/src/ota)
  - [Gen Ai Applied to ADAS](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/GenAI-for-ADAS-Spike.md)
  - [ADAS Spike](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/ADAS_Overview.md)

- **Tests**:
  - [OTA Tests](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/OTA/OTA_Comparison_Tests.md)

---

# 🔎 Retrospective
- ## **Went well**:
  - We were able to limit our sprint capacity to 100 points. 

- ## **To improve**: 
  - We need to move from research to reality — next sprint, we get our hands on real camera feeds and run our first model inference.

## 🔗 Useful Links
- Sprint board/view: [Sprint 9](https://github.com/orgs/SEAME-pt/projects/89/views/16)
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
