# Level 0 (Client) Requirements — PiRacer Warm-Up Project

This document defines the **Level 0 (Client) Requirements** for the PiRacer Warm-Up module.  
Each requirement is written from the client’s perspective, includes measurable acceptance criteria,  
and is testable and traceable to the system architecture and corresponding test cases.  
These requirements establish the scope, priorities, and evaluation criteria for the project.

---

## Table of Contents
1. [Overview](#overview)  
2. [Level 0 Requirements](#level-0-requirements)  
3. [Traceability and Verification](#traceability-and-verification)

---

## Overview

The PiRacer Warm-Up project aims to familiarize students with:
- The **PiRacer robot hardware**, including the Raspberry Pi 5, Hailo AI Hat, DC and servo motors, and built-in display.
- The development of a **simple Qt-based graphical application** running on the PiRacer’s display.
- The use of **Git and GitHub** for collaborative software development.
- The application of **Agile (Scrum)** methodologies for team organization and project management.
- The integration of the **Trustable Software Framework (TSF)** to ensure traceability between requirements, design, and testing.

---

## Level 0 Requirements

| **ID** | **Requirement (Client View)** | **Acceptance Criteria** | **Verification Method** | **Evidence** |
|--------|------------------------------|------------------------|------------------------|--------------|
| **L0-1** | The system architecture shall be defined and documented to describe hardware and software components, including the Expansion Board, Raspberry Pi 5, Hailo AI Hat, DC motors, servo motor, and display interface. | A system architecture diagram or document is present in the repository. | Documentation inspection. | See EXPECT-L0-1 for demos, guides, and artifacts |
| **L0-2** | Hardware Assembly: Fully assemble the PiRacer robot, including the Raspberry Pi 5, Hailo AI Hat, DC motors, servo motor, and display interface (first with the old architecture. Then with the new one). | All hardware components are correctly installed and operational. The PiRacer can power on and communicate with the display and motors. | Physical inspection and functional test. | See EXPECT-L0-2 for hardware assembly demos |
| **L0-3** | Software Setup: The system shall successfully boot the Raspberry Pi 5 and load a Linux environment (Trixie, Bookworm and finally AGL) capable of running Qt applications. | The PiRacer boots without system errors, and Qt development tools are accessible from the command line. | System boot demonstration and OS verification. | See EXPECT-L0-3 for AGL installation guides and cluster code |
| **L0-4** | Driveable Car with Remote Control/Driveable Car Model (Hardware + Software): The PiRacer shall support remote control functionality to operate the motors (steering and propulsion). | The system can receive remote input to control motor direction and speed. The car shall accept `forward`, `back`, `left`, `right`, and `stop` commands, and execute them reliably (controller event → wheel actuation) | Live demonstration or test log. | See EXPECT-L0-4 for joystick installation guides |
| **L0-5** | The system shall be capable of launching a basic Qt application on the PiRacer's built-in display. | When executed, the program successfully opens a Qt GUI window on the PiRacer's screen. | Demonstration or screenshot verification. | See EXPECT-L0-5 for Qt demo screenshots and QML guides |
| **L0-6** | The Qt GUI shall display static graphical elements such as shapes, text, or images to form a simple user interface. | Upon application launch, the display shows visible static content (e.g., rectangle, label, icon). | Visual inspection of the running application. | See EXPECT-L0-6 for UI mockups and cluster demos |
| **L0-7** | QT Crosscompilation should be achieved. | We should be able to crosscompile our QT program outside the Operating System and the included the exec file running on it. | QT program crosscompliation validated with final exec file obtained, and execution of the exec file in the Rasp5 OS. | See EXPECT-L0-7 for cross-compiler setup and guides |
| **L0-8** | Connect the Rasp5 to the STM 32 (bidirectional) using Can Protocol. | A communication signal should be sent by the Rasp 5 and received in the STM32. This should be observed from Rasp 5 to STM and from STM to Rasp5 (bidirectional communication). | Visual inspection of message passing from RP5 to STM and vice-versa. | See EXPECT-L0-8 for CAN protocol guides and diagrams |
| **L0-9** | ThreadX should be used on the Microcontroller (STM32) as a RTOS, receiving signals from the speedometer and controlling the servos and DC motor. | We should be able to install and use ThreadX on the Microcontroller (STM32), making all communications (including can) to pass in the STM32 and be controlled/managed there using priorizations and threads. | Software test including Visual inspection of the message bidirectional transmission. | See EXPECT-L0-9 for ThreadX installation guides |
| **L0-10** | Implement and test the changing from the old architecture to the new architecture (without the expansion board) including power consumption study, new electric scheme, individual module test, and visual test of all the components working. | A system architecture diagram and documents regarding power consumption and new electic scheme are present in the repository. | Documentation inspection. | See EXPECT-L0-10 for architecture migration validation |
| **L0-11** | The Qt cluster UI shall **auto-start on boot** and display fullscreen on the car's built-in display. | Upon application launch, the program successfully opens a Qt GUI window on the PiRacer's screen. | Visual inspection of the running application. | See EXPECT-L0-11 for cluster autostart configuration |
| **L0-12** | The DCI display fullscreen will be out of the car (wireless display) lauching Qt (first without autostart, then with autostart). | Upon application launch, the wireless display should open a Qt GUI window on the PiRacer's screen with our program. | Visual inspection of the running application. | See EXPECT-L0-12 for wireless display setup |
| **L0-13** | Project & GitHub setup: The team shall set up a GitHub repository hosting the project code, documentation, and version control history. | Repository contains all relevant source code, README, and requirements files. Branches, commits, and pull requests show collaboration among members. | GitHub repository inspection. | See EXPECT-L0-13 for GitHub guidelines and setup |
| **L0-14** | TSF & Documentation: The project shall apply the Trustable Software Framework (TSF) to define and trace requirements, architecture, and test cases. | TSF documentation exists within the repository, showing traceability. | Documentation review in the GitHub repository. | See EXPECT-L0-14 for TSF documentation |
| **L0-15** | Agile: The team shall follow Agile (Scrum) methodology for project management. | A Scrum board exists with backlog items, sprint goals, and progress updates. Team conducts dailys, stand-ups, sprint planning and sprint retrospectives. | Review of Scrum board and team reports (dailys and sprints planning and retrospectives). | See EXPECT-L0-15 for sprint docs and dailys |
| **L0-16** | CI/CD: The project repository shall include a working GitHub Actions workflow for continuous integration (CI) and continuous delivering (CD). | At least one CI workflow successfully builds or tests the project upon commits or pull requests. | Inspection of GitHub Actions run logs. | See EXPECT-L0-16 for CI/CD workflows |
| **L0-17** | Introduce in the project: AI - GENAI, ML, DL, NEURAL NETWORKS, ETC. | Introduce in the project: AI - GENAI, ML, DL, NEURAL NETWORKS, ETC. | Code implementation verification with achieved effect. | See EXPECT-L0-17 for AI/GenAI guides and resources |

---

## Evidence Details

### L0-1: System Architecture
**EXPECT-L0-1** — System architecture is defined and documented

**Demos/Images:**
- [assembled-car1.jpeg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/assembled-car1.jpeg)
- [assembled-car2.jpeg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/assembled-car2.jpeg)
- [eletric-scheme.png](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/eletric-scheme.png)
- [Validation-of-energy-layout-sprint3.jpeg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint3.jpeg)

**Guides/Docs:**
- [Power Consumption Analysis.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Power%20Consumption%20Analysis.md)

### L0-2: Hardware Assembly
**EXPECT-L0-2** — Hardware Assembly

**Artifacts/Images:**
- [3d-car.jpg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/3d-car.jpg)
- [3d-car2.jpg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/3d-car2.jpg)
- [3d-car3.jpg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/3d-car3.jpg)

### L0-3: Software Setup
**EXPECT-L0-3** — Software Setup (capable of running Qt applications)

**Guides/Docs:**
- [AGL_Installation_Guide.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/AGL_Installation_Guide.md)
- [AGL and ThreadX benefits Guide.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/AGL%20and%20ThreadX%20benefits%20Guide.md)

**Repository/Code:**
- [src/cluster (Qt app)](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/src/cluster)

### L0-4: Remote Control
**EXPECT-L0-4** — Driveable Car with Remote Control (Hardware + Software)

**Evidence/Guides:**
- [Install and Test Joystick in AGL.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Install%20and%20Test%20Joystick%20in%20AGL.md)

### L0-5: Qt Application Launch
**EXPECT-L0-5** — Launch basic Qt on built-in display

**Demos/Images:**
- [first_qt_app.jpeg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/first_qt_app.jpeg)

**Repository/Code:**
- [src/cluster (Qt app)](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/src/cluster)

**Guides:**
- [QML-guide.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/QML-guide.md)

### L0-6: Qt GUI Static Elements
**EXPECT-L0-6** — Qt GUI shall display static graphical elements

**Demos/Images:**
- [mockup1.jpeg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup1.jpeg)
- [mockup2.jpeg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup2.jpeg)
- [mockup3.jpeg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup3.jpeg)
- [mockup4.jpeg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup4.jpeg)
- [mockup5.jpeg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup5.jpeg)
- [mockup6.jpeg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup6.jpeg)
- [Cluster-display-sprint3.jpeg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Cluster-display-sprint3.jpeg)
- [Cluster-mockup-sprint3.jpeg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Cluster-mockup-sprint3.jpeg)
- [Cluster-study.jpeg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Cluster-study.jpeg)

**Guides:**
- [QML-guide.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/QML-guide.md)
- [UI Design README](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/docs/ui_design/README.md)

### L0-7: Qt Crosscompilation
**EXPECT-L0-7** — QT Crosscompilation should be achieved

**Demos/Images:**
- [system-architecture-cross.png](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/system-architecture-cross.png)

**Repository/Code:**
- [src/cross-compiler (Dockerfile, toolchain, README)](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/src/cross-compiler)

**Guides:**
- [Crosscompiling guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/fix-qt-app-minor-changes/src/cross-compiler)

### L0-8: CAN Protocol Communication
**EXPECT-L0-8** — Use CAN protocol to connect Rasp5 ↔ STM32 (bidirectional)

**Demos/Images:**
- [CANbus Scheme Updated.png](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/CANbus%20Scheme%20Updated.png)
- [CAN-layout-sprint3.jpeg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/CAN-layout-sprint3.jpeg)
- [CANFrame.png](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/images/CANFrame.png)
- [CANsignal.png](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/images/CANsignal.png)
- [CANtopology.png](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/images/CANtopology.png)
- [CANtopology2.png](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/images/CANtopology2.png)
- [Half-duplex-communication.png](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/images/Half-duplex-communication.png)

**Guides:**
- [CAN-overview.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/CAN-overview.md)
- [CAN-test-guide.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/CAN-test-guide.md)
- [CAN-explanation.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/CAN-explanation.md)

### L0-9: ThreadX RTOS
**EXPECT-L0-9** — ThreadX on STM32 as RTOS

**Guides:**
- [ThreadX_Installation_Guide.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/ThreadX_Installation_Guide.md)
- [AGL and ThreadX benefits Guide.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/AGL%20and%20ThreadX%20benefits%20Guide.md)

### L0-10: New Architecture Implementation
**EXPECT-L0-10** — Implement new architecture (without expansion board)

**Artifacts/Demos:**
- [Validation-of-energy-layout-sprint1.jpeg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint1.jpeg)
- [Validation-of-energy-layout-sprint2.jpeg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint2.jpeg)
- [Validation-of-energy-layout-sprint3.jpeg](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/Validation-of-energy-layout-sprint3.jpeg)
- [eletric-scheme.png](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/eletric-scheme.png)

### L0-11: Qt Cluster Auto-start
**EXPECT-L0-11** — Qt cluster UI auto-start on boot (fullscreen)

**Artifacts/References:**
- [src/cluster (app, autostart configs)](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/src/cluster)

### L0-12: Wireless Display
**EXPECT-L0-12** — DCI display (wireless) launching Qt

### L0-13: Project & GitHub Setup
**EXPECT-L0-13** — Project & GitHub setup

**Guides:**
- [github-guidelines.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/github-guidelines.md)
- [github-actions-guide.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/github-actions-guide.md)

### L0-14: TSF & Documentation
**EXPECT-L0-14** — TSF & Documentation

**Guides:**
- [WhatsTSF.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/TSF/WhatsTSF.md)

### L0-15: Agile Methodology
**EXPECT-L0-15** — Agile (Scrum)

**Artifacts:**
- [dailys_warmups.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/dailys/dailys_warmups.md)
- [Sprints folder](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/docs/sprints)
- [Project board](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/projects)

### L0-16: CI/CD
**EXPECT-L0-16** — CI/CD (GitHub Actions)

**Artifacts:**
- [github-actions-guide.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/github-actions-guide.md)
- [tsf-validate.yml](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/.github/workflows/tsf-validate.yml)

### L0-17: AI Integration
**EXPECT-L0-17** — Introduce AI / GENAI / ML / DL

**Guides/Resources:**
- [genAI-pair-programming-guidelines.md](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/genAI-pair-programming-guidelines.md)
- [Generative-AI-and-LLMs-for-Dummies.pdf](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/guides/Generative-AI-and-LLMs-for-Dummies.pdf)
- [External: Generative AI for Beginners](https://github.com/fprezado/generative-ai-for-beginners)

---

## Traceability and Verification

Each Level 0 requirement shall be traceable to:
- **System Architecture Elements** — defining how each requirement is implemented.
- **Level 1 Software Requirements** — specifying the detailed functional behavior.
- **Test Cases** — verifying that each requirement is met and validated.

Traceability will be maintained within the GitHub repository, following the principles of the **Trustable Software Framework (TSF)**.

---

**Document version:** 1.0  
**Last updated:** *October 2025*  
**Author(s):** *PiRacer Warm-Up Team*  
**Repository:** [GitHub Repository Link]
