# 🧭 PiRacer Warm-Up Project — Product Backlog (Draft v1.0)

Each backlog item is linked to one or more **L0 Requirements** and includes a short **User Story**, **Acceptance Criteria**, and **Priority** (MoSCoW: Must / Should / Could / Won’t).

---

## 🧩 Epic 1 — Hardware Assembly and System Setup

| ID | User Story | Acceptance Criteria | L0 Link | Priority |
|----|-------------|--------------------|----------|-----------|
| **PB-1** | As a team member, I want to assemble the PiRacer robot so that it can power on and interface with motors and the display. | - PiRacer hardware assembled and boots correctly.<br>- All components (motors, display, Hailo AI Hat) detected and functional. | L0-1 | **Must** |
| **PB-2** | As a developer, I want to boot the Raspberry Pi 5 with a Linux environment that supports Qt, so I can develop GUI applications. | - Raspberry Pi boots without errors.<br>- Qt tools (qmake, QtCreator) are installed and runnable. | L0-2 | **Must** |

---

## 💻 Epic 2 — Software Development and Qt GUI

| ID | User Story | Acceptance Criteria | L0 Link | Priority |
|----|-------------|--------------------|----------|-----------|
| **PB-3** | As a developer, I want to create a GitHub repository so that all team members can collaborate using version control. | - Repo created and accessible.<br>- Contains README, code, and requirements.<br>- Shows multiple commits/branches from team members. | L0-3 | **Must** |
| **PB-4** | As a developer, I want the PiRacer to launch a basic Qt GUI on its display so we can confirm display integration. | - GUI launches and shows on-screen.<br>- No crashes or missing dependencies. | L0-4 | **Must** |
| **PB-5** | As a user, I want the GUI to show static elements (text, shapes, or icons) so that I can visually confirm that the display works. | - Visible graphical elements appear on launch.<br>- Layout and rendering stable. | L0-5 | **Must** |

---

## 🧱 Epic 3 — Trustable Software Framework (TSF) Integration

| ID | User Story | Acceptance Criteria | L0 Link | Priority |
|----|-------------|--------------------|----------|-----------|
| **PB-6** | As a developer, I want to define and document software requirements using TSF so the project is traceable. | - L0 and L1 requirements documented.<br>- Traceability matrix created. | L0-6 | **Must** |
| **PB-7** | As a developer, I want to link requirements to architecture and test cases so I can ensure consistency. | - Each requirement has corresponding architecture and test case links. | L0-6 | **Should** |

---

## 🧠 Epic 4 — Agile and Team Organization

| ID | User Story | Acceptance Criteria | L0 Link | Priority |
|----|-------------|--------------------|----------|-----------|
| **PB-8** | As a team, we want to follow Scrum so we can manage the project efficiently. | - Scrum board set up (To Do / In Progress / Done).<br>- Team holds stand-ups and retrospectives.<br>- Sprint goals defined. | L0-7 | **Must** |
| **PB-9** | As a product owner, I want to maintain a sprint backlog so the team can track progress and priorities. | - Sprint backlog updated weekly.<br>- Tasks linked to L0/L1 requirements. | L0-7 | **Should** |

---

## ⚙️ Epic 5 — System Architecture and Documentation

| ID | User Story | Acceptance Criteria | L0 Link | Priority |
|----|-------------|--------------------|----------|-----------|
| **PB-10** | As a systems engineer, I want to define the system architecture so I can visualize hardware and software interactions. | - Architecture diagram (hardware + software).<br>- Saved in repo in `/docs/architecture/`. | L0-8 | **Must** |
| **PB-11** | As a developer, I want to maintain documentation in the repo so that future teams can easily reproduce the setup. | - README and docs folder complete.<br>- Includes build and run instructions. | L0-8 | **Should** |

---

## 🚗 Epic 6 — Remote Control and Hardware Interaction

| ID | User Story | Acceptance Criteria | L0 Link | Priority |
|----|-------------|--------------------|----------|-----------|
| **PB-12** | As a user, I want to remotely control the PiRacer’s steering and propulsion motors so I can drive it manually. | - Remote commands received and applied to DC/servo motors.<br>- Latency < 300 ms. | L0-9 | **Should** |
| **PB-13** | As a tester, I want to log motor inputs and outputs for debugging and safety checks. | - Log files created with timestamps.<br>- Accessible in `/logs/`. | L0-9 | **Could** |

---

## 🧪 Epic 7 — Continuous Integration (CI) and Automation

| ID | User Story | Acceptance Criteria | L0 Link | Priority |
|----|-------------|--------------------|----------|-----------|
| **PB-14** | As a developer, I want GitHub Actions to build and test the project automatically so I can ensure it compiles successfully. | - CI runs automatically on every commit or pull request.<br>- Successful build badges visible. | L0-10 | **Must** |
| **PB-15** | As a developer, I want to generate documentation automatically in CI so that TSF traceability is always up to date. | - Action job generates Markdown or HTML documentation.<br>- Output saved to `/docs/generated/`. | L0-10 | **Could** |

---

## ✅ Summary Table — Product Backlog Overview

| Epic | # Stories | Priority Breakdown (M/S/C) | Related L0 IDs |
|-------|------------|-----------------------------|----------------|
| **Hardware Assembly** | 2 | 2M | L0-1, L0-2 |
| **Qt GUI** | 3 | 3M | L0-3 → L0-5 |
| **TSF** | 2 | 1M, 1S | L0-6 |
| **Agile / Scrum** | 2 | 1M, 1S | L0-7 |
| **Architecture** | 2 | 1M, 1S | L0-8 |
| **Remote Control** | 2 | 1S, 1C | L0-9 |
| **CI / Automation** | 2 | 1M, 1C | L0-10 |

---

📘 *Version:* Draft v1.0  
📅 *Last Updated:* October 2025  
👥 *Maintainers:* Team 6 — PiRacer Warm-Up Project
