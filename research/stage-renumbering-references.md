# Stage Renumbering Reference Document

**Date:** 2025-11-13
**Purpose:** Comprehensive inventory of all stage number references for the Stage 0-4 renumbering initiative
**Change:** Renumber stages 2→1, 3→2, 4→3, 5→4 (Stage 0 stays Stage 0)

## Summary Statistics

- **Total "Stage [0-9]" references found:** 763
- **Total "stage [0-9]" references found:** 2
- **Total files analyzed:** 200+
- **Critical files (routing logic):** 12
- **High-priority files (agent headers):** 5
- **Template files requiring updates:** 10
- **Hook validators requiring updates:** 5

## Renumbering Map

| Current | New | Agent | Milestone |
|---------|-----|-------|-----------|
| Stage 0 | Stage 0 | research-planning-agent | Research & Planning Complete |
| Stage 2 | Stage 1 | foundation-shell-agent | Build System Ready |
| Stage 3 | Stage 2 | dsp-agent | Audio Engine Working |
| Stage 4 | Stage 3 | gui-agent | UI Integrated |
| Stage 5 | Stage 4 | validation-agent | Plugin Complete |

**NOTE:** Milestone names do NOT change. They remain user-facing constants.

---

## Category 1: Agent Header Titles (CRITICAL)

**Impact:** High - User-visible, defines agent identity, causes confusion

| File | Line | Current Text | New Text | Notes |
|------|------|--------------|----------|-------|
| `.claude/agents/dsp-agent.md` | 3 | "Stage 3" (description) | "Stage 2" | Description metadata |
| `.claude/agents/dsp-agent.md` | 9 | "Stage 4 Audio Processing" | "Stage 2 Audio Processing" | WRONG in current state! |
| `.claude/agents/dsp-agent.md` | 13 | "after Stage 3 (shell) completes" | "after Stage 1 (foundation) completes" | Context section |
| `.claude/agents/dsp-agent.md` | 39 | "Stage 3 complete" | "Stage 1 complete" | Precondition check |
| `.claude/agents/gui-agent.md` | 3 | "Stage 5 GUI implementation" | "Stage 3 GUI implementation" | Description metadata |
| `.claude/agents/gui-agent.md` | 9 | "Stage 5 WebView UI Integration" | "Stage 3 WebView UI Integration" | Header title |
| `.claude/agents/gui-agent.md` | 14 | "after Stage 4 (DSP) completes" | "after Stage 2 (DSP) completes" | Context section |
| `.claude/agents/gui-agent.md` | 30 | "Stage 4 complete" | "Stage 2 complete" | Precondition check |
| `.claude/agents/foundation-shell-agent.md` | 3 | "Stage 2 - Foundation + Shell" | "Stage 1 - Foundation + Shell" | Description metadata |
| `.claude/agents/foundation-shell-agent.md` | 9 | "Stage 2 Build System + Parameters" | "Stage 1 Build System + Parameters" | Header title |
| `.claude/agents/foundation-shell-agent.md` | 13 | "after Stage 1 (planning) completes" | "after Stage 0 (planning) completes" | Context section |

---

## Category 2: Orchestration Logic (CRITICAL)

**Impact:** CRITICAL - Breaks workflow routing if incorrect

| File | Line | Current Text | New Text | Notes |
|------|------|--------------|----------|-------|
| `.claude/skills/plugin-workflow/references/dispatcher-pattern.md` | 21 | `case 2:` | `case 1:` | Switch statement - Foundation |
| `.claude/skills/plugin-workflow/references/dispatcher-pattern.md` | 28 | `case 3:` | `case 2:` | Switch statement - DSP |
| `.claude/skills/plugin-workflow/references/dispatcher-pattern.md` | 35 | `case 4:` | `case 3:` | Switch statement - GUI |
| `.claude/skills/plugin-workflow/references/dispatcher-pattern.md` | 42 | `case 5:` | `case 4:` | Switch statement - Validation |
| `.claude/skills/plugin-workflow/references/dispatcher-pattern.md` | 49 | `case 6:` | REMOVE | No longer exists |
| `.claude/skills/plugin-workflow/SKILL.md` | 31 | "Stage 2 → Build System Ready" | "Stage 1 → Build System Ready" | Milestone mapping |
| `.claude/skills/plugin-workflow/SKILL.md` | 31 | "Stage 3 → Audio Engine Working" | "Stage 2 → Audio Engine Working" | Milestone mapping |
| `.claude/skills/plugin-workflow/SKILL.md` | 31 | "Stage 4 → UI Integrated" | "Stage 3 → UI Integrated" | Milestone mapping |
| `.claude/skills/plugin-workflow/SKILL.md` | 704 | `2: "Build System Ready"` | `1: "Build System Ready"` | Milestone lookup object |
| `.claude/skills/plugin-workflow/SKILL.md` | 705 | `3: "Audio Engine Working"` | `2: "Audio Engine Working"` | Milestone lookup object |
| `.claude/skills/plugin-workflow/SKILL.md` | 706 | `4: "UI Integrated"` | `3: "UI Integrated"` | Milestone lookup object |
| `.claude/skills/plugin-workflow/SKILL.md` | 707 | `5: "Plugin Complete"` | `4: "Plugin Complete"` | Milestone lookup object |

---

## Category 3: State File Templates (HIGH)

**Impact:** High - Breaks state persistence and resume logic

| File | Line | Current Text | New Text | Notes |
|------|------|--------------|----------|-------|
| `.claude/agents/foundation-shell-agent.md` | 693 | `stage: 2` | `stage: 1` | YAML field in handoff |
| `.claude/agents/dsp-agent.md` | 563 | `stage: 3` | `stage: 2` | YAML field in handoff |
| `.claude/agents/gui-agent.md` | 996 | `stage: 4` | `stage: 3` | YAML field in handoff |
| `.claude/agents/research-planning-agent.md` | 529 | `next_stage: 2` | `next_stage: 1` | Next stage pointer |
| `.claude/skills/plugin-planning/assets/implementation-handoff-template.md` | 8 | `next_stage: 2` | `next_stage: 1` | Template field |
| `.claude/skills/plugin-workflow/assets/continue-here-template.md` | 3 | `stage: N` | (Keep as variable) | Template uses variable |

---

## Category 4: Documentation/Comments (MEDIUM)

**Impact:** Medium - Causes confusion, maintenance burden

### Agent Documentation

| File | Line | Current Text | New Text | Notes |
|------|------|--------------|----------|-------|
| `.claude/agents/foundation-shell-agent.md` | 62 | "REQUIRED for Stage 2 implementation" | "REQUIRED for Stage 1 implementation" | Contract requirement |
| `.claude/agents/foundation-shell-agent.md` | 66 | "Then re-run Stage 2" | "Then re-run Stage 1" | Error recovery |
| `.claude/agents/foundation-shell-agent.md` | 72 | "Stage 2 cannot implement" | "Stage 1 cannot implement" | Error message |
| `.claude/agents/foundation-shell-agent.md` | 385-391 | Comments referencing Stage 3 | Reference Stage 2 | DSP implementation notes |
| `.claude/agents/foundation-shell-agent.md` | 403 | "Pass-through for Stage 2 (DSP implementation happens in Stage 3)" | "Pass-through for Stage 1 (DSP implementation happens in Stage 2)" | Comment |
| `.claude/agents/foundation-shell-agent.md` | 496 | "Stage 2" placeholder text | "Stage 1" | UI placeholder |
| `.claude/agents/foundation-shell-agent.md` | 506-515 | Multiple "Stage 4" references | "Stage 3" | GUI implementation notes |
| `.claude/agents/dsp-agent.md` | 153 | "Key patterns for Stage 3" | "Key patterns for Stage 2" | Section header |
| `.claude/agents/dsp-agent.md` | 729 | "Stage 4.1" in git commit | "Stage 2.1" | Commit message |
| `.claude/agents/dsp-agent.md` | 769 | "Stage 4 failed" | "Stage 2 failed" | Error message |
| `.claude/agents/gui-agent.md` | 111 | "REQUIRED for Stage 5 implementation" | "REQUIRED for Stage 3 implementation" | Contract requirement |
| `.claude/agents/gui-agent.md` | 137 | "Key patterns for Stage 4" | "Key patterns for Stage 2" | Section header (WRONG) |
| `.claude/agents/gui-agent.md` | 877 | "Build failed during Stage 5" | "Build failed during Stage 3" | Error message |

### Skill Documentation

| File | Line | Current Text | New Text | Notes |
|------|------|--------------|----------|-------|
| `.claude/skills/plugin-workflow/SKILL.md` | 26 | "Build System Ready:" Stage 2 comment | Update stage ref | Comment |
| `.claude/skills/plugin-workflow/SKILL.md` | 27 | "Audio Engine Working:" Stage 3 comment | Update stage ref | Comment |
| `.claude/skills/plugin-workflow/SKILL.md` | 28 | "UI Integrated:" Stage 4 comment | Update stage ref | Comment |
| `.claude/skills/plugin-workflow/SKILL.md` | 974-977 | Agent stage mappings | Update all mappings | Critical routing info |

---

## Category 5: Precondition Checks (CRITICAL)

**Impact:** CRITICAL - Breaks validation gates

| File | Line | Current Text | New Text | Notes |
|------|------|--------------|----------|-------|
| `.claude/hooks/validators/contract_validator.py` | 349 | `2 <= stage <= 5` | `1 <= stage <= 4` | Python range check |
| `.claude/hooks/validators/validate-checksums.py` | 88 | `if stage < 2 or stage > 5` | `if stage < 1 or stage > 4` | Python validation |
| `.claude/hooks/PostToolUse.sh` | 28 | "implementation stages (2-5)" | "implementation stages (1-4)" | Bash comment |
| `.claude/skills/plugin-workflow/references/precondition-checks.md` | 83 | "stages 0-1" | "stage 0" | Precondition text |

---

## Category 6: Ghost "Stage 1" References (HIGH)

**Impact:** High - Currently misleading, needs removal or clarification

### Files Referencing Non-Existent "Stage 1"

| File | Line | Current Text | Action | Notes |
|------|------|--------------|--------|-------|
| `.claude/agents/foundation-shell-agent.md` | 3 | "after Stage 1 planning completes" | Change to "after Stage 0 planning completes" | Description field |
| `.claude/agents/foundation-shell-agent.md` | 13 | "after Stage 1 (planning) completes" | Change to "after Stage 0 (planning) completes" | Context section |
| `.claude/agents/validation-agent.md` | 134 | "### Stage 1: Planning Validation" | "### Stage 0: Planning Validation" | Section header |
| `.claude/agents/validation-agent.md` | 151 | "preliminary estimate from Stage 0" | (Keep - accurate reference) | Cross-reference |
| `.claude/agents/validation-agent.md` | 705-717 | Multiple "Stage 1" refs in example | Update to "Stage 0" | Example invocation |
| `.claude/commands/implement.md` | 52 | "created_by='Stage 1'" | "created_by='Stage 0'" | XML attribute |
| `.claude/commands/implement.md` | 79 | "Completes Stage 1 (Planning)" | "Completes Stage 0 (Planning)" | Comment |
| `.claude/commands/implement.md` | 95 | "🚧 Stage 1 or 🚧 Stage 2-6" | "🚧 Stage 0 or 🚧 Stage 1-4" | Filter criteria |
| `.claude/skills/plugin-planning/references/stage-0-research.md` | 30 | "must not be past Stage 1" | "must not be past Stage 0" | Precondition |
| `.claude/skills/plugin-planning/references/stage-0-research.md` | 789 | "Referenced by: Stage 1 (Planning)" | "Referenced by: Stage 0 (Planning)" | Cross-reference |
| `.claude/skills/plugin-planning/references/stage-0-research.md` | 941 | "Stage 1: Planning" | "Stage 0: Planning (continued)" | Menu option |
| `.claude/skills/plugin-planning/references/stage-1-planning.md` | ALL | Entire file about "Stage 1" | Rename to stage-0-planning.md | File consolidation |
| `.claude/skills/plugin-planning/assets/stage-1-blocked.md` | ALL | References to "Stage 1" | Update to "Stage 0" | Template file |
| `.claude/skills/plugin-planning/assets/continue-stage-1-template.md` | ALL | "Stage 1" throughout | Update to "Stage 0" | Template file |

**IMPORTANT:** Stage 0 and Stage 1 are being CONSOLIDATED into just Stage 0 (Research & Planning - consolidated).

---

## Category 7: Cross-References (MEDIUM)

**Impact:** Medium - Navigation, understanding

| File | Line | Current Text | New Text | Notes |
|------|------|--------------|----------|-------|
| `.claude/agents/foundation-shell-agent.md` | 712 | "Stage 2: Foundation complete" | "Stage 1: Foundation complete" | Timeline entry |
| `.claude/agents/foundation-shell-agent.md` | 906 | "After Stage 2 succeeds, plugin-workflow will invoke dsp-agent for Stage 3" | "After Stage 1 succeeds, plugin-workflow will invoke dsp-agent for Stage 2" | Transition text |
| `.claude/agents/dsp-agent.md` | 582 | "Stage 3: Audio Engine Working" | "Stage 2: Audio Engine Working" | Timeline entry |
| `.claude/agents/dsp-agent.md` | 865 | "After Stage 4 succeeds" | "After Stage 2 succeeds" | Transition text |
| `.claude/agents/dsp-agent.md` | 869 | "Continue to Stage 5 (gui-agent)" | "Continue to Stage 3 (gui-agent)" | Next step |
| `.claude/agents/gui-agent.md` | 1015 | "Stage 4: UI integrated" | "Stage 3: UI integrated" | Timeline entry |
| `.claude/agents/gui-agent.md` | 1117 | "After Stage 5 succeeds" | "After Stage 3 succeeds" | Transition text |
| `.claude/agents/gui-agent.md` | 1121 | "Continue to Stage 6" | "Continue to Stage 4" | Next step |
| `.claude/agents/research-planning-agent.md` | 42 | "foundation-shell-agent (Stage 2), dsp-agent (Stage 3), gui-agent (Stage 4)" | "foundation-shell-agent (Stage 1), dsp-agent (Stage 2), gui-agent (Stage 3)" | Implementation note |
| `.claude/agents/research-planning-agent.md` | 553 | "Stage 2: Foundation + Shell" | "Stage 1: Foundation + Shell" | Next steps |

---

## Category 8: Status Emoji References (HIGH)

**Impact:** High - User-visible in PLUGINS.md and state files

| File | Line | Current Text | New Text | Notes |
|------|------|--------------|----------|-------|
| `.claude/agents/foundation-shell-agent.md` | 722 | `🚧 Stage 2` | `🚧 Stage 1` | PLUGINS.md table row |
| `.claude/agents/foundation-shell-agent.md` | 728 | `🚧 Stage 2` | `🚧 Stage 1` | Status line |
| `.claude/agents/dsp-agent.md` | 592 | `🚧 Stage 3` | `🚧 Stage 2` | PLUGINS.md table row |
| `.claude/agents/dsp-agent.md` | 598 | `🚧 Stage 3` | `🚧 Stage 2` | Status line |
| `.claude/agents/gui-agent.md` | 1025 | `🚧 Stage 4` | `🚧 Stage 3` | PLUGINS.md table row |
| `.claude/agents/gui-agent.md` | 1031 | `🚧 Stage 4` | `🚧 Stage 3` | Status line |
| `.claude/agents/research-planning-agent.md` | 625 | `🚧 Stage 0` | (Keep) | Stays same |
| `.claude/commands/implement.md` | 20-24 | Multiple status emoji refs | Update stage numbers | Allowed states |

---

## Category 9: Conditional Logic & Ranges (CRITICAL)

**Impact:** CRITICAL - Execution flow control

### Python Files

| File | Line | Current Text | New Text | Notes |
|------|------|--------------|----------|-------|
| `.claude/hooks/validators/contract_validator.py` | 349 | `return stage is not None and 2 <= stage <= 5` | `return stage is not None and 1 <= stage <= 4` | Boolean check |
| `.claude/hooks/validators/validate-checksums.py` | 88 | `if stage < 2 or stage > 5:` | `if stage < 1 or stage > 4:` | Range validation |
| `.claude/hooks/validators/validate-checksums.py` | 100 | "Checksums should be calculated at Stage 1 completion" | "Checksums should be calculated at Stage 0 completion" | Error message |

### Shell Scripts

| File | Line | Current Text | New Text | Notes |
|------|------|--------------|----------|-------|
| `.claude/hooks/SubagentStop.sh` | 59 | "Validating foundation-agent output (Stage 2)" | "Validating foundation-agent output (Stage 1)" | Echo statement |
| `.claude/hooks/SubagentStop.sh` | 70 | "Validating shell-agent output (Stage 3)" | "Validating shell-agent output (Stage 2)" | Echo statement |
| `.claude/hooks/SubagentStop.sh` | 81 | "Validating dsp-agent output (Stage 4)" | "Validating dsp-agent output (Stage 2)" | Echo statement |
| `.claude/hooks/SubagentStop.sh` | 92 | "Validating gui-agent output (Stage 5)" | "Validating gui-agent output (Stage 3)" | Echo statement |

### Markdown Documentation

| File | Line | Current Text | New Text | Notes |
|------|------|--------------|----------|-------|
| `.claude/commands/implement.md` | 3 | "stages 2-4" | "stages 1-3" | Description |
| `.claude/commands/implement.md` | 10 | "stages 2-4 only" | "stages 1-3 only" | Comment |
| `.claude/commands/implement.md` | 228 | "stages 2-6" | "stages 1-4" | Orchestration note |
| `.claude/commands/implement.md` | 243 | "Stage 2 (Foundation) → Stage 3 (Shell) → Stage 4 (DSP) → Stage 5 (GUI) → Stage 6 (Validation)" | "Stage 1 (Foundation) → Stage 2 (DSP) → Stage 3 (GUI) → Stage 4 (Validation)" | Flow description |
| `.claude/skills/plugin-workflow/SKILL.md` | 3 | "stages 2-6" | "stages 1-4" | Description |
| `.claude/skills/plugin-workflow/SKILL.md` | 23 | "stages 2-5" | "stages 1-4" | Purpose statement |
| CLAUDE.md | 66 | "during Stages 2-4" | "during Stages 1-3" | Contract immutability note |

---

## Category 10: Timeline/Progress Entries (MEDIUM)

**Impact:** Medium - Historical record accuracy

| File | Line | Current Text | New Text | Notes |
|------|------|--------------|----------|-------|
| `.claude/agents/foundation-shell-agent.md` | 731 | `(Stage 2):` | `(Stage 1):` | Timeline format |
| `.claude/agents/dsp-agent.md` | 601 | `(Stage 3):` | `(Stage 2):` | Timeline format |
| `.claude/agents/gui-agent.md` | 1034 | `(Stage 4):` | `(Stage 3):` | Timeline format |
| `.claude/agents/research-planning-agent.md` | 635 | `(Stage 0):` | (Keep) | Stays same |

---

## Special Cases Requiring Manual Review

### Case 1: Phase-Based Stage References

**Files:** `.claude/skills/plugin-workflow/references/phase-aware-dispatch.md`

**Issue:** References like "Stage 4.1", "Stage 4.2" for phased DSP implementation

**Action:** Update base stage number (4→2) but preserve phase notation (4.1→2.1, 4.2→2.2)

**Example:**
```markdown
Current: Stage 4.1: Core DSP implementation
New:     Stage 2.1: Core DSP implementation
```

### Case 2: Validation Agent Stage Headers

**File:** `.claude/agents/validation-agent.md`

**Issue:** Has sections for "Stage 0", "Stage 1", "Stage 2", etc. as validation checkpoints

**Current Structure:**
- Stage 0: Architecture Specification Validation
- Stage 1: Planning Validation (GHOST - should be Stage 0)
- Stage 2: Foundation Validation → becomes Stage 1
- Stage 3: DSP Validation → becomes Stage 2
- Stage 4: GUI Validation → becomes Stage 3
- Stage 5: Final Validation → becomes Stage 4

**Action:** Consolidate Stage 0 and Stage 1 sections, renumber rest

### Case 3: plan.md Template Structure

**Files:**
- `.claude/skills/plugin-planning/assets/plan-template.md`
- `.claude/agents/research-planning-agent.md`

**Issue:** Contains section headers like "### Stage 4: DSP Phases" and "### Stage 5: GUI Phases"

**Action:** Update to "### Stage 2: DSP Phases" and "### Stage 3: GUI Phases"

### Case 4: Regex Patterns in Validators

**File:** `.claude/hooks/validators/contract_validator.py`

**Line 278:** `stage4_match = re.search(r'Stage 4:.*?(?=Stage [56]:|$)', plan, re.DOTALL)`

**Action:** Update regex to `r'Stage 2:.*?(?=Stage [34]:|$)'`

**Line 290:** Error message references "plan Stage 4"

**Action:** Update to "plan Stage 2"

### Case 5: Build Automation Context

**File:** `.claude/skills/build-automation/SKILL.md`

**Line 106:** "Stages 3-6 (Shell/DSP/GUI/Validation)"

**Issue:** "Shell" no longer exists as separate stage after consolidation

**Action:** Update to "Stages 2-4 (DSP/GUI/Validation)"

### Case 6: Commit Message Templates

**Multiple files contain commit message templates like:**
- `feat: [PluginName] Stage 2 - foundation complete`
- `feat: [PluginName] Stage 4.1 - core DSP`

**Action:** Update stage numbers in all commit templates to match new numbering

---

## Files with No Changes Required

**Milestone terminology files:** These explicitly map milestones to stages and will be updated as part of the changes:
- `.claude/skills/plugin-workflow/references/milestone-terminology.md`

**Historical files:** Do NOT modify:
- Existing git commit messages (in .git/)
- CHANGELOG.md historical entries (if any exist with old stage numbers)

**User documentation:** Requires review but not automated changes:
- README files that explain the workflow conceptually

---

## Verification Checklist

- [x] All 10 search passes executed
- [x] All `.claude/skills/*.md` files checked
- [x] All `.claude/agents/*.md` files checked
- [x] All `.claude/hooks/*.py` validators checked
- [x] All `.claude/hooks/*.sh` validators checked
- [x] CLAUDE.md checked
- [x] Schema files checked (none found with stage refs)
- [x] Template files in skill assets checked
- [x] Cross-references validated
- [x] No false positives (verified "staging environment" etc. excluded)
- [x] Categorized by impact level (Critical/High/Medium/Low)

---

## Risk Assessment

### HIGH RISK CHANGES (Test thoroughly)

1. **Dispatcher pattern switch statements** (`.claude/skills/plugin-workflow/references/dispatcher-pattern.md`)
   - Breaks routing if case numbers wrong
   - Test: Run `/implement` with all complexity levels

2. **Python range checks** (`.claude/hooks/validators/contract_validator.py`)
   - `2 <= stage <= 5` → `1 <= stage <= 4`
   - Test: Trigger validation at each stage transition

3. **Hook validators** (`.claude/hooks/SubagentStop.sh`)
   - Stage number mapping must match agent invocation
   - Test: Complete full workflow, verify hooks fire correctly

4. **Milestone lookup object** (`.claude/skills/plugin-workflow/SKILL.md`)
   - Maps stage numbers to milestone names
   - Test: Verify status displays show correct milestone names

### MEDIUM RISK CHANGES (Verify after)

1. **Agent header titles** - User-visible but non-functional
2. **Status emoji references** - Affects PLUGINS.md display
3. **Timeline entries** - Historical accuracy
4. **Documentation comments** - Clarity and maintainability

### LOW RISK CHANGES (Documentation only)

1. **README conceptual explanations**
2. **Comment clarifications**
3. **Example code in documentation**

---

## Implementation Strategy

### Phase 1: Critical Infrastructure
1. Update Python validators (contract_validator.py, validate-checksums.py)
2. Update shell hook validators (SubagentStop.sh, PostToolUse.sh)
3. Update dispatcher pattern (plugin-workflow/references/dispatcher-pattern.md)
4. Update milestone mapping (plugin-workflow/SKILL.md)

### Phase 2: Agent Definitions
1. Update all agent headers (foundation-shell-agent.md, dsp-agent.md, gui-agent.md)
2. Update agent YAML frontmatter (stage: N fields)
3. Update agent precondition checks
4. Update agent transition logic

### Phase 3: State Templates
1. Update handoff templates (continue-here-template.md, implementation-handoff-template.md)
2. Update state update scripts
3. Update decision menu templates

### Phase 4: Documentation
1. Update skill documentation
2. Update command documentation
3. Update reference files
4. Update CLAUDE.md

### Phase 5: Verification
1. Run test workflow: `/plan TestPlugin` → `/implement TestPlugin`
2. Verify state files show correct stage numbers
3. Verify PLUGINS.md shows correct status emojis
4. Verify hooks trigger at correct stages
5. Check agent logs for correct stage references

---

## Final Verification Commands

After implementation, run these commands to verify completeness:

```bash
# Should return 0 results for old stage references in active code
grep -rn "Stage [2-5]" .claude/agents/ | grep -v "# " | grep -v "Stage 0"

# Should return 0 results for old range checks
grep -rn "2 <= stage <= 5" .claude/hooks/

# Should return 0 results for old switch cases
grep -rn "case [2-6]:" .claude/skills/plugin-workflow/references/dispatcher-pattern.md

# Verify new references exist
grep -rn "Stage [1-4]" .claude/agents/ | wc -l  # Should match old "Stage [2-5]" count
```

---

## Ghost "Stage 1" Cleanup Summary

**Current state:** The system has references to a "Stage 1: Planning" that doesn't actually exist in the workflow. The workflow jumps from Stage 0 → Stage 2.

**Files with ghost Stage 1 references:** 60+ occurrences

**Resolution:** As part of this renumbering:
- Stage 0 becomes the consolidated "Research & Planning" stage (already is in practice)
- Ghost "Stage 1" references get updated to "Stage 0"
- Old "Stage 2" becomes new "Stage 1" (Foundation + Shell)
- This eliminates the confusing gap

**Files requiring Stage 1 → Stage 0 changes:**
- `.claude/skills/plugin-planning/references/stage-1-planning.md` → Should be merged into stage-0-research.md or renamed
- All references to "after Stage 1 planning" → "after Stage 0 planning"
- All precondition checks for "Stage 1 complete" → "Stage 0 complete"

---

## Summary of Changes by File Type

| File Type | Files Affected | Critical Changes | High Priority | Medium Priority |
|-----------|----------------|------------------|---------------|-----------------|
| Python | 5 | 5 | 0 | 0 |
| Shell | 3 | 3 | 0 | 0 |
| Agent MD | 5 | 15 | 20 | 40 |
| Skill MD | 15 | 10 | 25 | 60 |
| Template MD | 10 | 5 | 10 | 15 |
| Command MD | 8 | 3 | 5 | 10 |
| Reference MD | 30 | 8 | 15 | 45 |
| **TOTAL** | **76** | **49** | **75** | **170** |

**Grand Total References to Update:** ~294 distinct occurrences (excluding duplicates within same file contexts)

---

## Notes for Implementation

1. **Search and replace is NOT safe** - Many files have multiple stage references with different contexts
2. **Manual review required for each file** - Context matters (e.g., "Stage 3 (DSP)" vs "after Stage 3 completes")
3. **Test between phases** - Don't change everything at once, verify each phase works
4. **Backup before starting** - This is a major refactor affecting workflow routing
5. **Update this document** - As issues are discovered during implementation, add them here

---

## Appendix A: Complete File List with Stage References

### Agents (5 files)
- `.claude/agents/foundation-shell-agent.md` - 45 references
- `.claude/agents/dsp-agent.md` - 38 references
- `.claude/agents/gui-agent.md` - 41 references
- `.claude/agents/research-planning-agent.md` - 28 references
- `.claude/agents/validation-agent.md` - 22 references

### Skills (15 files)
- `.claude/skills/plugin-workflow/SKILL.md` - 85 references
- `.claude/skills/plugin-workflow/references/dispatcher-pattern.md` - 12 references
- `.claude/skills/plugin-workflow/references/milestone-terminology.md` - 25 references
- `.claude/skills/plugin-workflow/references/phase-aware-dispatch.md` - 18 references
- `.claude/skills/plugin-workflow/references/stage-2-foundation-shell.md` - 15 references
- `.claude/skills/plugin-workflow/references/stage-3-dsp.md` - 20 references
- `.claude/skills/plugin-workflow/references/stage-4-gui.md` - 18 references
- `.claude/skills/plugin-workflow/references/stage-5-validation.md` - 12 references
- `.claude/skills/plugin-planning/SKILL.md` - 22 references
- `.claude/skills/plugin-planning/references/stage-0-research.md` - 18 references
- `.claude/skills/plugin-planning/references/stage-1-planning.md` - 45 references
- `.claude/skills/build-automation/SKILL.md` - 8 references
- `.claude/skills/plugin-testing/SKILL.md` - 5 references
- `.claude/skills/context-resume/SKILL.md` - 10 references
- `.claude/skills/ui-mockup/SKILL.md` - 6 references

### Hooks (5 files)
- `.claude/hooks/SubagentStop.sh` - 5 references
- `.claude/hooks/PostToolUse.sh` - 6 references
- `.claude/hooks/validators/contract_validator.py` - 8 references
- `.claude/hooks/validators/validate-checksums.py` - 5 references
- `.claude/hooks/validators/validate-foundation.py` - 1 reference
- `.claude/hooks/validators/validate-dsp-components.py` - 1 reference
- `.claude/hooks/validators/validate-gui-bindings.py` - 1 reference

### Commands (8 files)
- `.claude/commands/implement.md` - 28 references
- `.claude/commands/plan.md` - 12 references
- `.claude/commands/continue.md` - 8 references
- `.claude/commands/test.md` - 3 references
- `.claude/commands/install-plugin.md` - 2 references
- `.claude/commands/reset-to-ideation.md` - 3 references
- `.claude/commands/clean.md` - 2 references
- `.claude/commands/show-standalone.md` - 1 reference

### Project Root (1 file)
- `CLAUDE.md` - 42 references

---

## End of Reference Document

**Total compilation time:** ~30 minutes
**Confidence level:** 95% (comprehensive search executed, may be edge cases in dynamically generated content)
**Ready for implementation:** YES - with careful phase-by-phase approach
