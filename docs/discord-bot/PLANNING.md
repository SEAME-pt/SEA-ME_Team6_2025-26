# Discord Bot - Daily Updates Automation

## 📋 Project Overview
Automate daily standup updates from Discord to GitHub markdown table (`docs/dailys/dailys_warmups.md`).

---

## ✅ Requirements Confirmed

### Bot Behavior
- **Type**: 24/7 Discord bot running on Raspberry Pi 4
- **Language**: Python (discord.py)
- **Trigger**: When all 5 team members post their daily update
- **Update Window**: 11:00 - 15:00 daily
- **Reminders**: Hourly notifications after 15:00 for members who haven't posted

### GitHub Integration
- **Action**: Create Pull Request automatically
- **Validation**: GitHub Actions checks:
  - ✅ All 5 members posted
  - ✅ All mandatory fields filled
  - ✅ Markdown format correct
  - ✅ Date is correct

---

## 🎯 Discord Channels to Monitor

| Channel | Purpose | Status |
|---------|---------|--------|
| `#dailys` | Daily updates from 5 members | 🔴 To create |
| `#stand-up-meetings` (ID: `1440752763129565245`) | Standup meeting notes | ✅ Exists |
| `#sprint-planning` | Sprint planning notes | 🔴 To create |
| `#sprint-retrospective` | Sprint retrospective notes | 🔴 To create |

---

## 📝 Bot Commands

### `/daily` Command
Members use this command to submit their daily update:

```discord
/daily

✅ Done:
- Item 1
- Item 2

🔄 In Progress/To Do:
- Task 1

🏆 Victory: [optional]
- Achievement today

❌ Blockers: [optional]
- Blocker 1

REMINDERS:
🔔 Missing: [optional]
- Item missing

📋 Pending: [optional]
- Pending item

🎯 Next Module Issues: [optional]
- Issue 1

📝 Stand-up Meeting Notes: [optional]
Meeting with: [INPUT]
1. Team Updates
   - Any notes. If no INPUT, leave none.
2. Workflow & Github
   - Any notes. If no INPUT, leave none.
3. Technical Development 
   - Any notes. If no INPUT, leave none.

📚 Technical Concepts: [optional]
- Concept learned

📝 Sprint Planning Meeting Notes: [optional]
- Any notes. If no INPUT, leave none.

📝 Sprint Retrospective Meeting Notes: [optional]
- Any notes. If no INPUT, leave none.

📝 Other Meeting Notes: [optional]
- Any notes. If no INPUT, leave none.
```

---

## 🔄 Automatic Data Generation

### Daily Goals
**Formula**: `Daily Goals = Done (current) + In Progress (from yesterday that wasn't completed)`

### Sprint Info Detection
| Day | Marker | Logic |
|-----|--------|-------|
| Monday (1st of sprint) | `SPRINT PLANNING` | Manual/calendar detection |
| Tuesday (after planning) | `SPRINT PLANNING EXECUTION STARTING POINT` | Day after planning |
| Wednesday | `STANDUP MEETING` | Every Wednesday |
| Friday (last of sprint) | `SPRINT RETROSPECTIVE` | Manual/calendar detection |
| Other days | Team name only | Default |

### Owner/Team
- Always: `Team 6` or `Joao — TEAM 6` (depending on context)

---

## 📊 Table Structure Mapping

### Column 1: Date
- **Source**: Automatic (system date)
- **Format**: `DD/MM/YYYY`

### Column 2: Owner / Master / Sprint Info
- **Source**: Automatic detection + sprint info
- **Examples**: 
  - `Joao — SPRINT PLANNING`
  - `Joao — STANDUP MEETING`
  - `Joao`

### Column 3: Progress Goals of the Day / Pending
- **Source**: Auto-generated from Done + In Progress
- **Format**: `Daily Goals: <br> 1. Item 1 <br> 2. Item 2`

### Column 4: What Was Done
- **Source**: 
  - `Done` field from `/daily`
  - `Victory` field (optional)
- **Format**: 
  ```
  **Done:** <br> 1. Item 1 <br> 2. Item 2
  
  **Victory of today:** Description
  
  **In Progress / To Be Done:** Items not completed
  ```

### Column 5: Blockers & Notes
- **Sources**:
  - `Blockers` → If empty, inherit from previous day or "None"
  - `Missing` → If empty, inherit from previous day or omit
  - `Pending` → If empty, inherit from previous day or omit
  - `Next Module Issues` → If empty, inherit from previous day or omit
  - `Meeting Notes` → From Discord channels or `/daily` input
  - `Technical Concepts` → From `/daily` input

---

## 🏗️ File Structure

```
discord-bot/
├── bot.py                      # Main bot entry point
├── commands/
│   └── daily.py                # /daily command handler
├── handlers/
│   ├── daily_handler.py        # Process daily updates
│   ├── standup_handler.py      # Process standup meetings
│   └── sprint_handler.py       # Process sprint docs
├── github/
│   └── pr_creator.py           # Create GitHub PRs
├── utils/
│   ├── markdown_gen.py         # Generate markdown table
│   ├── validators.py           # Validate inputs
│   └── calendar_utils.py       # Sprint calendar logic (future)
├── config.py                   # Configuration
├── requirements.txt            # Python dependencies
├── .env.example                # Environment variables template
└── README.md                   # Setup guide
```

---

## 🔐 Required Credentials

### Discord
- ✅ **Server ID**: `1437837918671736993`
- ⏳ **Bot Token**: On Raspberry Pi 4 (accessible 16/12)
- ⏳ **Channel IDs**: To obtain when channels created

### GitHub
- ⏳ **Personal Access Token**: 
  - Permissions: `repo`, `workflow`
  - Create at: GitHub → Settings → Developer Settings → Tokens (classic)

### Google Calendar (Future)
- ⏳ **Calendar API**: For automatic sprint detection

---

## 🔄 Workflow

### Daily Update Flow
1. **11:00** - Bot activates, waiting for updates
2. **Member posts** `/daily` command with inputs
3. **Bot collects** inputs from all 5 members
4. **15:00** - Deadline reached
   - If 5/5 posted → Generate table entry
   - If < 5 → Send hourly reminders
5. **All posted** - Bot generates markdown
6. **Validation** - Check mandatory fields
7. **GitHub PR** - Create PR with changes
8. **GitHub Actions** - Run validation tests

### Standup Meeting Flow
1. **Wednesday** - Monitor `#stand-up-meetings` channel
2. **Paulo posts** meeting notes with structure:
   - Team Updates
   - Workflow & GitHub
   - Technical Development
3. **Bot extracts** and adds to daily table

### Sprint Docs Flow
1. **Bot reads** `/docs/sprints/sprint*.md`
2. **Extracts** Sprint Goals and Epics
3. **Adds** to daily table on sprint planning days

---

## ✅ Validation Rules

### Mandatory Fields
- ✅ **Done** (minimum 1 item)
- ✅ **In Progress/To Do** (minimum 1 item)
- ✅ **5/5 team members** posted

### Optional Fields
- Victory
- Blockers (if empty: "None")
- Missing (if empty: inherit or omit)
- Pending (if empty: inherit or omit)
- Next Module Issues (if empty: inherit or omit)
- Technical Concepts (if empty: inherit or omit)
- Meeting Notes (if empty: inherit or omit)

---

## 📅 Next Steps (16/12/2025)

### Phase 1: Access & Setup
- [ ] Access Raspberry Pi 4
- [ ] Obtain Bot Token
- [ ] Review existing bot structure
- [ ] Create missing Discord channels
- [ ] Obtain Channel IDs

### Phase 2: Development
- [ ] Implement `/daily` command
- [ ] Build daily handler
- [ ] Build markdown generator
- [ ] Test with mock data

### Phase 3: Integration
- [ ] GitHub PR automation
- [ ] Standup meeting monitor
- [ ] Sprint docs reader
- [ ] Validation system

### Phase 4: Deployment
- [ ] Deploy to Raspberry Pi
- [ ] Test with real data
- [ ] Team training
- [ ] Go live

---

## 📚 References

- Template reference: `.github/ISSUE_TEMPLATE/daily-meeting.yml`
- Target file: `docs/dailys/dailys_warmups.md`
- Sprint docs: `docs/sprints/sprint*.md`
- Discord channel: `#stand-up-meetings` (ID: `1440752763129565245`)

---

## 🤝 Team Members (5)
- Joao
- Ruben
- Vasco
- David
- Diogo

---

## 📝 Notes
- Bot runs 24/7 on Raspberry Pi 4 in office
- Access available from Tuesday 16/12
- Using existing bot structure from colleague
- Integration with TSF methodology

---

**Last Updated**: 12/12/2025
**Status**: Planning Phase
**Next Meeting**: 16/12/2025 (Access to RPi)
