# 🏁 Sprint 7 — (19/01/2026 → 30/01/2026)

> ## **Sprint Goal**: Stabilize the system with automated testing and CI/CD pipeline
---

## 📌 Epics

- #### **Epic** — Car Hardware Architecture - [#53](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315665&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C53)
  - [X] Assemble camera, LCD, and front sensor (3)

- #### **Epic** — Car Software Architecture - [#52](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315398&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C52)
  - [X] Development Environment Setup - Rethink and structure Dockerfiles (13)
      - Define cross-compilation images and development environment images
      - Evaluate layer reuse
      - Ensure consistency in environments
      - Packages on GitHub
  - [X] Continuation of Kuksa implementation with Qt (Continuation) (8)
      - Continue Kuksa integration with Qt
      - Solve integration issues
      - Test communication

- #### **Epic** - Testing and Validate Software - [#103](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/103)
  - [X] New tests missing for stm sensors (8)
  - [X] CAN message handlers tests (8)
  - [X] Unification of tests (8)
      - Execution method
      - Eliminate redundancies
      - Ensure all tests can be executed consistently
  - [X] Integrate tests with TSF and CI/CD to generate coverage report (13)

- #### **Epic** — Project Management & Traceability Refinement - [#55](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315926&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C55)
  - [X] Create new requirements, review requirements in TSF (13)

- #### **Epic** — Documentation - [#54](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315799&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C54)
  - [X] Create new car architecture documentation - new layout sensors (5)
  - [ ] Reorganize and Update documentation (5)
  - [X] Spread knowledge (5)
    - Team Knowlegde Update (TSF, github, agile scrum, CI/CD)
  
---

## 📈 Actual Progress

- ## Sprint 7 points: 84/89

- ## Undone:
- ❌ Undone: 
.  - [ ] Reorganize and Update documentation (5)

---

## ✅ Outcomes

- **Delivered**:

I. & II. Epic — Definition of Car Architecture - Software & Hardware - Epic 7 - #34

I. Epic — Car Hardware Architecture - #53
- Camera and sensors installation.
- New sensor to provide the battery status.
- New architecture diagram. 
- Car architecture finalized: Version 2.1.
- Presentation of the car with PoF (Proof of Concept): driving the car against a wall to prove that the emergency stop works using the front ultrasound sensors.

II. Epic — Car Architecture - Software - #52
- AGL build ready for Raspberry Pi 4 and Raspberry Pi 5, with SDK and RAUC, enabling:
  - End-to-end communication working.
- Development Environment Setup — Rethink and restructure Docker files #159 & Communication with Remote Display #129, allowing us to:
  - Display data in Qt.

III. Epic — Testing and Validate Software - #103
- Tests with CI/CD — packages (Qt, CAN, joystick, STM?)

IV. Epic — Project Management & Traceability Refinement - #55
- TSF: 
  - implementation, new requirements included. 
  - flow and process automation.
    
- **Demos**:

- Emergency Stop:
  - EXPECT-L0-27 - "Emergency braking stopping distance" 
    - https://github.com/user-attachments/assets/d39b158c-09e4-40ad-a6cf-308556d1b6da

- QT display live:
  - EXPECT-L0-25 - "Instrument Cluster availability (Qt on Raspberry Pi 4)"
  - EXPECT-L0-12 - "Wireless DCI display"
  - EXPECT-L0-7 - "QT Crosscompilation should be achieved" 
    - https://github.com/user-attachments/assets/b1ba19ad-0433-4411-96e4-704a57a6d639
    
- Unity Tests:
  - EXPECT-L0-14 - "TSF & Documentation"
  - EXPECT-L0-19 - "Test requirement for TSF automation"
  - EXPECT-L0-21 - "Test the complete TSF automation workflow"
    - ![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/CI-CD-tests-sprint7.png)

- Coverage Report:
  - EXPECT-L0-14 - "TSF & Documentation"
  - EXPECT-L0-19 - "Test requirement for TSF automation"
  - EXPECT-L0-21 - "Test the complete TSF automation workflow"
    - ![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/coverage-report-sprint7.png)

- TSF requirements:
  - EXPECT-L0-14 - "TSF & Documentation"
  - EXPECT-L0-19 - "Test requirement for TSF automation"
  - EXPECT-L0-21 - "Test the complete TSF automation workflow"

- TSF automation:
  - EXPECT-L0-14 - "TSF & Documentation"
  - EXPECT-L0-19 - "Test requirement for TSF automation"
  - EXPECT-L0-21 - "Test the complete TSF automation workflow"
  
- Reorganize and Update documentation:
  - EXPECT-L0-13 - "Project & GitHub setup"
  - EXPECT-L0-14 - "TSF & Documentation"
  - EXPECT-L0-15 - "Agile and Scrum"
    - ![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/folder-structure2-sprint7.jpeg)


- **Docs updated**:
  - Car circuit image -  EXPECT-L0-10 - "Implement new architecture." [circuit_image] (https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Car_Architecture/circuit_image.png)
  - Car circuit image explanation -  EXPECT-L0-10 - "Implement new architecture." [Circuit_Connection_Diagram] (https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Car_Architecture/Circuit_Connection_Diagram.md)
  - Sensor Integration w/kuksa -  EXPECT-L0-10 - "Implement new architecture." [circuit_image] (https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Car_Architecture/Sensor_Integration_from_Sensor2Kuksa.md)


- **Tests**:
- 


---

# 🔎 Retrospective
- ## **Went well**:
  - #### Goal completed - Stabilize the system with automated testing and CI/CD pipeline!
  - #### Better definition of issues - break big tasks into smaller ones!
  - #### Inclusion of Bonus Tasks.

  - ### Bonus tasks:
      - #### Ultrasound sensor implemented and working (car stops when an obstacule appears at the front).
      - #### TSF automatization.
      - #### OTA practical spike.
      - #### AGL build w/RAUC for RP5 & RP4.
      - #### Practical spike: Gesture recognition to demonstrate that the car can be commanded through gestures.


- ## **To improve**: 
  - #### Manage time in a better way and with more motivation regarding some tasks, in order not to let any task behind (namely Reorganize and Update documentation (5))

  ## 🔗 Useful Links
- Sprint board/view: [Sprint 7](https://github.com/orgs/SEAME-pt/projects/89/views/14)

## TSF Useful:

  EXPECT-L0-1 - "System architecture is defined and documented"
  
  EXPECT-L0-2 - "Hardware Assembly: Fully assemble the PiRacer robot"
  
  EXPECT-L0-3 - "Software Setup capable of running Qt applications"
  
  EXPECT-L0-4 - "Driveable Car with Remote Control"
  
  EXPECT-L0-5 - "Launch basic Qt on built-in display"
  
  EXPECT-L0-6 - "The Qt GUI shall display static graphical elements"
  
  EXPECT-L0-7 - "QT Crosscompilation should be achieved"
  
  EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)"
  
  EXPECT-L0-9 - "ThreadX running on the Microcontroller (STM32) as a RTOS"
  
  EXPECT-L0-10 - "Implement new architecture"
  
  EXPECT-L0-11 - "QT cluster UI auto-start on boot on display"
  
  EXPECT-L0-12 - "Wireless DCI display"
  
  EXPECT-L0-13 - "Project & GitHub setup"
  
  EXPECT-L0-14 - "TSF & Documentation"
  
  EXPECT-L0-15 - "Agile and Scrum"
  
  EXPECT-L0-16 - "CI/CD"
  
  EXPECT-L0-17 - "Introduce AI in the project"
  
  EXPECT-L0-18 - "The vehicle shall communicate via CAN bus also using ThreadX"
  
  EXPECT-L0-19 - "Test requirement for TSF automation"
  
  EXPECT-L0-20 - "Integrate AI for content generation"
  
  EXPECT-L0-21 - "Test the complete TSF automation workflow"
  
  EXPECT-L0-22 - "STM32 ThreadX safety-related startup"
  
  EXPECT-L0-23 - "Raspberry Pi 5 AGL boot time"
  
  EXPECT-L0-24 - "Combined startup (ThreadX + AGL)"
  
  EXPECT-L0-25 - "Instrument Cluster availability (Qt on Raspberry Pi 4)"
  
  EXPECT-L0-26 - "End-to-end system startup time (ThreadX + AGL + Qt)"
  
  EXPECT-L0-27 - "Emergency braking stopping distance"
  EXPECT-L0-28 - "Exterior temperature measurement accuracy"
  EXPECT-L0-29 - "Driver presence condition"
