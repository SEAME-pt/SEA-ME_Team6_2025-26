# 🏁 Sprint 6 — (05/01/2026 → 16/01/2026)

> ## **Sprint Goal**:  Improve end-to-end communication protocols
---

## 📌 Epics

- #### **Epic** — Car Software Architecture - [#52](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315398&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C52)
  - [x] Optimization of CAN Protocol (8)
  - [x] Manual Control Implementation (8)
  - [ ] Communication with Remote Display (13 - 8 achieved)
  - [x] Continue KUKSA Implementation (8)
  - [x] BONUS: Redesign of Car Hardware Architecture
#142 (8)

- #### **Epic** — Study and Integration of Core Technologies - [#56](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138316020&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C56)
  - [x] MQTT Spike (8)
- #### **Epic** - Testing and Validate Software - [#103](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/issues/103)
  - [x] Development of Unit Tests (13)
- #### **Epic** — Documentation - [#54](https://github.com/orgs/SEAME-pt/projects/89/views/1?pane=issue&itemId=138315799&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C54)
  - [x] Team Knowlegde Update (13)
  - [x] Creation of documentation (13)
---



## 📈 Actual Progress

- ## Sprint 6 points: 79/84

- ## Undone:
- ❌ Undone: Communication with Remote Display (13 - 8 achieved)
. still missing cross-compilation to allow us to provide communication between kuksa and QT using real data.
---

## ✅ Outcomes

- **Delivered**:
  - Redesign and reassembly of the car's hardware architecture - ready to move to next step: camera and sensors installation).
  - All STM32 Sensors (temperature, humidity, pressure, ambient light, accelerometer,  gyroscope, magnometer, time of flight - TOF) configured and working (sending data throught can to kuksa).
  - Optimize can messages: can frames message structure reorganization (environment, proximity, speed, heartbeat, IMU).
  - Communication end-to-end via CAN with KUKSA improved: VSS tree implementation (our VSS tree with more signals), increased signals to pass through communication (battery, emergency stop, heartbeat, environment, IMU acceleration, IMU gyroscope, time of flight - TOF)   
  . `Speedometer <-> STM32 (ThreadX) Sensors <-> CAN Bus <-> Raspberry Pi 5 (AGL) - (CAN decoder + KUKSA publisher) <-> KUKSA Databroker -> Qt App`
  - Implementation of manual controls (joystick and keyboard) done from AGL through Can to STM32.
  - Implementation of unit testing on: STM32 (emergency stop tests, speedometer tests, TOF tests), QT (speed, temperature provider, system status) and Can (Can Message Handlers).
  - Documentation and Team Knowledge Done: AGL, Tests, Can, Kuksa, Covesa, MQTT/Mosquito, UProtocol.
  
- **Demos**:
- car_architecture_old - EXPECT-L0-1 - "System architecture is defined and documented"
![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/car_architecture_old%20-%20sprint6.jpeg)

- car_architecture_new_1 - EXPECT-L0-10 - "Implement new architecture."
![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/car_architecture_new_1%20-%20sprint6.jpg)

- car_architecture_new_2 - EXPECT-L0-10 - "Implement new architecture."
![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/car_architecture_new_2%20-%20sprint6.jpg)

- STM sensors - EXPECT-L0-9 - "ThreadX running on the Microcontroller (STM32) as a RTOS"
(https://youtube.com/shorts/vUMV0UnSf04)

- stm sensors map scheme - EXPECT-L0-10 - "Implement new architecture"
![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/STM_Ilustra.jpg)

- Emergency brake with sensors - EXPECT-L0-9 - "ThreadX running on the Microcontroller (STM32) as a RTOS"
(https://youtube.com/shorts/rN4BKjbAcKE)

- Can frames protocol structs - EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)"
![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/can_frames_protocol_structs%20-%20sprint6.jpeg)

- stm-can transmission - EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)"
![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/stm_can_transmission%20-%20sprint6.jpg)

- kuksa -> Rasp4 reading data - EXPECT-L0-8 - "Use CAN protocol to connect the Rasp5 to the STM 32(bidirectional)"
(https://youtu.be/mMvbhmvh8Fg)

- qt crosscompiling
![Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/crosscompiling_scheme-sprint6.jpeg)

- **Docs updated**:
- MQTT session -  EXPECT-L0-10 - "Implement new architecture." [README.md] (https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/MQTT_spike.md)

- **Tests**:
- QT unity tests - EXPECT-L0-12 - "Wireless DCI display"[!Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/LCOV_coverage_report_QT%20-%20sprint%206.jpeg)

- kuksa tests - - EXPECT-L0-12 - "Wireless DCI display"[!Image](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/LCOV_coverage_report%20-%20sprint6.jpeg)


---

# 🔎 Retrospective
- ## **Went well**:
  - ### Objetive completed - Improve end-to-end communication protocols!
  - ### Bonus tasks:
      - #### Redesign of Car Hardware Architecture
      
- ## **To improve**: 
  - ### Better definition of issues - break big tasks into smaller ones!

  ## 🔗 Useful Links
- Sprint board/view: [Sprint 6](https://github.com/orgs/SEAME-pt/projects/89/views/13)

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
  EXPECT-L0-15 - "Agile and Scrum":
  EXPECT-L0-16 - "CI/CD":
  EXPECT-L0-17 - "Introduce AI in the project":
