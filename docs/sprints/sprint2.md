# 🏁 Sprint 2 — (27/10/2025 → 07/11/2025)

> **Sprint Goal**: Refine the project setup - Hardware layout + Software architecture - in preparation of the implementation of automotive software 

---

## 📌 Epics
- [x] **Epic** — Definition of Car Architecture - Software & Hardware - [Epic 7 - #34](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/34)
- [x] **Epic** — Study Technologies - [Epic 8 - #35](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/35)
- [x] **Epic** — Qt App - Cluster Instrument - [Epic 9 - #36](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/36)
- [ ] **Epic** — Implement TSF in Github - [Epic 6 - #33](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/33)
- [x] **Epic** — Engineering Process Refinement - [Epic 10 - #37](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/37)

---

## 📈 Actual Progress

- ✅ Done:
  - **Hardware layout defined** — 3D model of the car + energy consumption scheme (expansion board removed).
  - **Software architecture defined** — studies & documentation on AGL, ThreadX, and CANbus.
  - **Cluster Instrument** — Qt app cross-compiled and **auto-starts on boot** (PiRacer).
  - **TSF** — deep study of Trustable Software Framework; baseline structure prepared.
  - **Documentation** — team produced thorough docs covering steps, decisions, and processes.

- ⏳ Blocked:
  - GitHub privileges / protection rules (PRs, Actions) pending confirmation.
  - Waiting for hardware items (on the way) to fullfill our hardware layout. (Update - arrived today!)

---

## ✅ Outcomes
- **Delivered**:
  - Car **hardware layout** and **3D model**.
  - **Energy/power scheme** with expansion board removal plan.
  - **Architecture baseline** (AGL + ThreadX + CAN).
  - **Qt cluster app** artifact that **boots automatically** on the display.
  - TSF groundwork (structure + studies).

- **Demos**:

**ARCHITECTURE**
EXPECT-L0-1 - "System architecture is defined and documented":
  - Energy consumption plan
  - ![Eletric-Scheme](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/eletric-scheme.png)

**HARDWARE**
EXPECT-L0-2 - "Hardware Assembly: Fully assemble the PiRacer robot":
  - 3D model of the car
  - ![3d-car](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/3d-car.jpg)
  - ![3d-car2](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/3d-car2.jpg)
  - ![3d-car3](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/3d-car3.jpg)

**QT:** 
EXPECT-L0-5 - "Launch basic Qt on built-in display"
  - Qt app showing on display
  - ![Qt_app](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/first_qt_app.jpeg)

EXPECT-L0-7 - "QT Crosscompilation should be achieved":
  - Crosscompilation scheme
  - ![Crosscompilation](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/system-architecture-cross.png)

**CAN:** 
EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)":
  - CANbus communication scheme
  - ![CANScheme](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/CANbus%20Scheme%20Updated.png)


- **Docs/Guides updated**:

**ARCHITECTURE**
EXPECT-L0-1 - "System architecture is defined and documented":
  - Power consuption analysis - [power-analysis-doc](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Power%20Consumption%20Analysis.md)

**SOFTWARE**
EXPECT-L0-3 - "Software Setup capable of running Qt applications":
  - AGL guide - [agl-guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/guides/AGL_Installation_Guide.md)
  
**QT:**
EXPECT-L0-7 - "QT Crosscompilation should be achieved":
  - Crosscompilation guide - [crosscompiling-guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/fix-qt-app-minor-changes/src/cross-compiler)

EXPECT-L0-3 - "Software Setup capable of running Qt applications":
EXPECT-L0-5 - "Launch basic Qt on built-in display":
  - Cluster (Qt app) guide - [cluster-guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/development/src/cluster)

**CAN:** 
EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)":
- CAN guide - [can-guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/guides/CAN-overview.md)
  
**TSF** (Trustable software framework): 
EXPECT-L0-14 - "TSF & Documentation":
  - TSF guide - [tsf-guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/TSF/WhatsTSF.md)



---

## 🔎 Retrospective
- **Went well**:
  - Clear division of work; solid documentation culture.
  - All of the **high priority** goals completed ✅
  - Sprint goal completed ✅
  - AGL installed on Raspberry Pi - **bonus objective!**

- **To improve**:
  - Spikes showcase - presenting the study done to the team.
  - Enhance our working methodology - **continuous work**.

## 🔗 Useful Links
- Sprint board/view: [Sprint 2](https://github.com/orgs/SEAME-pt/projects/89/views/7)


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
  EXPECT-L0-15 - "Agile and Scrum";
  EXPECT-L0-16 - "CI/CD";

