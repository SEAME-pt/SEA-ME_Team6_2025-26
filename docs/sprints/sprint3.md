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
  - ThreadX running on STM32 

  
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
  
- **Demos**:

**ARCHITECTURE**
EXPECT-L0-1 - "System architecture is defined and documented":
EXPECT-L0-10 - "Implement new architecture.":
  - Validation of energy supply for Raspberry Pi 5
  - ![Rasp energy validation](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint3.jpeg)

**QT**
EXPECT-L0-6 - "The Qt GUI shall display static graphical elements":
  - Cluster updates
  - ![Atual state cluster](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Cluster-display-sprint3.jpeg)
  - ![Cluster mockup](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Cluster-mockup-sprint3.jpe
  - ![Cluster study](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Cluster-study.jpeg)

  **CAN**
EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)":
- CAN layout
    - ![CAN layout](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/CAN-layout-sprint3.jpeg) 
      
- **Docs updated**:
  **SOFTWARE**
EXPECT-L0-3 - "Software Setup capable of running Qt applications":
  - [AGL Guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/AGL_Installation_Guide.md)
EXPECT-L0-9 - "ThreadX running on the Microcontroller (STM32) as a RTOS";
  - [ThreadX Guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/ThreadX_Installation_Guide.md
EXPECT-L0-3 - "Software Setup capable of running Qt applications":
EXPECT-L0-9 - "ThreadX running on the Microcontroller (STM32) as a RTOS";
  - [AGL VS ThreadX ]https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/AGL%20and%20ThreadX%20benefits%20Guide.md

**QT**
EXPECT-L0-5 - "Launch basic Qt on built-in display":
  - [QML guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/QML-guide.md)
  - [Cluster Instrument](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/src/cluster/README.md)
  
**CAN**
EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)":
  - [CAN implementation](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/CAN-test-guide.md)
**TSF**
EXPECT-L0-14 - "TSF & Documentation":
  - [TSF Guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/WhatsTSF.md)

**JOYSTICK**
EXPECT-L0-4 - "Driveable Car with Remote Control":
  - [Joystick on AGL](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Install%20and%20Test%20Joystick%20in%20AGL.md)



---

## 🔎 Retrospective
- **Went well**:
  - Team is focused in the **sprint goal**

- **To improve**: 
  - Reserve at least **1 full day** in the sprint only for documentation review and approval.

## 🔗 Useful Links
- Sprint board/view: [Sprint 3](https://github.com/orgs/SEAME-pt/projects/89/views/8?visibleFields=%5B%22Title%22%2C%22Assignees%22%2C%22Status%22%2C%22Linked+pull+requests%22%2C%22Sub-issues+progress%22%5D&sortedBy%5Bdirection%5D=&sortedBy%5BcolumnId%5D=)


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
