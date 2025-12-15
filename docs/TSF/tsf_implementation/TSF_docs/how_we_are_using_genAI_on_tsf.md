# How We Are Using Generative AI on TSF

## 1. What is Generative AI?

**Generative AI (Gen AI)** refers to artificial intelligence systems that can create new content—text, code, images, or other outputs—based on patterns learned from training data. Unlike traditional software that follows explicit rules, Gen AI models learn from examples and generate contextually appropriate responses.

### Examples of Gen AI:
- **Claude (Anthropic)** - Advanced language model for coding and reasoning
- **ChatGPT (OpenAI)** - Conversational AI for various tasks
- **GitHub Copilot** - Code completion and generation assistant
- **Gemini (Google)** - Multimodal AI for text and analysis

### How Gen AI Works:
1. **Training**: Models learn patterns from vast amounts of text/code
2. **Prompting**: User provides context and instructions
3. **Generation**: Model creates new content matching the request
4. **Iteration**: Output can be refined through conversation

---

## 2. Gen AI in Our TSF Workflow

### Current Implementation (Template-Based)

**Location**: `docs/TSF/tsf_implementation/scripts/`

**What We Automated**:
- ✅ Detection of new requirements in `LO_requirements.md`
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

---

## 3. Future Gen AI Integration Options

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

## 4. Our TSF Generation Process

### Step 1: Detection
```python
# detectors.py - Parse LO_requirements.md
new_items = detect_new_requirements()
# Output: [{'id': 'L0-18', 'text': '...', 'evidence': '...'}]
```

### Step 2: Generation
```python
# generators.py - Create TSF items
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

## 5. Why We Use VSCode with Claude

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

## 6. Gen AI Best Practices for TSF

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

## 7. Maintenance and Updates

### When to Update Templates

Update `generators.py` templates when:
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

## 8. Team Guidelines

### For Developers

**DO**:
- ✅ Review generated TSF items before committing
- ✅ Report template gaps or errors
- ✅ Test automation with new requirements
- ✅ Keep LO_requirements.md table updated

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

## 9. Resources

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

## 10. Contact and Support

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

**Last Updated**: December 13, 2025  
**Authors**: SEA-ME Team 6  
**Status**: Active (Template-based generation implemented)
