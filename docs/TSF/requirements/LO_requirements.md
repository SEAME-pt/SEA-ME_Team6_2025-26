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
|:-------|:------------------------------|:------------------------|:------------------------|:------------------|
| **L0-1** | The system architecture shall be defined and documented to describe hardware and software components, including the Expansion Board, Raspberry Pi 5, Hailo AI Hat, DC motors, servo motor, and display interface. |A system architecture diagram or document is present in the repository. | Documentation inspection. |  |
| **L0-2** | Hardware Assembly: Fully assemble the PiRacer robot, including the Raspberry Pi 5, Hailo AI Hat, DC motors, servo motor, and display interface (first with the old architecture. Then with the new one). | All hardware components are correctly installed and operational. The PiRacer can power on and communicate with the display and motors. | Physical inspection and functional test. |  |
| **L0-3** | Software Setup: The system shall successfully boot the Raspberry Pi 5 and load a Linux environment (Trixie, Bookworm and finally AGL) capable of running Qt applications. | The PiRacer boots without system errors, and Qt development tools are accessible from the command line. | System boot demonstration and OS verification. |  |
| **L0-4** | Driveable Car with Remote Control/Driveable Car Model (Hardware + Software): The PiRacer shall support remote control functionality to operate the motors (steering and propulsion). | The system can receive remote input to control motor direction and speed. The car shall accept `forward`, `back`, `left`, `right`, and `stop` commands, and execute them reliably (controller event → wheel actuation) | Live demonstration or test log. |  |
| **L0-5** | The system shall be capable of launching a basic Qt application on the PiRacer’s built-in display. | When executed, the program successfully opens a Qt GUI window on the PiRacer’s screen. | Demonstration or screenshot verification. |  |
| **L0-6** | The Qt GUI shall display static graphical elements such as shapes, text, or images to form a simple user interface. | Upon application launch, the display shows visible static content (e.g., rectangle, label, icon). | Visual inspection of the running application. |  |
| **L0-7** | QT Crosscompilation should be achieved. | We should be able to crosscompile our QT program outside  the Operating System and the included the exec file running on it. | QT program crosscompliation validated with final exec file obtained, and execution of the exec file in the Rasp5 OS. |  |
| **L0-8** | Connect the Rasp5 to the STM 32 (bidirectional) using Can Protocol. | A communication signal should be sent by the Rasp 5 and received in the STM32. This should be observed from Rasp 5 to STM and from STM to Rasp5 (bidirectional communication). | Visual inspection of message passing from RP5 to STM and vice-versa. |  |
| **L0-9** | ThreadX should be used on the Microcontroller (STM32) as a RTOS, receiving signals from the speedometer and controlling the servos and DC motor. |  We should be able to install and use ThreadX on the Microcontroller (STM32), making all communications (including can) to pass in the STM32 and be controlled/managed there using priorizations and threads. | Software test including Visual inspection of the message bidirectional transmission. |  |
| **L0-10** | Implement and test the changing from the old architecture to the new architecture (without the expansion board) including power consumption study, new electric scheme, individual module test, and visual test of all the components working. | A system architecture diagram and documents regarding power consumption and new electic scheme are present in the repository. | Documentation inspection. |  |
| **L0-11** | The Qt cluster UI shall **auto-start on boot** and display fullscreen on the car’s built-in display. | Upon application launch, the program successfully opens a Qt GUI window on the PiRacer’s screen. | Visual inspection of the running application. |  |
| **L0-12** | The DCI display fullscreen will be out of the car (wireless display) lauching Qt (first without autostart, then with autostart). | Upon application launch, the wireless display should open a Qt GUI window on the PiRacer’s screen with our program. | Visual inspection of the running application. |  |
| **L0-xx** | xpto. | xpto. | xpto. |  |
| **L0-13** | Project & GitHub setup: The team shall set up a GitHub repository hosting the project code, documentation, and2 version control history. | Repository contains all relevant source code, README, and requirements files. Branches, commits, and pull requests show collaboration among members. | GitHub repository inspection. |  |
| **L0-14** | TSF & Documentation: The project shall apply the Trustable Software Framework (TSF) to define and trace requirements, architecture, and test cases. | TSF documentation exists within the repository, showing traceability. | Documentation review in the GitHub repository. |  |
| **L0-15** | Agile: The team shall follow Agile (Scrum) methodology for project management. | A Scrum board exists with backlog items, sprint goals, and progress updates. Team conducts dailys, stand-ups, sprint planning and sprint retrospectives. | Review of Scrum board and team reports (dailys and sprints planning and retrospectives). |  |
| **L0-16** | CI/CD: The project repository shall include a working GitHub Actions workflow for continuous integration (CI) and continuous delivering (CD). | At least one CI workflow successfully builds or tests the project upon commits or pull requests. | Inspection of GitHub Actions run logs. |  |
| **L0-17** | Introduce in the project: AI - GENAI, ML, DL, NEURAL NETWORKS, ETC". | Introduce in the project: AI - GENAI, ML, DL, NEURAL NETWORKS, ETC". | Code implementation verification with achieved effect. |  |

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
