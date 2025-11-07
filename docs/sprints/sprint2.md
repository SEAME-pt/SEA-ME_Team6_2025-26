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
  - Qt app showing on display
  - ![Qt_app](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/first_qt_app.jpeg)
  - Crosscompilation scheme
  - ![Crosscompilation](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/fix-qt-app-minor-changes/docs/demos/system-architecture-cross.png)
  - 3D model of the car
  - ![3d-car](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/demos/3d-car.jpg)
  - ![3d-car2](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/demos/3d-car2.jpg)
  - ![3d-car3](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/demos/3d-car3.jpg)
  - Energy consumption plan
  - ![Eletric-Scheme](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/demos/eletric-scheme.png)
  - CANbus communication scheme
  - ![CANScheme](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/demos/CANbus%20Scheme%20Updated.png)
      
- **Docs updated**:
  - Power consuption analysis - [power-analysis-doc](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/guides/Power%20Consumption%20Analysis.md)
  - Crosscompilation guide - [crosscompiling-guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/fix-qt-app-minor-changes/src/cross-compiler)
  - Cluster (Qt app) guide - [cluster-guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/development/src/cluster)
  - TSF guide - [tsf-guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/TSF/WhatsTSF.md)
  - CAN guide - [can-guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/guides/CAN-overview.md)
  - AGL guide - [agl-guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/guides/AGL_Installation_Guide.md)

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
