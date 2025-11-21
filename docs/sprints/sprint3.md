# 🏁 Sprint 3 — (10/11/2025 → 21/11/2025)

> ## **Sprint Goal**: Implement Automotive Grade Linux (AGL) and ThreadX RTOS environments, and establish CAN communication between the microcontroller, sensors, and the Raspberry Pi.

---

## 📌 Epics
- #### **Epic** — Car Software Architecture - [#52](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315398&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C52)
  - [x] Install and configure **AGL** on the Raspberry Pi 
  - [x] Install and test **ThreadX RTOS** on the STM32
  - [x] Establish **CANbus communication** between microcontroller and Raspberry Pi 
  - [x] Configure **auto-start of the Qt cluster app** on AGL (migrated from Bookworm)
- #### **Epic** — Car Hardware Architecture - [#53](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315665&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C53)
  - [x] Test the **new hardware layout** without the expansion board 
  - [x] Validate power distribution and component behavior under load
- #### **Epic** — Documentation - [#54](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315799&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C54)
  - [ ] Approve and apply **documentation tutorials** 
  - [ ] Review and approve **documentation templates and structure**
- #### **Epic** — Project Management & Traceability Refinement - [#55](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315926&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C55)
  - [x] Refine **TSF implementation** and traceability setup in GitHub 
  - [ ] Continue the **study of GenAI** and its potential application with the Hailo accelerator
- #### **Epic** — Study and Integration of Core Technologies - [#56](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138316020&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C56)
  - [x] Study **COVESA** uServices for future integration
  - [x] Study **uProtocol** for data exchange between ECUs 


---

## 📈 Actual Progress

- ✅ Done: Sprint goal achieved! 🎉

- ❌ Undone: Some spikes and review documentation tutorials.

---

## ✅ Outcomes
- **Delivered**:
  - Hardware layout validated (component test - not "under load" test) ✅
  - Running Qt app on AGL ✅
  - CAN communication validated ✅
  - ThreadX running on STM32 ✅
- **Demos**:
  - Validation of energy supply for Raspberry Pi 5
  - ![Rasp energy validation](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/demos/Validation-of-energy-layout-sprint3.jpeg)
  - Cluster updates
  - ![Atual state cluster](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/demos/Cluster-display-sprint3.jpeg)
  - 
  - ![Cluster mockup](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/demos/Cluster-mockup-sprint3.jpeg)
  - 
  - ![Cluster study](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/demos/Cluster-study.jpeg)
  - CAN layout
  - ![CAN layout](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/demos/CAN-layout-sprint3.jpeg) 
      
- **Docs updated**:
  - [AGL Guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/guides/AGL_Installation_Guide.md)
  - [ThreadX Guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/guides/ThreadX_Installation_Guide.md)
  - [CAN implementation](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/guides/CAN-test-guide.md)
  - [TSF Guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/TSF/WhatsTSF.md)
  - [Joystick on AGL](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/guides/Install%20and%20Test%20Joystick%20in%20AGL.md)

---

## 🔎 Retrospective
- **Went well**:
  - Team is focused in the **sprint goal**

- **To improve**: 
  - Reserve at least **1 full day** in the sprint only for documentation review and approval.

## 🔗 Useful Links
- Sprint board/view: [Sprint 3](https://github.com/orgs/SEAME-pt/projects/89/views/8?visibleFields=%5B%22Title%22%2C%22Assignees%22%2C%22Status%22%2C%22Linked+pull+requests%22%2C%22Sub-issues+progress%22%5D&sortedBy%5Bdirection%5D=&sortedBy%5BcolumnId%5D=)
