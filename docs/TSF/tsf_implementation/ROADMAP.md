# 🗺️ TSF Automation Implementation Roadmap

## 📋 **Current Status**
- ✅ Core modules: `ai_generator.py`, `generators.py`, `detectors.py`, `validate_items_formatation.py`
- ✅ Orchestrator: `sync_tsf_manager.py`
- ✅ Graph generator: `generate_graph_from_heuristics.py` (moved to tools/)
- ❌ Missing modules: `sync_evidence.py`, `trudag_runner.py`
- ❌ Missing config: `config.yaml`

## 🎯 **Phase 1: Complete Core Modules** (Priority: HIGH)

### 1.1 Implement `sync_evidence.py`
**Purpose**: Extract evidence from sprint files and update EVID-L0-X.md references
**Input**: `docs/sprints/sprint*.md` → Parse outcomes sections
**Output**: Update `items/evidences/EVID-L0-X.md` with extracted references
**Dependencies**: `detectors.py` (for file changes)

### 1.2 Implement `trudag_runner.py`
**Purpose**: Interface between graph generation and TruDAG commands
**Functions**:
- Generate graph.dot from items
- Initialize TruDAG DB
- Create items in DB
- Apply links and validate
**Dependencies**: `generate_graph_from_heuristics.py`, `trudag` CLI

### 1.3 Create `config.yaml`
**Purpose**: Centralized configuration for all modules
**Sections**:
- paths: repo_root, tsf_impl, items_dir, etc.
- ai: methods, fallbacks, timeouts
- validation: rules, severity levels
- trudag: db_path, graph_dir

## 🎯 **Phase 2: Integration & Testing** (Priority: MEDIUM)

### 2.1 Update `sync_tsf_manager.py`
**Changes**:
- Use new modular structure
- Load config from config.yaml
- Integrate sync_evidence.py calls
- Add trudag_runner.py integration

### 2.2 Verify `detectors.py`
**Check**: Does it properly detect new L0-X IDs from requirements table?

### 2.3 Test Complete Workflow
**Steps**:
1. Add new L0-XX to requirements table
2. Run detection → generation → validation
3. Run sync_evidence → update EVID files
4. Run trudag_runner → validate in DB

## 🎯 **Phase 3: Polish & Documentation** (Priority: LOW)

### 3.1 Error Handling
- Graceful fallbacks for missing dependencies
- Clear error messages for common issues
- Recovery mechanisms for partial failures

### 3.2 Documentation Updates
- Update PLANNING.md with new architecture
- Create module API documentation
- Add troubleshooting guide

### 3.3 Performance Optimization
- Cache expensive operations
- Parallel processing where possible
- Incremental updates instead of full rebuilds

## 📊 **Success Criteria**
- [ ] All 6 core modules implemented
- [ ] config.yaml with all settings
- [ ] End-to-end workflow: requirement → items → validation → TruDAG
- [ ] Evidence sync from sprints working
- [ ] Copilot CLI integration (when available)
- [ ] Comprehensive error handling

## 🔄 **Current Task**
**Implement `sync_evidence.py`** - Extract and sync evidence from sprint files to EVID items.</content>
<parameter name="filePath">/Volumes/Important_Docs/42/SEA-ME_Team6_2025-26/docs/TSF/tsf_implementation/ROADMAP.md