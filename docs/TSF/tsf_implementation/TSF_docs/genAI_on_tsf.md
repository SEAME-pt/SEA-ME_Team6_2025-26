# GenAI Integration in TSF

This document explains how Generative AI (GenAI) is integrated into the TSF automation workflow for the SEA:ME Team 6 project.

**Last Updated:** February 2026

---

## Table of Contents

1. [What is Generative AI?](#what-is-generative-ai)
2. [GenAI in Our TSF Workflow](#genai-in-our-tsf-workflow)
3. [Implementation Options Considered](#implementation-options-considered)
4. [Current Implementation](#current-implementation)
5. [How to Use AI Generation](#how-to-use-ai-generation)
6. [Best Practices](#best-practices)

---

## What is Generative AI?

**Generative AI (GenAI)** refers to AI systems that can create new content—text, code, images, or other outputs—based on patterns learned from training data.

### Examples of GenAI Used in Development

| Tool | Provider | Use Case |
|------|----------|----------|
| **Claude** | Anthropic | Coding, documentation, reasoning |
| **GitHub Copilot** | GitHub/Microsoft | Code completion, suggestions |
| **ChatGPT** | OpenAI | General assistance |
| **gh copilot CLI** | GitHub | Terminal-based AI |

---

## GenAI in Our TSF Workflow

### Where AI is Used

1. **Content Generation** - Creating header/text for TSF items
2. **Evidence Extraction** - Suggesting evidence links from sprint files
3. **Validation Assistance** - Identifying missing or incomplete content

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

## Implementation Options Considered

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

## Current Implementation

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

## How to Use AI Generation

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

## Best Practices

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

## AI Methods Summary

| Method | Automation | Cost | Setup | Best For |
|--------|------------|------|-------|----------|
| Option G (VSCode) | Semi-auto | Free | Minimal | Daily use |
| Option C (CLI) | Automated | Subscription | Moderate | Batch generation |
| Templates | Manual | Free | None | Fallback |

---

## Conclusion

Our TSF implementation uses a **hybrid approach**:
- **Primary:** VSCode with Claude/Copilot for semi-automated generation
- **Fallback:** gh copilot CLI for automated batch processing
- **Backup:** Template generation when AI is unavailable

This approach balances:
- ✅ Human oversight and quality control
- ✅ Automation efficiency
- ✅ Flexibility for different team setups
