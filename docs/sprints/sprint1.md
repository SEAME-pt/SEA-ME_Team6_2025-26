# 🏁 Sprint 1 — (13/10/2025 → 24/10/2025)

> **Brief summary**: By the end of this sprint, we hope to know the components of the PiRacer and have it assembled, create documentation and implement Agile methodologies and TSF, and create a Qt App including a mockup design of the cluster.

---

## 📌 Epics
- [x] **Epic 1** — Hardware Assembly - [Epic 1 - #2](https://github.com/orgs/SEAME-pt/projects/89/views/1?filterQuery=label%3A%22Type%3A+Epic%22&pane=issue&itemId=133634028&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C2)
- [x] **Epic 2** — Software Setup - [Epic 2 - #6](https://github.com/orgs/SEAME-pt/projects/89/views/1?filterQuery=label%3A%22Type%3A+Epic%22&pane=issue&itemId=133636567&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C6)
- [ ] **Epic 3** — Development of Qt App - [Epic 3 - #10](https://github.com/orgs/SEAME-pt/projects/89/views/1?filterQuery=label%3A%22Type%3A+Epic%22&pane=issue&itemId=133637985&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C10)
- [ ] **Epic 4** — Project & GitHub setup - [Epic 4 - #13](https://github.com/orgs/SEAME-pt/projects/89/views/1?filterQuery=label%3A%22Type%3A+Epic%22&pane=issue&itemId=133639290&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C13)
- [x] **Epic 5** — TSF & Documentation - [Epic 5 - #18](https://github.com/orgs/SEAME-pt/projects/89/views/1?filterQuery=label%3A%22Type%3A+Epic%22&pane=issue&itemId=133640653&issue=SEAME-pt%7CSEA-ME_Team6_2025-26%7C18)

---

## 📈 Actual Progress
- **Completed tasks**: `14 / 16` (**87.5%**) 

- ✅ Done: #3, #4, #5, #7, #8, #9, #11, #14, #15, #17, #19, #20, #21, #27  
- ⏳ Blocked: #2, #12

---

| Date       | Daily Facilitator | Daily Objectives                                                                                                                                                                                                                                                                                                                                              | What was done                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         | Blockers                                                                                                                                                                                                                                                                                                                                                                                                         | Next steps / What is missing                                                                                                                                                                                                                                                                                                                                                   |
| :--------- | :---------------- | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | :----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 2025-10-16 | **Team**          | 1️⃣ Integrate and test main hardware components (Raspberry Pi, Hailo Hat, DSI, CSI, Gamepad)<br>2️⃣ Clarify logistics and technical doubts during 16h meeting<br>3️⃣ Create Pull Request and Daily templates<br>4️⃣ Continue TSF implementation on GitHub<br>5️⃣ Analyze GitHub Actions<br>6️⃣ Continue Qt Application development<br>7️⃣ Update sprint board | ✅ Set almost fully assembled and tested:<br>• Raspberry Pi OS (Bookworm) installed<br>• Hailo Hat 8 functional ([rpicam-apps](https://github.com/raspberrypi/rpicam-apps))<br>• CSI camera working with Python script ([hailo-rpi5-examples](https://github.com/hailo-ai/hailo-rpi5-examples))<br>• DSI display functional<br>• Gamepad and dongle connected and working<br>✅ PR, Daily & Taskly templates created<br>🧩 TSF implementation started<br>⏸ GitHub Actions in standby<br>🚧 Qt Application and board updates in progress | - Missing components (headers, 5V/5A charger, DSI cables, HDMI adapters, heatsinks)<br>- Possible Wi-Fi config issues on Raspberry Pi<br>- Validation dependency with David & Filipe<br>- Pending decision: AGL vs ThreadX vs FreeRTOS vs Azure RTOS<br>- Large tasks need splitting into subtasks/epics<br>- Confirm TSF traceability matrix naming and usage<br>- Permissions for PR and Actions still pending | 1️⃣ Connect all components to expansion board and test full set<br>2️⃣ Prepare inventory of missing components → send to Filipe<br>3️⃣ Validate PR/Daily/Taskly templates (@joaocp @daviduartecf)<br>4️⃣ Continue TSF implementation on GitHub<br>5️⃣ Continue GitHub Actions analysis<br>6️⃣ Continue Qt Application development<br>7️⃣ Update board with current task status |
| 2025-10-17 | **Team**          | 1️⃣ Assemble car (set connection on expansion board)<br>2️⃣ Prepare inventory and send to Filipe<br>3️⃣ Validate PR/Daily/Taskly templates (@joaocp @daviduartecf)<br>4️⃣ Continue TSF implementation<br>5️⃣ Analyze GitHub Actions<br>6️⃣ Develop Qt Application<br>7️⃣ Update board                                                                         | Work pending from previous day — assembly and validation not yet done                                                                                                                                                                                                                                                                                                                                                                                                                                                                 | - Hardware integration incomplete<br>- Pending inventory validation<br>- TSF and Qt in development<br>- GitHub Actions on hold                                                                                                                                                                                                                                                                                   | 1️⃣ Complete hardware integration and testing<br>2️⃣ Submit inventory to Filipe<br>3️⃣ Validate templates<br>4️⃣ Continue TSF development<br>5️⃣ Continue Qt Application<br>6️⃣ Resume GitHub Actions testing<br>7️⃣ Update board                                                                                                                                              |
| 2025-10-20 | **Team**          | 1️⃣ Connect Raspberry Pi + Hailo + Camera + Display<br>2️⃣ Create GitHub guidelines and Actions<br>3️⃣ Study cluster design<br>4️⃣ Implement Action to download Taskly                                                                                                                                                                                        | ⚙️ Verified camera + display setup<br>✅ Cluster mockup completed<br>✅ GitHub and Actions guides created                                                                                                                                                                                                                                                                                                                                                                                                                               | - Unable to run JetRacer script on current OS                                                                                                                                                                                                                                                                                                                                                                    | 1️⃣ TSF requirements + traceability matrix (@jpjpcs)<br>2️⃣ Continue cluster design study (@souzitaaaa)<br>3️⃣ Test JetRacer script on Trixie SD (@rcosta-c @vasferre)<br>4️⃣ Create Sprint Summary + Daily Docs (@daviduartecf)                                                                                                                                               |

---

## ✅ Outcomes
- **Delivered**:
EXPECT-L0-3 - "Software Setup capable of running Qt applications":
EXPECT-L0-5 - "Launch basic Qt on built-in display":
    - Qt app code in [Qt app](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/src/cluster)

    
- **Demos**:
  
**ARCHITECTURE**:
EXPECT-L0-1 - "System architecture is defined and documented
      - [Assembled car](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/assembled-car1.jpeg)
    - [Assembled car2](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/assembled-car2.jpeg)

**QT**:
  EXPECT-L0-5 - "Launch basic Qt on built-in display":
    - [QtApp](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/first_qt_app.jpeg)
  EXPECT-L0-6 - "The Qt GUI shall display static graphical elements";
  - [mockup1](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup1.jpeg)
    - [mockup2](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup2.jpeg)
    - [mockup3](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup3.jpeg)
    - [mockup4](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup4.jpeg)
    - [mockup5](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup5.jpeg)
    - [mockup6](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/demos/mockup6.jpeg)
               
      
- **Docs/Guides updated**:

    - Documentation in [Docs](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/docs)


**GITHUB**

  EXPECT-L0-13 - "Project & GitHub setup":
- [Github guidelines](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/guides/github-guidelines.md)
- [Github guide](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/development/docs/guides/github-actions-guide.md)

**AGILE SCRUM**
EXPECT-L0-15 - "Agile and Scrum":
- [Scrum Implementation: Dailys](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/blob/main/docs/dailys/dailys_warmups.md)
- [Scrum Implementation: Sprints](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/tree/main/docs/sprints)
- [Scrum Implementation: Project Board](https://github.com/SEAME-pt/SEA-ME_Team6_2025-26/projects?query=is%3Aopen)

---

## 🔎 Retrospective
- **Went well**:
  - We were able to do **14** of the **16** tasks of this sprint! (**87.5%**)
  - We did research about the next steps
  - The team communicated well and was able to solve problems together
- **To improve**:
  - Improved Agile task managment:
      - Implement Priority level in tasks
      - Add Time estimates to tasks for better planning
- **What we didn't do**:
  - Auto-start of Qt app on boot
  - Complete GitHub setup (branch protection - reviewers)
 
## 🧭 **Action Items for Next Sprint**
- Implement auto-start for Qt app  
- Improve sprint planning templates (add estimated time & priority)


---

## 🔗 Useful Links
- Sprint board/view: [Sprint 1](https://github.com/orgs/SEAME-pt/projects/89/views/1)

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
