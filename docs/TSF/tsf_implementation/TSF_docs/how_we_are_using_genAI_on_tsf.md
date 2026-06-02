# How We Are Using Generative AI on TSF

## GenAI Integration in TSF

This document explains how Generative AI (GenAI) is integrated into the TSF automation workflow for the SEA:ME Team 6 project.

**Last Updated:** April 2026

## April 2026 Update

- Local plugin warning noise was removed by resolving `FileReference` symbol collision with TruDAG internals.
- `--check` now reports ASSUMP semantic defaults in addition to EVID placeholders.
- Run logs with `exit 130/143` are now explicitly interpreted as interrupted sessions.

## Table of Contents

0. [Script Execution Commands](#0-script-execution-commands)
1. [What is Generative AI?](#1-what-is-generative-ai)
2. [GenAI in Our TSF Workflow](#2-genai-in-our-tsf-workflow)
3. [Implementation Options Considered](#3-implementation-options-considered)
4. [Current Implementation](#4-current-implementation)
5. [How to Use AI Generation](#5-how-to-use-ai-generation)
6. [Best Practices](#6-best-practices)
7. [AI Methods Summary](#7-ai-methods-summary)
8. [Future Gen AI Integration Options](#8-future-gen-ai-integration-options)
9. [Our TSF Generation Process](#9-our-tsf-generation-process)
10. [Why We Use VSCode with Claude](#10-why-we-use-vscode-with-claude)
11. [Gen AI Best Practices for TSF](#11-gen-ai-best-practices-for-tsf)
12. [Maintenance and Updates](#12-maintenance-and-updates)
13. [Team](#13-team)
14. [Conclusion](#14-conclusion)
15. [Resources](#15-resources)
16. [Contact and Support](#16-contact-and-support)
17. [Post 0/124 Recovery Notes (Mar 2026)](#17-post-0124-recovery-notes-mar-2026)


## 0. Script Execution Commands

Run these commands from repository root.

Short form (from repo root, after activating venv):

```bash
source .venv/bin/activate

# Validate structure
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check

# Sync evidence
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --sync

# Validate + score + publish
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate

# Full run
python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --all
```

Full form (from anywhere, includes cd + venv activation):

```bash
# Validate structure
cd /home/seame/Documents/SEA-ME_Team6_2025-26 && source /home/seame/Documents/SEA-ME_Team6_2025-26/.venv/bin/activate && python3 /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --check

# Sync evidence
cd /home/seame/Documents/SEA-ME_Team6_2025-26 && source /home/seame/Documents/SEA-ME_Team6_2025-26/.venv/bin/activate && python3 /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --sync

# Validate + score + publish
cd /home/seame/Documents/SEA-ME_Team6_2025-26 && source /home/seame/Documents/SEA-ME_Team6_2025-26/.venv/bin/activate && python3 /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --validate

# Full run
cd /home/seame/Documents/SEA-ME_Team6_2025-26 && source /home/seame/Documents/SEA-ME_Team6_2025-26/.venv/bin/activate && python3 /home/seame/Documents/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --all
```

## 1. What is Generative AI?

**Generative AI (Gen AI)** refers to artificial intelligence systems that can create new content—text, code, images, or other outputs—based on patterns learned from training data. Unlike traditional software that follows explicit rules, Gen AI models learn from examples and generate contextually appropriate responses.

### Examples of Gen AI:
- **Claude (Anthropic)** - Advanced language model for coding and reasoning
- **ChatGPT (OpenAI)** - Conversational AI for various tasks
- **GitHub Copilot** - Code completion and generation assistant
- **Gemini (Google)** - Multimodal AI for text and analysis

### Examples of GenAI Used in Development

| Tool | Provider | Use Case |
|------|----------|----------|
| **Claude** | Anthropic | Coding, documentation, reasoning |
| **GitHub Copilot** | GitHub/Microsoft | Code completion, suggestions |
| **ChatGPT** | OpenAI | General assistance |
| **gh copilot CLI** | GitHub | Terminal-based AI |

### How Gen AI Works:
1. **Training**: Models learn patterns from vast amounts of text/code
2. **Prompting**: User provides context and instructions
3. **Generation**: Model creates new content matching the request
4. **Iteration**: Output can be refined through conversation

---

## 2. GenAI in Our TSF Workflow

### Current Implementation (Template-Based)

### Where AI is Used

1. **Content Generation** - Creating header/text for TSF items
2. **Evidence Extraction** - Suggesting evidence links from sprint files
3. **Validation Assistance** - Identifying missing or incomplete content


**Location**: `docs/TSF/tsf_implementation/scripts/`

**What We Automated**:
- ✅ Detection of new requirements in `tsf-requirements-table.md`
- ✅ Generation of TSF items (EXPECT, ASSERT, EVID, ASSUMP)
- ✅ Evidence synchronization from sprint files
- ✅ TruDAG validation and scoring
- ✅ Automatic backups and safety branches

**How It Works**:
```python
# Template-based generation with intelligent rules
def generate_expectation(requirement_text):
    """
    Analyzes requirement text and generates appropriate expectation
    using predefined patterns and keywords.
    """
    keywords = extract_keywords(requirement_text)
    
    if "communication" in keywords:
        template = "System shall enable {protocol} communication"
    elif "display" in keywords:
        template = "UI shall render {component} correctly"
    # ... more intelligent patterns
    
    return fill_template(template, keywords)
```

### Why This Approach?

**Pros**:
- ✅ Works immediately (no API keys needed)
- ✅ Fast and deterministic
- ✅ No external dependencies
- ✅ No cost per generation
- ✅ Fully automated (no human intervention during execution)

**Cons**:
- ⚠️ Not "true" Gen AI (rule-based, not learned)
- ⚠️ Limited to predefined patterns
- ⚠️ Requires manual template updates for new requirement types


### Automation Architecture

```
┌─────────────────────────────────────────────────────────────┐
│ open_check_sync_update_validate_run_publish_tsfrequirements.py │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│ 1. DETECT new requirements in table                        │
│    └─ New L0-X IDs without corresponding item files        │
│                                                             │
│ 2. GENERATE item content:                                  │
│    ├─ Option G: VSCode/Claude (semi-automated)             │
│    └─ Option C: gh copilot CLI (fallback)                  │
│                                                             │
│ 3. SYNC evidence from sprints                              │
│    └─ Extract links with EXPECT-L0-X markers               │
│                                                             │
│ 4. VALIDATE and publish                                    │
│    └─ TruDAG scoring and report generation                 │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 3. Implementation Options Considered

During development, we evaluated multiple AI integration options:

### Option A: GitHub Copilot API ❌ Rejected

**Description:** Direct API access to GitHub Copilot

**Reason rejected:** API not officially public, unreliable

### Option B: Interactive Script ❌ Rejected

**Description:** Local script with placeholder generation and user approval

**Reason rejected:** Would produce identical text for same-category items

### Option C: gh copilot CLI ✅ Chosen (Fallback)

**Description:** GitHub's official CLI tool with Copilot extension

**Command:**
```bash
gh extension install github/gh-copilot
gh copilot suggest -t shell "Generate TSF item content for..."
```

**Pros:**
- ✅ Official GitHub tool
- ✅ Works in scripts via subprocess
- ✅ Integrates with GitHub ecosystem

**Cons:**
- ⚠️ Requires GitHub Copilot subscription
- ⚠️ Requires CLI setup

### Option D: VSCode Extension API ❌ Rejected

**Description:** Direct API to call VSCode Copilot/Claude extensions

**Reason rejected:** No public API exists for script-based automation

### Option E: Local LLM (Ollama) ❌ Rejected

**Description:** Run open-source LLMs locally

**Reason rejected:** Hardware intensive (8GB+ RAM), not portable

### Option F: Cloud APIs (OpenAI/Anthropic) ❌ Rejected

**Description:** Use paid APIs like OpenAI GPT or Anthropic Claude

**Reason rejected:** Ongoing costs, privacy concerns

### Option G: Manual via VSCode ✅ Chosen (Primary)

**Description:** Use Copilot/Claude in VSCode semi-automatically

**How it works:**
1. Script opens file in VSCode
2. User triggers AI completion (Claude/Copilot)
3. AI generates content directly in file
4. User reviews and saves

**Pros:**
- ✅ Human oversight
- ✅ No external API setup
- ✅ Direct file editing

---

## 4. Current Implementation

### Primary Method: Option G (VSCode/Claude)

The script opens files in VSCode and shows prompt suggestions for AI generation:

```python
# From config.yaml
ai:
  primary_method: "manual"
  
  manual:
    open_in_vscode: true
    show_prompt_suggestion: true
    wait_for_user_confirmation: true
    prompt_template: |
      Generate content for TSF item {item_type}-L0-{id}:
      - Requirement: {requirement}
      - Acceptance Criteria: {acceptance_criteria}
      - Fill the header, text, and references fields appropriately.
```

### Fallback Method: Option C (gh copilot CLI)

When manual generation is not available:

```python
# ai_generator.py
def generate_with_copilot_cli(prompt: str) -> str:
    result = subprocess.run(
        ['gh', 'copilot', 'suggest', '-t', 'shell', prompt],
        capture_output=True,
        text=True,
        timeout=30
    )
    return result.stdout
```

### Configuration

**File:** `docs/TSF/tsf_implementation/scripts/config.yaml`

```yaml
ai:
  # Primary method: "manual" (Option G - Semi-automated with VSCode/Claude)
  primary_method: "manual"

  # Fallback methods in order of preference
  fallbacks: ["copilot_cli"]

  # Manual generation settings
  manual:
    open_in_vscode: true
    show_prompt_suggestion: true
    wait_for_user_confirmation: true
    prompt_template: |
      Generate content for TSF item {item_type}-L0-{id}:
      - Requirement: {requirement}
      - Acceptance Criteria: {acceptance_criteria}

  # Copilot CLI settings (fallback)
  copilot:
    timeout: 30
```

---

## 5. How to Use AI Generation

### Method 1: VSCode with Claude/Copilot (Recommended)

1. **Run sync command:**
   ```bash
   source .venv/bin/activate && python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --sync
   ```

2. **Script opens file in VSCode with prompt suggestion**

3. **Use AI to generate content:**
   - With Claude: Select text, press Ctrl+K, enter prompt
   - With Copilot: Start typing, accept suggestions

4. **Review and save file**

5. **Confirm in terminal to continue**

### Method 2: gh copilot CLI (Fallback)

1. **Setup (one-time):**
   ```bash
   gh auth login
   gh extension install github/gh-copilot
   ```

2. **Run sync with CLI fallback:**
   ```bash
   # Modify config.yaml to use copilot_cli as primary
   # Then run:
   source .venv/bin/activate && python3 docs/TSF/tsf_implementation/scripts/open_check_sync_update_validate_run_publish_tsfrequirements.py --sync
   ```

### Method 3: Manual Template (No AI)

If AI is not available, the script generates skeleton files:

```yaml
---
id: EXPECT-L0-X
header: "[Generated from requirements table]"
text: |
  [Content to be filled manually]
level: 1.X
normative: true
references: []
reviewers:
  - name: Joao Jesus Silva
    email: joao.silva@seame.pt
review_status: pending
---
```

---

## 6. Best Practices

### 1. Always Review AI-Generated Content

AI can make mistakes. Always review:
- Technical accuracy
- Reference validity
- Consistency with project terminology

### 2. Use Specific Prompts

```
# Good prompt
Generate content for ASSERT-L0-8 about CAN bus bidirectional communication.
The requirement states that RPi5 and STM32 must exchange messages via CAN.
Include references to CAN-test-guide.md and CAN-overview.md.

# Vague prompt
Generate assertion content.
```

### 3. Maintain Consistency

Ensure AI-generated content follows project conventions:
- Same terminology across items
- Consistent reference format
- Matching levels (1.X)

### 4. Document AI Usage

When using AI, consider adding a note:

```yaml
# In the item file
text: |
  [AI-assisted content, reviewed by team member]
  ...
```

---

## 7. AI Methods Summary

| Method | Automation | Cost | Setup | Best For |
|--------|------------|------|-------|----------|
| Option G (VSCode) | Semi-auto | Free | Minimal | Daily use |
| Option C (CLI) | Automated | Subscription | Moderate | Batch generation |
| Templates | Manual | Free | None | Fallback |


---

## 8. Future Gen AI Integration Options

### Option A: GitHub Copilot CLI (Recommended Next Step)

**Setup**:
```bash
# Install GitHub CLI extension
gh extension install github/gh-copilot
```

**Usage in Script**:
```python
import subprocess

def generate_with_copilot(prompt):
    """Use gh copilot CLI for true Gen AI generation"""
    result = subprocess.run(
        ['gh', 'copilot', 'suggest', '-t', 'shell', prompt],
        capture_output=True,
        text=True
    )
    return result.stdout
```

**Requirements**:
- GitHub Copilot subscription
- GitHub CLI installed
- Internet connection

### Option B: Local LLM (Ollama)

**Requirements**:
- 8GB+ RAM
- Local model download (~4GB)
- No API costs

**Not viable for our current setup** (insufficient RAM).

### Option C: Cloud APIs (Anthropic/OpenAI)

**Requirements**:
- Paid API key
- Cost per request

**Not chosen** due to cost concerns.

---

## 9. Our TSF Generation Process

### Step 1: Detection
```python
# detectors.py - Parse tsf-requirements-table.md
new_items = detect_new_requirements()
# Output: [{'id': 'L0-18', 'text': '...', 'evidence': '...'}]
```

### Step 2: Generation
```python
# generator_items_second_source_of_truth.py - Create TSF items (with generators integrated)
for item in new_items:
    generate_expectation(item)   # EXPECT-L0-18.md
    generate_assertion(item)     # ASSERT-L0-18.md
    generate_evidence(item)      # EVID-L0-18.md
    generate_assumption(item)    # ASSUMP-L0-18.md
```

### Step 3: Evidence Sync
```python
# sync_evidence.py - Extract from sprints
sprint_evidence = scan_sprint_files()
update_evidence_references(sprint_evidence)
```

### Step 4: Validation
```python
# validators.py - Check format and structure
validate_yaml_frontmatter()
validate_references()
validate_trudag_structure()
```

### Step 5: TruDAG Integration
```bash
# trudag_runner.py - Wrap setup_trudag_clean.sh
trudag score --all
# Verify all items score >= 0.8
```

---

## 10. Why We Use VSCode with Claude

**Current Tool**: Claude Sonnet 4.5 integrated in VSCode

### Advantages for TSF Work:
1. **Context-Aware**: Understands full project structure
2. **File Operations**: Can read/write TSF items directly
3. **Interactive Development**: Real-time feedback and iterations
4. **Git Integration**: Manages branches and commits safely
5. **Multi-File Edits**: Updates multiple TSF items simultaneously

### What Claude Does:
- ✅ Designs automation scripts
- ✅ Implements Python modules
- ✅ Creates documentation
- ✅ Reviews generated TSF items
- ✅ Debugs validation errors
- ✅ Suggests improvements

### What Claude CANNOT Do:
- ❌ Run inside Python scripts (no API available to scripts)
- ❌ Generate TSF items during script execution
- ❌ Replace template-based generation (yet)

**Note**: The automation script runs independently of Claude. Claude is used for **designing and implementing** the automation, not for **runtime generation**.

---

## 11. Gen AI Best Practices for TSF

### Writing Effective Prompts

**Bad Prompt**:
```
"Generate expectation"
```

**Good Prompt**:
```
"Generate an expectation item for requirement L0-18:
'The vehicle shall communicate via CAN bus'

Requirements:
- Format: YAML frontmatter + markdown body
- Include: id, header, text, level, normative, references
- Style: Technical, precise, testable
- Level: 1.18 (derived from L0-18)
- References: Link to ASSERT-L0-18"
```

### Validation Checklist

After Gen AI generation, always verify:
- ✅ YAML syntax is valid
- ✅ All required fields present
- ✅ References are bidirectional
- ✅ Level format matches pattern (e.g., `1.18`)
- ✅ Normative boolean is correct
- ✅ Text is clear and testable

---

## 12. Maintenance and Updates

### When to Update Templates

Update `generator_items_second_source_of_truth.py` templates when:
1. New requirement patterns emerge (e.g., "security", "performance")
2. Team identifies common generation errors
3. TruDAG validation reveals structural issues
4. New TSF item types are introduced

### When to Switch to Gen AI

Consider migrating to `gh copilot` CLI when:
1. Template coverage becomes insufficient
2. Generation quality needs improvement
3. Team has GitHub Copilot subscriptions
4. More natural language generation is required

### Monitoring Generation Quality

Track metrics:
- **Coverage**: % of requirements successfully auto-generated
- **Accuracy**: % of generated items passing validation
- **TruDAG Score**: Average score of auto-generated items
- **Manual Edits**: % of generated items requiring human correction

**Target**: 90%+ items auto-generated with score >= 0.8

---

## 13. Team

### For Developers

**DO**:
- ✅ Review generated TSF items before committing
- ✅ Report template gaps or errors
- ✅ Test automation with new requirements
- ✅ Keep tsf-requirements-table.md table updated

**DON'T**:
- ❌ Manually create TSF items (use script)
- ❌ Edit items without running validation
- ❌ Skip backup creation
- ❌ Commit without TruDAG verification

### For Reviewers

**Check**:
1. Generated items follow TSF structure
2. References are complete and bidirectional
3. Evidence links are valid
4. TruDAG scores are acceptable (>= 0.8)
5. Backup was created before modifications

---

## 14. Conclusion

Our TSF implementation uses a **hybrid approach**:
- **Primary:** VSCode with Claude/Copilot for semi-automated generation
- **Fallback:** gh copilot CLI for automated batch processing
- **Backup:** Template generation when AI is unavailable

This approach balances:
- ✅ Human oversight and quality control
- ✅ Automation efficiency
- ✅ Flexibility for different team setups

## 15. Resources

### Internal Documentation
- `docs/TSF/tsf_implementation/USAGE.md` - TruDAG basics
- `docs/TSF/tsf_implementation/VALIDATOR_GUIDE.md` - Validation rules
- `docs/TSF/requirements/TSF_requirements_type.md` - Item types

### External Resources
- [TruDAG Documentation](https://github.com/trustable/trudag) - Official docs
- [GitHub Copilot CLI](https://docs.github.com/en/copilot/github-copilot-in-the-cli) - CLI usage
- [Anthropic Claude](https://www.anthropic.com/claude) - About Claude

### Gen AI Learning
- [Prompt Engineering Guide](https://www.promptingguide.ai/) - Writing effective prompts
- [OpenAI Best Practices](https://platform.openai.com/docs/guides/prompt-engineering) - Gen AI patterns

---

## 16. Contact and Support

**Questions about Gen AI in TSF?**
- Check this documentation first
- Review existing generated items as examples
- Test with simple requirements (e.g., L0-18)
- Ask team during sprint retrospectives

**Automation Issues?**
- Run validators manually: `python3 validate_items_formatation.py`
- Check TruDAG logs: `trudag score --verbose`
- Review backup files: `items_backupN.tar.gz`
- Verify git branch: `feature/TSF/automatize-tsf-in-github`

---

## 17. Post 0/124 Recovery Notes (Mar 2026)

Applied corrections:

1. `setup_trudag_clean.sh`:
- fixed broken symlink cleanup checks (`-e` and `-L`)
- replaced hardcoded `/Volumes/...` path with dynamic `TSF_IMPL`

2. `.dotstop_extensions/validators.py`:
- imported `yaml` from `trudag.dotstop.core.validator` for strict signature matching
- updated `validate_software_dependencies` to accept `components`, `dependencies`, and `packages`

Outcome after revalidation:

- score improved from `0/124` to `82/124`
- `ASSUMP_L0_23` to `ASSUMP_L0_31` moved from `0.0` to `1.0`

---

**Last Updated**: March 17, 2026  
**Authors**: joao Silva SEA-ME Team 6  
**Status**: Active (Template-based generation implemented)
