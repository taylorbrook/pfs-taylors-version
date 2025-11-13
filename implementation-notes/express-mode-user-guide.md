# Express Mode User Guide

**Feature:** Workflow automation for plugin implementation
**Time Savings:** 3-5 minutes per plugin (eliminates 5 manual decision gates)
**Status:** Production-ready (Phase 7 complete)

---

## Quick Start

### Enable Express Mode

1. **Edit `.claude/preferences.json`:**
   ```json
   {
     "workflow": {
       "mode": "express"
     }
   }
   ```

2. **Run workflow:**
   ```
   /implement MyPlugin
   ```

3. **Observe auto-progression:**
   ```
   ✓ Build System Ready → Implementing Audio Engine...
   ✓ Audio Engine Working → Adding GUI...
   ✓ UI Integrated → Running Validation...
   ✓ Plugin Complete

   What's next?
   1. Install to system folders
   2. Run validation tests
   3. Create installer package
   4. Pause here
   5. Other
   ```

---

## What Is Express Mode?

Express Mode eliminates manual decision gates during plugin implementation. Instead of stopping at each stage and waiting for you to click "Continue," the system automatically progresses through all stages.

### Manual Mode (Default)

**Workflow with manual mode:**
1. Stage 2 completes
2. System shows menu: "What's next? 1) Continue to Stage 3..."
3. You select option 1
4. Stage 3 begins
5. Stage 3 completes
6. System shows menu again
7. You select option 1 again
8. Stage 4 begins
9. (Repeat for Stages 4 and 5)

**Total decisions:** 5 manual selections (Stages 0→2, 2→3, 3→4, 4→5, final menu)
**Time cost:** 3-5 minutes per plugin

### Express Mode

**Workflow with express mode:**
1. Stage 2 completes
2. System displays: "✓ Build System Ready → Implementing Audio Engine..."
3. Stage 3 begins immediately (no menu, no wait)
4. Stage 3 completes
5. System displays: "✓ Audio Engine Working → Adding GUI..."
6. Stage 4 begins immediately
7. (Continues to Stage 5)
8. Final menu appears (install/test/package decisions)

**Total decisions:** 1 (final menu only)
**Time savings:** 3-5 minutes

---

## Configuration

### Preferences File

**Location:** `.claude/preferences.json` (project root)

**Full configuration:**
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

### Settings

#### `workflow.mode`

**Values:** "express" | "manual"
**Default:** "manual"

Controls whether decision menus appear at checkpoints:
- **"manual"**: Show all menus (current behavior, 5 decisions per plugin)
- **"express"**: Auto-progress through stages (0 intermediate decisions)

**When to use manual:**
- Learning the system
- Troubleshooting issues
- Complex plugins requiring careful review
- Want control over each stage

**When to use express:**
- Building simple/familiar plugins
- Comfortable with the workflow
- Want speed over control
- Production workflows

#### `workflow.auto_test`

**Values:** true | false
**Default:** false

Run automated validation tests (pluginval) after Stage 5.

**Effect:**
- `true`: After final menu, automatically run pluginval validation
- `false`: Present "Run validation tests" option in menu

**Notes:**
- Tests run AFTER final menu (you still see the menu)
- If tests fail, workflow drops to manual mode
- Test output is always shown (transparency)

#### `workflow.auto_install`

**Values:** true | false
**Default:** false

Install plugin to system folders after tests pass.

**Effect:**
- `true`: After tests pass, automatically install to ~/Library/Audio/Plug-Ins/
- `false`: Present "Install to system folders" option in menu

**Prerequisite:** Tests must pass first (if auto_test=true)

#### `workflow.auto_package`

**Values:** true | false
**Default:** false

Create PKG installer after installation.

**Effect:**
- `true`: After installation, automatically create branded PKG installer
- `false`: Present "Create installer package" option in menu

**Prerequisite:** Plugin must be installed first

**Output:** `plugins/[PluginName]/dist/[PluginName]-by-TACHES.pkg`

---

## Command-Line Flags

Override preferences for a single run without editing the config file.

### `/implement [PluginName] --express`

Force express mode for this workflow only.

**Example:**
```
/implement GainPlugin --express
```

**Use case:** You have preferences.json set to manual, but want express mode for this one plugin.

### `/implement [PluginName] --manual`

Force manual mode for this workflow only.

**Example:**
```
/implement ComplexPlugin --manual
```

**Use case:** You have preferences.json set to express, but want manual control for a complex plugin.

### `/continue [PluginName] --express`

Resume workflow in express mode.

**Example:**
```
/continue MyPlugin --express
```

**Use case:** You paused a workflow in manual mode, but want to finish in express mode.

### `/continue [PluginName] --manual`

Resume workflow in manual mode.

**Example:**
```
/continue MyPlugin --manual
```

**Use case:** You started in express mode, but want manual control for the rest.

---

## Precedence Rules

**Order of precedence (highest to lowest):**
1. Command-line flag (--express or --manual)
2. preferences.json (workflow.mode)
3. Default ("manual")

**Examples:**

| Flag | preferences.json | Effective Mode | Why |
|------|-----------------|----------------|-----|
| `--express` | `"manual"` | `"express"` | Flag overrides preferences |
| `--manual` | `"express"` | `"manual"` | Flag overrides preferences |
| (none) | `"express"` | `"express"` | Preferences apply |
| (none) | (missing) | `"manual"` | Default to safe mode |

---

## Safety Features

### Error Interruption

If ANY error occurs during express mode, the system automatically drops to manual mode:

**Errors that interrupt:**
- Build failures (CMake, compilation, linking)
- Test failures (pluginval errors)
- Installation failures (permissions, missing binaries)
- Packaging failures (prerequisites not met)

**Example:**
```
✓ Build System Ready → Implementing Audio Engine...

[Stage 3 DSP implementation...]

✗ Build failed at Stage 3

Errors:
- PluginProcessor.cpp:124: 'processBlock' is not a member of 'juce::AudioProcessor'

✗ Error detected - dropping to manual mode

What should I do?
1. Investigate error (deep-research)
2. Show full build log
3. Review code
4. Manual fix (pause workflow)
5. Other

Choose (1-5): _
```

**After interruption:**
- You investigate and fix the error
- Resume with `/continue PluginName`
- Workflow continues in manual mode (safety first)
- Use `/continue PluginName --express` to re-enable express mode

### Final Menu Always Appears

Even in express mode, the final menu ALWAYS appears after Stage 5:

**Why:**
- No obvious "next stage" after completion
- User must decide: install? test? package? done?
- Auto-actions (auto_test, auto_install, auto_package) run AFTER menu

**This ensures:**
- You always have control over deployment
- No silent auto-installations
- Transparency in all actions

---

## Use Cases

### Scenario 1: Learning the System

**Goal:** Understand what happens at each stage

**Configuration:**
```json
{
  "workflow": {
    "mode": "manual"
  }
}
```

**Benefits:**
- See decision menu after each stage
- Review code at your own pace
- Test builds incrementally
- Full control over progression

---

### Scenario 2: Building Simple Plugins Quickly

**Goal:** Create gain plugins, filters, basic effects without friction

**Configuration:**
```json
{
  "workflow": {
    "mode": "express"
  }
}
```

**Benefits:**
- No decision gates (0 vs 5 manual selections)
- 3-5 minutes saved per plugin
- Final menu still appears (control over install/test)
- Can interrupt with Ctrl+C anytime

---

### Scenario 3: Full Automation Pipeline

**Goal:** Build, test, install, and package without manual steps

**Configuration:**
```json
{
  "workflow": {
    "mode": "express",
    "auto_test": true,
    "auto_install": true,
    "auto_package": true
  }
}
```

**Workflow:**
1. Stages auto-flow (0 → 2 → 3 → 4 → 5)
2. Final menu appears
3. After menu selection, tests run automatically
4. If tests pass, installation happens automatically
5. After install, PKG created automatically
6. Complete workflow with minimal manual intervention

**Use cases:**
- Building multiple similar plugins
- Production workflows
- CI/CD scenarios
- Batch plugin creation

---

### Scenario 4: Testing a Specific Complex Plugin

**Goal:** Override express mode for one plugin that needs careful review

**Command:**
```
/implement ComplexSynthesizer --manual
```

**Effect:**
- Uses manual mode despite preferences.json having `mode: "express"`
- Allows careful inspection of DSP and GUI code
- Preferences file unchanged (one-off override)
- Next plugin uses express mode again

---

### Scenario 5: Resuming with Different Mode

**Scenario:**
1. Start workflow: `/implement MyPlugin` (manual mode)
2. Complete Stage 2, pause at Stage 3
3. Resume: `/continue MyPlugin --express`

**Result:**
- Stage 3 continues in express mode (flag overrides saved mode)
- Stages 3 → 4 → 5 auto-progress
- `.continue-here.md` updated to `workflow_mode: express`

---

## Mode Persistence

### How Mode is Saved

When you start a workflow, the mode is stored in `.continue-here.md`:

```yaml
---
plugin: MyPlugin
stage: 3
workflow_mode: express
auto_test: false
auto_install: true
auto_package: false
---
```

### Resume Behavior

When you resume with `/continue MyPlugin`:

1. System reads `.continue-here.md`
2. Extracts `workflow_mode` field
3. Uses saved mode for remainder of workflow

**Override on resume:**
```
/continue MyPlugin --manual
```
Switches to manual mode, updates `.continue-here.md`

---

## Troubleshooting

### Problem: Mode Not Taking Effect

**Symptoms:**
- Changed preferences.json to "express"
- Still seeing decision menus

**Possible causes:**
1. **Flag override:** Are you using `--manual` flag?
   - **Solution:** Remove flag or use `--express` flag
2. **Invalid JSON:** Is preferences.json valid JSON?
   - **Solution:** Validate JSON syntax (use `jq . .claude/preferences.json`)
3. **Invalid mode value:** Did you use "turbo" instead of "express"?
   - **Solution:** Use exact values: "express" or "manual"
4. **File location:** Is preferences.json in the right place?
   - **Solution:** Must be at `.claude/preferences.json` (project root)

---

### Problem: Want Express Mode for One Plugin Only

**Symptoms:**
- Have preferences.json set to "manual"
- Don't want to change config for all plugins
- Need express mode for just one plugin

**Solution:**
Use flag override:
```
/implement MyPlugin --express
```

**Effect:**
- This run uses express mode
- Preferences.json unchanged
- Next plugin uses manual mode (from preferences)

---

### Problem: Build Error, Stuck in Manual Mode

**Symptoms:**
- Started in express mode
- Build error occurred
- Now in manual mode, can't re-enable express

**This is expected behavior:**
- Express mode drops to manual on errors (safety feature)
- Prevents auto-retrying failed builds
- Forces manual intervention

**Solution:**
1. Fix the error
2. Resume with flag: `/continue MyPlugin --express`
3. Express mode re-enabled for remainder

---

### Problem: Want to Change Mode Mid-Workflow

**Scenario:**
- Started in manual mode
- Completed Stages 2 and 3 manually
- Want to finish Stages 4 and 5 in express mode

**Solution:**
1. Pause workflow (Ctrl+C or select "Pause")
2. Resume with flag: `/continue MyPlugin --express`
3. Stages 4 and 5 auto-progress

---

### Problem: Auto-Test Not Running

**Symptoms:**
- Set `auto_test: true` in preferences.json
- Tests not running automatically

**Possible causes:**
1. **Auto-test runs AFTER final menu:**
   - You must select an option from final menu first
   - Auto-test triggers after your selection
2. **Build errors interrupted workflow:**
   - Auto-test skipped if build failed
   - Fix errors and retry

**Correct behavior:**
```
✓ Plugin Complete

What's next?
1. Install to system folders
2. Run validation tests
3. Create installer package
4. Pause here
5. Other

Choose (1-5): 1

[You select option 1]

⚡ Auto-testing enabled, running pluginval...

[Tests run automatically]
```

---

## Advanced Configuration

### Selective Auto-Actions

You can mix and match auto-actions:

**Example 1: Auto-test only**
```json
{
  "workflow": {
    "mode": "express",
    "auto_test": true,
    "auto_install": false,
    "auto_package": false
  }
}
```
Result: Stages auto-flow, tests run automatically, then you decide install/package manually.

**Example 2: Auto-test and auto-install**
```json
{
  "workflow": {
    "mode": "express",
    "auto_test": true,
    "auto_install": true,
    "auto_package": false
  }
}
```
Result: Stages auto-flow, tests run, if tests pass, plugin installs, then you decide packaging manually.

**Example 3: Full automation**
```json
{
  "workflow": {
    "mode": "express",
    "auto_test": true,
    "auto_install": true,
    "auto_package": true
  }
}
```
Result: Entire workflow automated (stages → tests → install → package).

---

## Comparison: Manual vs Express

### Manual Mode Workflow

```
/implement MyPlugin

[Stage 2 executes...]

✓ Build System Ready

What's next?
1. Continue to Stage 3 (recommended)
2. Test build
3. Pause workflow
4. Review code
5. Other

Choose (1-5): 1  ← Manual selection

[Stage 3 executes...]

✓ Audio Engine Working

What's next?
1. Continue to Stage 4 (recommended)
2. Test audio processing
3. Pause workflow
4. Review DSP code
5. Other

Choose (1-5): 1  ← Manual selection

[Stage 4 executes...]

✓ UI Integrated

What's next?
1. Continue to Stage 5 (recommended)
2. Test UI
3. Pause workflow
4. Review GUI code
5. Other

Choose (1-5): 1  ← Manual selection

[Stage 5 executes...]

✓ Plugin Complete

What's next?
1. Install to system folders (recommended)
2. Run validation tests
3. Create installer package
4. Pause here
5. Other

Choose (1-5): _  ← Final decision
```

**Total decisions:** 5 (including final menu)
**Time cost:** ~3-5 minutes

---

### Express Mode Workflow

```
/implement MyPlugin

[Stage 2 executes...]

✓ Build System Ready → Implementing Audio Engine...

[Stage 3 executes immediately]

✓ Audio Engine Working → Adding GUI...

[Stage 4 executes immediately]

✓ UI Integrated → Running Validation...

[Stage 5 executes immediately]

✓ Plugin Complete

What's next?
1. Install to system folders (recommended)
2. Run validation tests
3. Create installer package
4. Pause here
5. Other

Choose (1-5): _  ← Only decision
```

**Total decisions:** 1 (final menu only)
**Time savings:** 3-5 minutes

---

## Summary

| Feature | Manual Mode | Express Mode |
|---------|-------------|--------------|
| Decision menus | 5 (all checkpoints) | 1 (final only) |
| Time per plugin | ~15-20 min | ~12-15 min |
| Time savings | - | 3-5 minutes |
| Control level | Full | Minimal (interrupt anytime) |
| Error handling | Show menu | Drop to manual |
| Final menu | Always | Always |
| Best for | Learning, complex plugins | Simple plugins, speed |

**Key takeaways:**
- Express mode saves 3-5 minutes per plugin (eliminates 4 decision gates)
- Final menu always appears (no silent auto-deployment)
- Safety first: Errors interrupt express mode, drop to manual
- Flexible: Command-line flags override preferences for one-off control
- Persistent: Mode saved in `.continue-here.md` for resume scenarios
- Backward compatible: No preferences.json = works exactly as before

**When in doubt:** Start with manual mode, switch to express when comfortable.
