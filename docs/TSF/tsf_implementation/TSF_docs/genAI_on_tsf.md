# GenAI on TSF: Unified Documentation

## 1. Introduction
This document explains how Generative AI (GenAI) is integrated and used in the TSF automation workflow for SEA-ME Team 6 (2025-26). It merges and updates all previous documentation, including technical options, process decisions, and rationale for the chosen approach.

---

## 2. What is Generative AI?
Generative AI (GenAI) refers to AI systems that can create new content—text, code, images, or other outputs—based on patterns learned from training data. In TSF, GenAI is used to generate technical documentation, requirements, and code artifacts.

---

## 3. TSF Automation Architecture (2025/26)

**Main Directory Structure:**
```
docs/TSF/tsf_implementation/
├── scripts/
│   ├── sync_tsf_manager.py          # Main orchestrator script (detects new IDs, generates items with AI)
│   ├── modules/
│   │   ├── __init__.py
│   │   ├── detectors.py             # Detects changes in requirements table (new L0-X IDs)
│   │   ├── generators.py            # Creates EXPECT/ASSERT/EVID/ASSUMP file structures
│   │   ├── sync_evidence.py         # Syncs sprint outcomes to EVID files (extracts from sprint1.md, etc.)
│   │   ├── ai_generator.py          # AI content generation (Copilot CLI or manual templates)
│   │   ├── validators.py            # Wrapper for validate_items_formatation.py (YAML validation)
│   │   └── trudag_runner.py         # Wrapper for setup_trudag_clean.sh (graph.dot + TruDAG commands)
│   └── config.yaml                  # Configurations (AI methods, paths, etc.)
├── backups/
│   └── items_backup1.tar.gz         # Numbered backups
└── validators/
    └── validate_items_formatation.py  # YAML/structure validator
```

**Process Overview:**
- Detection of new requirements in `tsf-requirements-table.md`
- Generation of TSF items (EXPECT, ASSERT, EVID, ASSUMP)
- Evidence synchronization from sprint files
- Validation of all items using TruDAG
- Automated backups

---

## 4. GenAI Integration Options Considered

### Option A: GitHub Copilot API
- **Description:** Direct API access to GitHub Copilot for completions.
- **How it works:** Use requests library in Python to call the API with prompts.
- **Pros:** Direct integration, potentially powerful.
- **Cons:** API not officially public (as of Dec 2025), may not work reliably.
- **Status:** Not chosen due to unreliability and lack of official support.

### Option B: Script Local Interativo (Interactive Script)
- **Description:** A local script that generates placeholders, shows suggestions, and waits for user approval/edit.
- **How it works:** Script creates content suggestions (with or without AI), displays them, and prompts user to approve, edit, or reject.
- **Pros:** Full user control, no external dependencies, flexible editing.
- **Cons:** Manual process, not fully automated, requires user interaction for each item.
- **Status:** Rejected because the text created would be equal in all the categories identified as the same category, as also the categories that doesn´t have any correspondency, would receive a geral message. It was not rejected due to lack of automation.

### Option C: gh copilot CLI (Command-Line Tool) ✅ **Chosen**
- **Description:** GitHub's official CLI tool with Copilot extension for AI suggestions in terminal.
- **How it works:** Install `gh` CLI and `gh copilot` extension, call via subprocess in Python scripts.
- **Pros:** Official, works in scripts, integrates with GitHub ecosystem, cost-effective with subscription.
- **Cons:** Requires GitHub Copilot subscription and CLI setup.
- **Status:** Chosen for automation balance.

### Option D: VSCode Extension API (Hypothetical)
- **Description:** Direct API to call VSCode Copilot/Claude extensions from scripts.
- **How it works:** Would use a library like `vscode_copilot` to generate content programmatically.
- **Pros:** Seamless integration with VSCode.
- **Cons:** No such public API exists; extensions are manual-only and cannot be called from scripts without human interaction.
- **Status:** Rejected because VSCode extensions lack script-callable APIs. Any "integration" would require manual copy-paste, defeating automation goals.

### Option E: Local LLM (Ollama)
- **Description:** Run open-source LLMs locally using Ollama.
- **Pros:** No costs, privacy.
- **Cons:** Hardware intensive (8GB+ RAM), not portable for all team members.
- **Status:** Rejected due to hardware constraints.

### Option F: Cloud APIs (OpenAI/Anthropic)
- **Description:** Use paid APIs like OpenAI GPT or Anthropic Claude.
- **Pros:** Powerful, flexible.
- **Cons:** Ongoing costs, privacy concerns, API key management.
- **Status:** Rejected due to cost and privacy.

### Option G: Manual Generation via VSCode
- **Description:** Use Copilot/Claude in VSCode manually to generate content, then paste into files.
- **Pros:** Human oversight, no setup needed beyond VSCode.
- **Cons:** Not automatable, time-consuming for multiple items.
- **Status:** Kept as fallback for manual mode. No copy-paste required if AI (like Claude) can edit files directly via VSCode tools.





### Option H: Manual Generation via VSCode
3. O que é o MCP?
O MCP (Model Context Protocol) é um protocolo que permite a ferramentas externas comunicarem diretamente com o Copilot/Claude. Deixa-me verificar se tens algum configurado:

Searched for files matching **/.vscode/mcp*.json, no matches

Searched for files matching **/mcp.json, no matches

Read .vscode

Não tens MCP configurado (nem pasta .vscode no projeto).

O que é o MCP em termos simples:
É um protocolo novo que permite scripts/ferramentas "falarem" diretamente com o Claude
Exemplo: Um script Python poderia enviar "gera o conteúdo para EXPECT-L0-5" e receber a resposta automaticamente
Mas: Requer configuração complexa e está em fase inicial - não vale a pena agora
Conclusão para o MCP:
Ignora por agora. A Opção G híbrida que vamos implementar é mais simples e funcional. No futuro, se o MCP amadurecer, podemos integrar.
---

## 4.2 Semi-Automated Workflow Considered
An alternative workflow was proposed for easier implementation:
- Script detects new requirements and creates empty file structures.
- Script pauses and prompts user to request content generation from Claude/Copilot in VSCode chat.ar
- User asks AI to generate content for the files.
- AI generates and edits files directly (no copy-paste needed).
- Script resumes, validates, and commits.

**Why Not Chosen as Primary:**
- Still requires human requests to AI, not fully automated.
- Dependent on user interaction for each generation.
- But practical for testing; can transition to full automation later.

**Why No Copy-Paste in This Mode:**
- AI can use VSCode tools to edit files directly, avoiding manual copy-paste.
- Process: User requests generation, AI creates/edits files automatically.

---

## 4.1 Why We Chose Option C (gh copilot CLI)
- **Automation:** Allows scripts to call AI directly, enabling full pipeline automation.
- **Quality:** Leverages GitHub Copilot's context-aware generation for technical content.
- **Integration:** Works in CI/CD (GitHub Actions), local scripts, and team workflows.
- **Cost:** Covered by GitHub Copilot subscription (already in use).
- **Fallback:** Easy to switch to manual if needed.

**Why Not Others:**
- **API Options (A, F):** Unreliable (A) or costly/privacy issues (F).
- **Interactive Script (B):** Too manual, defeats automation goal.
- **VSCode Extension (D):** No API available for script integration.
- **Local LLM (E):** Hardware barriers for team.
- **Manual (G):** Not scalable for full automation.

---

## 4.2 Easier Alternative Considered: Semi-Automated Mode
An alternative workflow was considered:
- Script detects new requirements and creates empty file structures.
- User requests content generation from Claude/Copilot in VSCode chat.
- AI generates content, user pastes into files.
- Script validates and commits.

**Why Not Chosen as Primary:**
- Still requires manual copy-paste and human requests.
- Less efficient than direct CLI integration.
- But kept as Option G for fallback.

---

## 5. Why Keep References in YAML?
- The `graph.dot` file visualizes relationships between TSF items (EXPECT ↔ ASSERT ↔ ASSUMP, etc.).
- However, TruDAG and validation scripts require references in the YAML frontmatter for:
  - Direct file-to-file traceability.
  - Validation of structure and completeness.
  - Navigation and dependency checking.
- **If removed:** TruDAG may fail validation, losing automated scoring and traceability.
- **Conclusion:** Always maintain references in both YAML and graph.dot. Removing them would break the validation pipeline. Even though the original TSF creators may have said it's not needed (as their examples use only `links` with hashes for TruDAG), our automation and validation scripts depend on explicit `references` for cross-file navigation and error-free processing.

---

## 5. Why Not Store All Relations Only in graph.dot?
- The `graph.dot` file encodes relationships between items (EXPECT, ASSERT, ASSUMP, etc.) for visualization and TruDAG processing.
- However, TruDAG and other tools expect references to be present in the YAML frontmatter of each item for:
  - Direct navigation
  - Validation
  - Traceability
- **If you remove references from the YAML, TruDAG and validators may fail or lose traceability.**
- **Conclusion:** Keep references in both the YAML and the graph.dot for full compatibility and traceability.

---

## 6. GenAI Usage Modes in TSF

### A) Fully Automated (Copilot CLI)
- Script detects new requirements
- Calls Copilot CLI to generate content for all items
- Writes files automatically
- Validates and scores with TruDAG
- User reviews and commits

### B) Semi-Automated (Manual/VSCode)
- Script creates placeholders for new items
- User requests content from Claude/Copilot in VSCode
- User pastes/generated content into files
- Script validates and finalizes

### C) Interactive Script
- Script suggests content and asks user for approval/edit
- User can accept, reject, or edit suggestions
- No AI API required, but less scalable

---

## 7. Why We Chose Option A (Copilot CLI)
- **Automation:** Enables end-to-end automation with minimal human intervention
- **Quality:** Leverages GitHub Copilot's context-aware generation
- **Integration:** Works with GitHub Actions and local scripts
- **Fallback:** If Copilot CLI is unavailable, can fall back to manual or interactive modes

**Rejected Options:**
- VSCode Extension API: No public API for direct script calls
- Cloud APIs: Cost and privacy
- Local LLM: Hardware constraints

---

## 8. Validation and Correction
- All generated items are validated using `validate_items_formatation.py` and TruDAG
- If structure is invalid, scripts can auto-correct or prompt the user
- Validators are integrated into the main workflow and can be run standalone

---

## 9. Decision Log and Rationale
- All options were tested and compared for automation, cost, and team accessibility
- Copilot CLI was chosen for its balance of automation, cost, and integration
- Manual/VSCode mode is kept as fallback for edge cases
- Documentation is updated to reflect all decisions and options

---

## 11. Current Implementation Status
- **Documentation:** Complete, with all options and rationale documented.
- **Chosen Approach:** **Option G (Semi-Automated/VSCode) as PRIMARY**, with Option C (gh copilot CLI) as FALLBACK.
- **Scripts:** 
  - `sync_tsf_manager.py` exists (to be replaced by unified script)
  - New unified script: `open_check_sync_update_validate_run_publish_tsfrequirements.py` (in development)
- **Validation:** YAML references fixed in all evidence files; ready for TruDAG integration.
- **Workflow:**
  1. Script detects new requirements / empty fields in table
  2. Script creates files with structure/template (EXPECT, ASSERT, EVID, ASSUMP)
  3. Script pauses and shows suggested prompt for Claude/Copilot
  4. User asks AI (via VSCode chat or Cmd+I) to generate content
  5. AI edits files directly using VSCode tools (no copy-paste needed)
  6. User confirms (presses Enter)
  7. Script resumes → validates → runs TruDAG
- **Fallback:** If user prefers, can use `gh copilot` CLI (Option C) for full automation.
- **Next:** Create `tsf_config.yaml` and implement unified script.

---
