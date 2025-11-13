# Express Mode Implementation Specification

**Feature Name:** Express Mode
**Purpose:** Eliminate manual checkpoint decision gates for automated workflows
**Time Savings:** 3-5 minutes per plugin (no manual decisions at 4-5 checkpoints)
**Created:** 2025-11-13

---

## 1. Executive Summary

### 1.1 Overview

Express Mode is a preferences-based workflow automation feature that eliminates manual decision gates during plugin implementation. Instead of presenting decision menus after each stage (0, 2, 3, 4, 5), the system auto-progresses to the next stage when in express mode.

**Current behavior (Manual mode):**
```
Stage 2 complete → Present menu → Wait for user → User selects "Continue" → Stage 3
```

**New behavior (Express mode):**
```
Stage 2 complete → Auto-invoke Stage 3 → Show progress: "✓ Build System Ready → Implementing Audio Engine..."
```

### 1.2 Integration Points

**Files modified:**
- `.claude/preferences.json` (CREATE - new file)
- `.claude/commands/implement.md` (UPDATE - add flag parsing)
- `.claude/skills/plugin-workflow/SKILL.md` (UPDATE - mode-aware checkpoints)
- `.claude/skills/plugin-lifecycle/SKILL.md` (UPDATE - auto_install integration)
- `.claude/skills/plugin-packaging/SKILL.md` (UPDATE - auto_package integration)
- `.claude/skills/plugin-testing/SKILL.md` (UPDATE - auto_test integration)

**No breaking changes:** Existing behavior preserved when no preferences file exists.

### 1.3 Key Design Decisions

**Decision 1: Preferences file over complexity detection**
- **Rationale:** Simpler, more predictable, user-controlled
- **Alternative rejected:** Auto-detect complexity and enable express mode
- **Why:** User preferences trump algorithmic guessing

**Decision 2: Command-line flags override preferences**
- **Rationale:** One-off control without modifying config file
- **Precedence:** `--express` flag > `preferences.json` > default (manual)

**Decision 3: Mode stored in .continue-here.md**
- **Rationale:** Resume must preserve mode across sessions
- **Location:** `orchestration_mode` field already exists, add `workflow_mode: "express" | "manual"`

**Decision 4: Safety gates preserved**
- **Rationale:** Build errors must interrupt, not auto-retry
- **Behavior:** Express mode drops to manual on any error/failure

---

## 2. Current State Analysis

### 2.1 How Checkpoints Work Today

**Checkpoint locations identified:**

1. **After Stage 0 (plugin-planning skill):**
   - Location: `.claude/skills/plugin-planning/SKILL.md` lines 700-730
   - Menu: "Continue to Stage 2", "Pause", "Review plan", "Other"
   - Waits for user response before returning to /implement command

2. **After Stage 2 (plugin-workflow skill):**
   - Location: `.claude/skills/plugin-workflow/SKILL.md` lines 459-580 (checkpoint protocol)
   - Menu: "Continue to Stage 3", "Test build", "Pause", "Review code", "Other"
   - Blocking wait at line 561 (decision gate)

3. **After Stage 3 (plugin-workflow skill):**
   - Location: Same as Stage 2 (shared checkpoint protocol)
   - Menu: "Continue to Stage 4", "Test audio", "Pause", "Review DSP", "Other"
   - Blocking wait

4. **After Stage 4 (plugin-workflow skill):**
   - Location: Same as Stages 2-3 (shared checkpoint protocol)
   - Menu: "Continue to Stage 5", "Test UI", "Pause", "Review GUI", "Other"
   - Blocking wait

5. **After Stage 5 (plugin-workflow skill):**
   - Location: Same as Stages 2-4 (shared checkpoint protocol)
   - Menu: "Install plugin", "Package installer", "Test again", "Pause", "Other"
   - Blocking wait (final menu)

**Phase checkpoints (complex plugins only):**
- For complexity ≥3, phases 3.1→3.2→3.3 and 4.1→4.2→4.3 also have checkpoints
- Location: `.claude/skills/plugin-workflow/references/phase-aware-dispatch.md` lines 146-169
- Same checkpoint protocol applies

### 2.2 Checkpoint Protocol Steps

**Current 6-step checkpoint (plugin-workflow lines 474-512):**

```javascript
1. verifyStateUpdate() - Check if subagent updated .continue-here.md
2. fallbackStateUpdate() - If verification failed, orchestrator updates state
3. commitStage() - Git commit all changes (atomic)
4. verifyCheckpoint() - Validate all steps succeeded
5. presentDecisionMenu() - BLOCKING - Present numbered menu
6. WAIT for user response - getUserInput() blocks execution
```

**What needs to change:**
- Step 5: Check `workflow_mode` before presenting menu
- If `mode == "express"`: Skip menu, auto-select "continue to next stage"
- If `mode == "manual"`: Present menu, wait for user (current behavior)

### 2.3 What Stays the Same

**Unchanged:**
- State verification (steps 1-2)
- Git commits (step 3)
- Checkpoint validation (step 4)
- Error handling (failures still interrupt)
- Final menu after Stage 5 (ALWAYS shown)

**Why final menu is always shown:**
- User must decide: install? package? test? done?
- No sensible default (unlike "continue to next stage")
- Auto-install preferences apply AFTER final menu

---

## 3. Preferences File Specification

### 3.1 File Location & Format

**Path:** `.claude/preferences.json`

**Format:** JSON with schema validation

**Example:**
```json
{
  "workflow": {
    "mode": "express",
    "auto_test": false,
    "auto_install": true,
    "auto_package": false
  }
}
```

**Schema:**
```json
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "type": "object",
  "properties": {
    "workflow": {
      "type": "object",
      "properties": {
        "mode": {
          "type": "string",
          "enum": ["express", "manual"],
          "description": "Workflow automation level"
        },
        "auto_test": {
          "type": "boolean",
          "description": "Run pluginval after Stage 5 completes"
        },
        "auto_install": {
          "type": "boolean",
          "description": "Install to system folders without asking"
        },
        "auto_package": {
          "type": "boolean",
          "description": "Create PKG installer automatically"
        }
      },
      "required": ["mode"],
      "additionalProperties": false
    }
  },
  "required": ["workflow"],
  "additionalProperties": true
}
```

### 3.2 Preference Meanings

**`workflow.mode`:**
- **Type:** String enum ("express" | "manual")
- **Default:** "manual" (if file missing or field missing)
- **Effect:**
  - `"express"`: Auto-progress through Stages 0→2→3→4→5 without decision menus
  - `"manual"`: Present menu after each stage, wait for user (current behavior)

**`workflow.auto_test`:**
- **Type:** Boolean
- **Default:** `false` (if missing)
- **Effect:**
  - `true`: After Stage 5 completes, auto-invoke plugin-testing skill (Mode 2: Build + Pluginval)
  - `false`: Present final menu, user decides whether to test
- **Trigger point:** After final menu (if user chooses "Test" or after express mode Stage 5)
- **Integration:** plugin-testing skill lines 104-165

**`workflow.auto_install`:**
- **Type:** Boolean
- **Default:** `false` (if missing)
- **Effect:**
  - `true`: After tests pass (or if auto_test=false, after Stage 5), auto-invoke plugin-lifecycle skill (Mode 1: Installation)
  - `false`: Present final menu with "Install" option
- **Trigger point:** After final menu or after auto_test completes
- **Integration:** plugin-lifecycle skill lines 71-133

**`workflow.auto_package`:**
- **Type:** Boolean
- **Default:** `false` (if missing)
- **Effect:**
  - `true`: After installation completes, auto-invoke plugin-packaging skill
  - `false`: Present final menu with "Package" option
- **Prerequisite:** Plugin must be installed first (auto_install=true or manual install)
- **Trigger point:** After installation completes
- **Integration:** plugin-packaging skill lines 22-203

### 3.3 Default Values

**If `.claude/preferences.json` does not exist:**
- Mode: `"manual"`
- Auto-test: `false`
- Auto-install: `false`
- Auto-package: `false`

**Behavior:** System works exactly as it does today (100% backward compatible).

### 3.4 Validation Rules

**File validation:**
1. Check file is valid JSON (parse error → default to manual mode, warn user)
2. Check `workflow` key exists (missing → default to manual mode)
3. Check `workflow.mode` is "express" or "manual" (invalid → default to manual, warn user)
4. Check boolean fields are true/false (invalid → default to false, warn user)

**Error handling:**
- Malformed JSON → Default to manual mode, display: "Warning: preferences.json is invalid JSON, using manual mode"
- Invalid enum value → Default to manual mode, display: "Warning: workflow.mode must be 'express' or 'manual', using manual mode"
- Continue workflow (don't block on config errors)

---

## 4. Workflow Integration Points

### 4.1 /implement Command Changes

**File:** `.claude/commands/implement.md`

**Current behavior (lines 93-103):**
1. Parse plugin name from arguments
2. Verify preconditions (status check, contract verification)
3. Invoke plugin-workflow skill via Skill tool
4. If preconditions fail: Display blocking error and stop

**New behavior:**

**Step 1: Read preferences and parse flags**

```javascript
// Add after line 100 (before skill invocation)

// 1. Parse command-line flags
const args = parseArguments(userInput) // "/implement PluginName --express"
const pluginName = args.pluginName
const flagMode = args.flag // "--express" or "--manual" or null

// 2. Read preferences.json (if exists)
let preferencesMode = "manual" // default
let autoTest = false
let autoInstall = false
let autoPackage = false

if (fileExists(".claude/preferences.json")) {
  try {
    const prefs = JSON.parse(readFile(".claude/preferences.json"))
    if (prefs.workflow && prefs.workflow.mode) {
      preferencesMode = prefs.workflow.mode // "express" or "manual"
    }
    if (prefs.workflow) {
      autoTest = prefs.workflow.auto_test || false
      autoInstall = prefs.workflow.auto_install || false
      autoPackage = prefs.workflow.auto_package || false
    }
  } catch (error) {
    console.warn("Warning: preferences.json is invalid, using manual mode")
    preferencesMode = "manual"
  }
}

// 3. Determine effective mode (flags override preferences)
const effectiveMode = flagMode || preferencesMode // "--express" > preferences > default

console.log(`Workflow mode: ${effectiveMode} (${flagMode ? "from flag" : "from preferences"})`)
```

**Step 2: Pass mode to plugin-workflow**

```javascript
// Modified line 131 (skill invocation)

// OLD:
Skill({ skill: "plugin-workflow" })

// NEW:
const workflowContext = {
  pluginName: pluginName,
  mode: effectiveMode,
  autoTest: autoTest,
  autoInstall: autoInstall,
  autoPackage: autoPackage
}

// Store in environment for skill to read
process.env.WORKFLOW_MODE = effectiveMode
process.env.AUTO_TEST = autoTest.toString()
process.env.AUTO_INSTALL = autoInstall.toString()
process.env.AUTO_PACKAGE = autoPackage.toString()

Skill({ skill: "plugin-workflow" })
```

**Step 3: Add flag parsing logic**

```bash
# Add to implement.md after line 100

## Flag Parsing

Supported flags:
- `--express`: Force express mode (auto-progress)
- `--manual`: Force manual mode (show menus)

Examples:
- `/implement PluginName --express`
- `/implement PluginName --manual`
- `/implement PluginName` (use preferences or default)

Flag precedence: flags > preferences.json > default (manual)
```

### 4.2 Plugin-Workflow Changes

**File:** `.claude/skills/plugin-workflow/SKILL.md`

**Current checkpoint protocol (lines 459-580):**

```javascript
<checkpoint_protocol>
  After EVERY subagent return, execute checkpoint sequence:

  1. verifyStateUpdate()
  2. fallbackStateUpdate()
  3. commitStage()
  4. verifyCheckpoint()
  5. presentDecisionMenu() // BLOCKING
  6. WAIT for user response
</checkpoint_protocol>
```

**New checkpoint protocol:**

```javascript
<checkpoint_protocol>
  After EVERY subagent return, execute checkpoint sequence:

  1. verifyStateUpdate()
  2. fallbackStateUpdate()
  3. commitStage()
  4. verifyCheckpoint()
  5. checkWorkflowMode() // NEW
     IF mode == "express" AND no errors:
       autoProgressToNextStage()
     ELSE:
       presentDecisionMenu() // BLOCKING
       WAIT for user response
</checkpoint_protocol>
```

**Implementation details:**

**Add mode detection (lines 300-310):**

```javascript
// Read mode from environment (set by /implement command)
function getWorkflowMode() {
  const envMode = process.env.WORKFLOW_MODE || "manual"

  // Also check .continue-here.md for resumed workflows
  const handoffPath = `plugins/${pluginName}/.continue-here.md`
  if (fileExists(handoffPath)) {
    const yaml = parseFrontmatter(readFile(handoffPath))
    if (yaml.workflow_mode) {
      return yaml.workflow_mode // Preserved from initial /implement
    }
  }

  return envMode // Default to environment
}

const workflowMode = getWorkflowMode()
```

**Store mode in .continue-here.md (lines 158-260 in state-management.md):**

```yaml
# Add to .continue-here.md YAML frontmatter
---
plugin: PluginName
stage: 3
workflow_mode: express  # NEW FIELD
orchestration_mode: true
auto_test: false        # NEW FIELD
auto_install: true      # NEW FIELD
auto_package: false     # NEW FIELD
---
```

**Modify presentDecisionMenu() (lines 507-512):**

```javascript
// BEFORE (current):
<step order="5" required="true" blocking="true" function="presentDecisionMenu">
  presentDecisionMenu({ stage, completionStatement, pluginName })
  Present numbered decision menu and WAIT for user response
</step>

// AFTER (new):
<step order="5" required="true" function="handleCheckpoint">
  IF workflowMode == "express" AND currentStage < 5 AND noErrors:
    displayProgressMessage(currentStage, nextStage)
    autoProgressToNextStage()
  ELSE:
    presentDecisionMenu({ stage, completionStatement, pluginName })
    WAIT for user response
</step>
```

**Add express mode progress messages:**

```javascript
function displayProgressMessage(currentStage, nextStage) {
  const milestones = {
    0: "Research Complete",
    2: "Build System Ready",
    3: "Audio Engine Working",
    4: "UI Integrated",
    5: "Plugin Complete"
  }

  console.log(`\n✓ ${milestones[currentStage]} → ${milestones[nextStage]}...\n`)
}
```

**Auto-progress logic:**

```javascript
function autoProgressToNextStage() {
  // No menu, no wait, just increment stage and continue loop
  currentStage++

  // Loop continues immediately (no blocking getUserInput())
}
```

### 4.3 Progress Messages

**Express mode format:**

```
✓ Build System Ready → Implementing Audio Engine...

[Stage 3 execution begins immediately]
```

**Manual mode format (unchanged):**

```
✓ Build System Ready

What's next?
1. Continue to Stage 3 (recommended)
2. Test build
3. Pause workflow
4. Review code
5. Other

Choose (1-5): _
```

**Final menu (ALWAYS shown, even in express mode):**

```
✓ Plugin Complete

What's next?
1. Install to system folders (recommended)
2. Run validation tests
3. Create installer package
4. Pause here
5. Other

Choose (1-5): _
```

---

## 5. Auto-Test Integration

### 5.1 When auto_test Triggers

**Trigger point:** After Stage 5 completes AND final menu choice

**Express mode with auto_test=true:**
1. Stage 5 completes
2. Final menu presented (ALWAYS shown)
3. User selects option (manual decision)
4. If user selects "Test" OR "Install" → auto_test runs first
5. plugin-testing skill invoked (Mode 2: Build + Pluginval)

**Manual mode with auto_test=true:**
- Same behavior (auto_test only after final menu)

**Why:** Testing must happen AFTER user sees final menu (no fully automated path).

### 5.2 How Pluginval is Invoked

**File:** `.claude/skills/plugin-testing/SKILL.md` lines 104-165

**Current invocation:**
- User selects Mode 2 from menu
- Skill executes Steps 1-5 (prerequisite check, build, validate, parse, present results)

**New invocation (auto_test=true):**

```javascript
// After final menu, check auto_test preference
const autoTest = process.env.AUTO_TEST === "true"

if (autoTest && userChoice !== "pause") {
  console.log("\n⚡ Auto-testing enabled, running pluginval...\n")

  // Invoke plugin-testing skill directly (Mode 2)
  const testResult = await invokePluginTesting(pluginName, mode: 2)

  // Parse result
  if (testResult.status === "passed") {
    console.log("✓ All tests passed")
    // Continue to auto_install if enabled
  } else {
    console.log("✗ Tests failed - review results")
    // Present failure menu (interrupt express mode)
  }
}
```

### 5.3 What Happens on Validation Failure

**Failure scenarios:**
- Pluginval not installed → Present install menu, WAIT for user
- Build fails → Present error menu, WAIT for user
- Tests fail → Present failure investigation menu, WAIT for user

**Behavior:** Express mode DROPS to manual mode on any error.

**Rationale:** Errors require human decision (retry? investigate? fix?). No sensible auto-recovery.

### 5.4 Output Visibility

**Auto_test output:**
- Full pluginval output displayed (not hidden)
- Pass/fail summary shown
- User sees validation results before install

**Why:** Transparency - user must see what passed/failed.

---

## 6. Auto-Install Integration

### 6.1 When auto_install Triggers

**Trigger sequence:**

1. **Stage 5 completes**
2. **Final menu presented**
3. **User makes selection** (manual decision)
4. **IF auto_test=true:** Run tests first
5. **Tests pass** (or auto_test=false)
6. **IF auto_install=true:** Run installation automatically
7. **Installation completes**
8. **IF auto_package=true:** Run packaging automatically

**Key point:** Auto-install ONLY runs after tests pass (or if auto_test=false).

### 6.2 How Installation is Invoked

**File:** `.claude/skills/plugin-lifecycle/SKILL.md` lines 71-133

**Current invocation:**
- User runs `/install-plugin PluginName`
- Command expands → Invokes plugin-lifecycle skill (Mode 1)
- Skill executes 8-step installation process

**New invocation (auto_install=true):**

```javascript
// After auto_test completes (or if auto_test=false)
const autoInstall = process.env.AUTO_INSTALL === "true"

if (autoInstall && testsPassedOrSkipped) {
  console.log("\n⚡ Auto-install enabled, deploying to system folders...\n")

  // Invoke plugin-lifecycle skill directly (Mode 1: Installation)
  const installResult = await invokePluginLifecycle(pluginName, mode: 1)

  // Parse result
  if (installResult.status === "success") {
    console.log("✓ Plugin installed successfully")
    updatePluginStatus(pluginName, "📦 Installed")
    // Continue to auto_package if enabled
  } else {
    console.log("✗ Installation failed - review errors")
    // Present failure menu (interrupt express mode)
  }
}
```

### 6.3 What Happens on Install Failure

**Failure scenarios:**
- Release binaries missing → Offer to build, WAIT for user
- System permissions error → Display fix instructions, WAIT
- Cache clearing fails → Warn user, continue anyway

**Behavior:**
- Critical failures → Drop to manual mode, present error menu
- Non-critical warnings → Display warning, continue

**Critical vs non-critical:**
- Critical: Binaries missing, permission denied, copy failed
- Non-critical: Cache clear failed (plugin still installed)

### 6.4 Output Visibility

**Auto-install output:**
- Installation steps shown (copy to VST3, copy to AU, set permissions)
- Success/failure for each step
- Final verification (file sizes, timestamps)

**Why:** User must see what was installed and where.

---

## 7. Auto-Package Integration

### 7.1 When auto_package Triggers

**Trigger sequence:**

1. **Auto-install completes successfully**
2. **Plugin status = 📦 Installed**
3. **IF auto_package=true:** Run packaging automatically

**Prerequisite check:**
```javascript
if (autoPackage && pluginStatus === "📦 Installed") {
  // Proceed with packaging
} else if (autoPackage && pluginStatus !== "📦 Installed") {
  console.warn("Warning: auto_package requires plugin to be installed first")
  // Skip packaging
}
```

### 7.2 How PKG Creation is Invoked

**File:** `.claude/skills/plugin-packaging/SKILL.md` lines 22-203

**Current invocation:**
- User runs `/package PluginName`
- Command expands → Invokes plugin-packaging skill
- Skill executes 6-step packaging process

**New invocation (auto_package=true):**

```javascript
// After auto_install completes
const autoPackage = process.env.AUTO_PACKAGE === "true"

if (autoPackage && pluginStatus === "📦 Installed") {
  console.log("\n⚡ Auto-package enabled, creating installer...\n")

  // Invoke plugin-packaging skill directly
  const packageResult = await invokePluginPackaging(pluginName)

  // Parse result
  if (packageResult.status === "success") {
    console.log(`✓ Installer created: plugins/${pluginName}/dist/${pluginName}-by-TACHES.pkg`)
    // Workflow complete
  } else {
    console.log("✗ Packaging failed - review errors")
    // Present failure menu
  }
}
```

### 7.3 Prerequisites and Dependencies

**Prerequisites:**
1. Plugin must be installed (status = 📦 Installed)
2. VST3 binary exists in `~/Library/Audio/Plug-Ins/VST3/`
3. AU binary exists in `~/Library/Audio/Plug-Ins/Components/`

**Dependency chain:**
```
Stage 5 → (auto_test) → Tests pass → (auto_install) → Installation → (auto_package) → PKG created
```

**If any step fails:** Chain stops, drop to manual mode.

### 7.4 Output Visibility

**Auto-package output:**
- PKG creation steps (metadata extraction, branding, pkgbuild, productbuild)
- File size of created PKG
- Location: `plugins/[PluginName]/dist/[PluginName]-by-TACHES.pkg`

**Final summary:**
```
✓ Workflow complete

Created:
- Plugin installed to ~/Library/Audio/Plug-Ins/
- Installer package: plugins/PluginName/dist/PluginName-by-TACHES.pkg (4.2 MB)

Ready to share!
```

---

## 8. Safety Mechanisms

### 8.1 Error Interruption

**Interruption triggers:**

1. **Build failure:**
   - CMake configuration error
   - Compilation error
   - Linker error
   - **Action:** Drop to manual mode, present error menu

2. **Validation failure:**
   - Pluginval errors
   - Test crashes
   - **Action:** Drop to manual mode, present investigation menu

3. **Installation failure:**
   - Binaries missing
   - Permission denied
   - Copy failed
   - **Action:** Drop to manual mode, present fix menu

4. **Packaging failure:**
   - Plugin not installed
   - pkgbuild error
   - **Action:** Drop to manual mode, present error details

**Error detection:**
```javascript
function checkForErrors(result) {
  if (result.status === "error" || result.status === "failure") {
    console.log("✗ Error detected - dropping to manual mode")
    workflowMode = "manual" // Override mode
    return true
  }
  return false
}
```

**Error menu example:**
```
✗ Build failed at Stage 3

Errors:
- PluginProcessor.cpp:124: 'processBlock' is not a member of 'juce::AudioProcessor'

What should I do?
1. Investigate error (deep-research)
2. Show full build log
3. Review code
4. Manual fix (pause workflow)
5. Other

Choose (1-5): _
```

### 8.2 User Interruption

**Ctrl+C behavior:**
- User presses Ctrl+C during stage execution
- Subagent stops immediately
- State saved to .continue-here.md (last completed stage)
- Resume with `/continue PluginName`

**Resume mode:**
```javascript
// When resuming, check workflow_mode in .continue-here.md
const handoff = readHandoffFile(pluginName)
const resumeMode = handoff.workflow_mode || "manual"

console.log(`Resuming in ${resumeMode} mode (from checkpoint)`)
```

**User can override mode on resume:**
```bash
/continue PluginName --manual   # Force manual mode
/continue PluginName --express  # Force express mode
/continue PluginName            # Use mode from checkpoint
```

---

## 9. File Modifications Required

### 9.1 New Files

**`.claude/preferences.json`** - [CREATE]
```json
{
  "workflow": {
    "mode": "manual",
    "auto_test": false,
    "auto_install": false,
    "auto_package": false
  }
}
```
- **Purpose:** Default preferences template
- **Location:** Project root (`.claude/preferences.json`)
- **Gitignore:** NO (commit as example, users can modify)

### 9.2 Modified Files

**`.claude/commands/implement.md`** - [UPDATE]
- **Line 100:** Add flag parsing logic (`--express`, `--manual`)
- **Line 100:** Add preferences.json reading
- **Line 103:** Pass mode to plugin-workflow via environment variables
- **Lines 93-99:** Add flag documentation

**`.claude/skills/plugin-workflow/SKILL.md`** - [UPDATE]
- **Lines 300-310:** Add `getWorkflowMode()` function
- **Lines 507-512:** Replace `presentDecisionMenu()` with mode-aware `handleCheckpoint()`
- **Lines 580-595:** Add `autoProgressToNextStage()` function
- **Lines 596-605:** Add `displayProgressMessage()` function

**`.claude/skills/plugin-workflow/references/state-management.md`** - [UPDATE]
- **Lines 230-260:** Add `workflow_mode` field to .continue-here.md YAML spec
- **Lines 158-260:** Update `updateHandoff()` to store mode

**`.claude/skills/plugin-lifecycle/SKILL.md`** - [UPDATE]
- **Lines 71-133:** Add auto_install invocation logic
- **Lines 134-145:** Add auto_install error handling

**`.claude/skills/plugin-packaging/SKILL.md`** - [UPDATE]
- **Lines 22-35:** Add auto_package invocation logic
- **Lines 260-275:** Add auto_package prerequisite checks

**`.claude/skills/plugin-testing/SKILL.md`** - [UPDATE]
- **Lines 38-48:** Add auto_test invocation logic
- **Lines 160-165:** Add auto_test result handling

**`.claude/commands/continue.md`** - [UPDATE]
- **Lines 42-46:** Add mode preservation from .continue-here.md
- **Lines 94-96:** Add flag override on resume

---

## 10. Command-Line Flag Design

### 10.1 Flag Parsing

**Supported flags:**
- `--express`: Force express mode for this run
- `--manual`: Force manual mode for this run

**Examples:**
```bash
/implement PluginName --express
/implement PluginName --manual
/implement PluginName            # Use preferences or default
/continue PluginName --express
/continue PluginName --manual
```

**Parsing implementation:**
```javascript
function parseArguments(input) {
  // Input: "/implement PluginName --express"
  const parts = input.trim().split(/\s+/)

  const pluginName = parts[1] // "PluginName"
  const flag = parts[2] || null // "--express" or null

  let flagMode = null
  if (flag === "--express") {
    flagMode = "express"
  } else if (flag === "--manual") {
    flagMode = "manual"
  } else if (flag) {
    console.warn(`Warning: Unknown flag '${flag}', ignoring`)
  }

  return { pluginName, flag: flagMode }
}
```

### 10.2 Precedence

**Order of precedence (highest to lowest):**
1. Command-line flag (`--express` or `--manual`)
2. preferences.json (`workflow.mode`)
3. Default (`"manual"`)

**Example scenarios:**

**Scenario 1:**
- Flag: `--express`
- Preferences: `"manual"`
- **Effective mode:** `"express"` (flag wins)

**Scenario 2:**
- Flag: none
- Preferences: `"express"`
- **Effective mode:** `"express"` (preferences apply)

**Scenario 3:**
- Flag: none
- Preferences: file missing
- **Effective mode:** `"manual"` (default)

**Scenario 4:**
- Flag: `--manual`
- Preferences: `"express"`
- **Effective mode:** `"manual"` (flag overrides)

### 10.3 Temporary vs Persistent Settings

**Flags are temporary:**
- Only affect current `/implement` or `/continue` invocation
- Do NOT modify `.claude/preferences.json`
- Do NOT persist to `.continue-here.md` (use preferences mode instead)

**Preferences are persistent:**
- Apply to ALL workflows until file is modified
- Stored in `.continue-here.md` for resume scenarios
- Committed to git (shared across sessions)

**Why flags don't persist:**
- One-off overrides (testing, debugging, special cases)
- User might want express mode usually, manual mode once
- Avoids accidental permanent changes

---

## 11. Edge Cases & Error Handling

### Edge Case 1: User Creates preferences.json Mid-Workflow

**Scenario:**
- User runs `/implement PluginName` (manual mode, no preferences)
- Stage 2 completes, menu shown
- User pauses, creates `.claude/preferences.json` with `mode: "express"`
- User runs `/continue PluginName`

**Expected behavior:**
- Resume reads `workflow_mode` from `.continue-here.md`
- `.continue-here.md` says `workflow_mode: manual` (set at start)
- Resume continues in manual mode (preserves original mode)

**Why:** Consistency - don't change mode mid-workflow.

**Override:** User can force mode change with `/continue PluginName --express`

---

### Edge Case 2: User Deletes preferences.json Mid-Workflow

**Scenario:**
- User runs `/implement PluginName --express` (flag forces express)
- Stage 2 completes, auto-progresses
- User deletes `.claude/preferences.json`
- Stage 3 completes

**Expected behavior:**
- Mode stored in `.continue-here.md` (workflow_mode: express)
- Workflow continues in express mode (unaffected by file deletion)

**Why:** Mode is snapshot at workflow start, not dynamic.

---

### Edge Case 3: User Runs /continue with Different Mode

**Scenario:**
- Original workflow: `/implement PluginName` (manual mode, paused at Stage 3)
- Resume: `/continue PluginName --express`

**Expected behavior:**
- Flag overrides stored mode
- Stage 3 completes, auto-progresses to Stage 4 (express mode)
- `.continue-here.md` updated: `workflow_mode: express`

**Why:** Flag allows mode switching on resume.

---

### Edge Case 4: preferences.json Has Invalid Mode Value

**Scenario:**
- `.claude/preferences.json` contains:
  ```json
  {
    "workflow": {
      "mode": "turbo"
    }
  }
  ```

**Expected behavior:**
- JSON parses successfully (valid JSON)
- Mode validation fails (`"turbo"` not in enum ["express", "manual"])
- Default to `"manual"` mode
- Display warning: "Warning: workflow.mode must be 'express' or 'manual', using manual mode"
- Continue workflow

**Implementation:**
```javascript
const validModes = ["express", "manual"]
if (!validModes.includes(preferencesMode)) {
  console.warn(`Warning: workflow.mode must be 'express' or 'manual', using manual mode`)
  preferencesMode = "manual"
}
```

---

### Edge Case 5: Mode Conflicts (Preferences vs Flag)

**Scenario:**
- Preferences: `"express"`
- Flag: `--manual`

**Expected behavior:**
- Flag wins (precedence: flag > preferences > default)
- Effective mode: `"manual"`
- Display: `Workflow mode: manual (from flag)`

**No conflict:** This is expected behavior, not an error.

---

### Edge Case 6: Build Error During Express Mode

**Scenario:**
- Mode: express
- Stage 2 completes successfully
- Stage 3 builds, compilation error occurs

**Expected behavior:**
1. Build error detected
2. Drop to manual mode: `workflowMode = "manual"`
3. Present error menu:
   ```
   ✗ Build failed at Stage 3

   Errors:
   - PluginProcessor.cpp:45: 'processBlock' undefined

   What should I do?
   1. Investigate error
   2. Show build log
   3. Review code
   4. Manual fix (pause)
   5. Other
   ```
4. WAIT for user selection (blocking)
5. User investigates, fixes error
6. User runs `/continue PluginName`
7. Resume in manual mode (error flag persists)

**Why drop to manual:**
- Errors require human decisions
- No sensible auto-recovery
- Safety first

---

### Edge Case 7: Pluginval Fails During auto_test

**Scenario:**
- Mode: express
- auto_test: true
- Stage 5 completes
- Final menu shown, user selects "Install"
- auto_test runs, pluginval fails with 3 errors

**Expected behavior:**
1. Pluginval failure detected
2. Drop to manual mode
3. Present investigation menu:
   ```
   ✗ Pluginval failed (3 errors)

   Errors:
   - [ERROR] State save/load failed
   - [ERROR] Parameter out of range
   - [WARNING] High latency detected

   What should I do?
   1. Investigate failures (deep-research)
   2. Show full validation output
   3. Fix and retry
   4. Continue anyway (skip validation)
   5. Other
   ```
4. WAIT for user selection

**Auto-install blocked:** Installation will NOT run automatically if tests fail.

---

### Edge Case 8: User Interrupts with Ctrl+C During Express Mode

**Scenario:**
- Mode: express
- Stage 3 executing (DSP implementation)
- User presses Ctrl+C

**Expected behavior:**
1. Subagent stops immediately
2. State saved to `.continue-here.md`:
   ```yaml
   stage: 2  # Last COMPLETED stage
   workflow_mode: express
   status: paused
   ```
3. Display: "⏸ Workflow paused at Stage 2. Resume with /continue PluginName"
4. User runs `/continue PluginName`
5. Resume at Stage 3 in express mode (preserves mode)

**Safety:** Stage 3 NOT marked complete (only completed stages saved).

---

### Edge Case 9: Phased Implementation (Complexity ≥3)

**Scenario:**
- Mode: express
- Complexity: 5.0 (phased implementation)
- Stage 3 has 3 phases (3.1, 3.2, 3.3)

**Expected behavior:**
- Phase 3.1 completes → Auto-progress to Phase 3.2 (no menu)
- Phase 3.2 completes → Auto-progress to Phase 3.3 (no menu)
- Phase 3.3 completes → Auto-progress to Stage 4 (no menu)

**No phase menus in express mode:** Phases treated same as stages.

---

### Edge Case 10: Final Menu in Express Mode

**Scenario:**
- Mode: express
- Stage 5 completes

**Expected behavior:**
- Final menu ALWAYS shown (even in express mode)
- User MUST make decision:
  ```
  ✓ Plugin Complete

  What's next?
  1. Install to system folders
  2. Run validation tests
  3. Create installer package
  4. Pause here
  5. Other
  ```
- WAIT for user selection (blocking)

**Why final menu is special:**
- No obvious "next stage" (workflow complete)
- User must decide: install? package? test? done?
- Auto-actions happen AFTER menu (auto_install, auto_package)

---

## 12. Backward Compatibility

### 12.1 No preferences.json

**Behavior:**
- System defaults to `mode: "manual"`
- All checkpoints present decision menus
- User must select "Continue to next stage" (current behavior)
- 100% identical to current system

**No breaking changes:** Existing workflows unaffected.

---

### 12.2 Existing Workflows

**Scenario:** User has in-progress workflows (plugins at Stage 2, 3, 4)

**Resume behavior:**
- `/continue PluginName` reads `.continue-here.md`
- If `workflow_mode` field missing → Default to `"manual"`
- Resume in manual mode (safe default)

**Why manual default:** Better to be conservative (show menus) than aggressive (auto-progress).

---

### 12.3 No Breaking Changes

**API compatibility:**
- `/implement PluginName` still works (no flags required)
- `/continue PluginName` still works
- plugin-workflow skill still works
- All existing commands unchanged

**State file compatibility:**
- Old `.continue-here.md` files missing `workflow_mode` → Default to manual
- New `.continue-here.md` files have `workflow_mode` → Use stored mode
- No migration needed

---

### 12.4 Migration Path

**None required.**

**For users who want express mode:**
1. Create `.claude/preferences.json` (copy template)
2. Set `mode: "express"`
3. Run `/implement PluginName` (express mode enabled)

**For users who don't:**
- Do nothing
- System works exactly as before

---

## 13. Testing Strategy

### 13.1 Test Scenarios

**Scenario 1: Manual Mode (No preferences.json)**
- **Setup:** No `.claude/preferences.json` file exists
- **Action:** `/implement TestPlugin`
- **Expected:**
  - Stage 0 completes → Menu shown → User selects "Continue"
  - Stage 2 completes → Menu shown → User selects "Continue"
  - Stage 3 completes → Menu shown → User selects "Continue"
  - Stage 4 completes → Menu shown → User selects "Continue"
  - Stage 5 completes → Final menu shown
- **Verify:** All menus presented, workflow requires 5 manual decisions

---

**Scenario 2: Express Mode (preferences.json mode=express)**
- **Setup:** `.claude/preferences.json` with `mode: "express"`
- **Action:** `/implement TestPlugin`
- **Expected:**
  - Stage 0 completes → Auto-progress to Stage 2 (no menu)
  - Stage 2 completes → Auto-progress to Stage 3 (no menu)
  - Stage 3 completes → Auto-progress to Stage 4 (no menu)
  - Stage 4 completes → Auto-progress to Stage 5 (no menu)
  - Stage 5 completes → Final menu shown (BLOCKING)
- **Verify:** Zero intermediate menus, final menu appears, 3-5 min time savings

---

**Scenario 3: Override with --manual Flag**
- **Setup:** `.claude/preferences.json` with `mode: "express"`
- **Action:** `/implement TestPlugin --manual`
- **Expected:**
  - Flag overrides preferences
  - Stage 0 completes → Menu shown (manual mode)
  - Stage 2 completes → Menu shown
- **Verify:** Flag precedence works, manual mode enforced

---

**Scenario 4: Override with --express Flag**
- **Setup:** No `.claude/preferences.json` (defaults to manual)
- **Action:** `/implement TestPlugin --express`
- **Expected:**
  - Flag overrides default
  - Stage 0 completes → Auto-progress (express mode)
  - Stage 2 completes → Auto-progress
- **Verify:** Flag precedence works, express mode enforced

---

**Scenario 5: Build Error Interrupts Express Mode**
- **Setup:** `.claude/preferences.json` with `mode: "express"`
- **Action:** `/implement TestPlugin`
- **Inject error:** Modify PluginProcessor.cpp to cause compilation error before Stage 3
- **Expected:**
  - Stage 2 completes → Auto-progress to Stage 3
  - Stage 3 build fails → Error detected
  - Drop to manual mode
  - Error menu presented: "What should I do? 1) Investigate 2) Show log..."
  - WAIT for user selection (blocking)
- **Verify:** Error interruption works, express mode drops to manual

---

**Scenario 6: Resume Workflow in Express Mode**
- **Setup:** Start workflow in express mode, pause at Stage 3
- **Action:** `/continue TestPlugin`
- **Expected:**
  - Read `.continue-here.md` → `workflow_mode: express`
  - Resume at Stage 3
  - Stage 3 completes → Auto-progress to Stage 4 (no menu)
- **Verify:** Mode preserved across pause/resume

---

### 13.2 Validation Points

**Mode Propagation:**
- ✅ Flag parsed correctly from `/implement PluginName --express`
- ✅ Preferences read from `.claude/preferences.json`
- ✅ Effective mode determined (flag > preferences > default)
- ✅ Mode stored in `.continue-here.md` (`workflow_mode` field)
- ✅ Mode retrieved on resume

**Checkpoint Behavior:**
- ✅ Express mode skips intermediate menus (Stages 0→2→3→4)
- ✅ Manual mode shows all menus (current behavior)
- ✅ Final menu ALWAYS shown (Stage 5 complete)
- ✅ Phase checkpoints skipped in express mode (complex plugins)

**Error Handling:**
- ✅ Build errors drop to manual mode
- ✅ Test errors drop to manual mode
- ✅ Install errors drop to manual mode
- ✅ Error menus presented after interruption

**Auto-Actions:**
- ✅ auto_test runs after final menu (if enabled)
- ✅ auto_install runs after tests pass (if enabled)
- ✅ auto_package runs after install (if enabled)
- ✅ Failures interrupt auto-actions

**Backward Compatibility:**
- ✅ No preferences.json → Manual mode (default)
- ✅ Old `.continue-here.md` missing `workflow_mode` → Manual mode
- ✅ Existing commands still work (`/implement`, `/continue`)

---

## 14. Implementation Checklist

Ordered steps for implementation (execute sequentially):

- [ ] **Step 1:** Create `.claude/preferences.json` template
  - File: `.claude/preferences.json`
  - Content: Default settings (mode: manual, all auto flags false)
  - Commit to git as example

- [ ] **Step 2:** Update /implement command for flag parsing
  - File: `.claude/commands/implement.md`
  - Add: Flag parsing logic (lines 100-110)
  - Add: preferences.json reading (lines 111-125)
  - Add: Mode determination (lines 126-130)
  - Add: Environment variable export (lines 131-135)

- [ ] **Step 3:** Update plugin-workflow for mode detection
  - File: `.claude/skills/plugin-workflow/SKILL.md`
  - Add: `getWorkflowMode()` function (lines 300-315)
  - Add: Mode storage in .continue-here.md (reference state-management.md)

- [ ] **Step 4:** Update checkpoint protocol for mode-awareness
  - File: `.claude/skills/plugin-workflow/SKILL.md`
  - Modify: `presentDecisionMenu()` → `handleCheckpoint()` (lines 507-530)
  - Add: `autoProgressToNextStage()` function (lines 580-590)
  - Add: `displayProgressMessage()` function (lines 591-605)

- [ ] **Step 5:** Update state-management for workflow_mode field
  - File: `.claude/skills/plugin-workflow/references/state-management.md`
  - Add: `workflow_mode` field to YAML spec (lines 240-245)
  - Add: Auto-action flags (auto_test, auto_install, auto_package)
  - Update: `updateHandoff()` to store mode

- [ ] **Step 6:** Update /continue command for mode preservation
  - File: `.claude/commands/continue.md`
  - Add: Mode reading from .continue-here.md (lines 42-50)
  - Add: Flag override on resume (lines 94-100)

- [ ] **Step 7:** Integrate auto_test with plugin-testing
  - File: `.claude/skills/plugin-testing/SKILL.md`
  - Add: Auto-invocation after final menu (lines 38-55)
  - Add: Error handling for failures (lines 160-175)

- [ ] **Step 8:** Integrate auto_install with plugin-lifecycle
  - File: `.claude/skills/plugin-lifecycle/SKILL.md`
  - Add: Auto-invocation after tests (lines 71-85)
  - Add: Prerequisite check (tests passed or skipped)

- [ ] **Step 9:** Integrate auto_package with plugin-packaging
  - File: `.claude/skills/plugin-packaging/SKILL.md`
  - Add: Auto-invocation after install (lines 22-40)
  - Add: Prerequisite check (plugin installed)

- [ ] **Step 10:** Test all scenarios
  - Run Scenario 1: Manual mode (no preferences)
  - Run Scenario 2: Express mode (preferences)
  - Run Scenario 3: Flag override (--manual)
  - Run Scenario 4: Flag override (--express)
  - Run Scenario 5: Build error interruption
  - Run Scenario 6: Resume in express mode

- [ ] **Step 11:** Document user-facing features
  - Update: Main CLAUDE.md with Express Mode usage
  - Create: Example preferences.json with comments
  - Add: Troubleshooting for common issues

- [ ] **Step 12:** Commit changes
  - Commit all modified files
  - Message: "feat: add Express Mode for automated workflows"

---

## 15. Success Criteria

### 15.1 Functional Requirements

✅ **Express mode skips intermediate checkpoints:**
- Stage 0 → Stage 2 (no menu)
- Stage 2 → Stage 3 (no menu)
- Stage 3 → Stage 4 (no menu)
- Stage 4 → Stage 5 (no menu)
- Final menu ALWAYS shown after Stage 5

✅ **Manual mode unchanged:**
- All menus presented (current behavior)
- User selects "Continue" manually
- Backward compatible

✅ **Flags override preferences correctly:**
- `--express` forces express mode
- `--manual` forces manual mode
- Precedence: flag > preferences > default

✅ **Build errors interrupt express mode:**
- Error detected → Drop to manual mode
- Error menu presented
- WAIT for user decision

✅ **Auto-test runs when enabled:**
- After final menu selection
- Tests pass → Continue to auto-install
- Tests fail → Present investigation menu

✅ **Auto-install works when enabled:**
- After tests pass (or auto_test=false)
- Install succeeds → Update status to 📦 Installed
- Install fails → Present error menu

✅ **Auto-package works when enabled:**
- After install succeeds
- PKG created in `plugins/[Name]/dist/`
- Packaging fails → Present error details

✅ **Final menu always appears:**
- Even in express mode
- User must make decision
- Auto-actions run AFTER menu

✅ **Time savings measured:**
- Manual mode: 3-5 minutes per plugin (5 decision points)
- Express mode: 0 minutes (0 decision points)
- Savings: 3-5 minutes confirmed

### 15.2 Non-Functional Requirements

✅ **Backward compatibility:**
- No preferences.json → Defaults to manual mode
- Existing workflows unaffected
- No breaking changes

✅ **Error handling:**
- Invalid JSON → Default to manual, warn user
- Invalid mode value → Default to manual, warn user
- Missing file → Default to manual (silent)

✅ **Mode persistence:**
- Mode stored in `.continue-here.md`
- Resume preserves mode
- User can override with flags

✅ **Safety:**
- Errors interrupt express mode
- No auto-retry on failure
- User always makes critical decisions

### 15.3 Testing Checklist

✅ **All scenarios pass:**
- Scenario 1: Manual mode works
- Scenario 2: Express mode works
- Scenario 3: --manual flag overrides
- Scenario 4: --express flag overrides
- Scenario 5: Build error interrupts
- Scenario 6: Resume preserves mode

✅ **Edge cases handled:**
- preferences.json created mid-workflow
- preferences.json deleted mid-workflow
- Invalid mode value
- Mode conflicts
- Ctrl+C interruption
- Phased implementation

✅ **Integration verified:**
- auto_test integration works
- auto_install integration works
- auto_package integration works
- Dependency chain works (test → install → package)

---

**END OF SPECIFICATION**
