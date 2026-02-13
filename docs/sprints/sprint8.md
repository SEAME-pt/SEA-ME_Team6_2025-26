# 🏁 Sprint 8 — (02/02/2026 → 13/02/2026)

> ## **Sprint Goal**:  Finalize this Module and prepare the car for the next Module
---

## 📌 Epics

- #### **Epic** — Car Software Architecture - [#52](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315398&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C52)
  - [X] Refactored STM32 ThreadX application (13)
  - [X] OTA implementation (13)
  - [X] Creation of backup service of AGL (5)
  - [X] Qt design modifications - integrate more data from KUKSA (13)
  - [X] Implement security on KUKSA (TLS/JWT) (8)

- #### **Epic** — Study and Integration of Core Technologies - [#56](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138316020&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C56)
  - [X] Message queue Spike (8)
  - [X] AEB (Automatic Emergency Brake) Spike (8)
  - [X] OTA multiversion Spike (8)

- #### **Epic** - Testing and Validate Software - [#103](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/103)
  - [X] Temperature benchmark (cooler vs no cooler) (5)
  - [X] Joystick unit tests (3)
  - [X] Integration of tests - Python tests + Cpp tests (5)

- #### **Epic** — Documentation - [#54](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315799&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C54)
  - [X] TSF Documentation review (3)
  - [X] Review TSF evidences (5)
  - [ ] Restructure folder structure and documentation (3)
  
---

## 📈 Actual Progress

- ## Sprint 8 points: 97/100

- ## Undone:
.  - [ ] Reorganize and Update documentation (3)  
.  - [ ] Integrate RAUC

---

## ✅ Outcomes

- **Delivered**:

I. Epic — Car Hardware Architecture - #53
  - Nothing new 

II. Epic — Car Architecture - Software - #52
  - Creation of script to backup all the data from AGL data partition
  - Instrument Cluster design modifications with new data displayed
  - BONUS: HMI creation and basic design
  - Security on KUKSA using TLS and JWT to allow communication
  - OTA system fully functional (Phase A.2 complete)
    - CI/CD pipeline: git push tag → ARM build → GitHub Release → AGL install
    - Binaries deployed: `can_to_kuksa_publisher` + `HelloQt6Qml`(cluster)
  - Refactor of the ThreadX application code


III. Epic — Testing and Validate Software - #103
  - Tests for the joystick control created and added to the unit test action
  - Creation of a temperature benchmark analysis with and without cooler with respective evidence
  - Tests unified into a single workflow with coverage report

IV. Epic — Project Management & Traceability Refinement - #55
  - Nothing new

V. Epic — Study and Integration of Core Technologies - #56
  - Deeper understanding of AEB, OTA multiversion and Message Queue

VI. Epic — Documentation - #54
  - Update of TSF content documetation and content organization (index)
  - Review and creation of new Requirements
  - BONUS: VSS tree file documentation for easy understading

- **Demos**:
  - OTA
    - ![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/OTA-Release.png)
    - ![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/OTA-Tags.png)
    - ![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/OTA-Action.png)
  - Cluster Update
    - EXPECT_L0_25 - "Instrument Cluster availability (Qt on Raspberry Pi 4)"
    - EXPECT_L0_12 - "Wireless DCI Display"
    - EXPECT_L0_7 - "QT Crosscompilation should be achieved"
      - https://github.com/user-attachments/assets/c83fda50-2420-4e96-836c-1df1c79a8fcd
  - HMI
    - https://github.com/user-attachments/assets/7c20d35f-e41d-41df-9ad1-ca6b90f5dcd8

- **Docs updated**:
  - `docs/guides/OTA/OTA_Implementation_Guide.md`
  - `docs/guides/OTA/OTA_Presentation-sprint8.md`
  - `docs/guides/OTA/OTA_multiversion-arch-sprint8.md`
  - `docs/guides/VSS.md` — VSS Tree Signals designation
  - `docs/TSF/TSF-docs/*.md` — TSF Documentation all changed
  - `docs/sprints/sprint8.md` — Sprint progress

- **Tests**:
  - OTA
    - ![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/OTA-r5-script.jpeg)
    - ![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/OTA-r4-script.jpeg)
  - Coverage
    - ![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/coverage-report-srint8.png)

---

# 🔎 Retrospective
- ## **Went well**:
  - #### Good end of Module and preparation for the next one!
  - #### Inclusion of Bonus Tasks

  - ### Bonus Tasks:
    - #### Another DCI Display setup for the HMI
    - #### HMI created with a simple Qml Display
      
- ## **To improve**: 
  - #### Managing time in order to not let any task behind

  ## 🔗 Useful Links
- Sprint board/view: [Sprint 8](https://github.com/orgs/SEAME-pt/projects/89/views/15)
- Releases view: [Releases](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/releases/tag/v1.9.0)

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
