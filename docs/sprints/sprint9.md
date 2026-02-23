# 🏁 Sprint 9 — (23/02/2026 → 06/03/2026)

> ## **Sprint Goal**: *MUDAR ESTA PARTE *Kickstart the ADAS module — establish AI/perception foundations, begin AEB implementation, and finalize car physical architecture (v3.1)
---

## 📌 Epics

- #### **Epic** — Car Hardware Architecture - [#53](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315665&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C53)
  - [ ] Reformulation of physical car architecture — Version 3.1 (13)
      - Final physical layout design
      - Camera and sensor positioning optimization for ADAS

- #### **Epic** — Car Software Architecture - [#52](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315398&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C52)
  - [ ] OTA implementation with RAUC (continuation) (13)
      - Continue RAUC integration for A/B partition management
      - Validate OTA update flow end-to-end
      - Multi-version deployment support
  - [ ] AEB — Automatic Emergency Braking initial implementation (13)
      - Design AEB architecture
      - Implement progressive braking algorithm on ThreadX
      - Define safety thresholds and response times
  - [ ] Qt HMI & Cluster continued development (21)
      - HMI design improvements and new ADAS-related displays
      - Cluster integration with new data signals
      - Continuous work throughout the sprint
  - [ ] Qt Backend refactoring (8)
      - Code restructuring for scalability
      - Improve data model and KUKSA integration

- #### **Epic** — Study and Integration of Core Technologies - [#56](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138316020&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C56)
  - [ ] ADAS Spike — Perception, Control & Path Planning (13)
      - Study AEB, LDW, ACC, TSR concepts and feasibility
  - [ ] AI/ML Model Survey (13)
      - Research open-source models for object detection and obstacle avoidance
  - [ ] AI Spike — GenAI applied to ADAS (13)
      - Study GenAI tools for data augmentation and scenario generation
      - Evaluate GANs, diffusion models, transformers for ADAS context
  
- #### **Epic** — Project Management & Traceability Refinement - [#55](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315926&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C55)
  - [ ] TSF refinement — complete pending tasks from previous module (8)
      - Review and close open TSF requirements
      - Refine TSF automation algorithm
---

## 📊 Sprint Points Summary

| Member | Tasks | Points |
|--------|-------|--------|
| **João** | OTA with RAUC (13) + TSF refinement (8) + AI Spike (13) | **34** |
| **Ruben** | Car Architecture v3.1 (13) + AEB implementation (13) | **26** |
| **David** | AEB implementation (shared with Ruben) + ThreadX ADAS support | **~13** |
| **Vasco** | ADAS Spike (13) + AI/ML Model Survey (13) | **26** |
| **Diogo** | Qt HMI & Cluster (21) + Qt Backend refactoring (8) | **29** |

**Total Sprint Points: 115**

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

- Sprint 9 points: **___ / ~115**

- ❌ Undone: 

---

## ✅ Outcomes

- **Delivered**:
  - _(to be filled at sprint end)_

- **Demos**:
  - _(to be filled at sprint end)_

- **Docs updated**:
  - _(to be filled at sprint end)_

- **Tests**:
  - _(to be filled at sprint end)_

---

# 🔎 Retrospective
- ## **Went well**:
  - _(to be filled at sprint end)_

- ## **To improve**: 
  - _(to be filled at sprint end)_

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
