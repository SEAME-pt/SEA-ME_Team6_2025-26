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

| **ID** | **Requirement (Client View)** | **Acceptance Criteria** | **Verification Method** |
|:-------|:------------------------------|:------------------------|:------------------------|
| **L0-1** | Fully assemble the PiRacer robot, including the Raspberry Pi 5, Hailo AI Hat, DC motors, servo motor, and display interface. | All hardware components are correctly installed and operational. The PiRacer can power on and communicate with the display and motors. | Physical inspection and functional test. |
| **L0-2** | The system shall successfully boot the Raspberry Pi 5 and load a Linux environment capable of running Qt applications. | The PiRacer boots without system errors, and Qt development tools are accessible from the command line. | System boot demonstration and OS verification. |
| **L0-3** | The team shall set up a GitHub repository hosting the project code, documentation, and2 version control history. | Repository contains all relevant source code, README, and requirements files. Branches, commits, and pull requests show collaboration among members. | GitHub repository inspection. |
| **L0-4** | The system shall be capable of launching a basic Qt application on the PiRacer’s built-in display. | When executed, the program successfully opens a Qt GUI window on the PiRacer’s screen. | Demonstration or screenshot verification. |
| **L0-5** | The Qt GUI shall display static graphical elements such as shapes, text, or images to form a simple user interface. | Upon application launch, the display shows visible static content (e.g., rectangle, label, icon). | Visual inspection of the running application. |
| **L0-6** | The project shall apply the Trustable Software Framework (TSF) to define and trace requirements, architecture, and test cases. | TSF documentation exists within the repository, showing traceability between L0 requirements, architecture components, and test cases. | Documentation review in the GitHub repository. |
| **L0-7** | The team shall follow Agile (Scrum) methodology for project management. | A Scrum board exists with backlog items, sprint goals, and progress updates. Team conducts stand-ups and retrospectives. | Review of Scrum board and team reports. |
| **L0-8** | The system architecture shall be defined and documented to describe hardware and software components, including the display interface and motor control modules. | A system architecture diagram or document is present in the repository. | Documentation inspection. |
| **L0-9** | The PiRacer shall support remote control functionality to operate the motors (steering and propulsion). | The system can receive remote input to control motor direction and speed. | Live demonstration or test log. |
| **L0-10** | The project repository shall include a working GitHub Actions workflow for continuous integration (CI). | At least one CI workflow successfully builds or tests the project upon commits or pull requests. | Inspection of GitHub Actions run logs. |

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
