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
    
- Unit Tests:
  - EXPECT-L0-16 - "CI/CD"
  - EXPECT-L0-31 - "Unit tests"
    
    - ![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/CI-CD-tests-sprint7.png)

- Coverage Report:
  - EXPECT-L0-16 - "CI/CD"
  - EXPECT-L0-31 - "Unit tests"
    
    - ![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/coverage-report-sprint7.png)

- TSF requirements:
  - EXPECT-L0-14 - "TSF & Documentation"
  - EXPECT-L0-19 - "Test requirement for TSF automation"
  - EXPECT-L0-21 - "Test the complete TSF automation workflow"
    - https://github.com/user-attachments/assets/ddbec832-6fa0-4775-9f85-cee60e18d6c0



- TSF automation:
  - EXPECT-L0-14 - "TSF & Documentation"
  - EXPECT-L0-19 - "Test requirement for TSF automation"
  - EXPECT-L0-21 - "Test the complete TSF automation workflow"
    - https://github.com/user-attachments/assets/2dd313fc-2a01-403a-8a98-09db62c380ee
    - https://github.com/user-attachments/assets/c73075ca-e46a-42ef-9375-266f1cd51844
   
<img width="1168" height="729" alt="1  Sync Screenshot 2026-01-30 at 16 02 49" src="https://github.com/user-attachments/assets/12b5fd59-f067-4699-949a-2b08fb09dc6a" />



  
 - https://github.com/user-attachments/assets/71f868f6-5710-4b5d-b398-342c3eabc981
 - https://github.com/user-attachments/assets/5b0fe7f8-6e8e-4221-87c6-f0af47443a3d
   
<img width="1145" height="728" alt="1  check Screenshot 2026-01-30 at 15 59 18" src="https://github.com/user-attachments/assets/f08b2def-b9e0-4f68-ac2a-db47afe58e15" />

<img width="1120" height="709" alt="2  check Screenshot 2026-01-30 at 15 59 26" src="https://github.com/user-attachments/assets/319a8bea-9b10-483e-ba08-c5f12b59f854" />

<img width="1100" height="723" alt="3  check Screenshot 2026-01-30 at 15 59 51" src="https://github.com/user-attachments/assets/d8f5f5ff-8409-444b-9e43-05e9a8e784f3" />


<img width="1314" height="658" alt="1  Screenshot 2026-01-30 at 15 41 21" src="https://github.com/user-attachments/assets/40bbf372-6a1f-4ec0-8fd4-472bc3c17c13" />

<img width="1301" height="657" alt="2  Screenshot 2026-01-30 at 15 42 05" src="https://github.com/user-attachments/assets/167f3012-d899-4c58-9f56-3ffe216b9098" />

<img width="1289" height="640" alt="3  Screenshot 2026-01-30 at 15 42 21" src="https://github.com/user-attachments/assets/8774796d-9a81-4aea-97ea-f631f2d1a34c" />

<img width="1272" height="658" alt="4  Screenshot 2026-01-30 at 15 42 49" src="https://github.com/user-attachments/assets/b4e4a52d-9678-4a65-ba1e-a3aa3ea7e5e3" />

<img width="1278" height="642" alt="5  Screenshot 2026-01-30 at 15 43 06" src="https://github.com/user-attachments/assets/39efa45b-f4f0-4aa6-99b3-dbe5f7b6e797" />

<img width="1274" height="641" alt="6  Screenshot 2026-01-30 at 15 43 28" src="https://github.com/user-attachments/assets/f6cd9e83-fdde-4be7-9e1c-74799e9f1efd" />

<img width="1274" height="641" alt="7  Screenshot 2026-01-30 at 15 43 28 1" src="https://github.com/user-attachments/assets/671e88dd-c4f2-45ee-977c-fd6a00102c8d" />

<img width="1285" height="572" alt="8  Screenshot 2026-01-30 at 15 45 22" src="https://github.com/user-attachments/assets/3fea2956-2cf9-4682-a251-ab304d85e9c6" />

<img width="1276" height="619" alt="9  Screenshot 2026-01-30 at 15 45 44" src="https://github.com/user-attachments/assets/2e2aca3a-ed7b-493d-a820-0760310e4744" />


https://github.com/user-attachments/assets/527a3263-0580-4768-b182-8a8f1f484b4d


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
- unit tests - ![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/CI-CD-tests-sprint7.png)
- coverage - ![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/coverage-report-sprint7.png)


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
