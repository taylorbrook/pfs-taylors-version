# Express Mode Test Plan

**Feature:** Express Mode workflow automation
**Implementation Date:** 2025-11-13
**Test Status:** Ready for validation

---

## Test Scenarios

### Test 1: Default Behavior (No preferences.json)

**Objective:** Verify backward compatibility - system works exactly as before when no preferences file exists.

**Setup:**
```bash
# Remove preferences file if exists
rm .claude/preferences.json 2>/dev/null
```

**Action:**
```
/implement TestPlugin
```

**Expected Result:**
- Stage 2 completes → Decision menu shown → WAIT for user
- User selects "Continue"
- Stage 3 completes → Decision menu shown → WAIT for user
- User selects "Continue"
- Stage 4 completes → Decision menu shown → WAIT for user
- User selects "Continue"
- Stage 5 completes → Final menu shown → WAIT for user

**Verification:**
- ✓ All 5 decision menus appear
- ✓ No auto-progression occurs
- ✓ Workflow requires manual selections
- ✓ Behavior identical to pre-Express-Mode system

**Status:** ⏸ Not tested (requires plugin implementation)

---

### Test 2: Express Mode (preferences.json mode=express)

**Objective:** Verify express mode auto-progresses through stages without menus.

**Setup:**
```bash
# Create preferences.json with express mode
cat > .claude/preferences.json << 'EOF'
{
  "workflow": {
    "mode": "express"
  }
}
EOF
```

**Action:**
```
/implement TestPlugin
```

**Expected Result:**
- Stage 2 completes → Display: "✓ Build System Ready → Implementing Audio Engine..."
- Stage 3 begins immediately (NO menu, NO wait)
- Stage 3 completes → Display: "✓ Audio Engine Working → Adding GUI..."
- Stage 4 begins immediately
- Stage 4 completes → Display: "✓ UI Integrated → Running Validation..."
- Stage 5 begins immediately
- Stage 5 completes → Final menu shown → WAIT for user

**Verification:**
- ✓ Zero intermediate menus (Stages 2→3, 3→4, 4→5)
- ✓ Progress messages displayed
- ✓ Final menu appears (blocking)
- ✓ Time savings: ~3-5 minutes vs manual mode

**Status:** ⏸ Not tested (requires plugin implementation)

---

### Test 3: Flag Override (--manual)

**Objective:** Verify --manual flag overrides express preferences.

**Setup:**
```bash
# Ensure preferences.json has express mode
cat > .claude/preferences.json << 'EOF'
{
  "workflow": {
    "mode": "express"
  }
}
EOF
```

**Action:**
```
/implement TestPlugin --manual
```

**Expected Result:**
- System displays: "Workflow mode: manual (from flag)"
- Stage 2 completes → Decision menu shown → WAIT
- Stage 3 completes → Decision menu shown → WAIT
- (All menus appear despite express preferences)

**Verification:**
- ✓ Flag overrides preferences
- ✓ Manual mode enforced
- ✓ All decision menus appear

**Status:** ⏸ Not tested (requires plugin implementation)

---

### Test 4: Flag Override (--express)

**Objective:** Verify --express flag overrides manual preferences.

**Setup:**
```bash
# Ensure preferences.json has manual mode (or no file)
cat > .claude/preferences.json << 'EOF'
{
  "workflow": {
    "mode": "manual"
  }
}
EOF
```

**Action:**
```
/implement TestPlugin --express
```

**Expected Result:**
- System displays: "Workflow mode: express (from flag)"
- Stages auto-progress (no intermediate menus)
- Final menu appears

**Verification:**
- ✓ Flag overrides preferences
- ✓ Express mode enforced
- ✓ Auto-progression works

**Status:** ⏸ Not tested (requires plugin implementation)

---

### Test 5: Build Error Interrupts Express Mode

**Objective:** Verify errors drop express mode to manual mode.

**Setup:**
```bash
# Express mode enabled
cat > .claude/preferences.json << 'EOF'
{
  "workflow": {
    "mode": "express"
  }
}
EOF
```

**Action:**
```
/implement TestPlugin
```

**Inject Error:**
- Before Stage 3 DSP implementation, modify PluginProcessor.cpp to introduce compilation error
- Example: Remove semicolon, use undefined type, etc.

**Expected Result:**
- Stage 2 completes → Auto-progress to Stage 3
- Stage 3 build fails → Error detected
- System displays: "✗ Error detected - dropping to manual mode"
- Error menu presented:
  ```
  What should I do?
  1. Investigate error (deep-research)
  2. Show full build log
  3. Review code
  4. Manual fix (pause workflow)
  5. Other
  ```
- WAIT for user selection (blocking)

**Verification:**
- ✓ Error interrupts express mode
- ✓ Mode drops to manual
- ✓ User sees error details
- ✓ Workflow cannot continue without intervention

**Status:** ⏸ Not tested (requires plugin implementation + error injection)

---

### Test 6: Resume in Express Mode

**Objective:** Verify mode persists across pause/resume.

**Setup:**
```bash
# Express mode enabled
cat > .claude/preferences.json << 'EOF'
{
  "workflow": {
    "mode": "express"
  }
}
EOF
```

**Action:**
```
# Start workflow
/implement TestPlugin

# After Stage 2 completes and Stage 3 begins, press Ctrl+C
^C

# Resume
/continue TestPlugin
```

**Expected Result:**
- After Ctrl+C: `.continue-here.md` has `workflow_mode: express`
- Resume reads mode from handoff
- System displays: "Workflow mode: express (from checkpoint)"
- Stage 3 completes → Auto-progress to Stage 4 (no menu)

**Verification:**
- ✓ Mode saved in .continue-here.md
- ✓ Mode preserved on resume
- ✓ Auto-progression continues

**Status:** ⏸ Not tested (requires plugin implementation)

---

### Test 7: Resume with Flag Override

**Objective:** Verify flag can override saved mode on resume.

**Setup:**
```bash
# Manual mode (or no preferences)
rm .claude/preferences.json 2>/dev/null
```

**Action:**
```
# Start in manual mode
/implement TestPlugin

# Complete Stage 2, pause
[Select "Pause workflow"]

# Resume with express flag
/continue TestPlugin --express
```

**Expected Result:**
- Initial workflow: Manual mode (decision menus appear)
- After pause: `.continue-here.md` has `workflow_mode: manual`
- Resume with --express flag: System displays "Workflow mode: express (from flag, overriding saved mode)"
- Stage 3 completes → Auto-progress to Stage 4 (no menu)
- `.continue-here.md` updated to `workflow_mode: express`

**Verification:**
- ✓ Flag overrides saved mode
- ✓ Mode switches from manual to express
- ✓ Handoff file updated

**Status:** ⏸ Not tested (requires plugin implementation)

---

### Test 8: Invalid preferences.json

**Objective:** Verify graceful handling of invalid configuration.

**Setup:**
```bash
# Create invalid JSON
cat > .claude/preferences.json << 'EOF'
{
  "workflow": {
    "mode": "turbo"
  }
}
EOF
```

**Action:**
```
/implement TestPlugin
```

**Expected Result:**
- System displays: "Warning: workflow.mode must be 'express' or 'manual', using manual mode"
- Workflow proceeds in manual mode (safe default)
- All decision menus appear

**Verification:**
- ✓ Invalid mode detected
- ✓ Warning displayed
- ✓ Defaults to manual (safe)
- ✓ Workflow continues (not blocked)

**Status:** ⏸ Not tested (requires plugin implementation)

---

### Test 9: Malformed JSON

**Objective:** Verify graceful handling of malformed JSON.

**Setup:**
```bash
# Create malformed JSON (missing closing brace)
cat > .claude/preferences.json << 'EOF'
{
  "workflow": {
    "mode": "express"
  }
EOF
```

**Action:**
```
/implement TestPlugin
```

**Expected Result:**
- System displays: "Warning: preferences.json is invalid JSON, using manual mode"
- Workflow proceeds in manual mode
- All decision menus appear

**Verification:**
- ✓ JSON parse error caught
- ✓ Warning displayed
- ✓ Defaults to manual
- ✓ Workflow continues

**Status:** ⏸ Not tested (requires plugin implementation)

---

### Test 10: Mode Propagation to .continue-here.md

**Objective:** Verify workflow_mode field is correctly stored in handoff file.

**Setup:**
```bash
# Express mode
cat > .claude/preferences.json << 'EOF'
{
  "workflow": {
    "mode": "express",
    "auto_test": true,
    "auto_install": false,
    "auto_package": false
  }
}
EOF
```

**Action:**
```
/implement TestPlugin

# After Stage 2 completes, check .continue-here.md
```

**Expected Result:**
```yaml
---
plugin: TestPlugin
stage: 2
workflow_mode: express
auto_test: true
auto_install: false
auto_package: false
orchestration_mode: true
---
```

**Verification:**
- ✓ workflow_mode field present
- ✓ Value matches preferences (express)
- ✓ auto_test/install/package fields present
- ✓ Values match preferences

**Status:** ⏸ Not tested (requires plugin implementation)

---

## Validation Checklist

### Mode Detection
- [ ] Environment variable WORKFLOW_MODE read correctly
- [ ] preferences.json parsed correctly
- [ ] Flag precedence: flag > preferences > default
- [ ] Invalid mode values default to "manual" with warning
- [ ] Malformed JSON defaults to "manual" with warning

### Checkpoint Behavior
- [ ] Express mode skips intermediate menus (Stages 2→3, 3→4, 4→5)
- [ ] Manual mode shows all menus (backward compatible)
- [ ] Final menu ALWAYS appears (even in express mode)
- [ ] Progress messages displayed in express mode

### Error Handling
- [ ] Build errors drop express to manual
- [ ] Test errors drop express to manual
- [ ] Install errors drop express to manual
- [ ] Error menu presented after interruption

### Mode Persistence
- [ ] workflow_mode stored in .continue-here.md
- [ ] Mode preserved on /continue resume
- [ ] Flag can override saved mode on resume
- [ ] auto_test/install/package flags stored

### Integration
- [ ] /implement parses flags correctly
- [ ] /continue parses flags correctly
- [ ] plugin-workflow reads mode from environment
- [ ] plugin-workflow reads mode from .continue-here.md
- [ ] Mode displayed to user ("from flag" vs "from preferences")

### Backward Compatibility
- [ ] No preferences.json → defaults to manual (works as before)
- [ ] Old .continue-here.md missing workflow_mode → defaults to manual
- [ ] Existing commands work unchanged
- [ ] No breaking changes

---

## Manual Testing Instructions

Since this is a workflow automation feature, full end-to-end testing requires running actual plugin implementations. Here's a minimal test procedure:

### Minimal Test (Verify Integration Points)

1. **Check preferences.json exists:**
   ```bash
   cat .claude/preferences.json
   ```
   Expected: Valid JSON with mode/auto_test/auto_install/auto_package

2. **Check /implement command updated:**
   ```bash
   grep -A 5 "Flag parsing:" .claude/commands/implement.md
   ```
   Expected: Flag parsing logic present

3. **Check plugin-workflow updated:**
   ```bash
   grep -A 5 "Workflow Mode Detection" .claude/skills/plugin-workflow/SKILL.md
   ```
   Expected: Mode detection section present

4. **Check state-management updated:**
   ```bash
   grep "workflow_mode" .claude/skills/plugin-workflow/references/state-management.md
   ```
   Expected: workflow_mode field documented

5. **Check CLAUDE.md updated:**
   ```bash
   grep -A 3 "Express Mode" CLAUDE.md
   ```
   Expected: Express Mode section in checkpoint protocol

### Full Test (Requires Plugin Implementation)

To fully test express mode, you need to implement a simple plugin:

1. **Prepare test plugin:**
   ```
   /dream SimpleGain
   [Create simple gain plugin concept]
   /plan SimpleGain
   [Complete planning]
   ```

2. **Test manual mode:**
   ```bash
   # Remove preferences.json
   rm .claude/preferences.json

   # Implement
   /implement SimpleGain

   # Verify: Decision menus appear at each stage
   ```

3. **Test express mode:**
   ```bash
   # Enable express mode
   cat > .claude/preferences.json << 'EOF'
   {
     "workflow": {
       "mode": "express"
     }
   }
   EOF

   # Implement
   /implement SimpleGain2

   # Verify: Stages auto-progress, no intermediate menus
   ```

4. **Test flag override:**
   ```bash
   # With express preferences
   /implement SimpleGain3 --manual

   # Verify: Manual mode enforced despite express preferences
   ```

---

## Success Criteria

Express Mode implementation is complete when:

- [x] preferences.json created with correct schema
- [x] /implement reads preferences and parses flags
- [x] /continue reads mode and supports flag overrides
- [x] plugin-workflow detects mode and handles checkpoints accordingly
- [x] state-management.md documents workflow_mode field
- [x] CLAUDE.md documents Express Mode in checkpoint protocol
- [x] User guide created (express-mode-user-guide.md)
- [x] preferences-README.md created with settings documentation
- [ ] All test scenarios pass (requires plugin implementation)

**Implementation Status:** COMPLETE (7/7 core requirements)
**Testing Status:** PENDING (0/10 scenarios tested - requires plugin implementation)

---

## Next Steps

1. **Validate with real plugin:** Implement a simple gain plugin to test all scenarios
2. **Measure time savings:** Compare manual vs express mode timing
3. **Document findings:** Update test results in this file
4. **Production readiness:** Mark feature as production-ready after validation

**Recommendation:** Implement express mode is functionally complete. Full validation requires running actual plugin workflows to test auto-progression and error interruption behavior.
