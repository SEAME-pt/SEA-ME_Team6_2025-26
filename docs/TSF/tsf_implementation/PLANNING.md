# TSF GenAI Automation - Planning & Decision Log

## Purpose
This document provides a clear, high-level plan and rationale for the TSF automation pipeline using Generative AI. It is designed to:
- Serve as a quick reference for the team
- Document all major decisions, options, and trade-offs
- Make it easy to adapt the process if the team wants to switch to a different AI or workflow in the future

---

## 1. Pipeline Overview
- **Source of Truth:** `tsf-requirements-table.md`
- **Detection:** Scripts detect new/changed requirements
- **Generation:** AI (Copilot CLI, or fallback) generates EXPECT, ASSERT, EVID, ASSUMP
- **Validation:** All items validated (YAML, references, TruDAG)
- **Sync:** Evidence and references updated
- **Backup:** Automated backups before changes
- **CI/CD:** GitHub Actions runs validation and scoring

---

## 2. GenAI Integration Options (Evaluated)
- **A) GitHub Copilot CLI** (chosen)
  - Pros: Automation, integration, cost-effective, works in CI
  - Cons: Needs subscription, CLI install
- **B) Local LLM (Ollama)**
  - Pros: No API cost, local privacy
  - Cons: Hardware requirements, not portable
- **C) Cloud APIs (OpenAI/Anthropic)**
  - Pros: Powerful, flexible
  - Cons: Cost, privacy, API management
- **D) VSCode Extension (Manual)**
  - Pros: Human-in-the-loop, flexible
  - Cons: Not automatable, manual effort

**Decision:** Copilot CLI is default. Manual/VSCode is fallback. All options documented for future change.

---

## 3. Why Keep References in YAML?
- TruDAG and validators require references in YAML for traceability and validation
- `graph.dot` is for visualization, not for full validation
- **Rule:** Always keep references in both YAML and graph.dot

---

## 5. Current Status (December 18, 2025)
- **Documentation:** Fully updated in `genAI_on_tsf.md` with all options, rationale, and decisions.
- **GenAI Choice:** Copilot CLI (Option C) chosen for automation, with manual/VSCode (Option G) as immediate fallback.
- **Scripts Status:** 
  - `sync_tsf_manager.py`: Created and tested with L0-19 (semi-automated workflow working).
  - `ai_generator.py`: Not yet created (AI integration module).
  - Other modules (detectors.py, generators.py, etc.): Partially implemented.
- **Test Results:** Semi-automated workflow (Option G) tested successfully - detected new ID, created placeholders, opened in VSCode, generated content, validated.
- **Next Steps:** Implement `ai_generator.py` with Option C (Copilot CLI) for full automation, or continue testing Option G.

---

## 6. Implementation Plan
- **Phase 1:** Implement `ai_generator.py` with Option G (semi-automated manual mode) in `sync_tsf_manager.py`.
- **Phase 2:** Test semi-automated generation workflow.
- **Phase 3:** If successful, integrate Copilot CLI for full automation.
- **Phase 4:** Add validation, backups, and CI/CD integration.

---
