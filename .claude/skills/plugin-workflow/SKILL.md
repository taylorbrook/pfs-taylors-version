---
name: plugin-workflow
description: Implementation orchestrator for stages 2-6 (Foundation through Validation)
allowed-tools:
  - Task # REQUIRED - All stages 2-5 MUST invoke subagents
  - Bash # For git commits
  - Read # For contracts
  - Write # For documentation
  - Edit # For state updates
preconditions:
  - architecture.md must exist (from /plan)
  - plan.md must exist (from /plan)
  - Status must be 🚧 Stage 1 OR resuming from 🚧 Stage 2+
  - Plugin must NOT be ✅ Working or 📦 Installed (use /improve instead)
---

# plugin-workflow Skill

**Purpose:** Pure orchestrator for stages 2-6 of JUCE plugin implementation. This skill NEVER implements directly - it always delegates to specialized subagents and presents decision menus after each stage completes.

## Overview

This skill orchestrates plugin implementation stages 2-6. Stages 0-1 (Research & Planning) are handled by the `plugin-planning` skill.

**Implementation Stages:**
- **Stage 2:** Foundation - Create build system, verify compilation (foundation-agent)
- **Stage 3:** Shell - Implement APVTS with all parameters (shell-agent)
- **Stage 4:** DSP - Implement audio processing (dsp-agent)
- **Stage 5:** GUI - Integrate WebView UI with parameter bindings (gui-agent)
- **Stage 6:** Validation - Factory presets, pluginval, CHANGELOG (direct or validator)

<orchestration_rules enforcement_level="STRICT">
  <delegation_rule
    id="subagent-dispatch-only"
    stages="2-5"
    tool="Task"
    violation="IMMEDIATE_STOP">

    Stages 2-5 MUST be delegated to subagents via Task tool.
    This skill is a PURE ORCHESTRATOR - it NEVER implements plugin code directly.

    <enforcement>
      IF stage in [2,3,4,5] AND action != "invoke_subagent_via_Task":
        STOP execution
        DISPLAY error: "Stage {stage} requires subagent delegation. Use Task tool to invoke {subagent_name}."
    </enforcement>

    <valid_delegations>
      - Stage 2: foundation-agent
      - Stage 3: shell-agent
      - Stage 4: dsp-agent
      - Stage 5: gui-agent
    </valid_delegations>

    Stage 6 can optionally run directly in orchestrator or via validator subagent.
  </delegation_rule>

  <checkpoint_protocol
    id="stage-completion-checkpoint"
    frequency="after_every_subagent"
    auto_proceed="NEVER"
    violation="WORKFLOW_CORRUPTION">

    After EVERY subagent return (whether full stage or phase completion), orchestrator MUST execute checkpoint sequence.

    <critical_sequence enforce_order="true">
      <step order="1" required="true">Commit all changes with git</step>
      <step order="2" required="true">Update .continue-here.md with current state</step>
      <step order="3" required="true">Update PLUGINS.md status</step>
      <step order="4" required="true">Update plan.md if phased implementation</step>
      <step order="5" required="true">Present numbered decision menu</step>
      <step order="6" required="true" blocking="true">WAIT for user response</step>
    </critical_sequence>

    <enforcement>
      All 6 steps must complete in order before proceeding.
      Step 6 is blocking - NEVER auto-proceed to next stage.
    </enforcement>

    <applies_to>
      - Simple plugins (complexity ≤2): After stages 2, 3, 4, 5, 6
      - Complex plugins (complexity ≥3): After stages 2, 3 AND after EACH DSP/GUI phase (4.1, 4.2, 4.3+, 5.1, 5.2, 5.3+), then 6

      Note: Phase count determined by plan.md (varies by complexity)
    </applies_to>
  </checkpoint_protocol>

  <handoff_protocol id="subagent-orchestrator-handoff">
    Subagents NEVER commit - they only implement and return JSON report.
    Orchestrator handles all state management and git operations.

    <handoff_format>
      Subagent returns JSON:
      {
        "status": "success" | "error",
        "stage": 2-6,
        "completionStatement": "...",
        "filesCreated": [...],
        "nextSteps": [...]
      }
    </handoff_format>
  </handoff_protocol>

  <state_requirement id="required-reading-injection">
    All subagents (stages 2-5) MUST receive Required Reading file to prevent repeat mistakes.

    <enforcement>
      BEFORE invoking subagent via Task tool:
      1. Read troubleshooting/patterns/juce8-critical-patterns.md
      2. Inject content into subagent prompt
      3. Verify injection succeeded
    </enforcement>
  </state_requirement>
</orchestration_rules>

Each stage is fully documented in its own reference file in `references/` subdirectory.

<preconditions blocking="true">
  <contract_verification required="true" blocking="true">
    Before starting Stage 2, verify these contract files exist:

    <required_file path="plugins/$PLUGIN_NAME/.ideas/architecture.md" created_by="Stage 0" />
    <required_file path="plugins/$PLUGIN_NAME/.ideas/plan.md" created_by="Stage 1" />
    <required_file path="plugins/$PLUGIN_NAME/.ideas/creative-brief.md" created_by="ideation" />

    <on_missing_files action="BLOCK">
      Display error message:
      "[PluginName] is missing required planning documents.

      Missing files will be listed here:
      - architecture.md (from Stage 0)
      - plan.md (from Stage 1)
      - creative-brief.md (from ideation)

      Run /plan [PluginName] to complete planning stages 0-1."
    </on_missing_files>

    See [references/precondition-checks.md](references/precondition-checks.md) for bash implementation.
  </contract_verification>

  <status_verification required="true" blocking="true">
    Read PLUGINS.md and verify status is appropriate:

    <allowed_state status="🚧 Stage 1">
      OK to proceed (just finished planning)
    </allowed_state>

    <allowed_state status="🚧 Stage N" condition="N >= 2">
      OK to proceed (resuming implementation)
    </allowed_state>

    <blocked_state status="💡 Ideated">
      BLOCK with message:
      "[PluginName] needs planning before implementation.
      Run /plan [PluginName] to complete stages 0-1."
    </blocked_state>

    <blocked_state status="✅ Working">
      BLOCK with message:
      "[PluginName] is already complete.
      Use /improve [PluginName] to make changes."
    </blocked_state>

    <blocked_state status="📦 Installed">
      BLOCK with message:
      "[PluginName] is already complete.
      Use /improve [PluginName] to make changes."
    </blocked_state>
  </status_verification>
</preconditions>

---

## Resume Entry Point

**Purpose:** Handle workflow resume from .continue-here.md handoff file.

<decision_gate type="resume_or_fresh_start">
  <condition check="handoff_file_exists">
    IF plugins/${PLUGIN_NAME}/.continue-here.md exists:
      - Parse YAML frontmatter
      - Extract current_stage, next_action, next_phase
      - Resume at specified stage/phase
    ELSE:
      - Start fresh at Stage 2
      - No handoff context available
  </condition>

  <routing_logic>
    Based on current_stage value:
    - Stage 2 → invoke foundation-agent
    - Stage 3 → invoke shell-agent
    - Stage 4 → invoke dsp-agent
    - Stage 5 → invoke gui-agent
    - Stage 6 → execute validation

    If next_phase is set: Resume phased implementation at specified phase.
  </routing_logic>
</decision_gate>

---

## Stage Dispatcher

**Purpose:** Pure orchestration dispatcher that ONLY invokes subagents via Task tool.

**Entry point:** Called by /implement command or /continue command after plugin-planning completes.

<enforcement_reminder ref="subagent-dispatch-only" severity="CRITICAL">
This orchestrator MUST NEVER implement plugin code directly. ALL stages 2-5 MUST be delegated via Task tool.
</enforcement_reminder>

### Implementation

1. **Verify state integrity BEFORE dispatch:**

```bash
# CRITICAL: Verify state consistency before proceeding
# Prevents corruption from non-atomic commits or contract tampering
bash -c "$(cat references/state-management.md | grep -A100 'verifyStateIntegrity')"
```

See `references/state-management.md` for `verifyStateIntegrity()` function.

**What it checks:**
- Handoff file exists (exit 1 if missing)
- Stage consistency between .continue-here.md and PLUGINS.md (exit 2 if mismatch)
- Contract checksums match (exit 3 if tampered)
- No stale handoffs from completed plugins (exit 4, auto-cleanup)

If verification fails, BLOCK dispatch and guide user to resolution (/reconcile, restore contracts, etc.)

2. **Determine current stage:**

```bash
# Check if handoff file exists (resuming)
if [ -f "plugins/${PLUGIN_NAME}/.continue-here.md" ]; then
    # Parse stage from handoff YAML frontmatter
    CURRENT_STAGE=$(grep "^stage:" plugins/${PLUGIN_NAME}/.continue-here.md | awk '{print $2}')
    echo "Resuming from Stage ${CURRENT_STAGE}"
else
    # Starting fresh after planning
    CURRENT_STAGE=2
    echo "Starting implementation at Stage 2"
fi
```

3. **Verify preconditions for target stage:**

See `references/state-management.md` for `checkStagePreconditions()` function.

<dispatcher_pattern>
  The orchestrator routes stages to subagents using this logic:

  1. Verify state integrity (verifyStateIntegrity) → BLOCK if corrupted
  2. Check preconditions → If failed, BLOCK with reason
  3. **MANDATORY: design-sync validation before Stage 2** → BLOCK if drift detected
  4. Route to subagent based on stage number:
     - Stage 2 → foundation-agent (single-pass)
     - Stage 3 → shell-agent (single-pass)
     - Stage 4 → dsp-agent (phase-aware dispatch)
     - Stage 5 → gui-agent (phase-aware dispatch)
     - Stage 6 → validator (single-pass or direct execution)
  5. Pass contracts and Required Reading to subagent
  6. Wait for subagent completion

  See [references/dispatcher-pattern.md](references/dispatcher-pattern.md) for full pseudocode.
</dispatcher_pattern>

<phase_aware_dispatch stages="4,5" enforcement_level="MANDATORY">
  **Purpose:** Prevent "implement all phases" errors by detecting and looping phases for complex plugins.

  **When:** Before invoking dsp-agent (Stage 4) or gui-agent (Stage 5)

  **Applies to:** Stages 4 and 5 only. Stages 2, 3, and 6 remain single-pass.

  <phase_detection_algorithm>
    **Determine if phased implementation is needed:**

    ```typescript
    // 1. Read plan.md to check for phases
    const planContent = readFile(`plugins/${pluginName}/.ideas/plan.md`);

    // 2. Extract complexity score
    const complexityMatch = planContent.match(/\*\*Complexity Score:\*\*\s+([\d.]+)/);
    const complexityScore = complexityMatch ? parseFloat(complexityMatch[1]) : 0;

    // 3. Check for phase markers based on current stage
    const stagePhasePattern = currentStage === 4
      ? /### Phase 4\.\d+/g
      : /### Phase 5\.\d+/g;

    const hasPhases = stagePhasePattern.test(planContent);

    // 4. Determine execution strategy
    const needsPhasedImplementation = complexityScore >= 3 && hasPhases;

    console.log(`Complexity: ${complexityScore}, Has phases: ${hasPhases}`);
    console.log(`Execution mode: ${needsPhasedImplementation ? "PHASED" : "SINGLE-PASS"}`);
    ```
  </phase_detection_algorithm>

  <routing_decision>
    **Based on detection results, route to appropriate handler:**

    <single_pass_condition>
      **IF complexity < 3 OR no phase markers found:**

      1. Invoke subagent ONCE for entire stage
      2. Use prompt template from reference file single-pass section:
         - Stage 4: `references/stage-4-dsp.md` lines 45-87 (single-pass implementation)
         - Stage 5: `references/stage-5-gui.md` lines 83-135 (single-pass implementation)
      3. Checkpoint after stage completes (standard 6-step checkpoint)
      4. Present decision menu (continue to next stage, pause, test, etc.)
    </single_pass_condition>

    <phased_implementation_condition>
      **IF complexity ≥3 AND phase markers found:**

      **Phase parsing:**
      ```typescript
      // Extract all phases for current stage from plan.md
      const phasePattern = currentStage === 4
        ? /### Phase (4\.\d+):\s*(.+?)$/gm
        : /### Phase (5\.\d+):\s*(.+?)$/gm;

      const phases = [];
      let match;
      while ((match = phasePattern.exec(planContent)) !== null) {
        phases.push({
          number: match[1],        // e.g., "4.1" or "5.1"
          description: match[2]    // e.g., "Voice Architecture" or "Layout and Basic Controls"
        });
      }

      console.log(`Stage ${currentStage} will execute in ${phases.length} phases:`);
      phases.forEach(phase => {
        console.log(`  - Phase ${phase.number}: ${phase.description}`);
      });
      ```

      **Phase execution loop:**
      ```typescript
      for (let i = 0; i < phases.length; i++) {
        const phase = phases[i];

        console.log(`\n━━━ Stage ${phase.number} - ${phase.description} ━━━\n`);

        // Invoke subagent for THIS PHASE ONLY
        const phaseResult = Task({
          subagent_type: currentStage === 4 ? "dsp-agent" : "gui-agent",
          description: `Implement Phase ${phase.number} for ${pluginName}`,
          prompt: constructPhasePrompt(phase, pluginName, currentStage, phases.length)
        });

        // Parse subagent report
        const phaseReport = parseSubagentReport(phaseResult);

        // Validate phase completion
        if (!phaseReport || phaseReport.status === "failure") {
          console.log(`✗ Phase ${phase.number} (${phase.description}) failed`);
          presentPhaseFailureMenu(phase, phaseReport);
          return; // BLOCK progression - do not continue to next phase
        }

        // Phase succeeded
        console.log(`✓ Phase ${phase.number} complete: ${phase.description}`);
        console.log(`  - Components: ${phaseReport.outputs.components_this_phase?.join(", ") || "N/A"}`);

        // CHECKPOINT: Commit phase changes
        commitPhase(pluginName, phase, i + 1, phases.length);

        // CHECKPOINT: Update handoff file
        updateHandoff(
          pluginName,
          currentStage,
          phase.number,
          `Phase ${phase.number} complete: ${phase.description}`,
          i < phases.length - 1
            ? [`Continue to Phase ${phases[i + 1].number}`, "Review phase code", "Test", "Pause"]
            : ["Continue to Stage " + (currentStage + 1), "Review complete stage", "Test", "Pause"]
        );

        // CHECKPOINT: Update plugin status
        updatePluginStatus(pluginName, `🚧 Stage ${phase.number}`);

        // CHECKPOINT: Update plan.md with phase completion timestamp
        const timestamp = new Date().toISOString();
        updatePlanMd(pluginName, phase.number, timestamp);

        // CHECKPOINT: Verify all steps completed
        verifyPhaseCheckpoint(pluginName, phase.number);

        // DECISION MENU: Present between phases (BLOCKING)
        if (i < phases.length - 1) {
          console.log(`
✓ Phase ${phase.number} complete

Progress: ${i + 1} of ${phases.length} phases complete

What's next?

1. Continue to Phase ${phases[i + 1].number} (recommended)
2. Review Phase ${phase.number} code
3. Test current implementation
4. Pause here
5. Other

Choose (1-5): _
          `);

          const choice = getUserInput(); // BLOCKING WAIT
          if (choice === "4" || choice.toLowerCase() === "pause") {
            console.log(`\n⏸ Paused between phases. Resume with /continue ${pluginName}`);
            return; // Exit workflow, state saved for resume
          }
          // Other choices handled by menu router
        }
      }

      console.log(`\n✓ All ${phases.length} phases complete for Stage ${currentStage}!`);
      ```

      **After ALL phases complete:**
      1. Commit final stage state (if not already committed)
      2. Update handoff to next stage
      3. Update plugin status to reflect stage completion
      4. Present stage completion decision menu
      5. WAIT for user response
    </phased_implementation_condition>
  </routing_decision>

  <prompt_construction>
    **For each phase invocation, construct phase-specific prompt:**

    ```typescript
    function constructPhasePrompt(phase, pluginName, currentStage, totalPhases) {
      // Read Required Reading (MANDATORY for all subagents)
      const criticalPatterns = readFile("troubleshooting/patterns/juce8-critical-patterns.md");

      // Read contracts
      const architectureMd = readFile(`plugins/${pluginName}/.ideas/architecture.md`);
      const parameterSpecMd = readFile(`plugins/${pluginName}/.ideas/parameter-spec.md`);
      const planMd = readFile(`plugins/${pluginName}/.ideas/plan.md`);

      // Stage-specific additional contracts
      const creativeBriefMd = currentStage === 5
        ? readFile(`plugins/${pluginName}/.ideas/creative-brief.md`)
        : null;

      const mockupPath = currentStage === 5
        ? findLatestMockup(pluginName)
        : null;

      return `CRITICAL PATTERNS (MUST FOLLOW):

${criticalPatterns}

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Implement Phase ${phase.number} for plugin at plugins/${pluginName}.

**Current Phase:** ${phase.number} - ${phase.description}
**Total Phases:** ${totalPhases}
**Plugin Name:** ${pluginName}

**Contracts:**
- architecture.md:
${architectureMd}

- parameter-spec.md:
${parameterSpecMd}

- plan.md (Phase ${phase.number} section):
${planMd}

${currentStage === 5 ? `
- creative-brief.md:
${creativeBriefMd}

- UI Mockup: ${mockupPath}
` : ''}

**Tasks for Phase ${phase.number}:**
1. Read plan.md and extract Phase ${phase.number} components ONLY
2. Read architecture.md for component specifications
${currentStage === 4 ? `3. Add member variables for Phase ${phase.number} DSP components
4. Implement Phase ${phase.number} components in processBlock()
5. Build on existing code from previous phases (do NOT remove previous work)
6. Connect Phase ${phase.number} parameters only
7. Ensure real-time safety (no allocations, use juce::ScopedNoDenormals)` : `3. Implement UI elements for Phase ${phase.number}
4. Add parameter bindings for Phase ${phase.number} controls
5. Build on existing UI from previous phases (do NOT remove previous work)
6. Verify member order (Relays → WebView → Attachments)
7. Ensure all Phase ${phase.number} parameter IDs match HTML element IDs`}
8. Update plan.md with phase completion timestamp
9. Return JSON report with phase_completed: "${phase.number}"

**CRITICAL:** Implement ONLY Phase ${phase.number} components. Preserve all code from previous phases.

Build verification handled by workflow after agent completes.
`;
    }
    ```

    **Key differences from single-pass prompt:**
    - Explicitly states current phase number and description
    - Emphasizes "THIS PHASE ONLY" - not all phases
    - Reminds to preserve previous phase code
    - Includes phase completion timestamp requirement
    - JSON report must include phase_completed field
  </prompt_construction>

  <error_prevention>
    <anti_pattern severity="CRITICAL">
      **The problem this solves:**

      ❌ **NEVER send** "Implement ALL phases" to subagent
      - Causes compilation errors from attempting too much
      - Led to DrumRoulette Stage 5 failure (3 phases → single invocation → build errors)
      - Violates incremental implementation principle

      ✓ **ALWAYS invoke** subagent once per phase with phase-specific prompt
      - One phase at a time, sequential execution
      - Checkpoint after EACH phase
      - User confirmation between phases
      - Incremental testing and validation
    </anti_pattern>

    <enforcement>
      **Phase-aware dispatch is MANDATORY for Stages 4-5 when:**
      1. Complexity score ≥3 (from plan.md)
      2. Phase markers exist in plan.md (### Phase 4.X or ### Phase 5.X)

      **Phase-aware dispatch is SKIPPED for Stages 4-5 when:**
      1. Complexity score <3 (simple plugin, single-pass sufficient)
      2. No phase markers in plan.md (plan didn't define phases)

      **Phase-aware dispatch DOES NOT APPLY to:**
      - Stage 2 (Foundation) - always single-pass
      - Stage 3 (Shell) - always single-pass
      - Stage 6 (Validation) - always single-pass

      **The orchestrator MUST:**
      - Read plan.md to detect phases BEFORE invoking subagent
      - Parse ALL phases for the stage
      - Loop through phases sequentially
      - Present decision menu after EACH phase
      - WAIT for user confirmation before next phase

      **The orchestrator MUST NOT:**
      - Skip phase detection (this is mandatory control flow)
      - Invoke subagent with multiple phases at once
      - Auto-proceed between phases without user confirmation
      - Reference stage-4-dsp.md or stage-5-gui.md reference files for control flow (those are documentation/templates only)
    </enforcement>
  </error_prevention>

  <integration_with_checkpoint_protocol>
    **Phase checkpoints are identical to stage checkpoints:**

    After each phase completes:
    1. ✓ Commit phase changes (git commit with phase number)
    2. ✓ Update handoff (.continue-here.md with current phase)
    3. ✓ Update plugin status (PLUGINS.md with phase emoji)
    4. ✓ Update plan.md (phase completion timestamp)
    5. ✓ Verify checkpoint succeeded (all files updated)
    6. ✓ Present decision menu (BLOCKING)

    This mirrors the standard checkpoint protocol (lines 82-85, 338-443) but executes after EACH phase instead of EACH stage.

    **Why this matters:**
    - User can pause between phases (not just between stages)
    - State is saved incrementally (phase-level granularity)
    - Build failures isolated to single phase (easier to debug)
    - /continue can resume mid-stage at specific phase
  </integration_with_checkpoint_protocol>
</phase_aware_dispatch>

<design_sync_gate enforcement_level="MANDATORY">
  **Purpose:** Prevent design drift before implementation begins.

  **When:** BEFORE dispatching Stage 2 (foundation-agent), IF mockup exists.

  **Conditions:**
  - IF plugins/[PluginName]/.ideas/mockups/ directory exists
  - AND parameter-spec.md exists (mockup finalized)
  - THEN design-sync validation is REQUIRED

  **Implementation:**
  ```
  Before dispatching Stage 2:

  1. Check if mockup exists:
     - Look for plugins/[PluginName]/.ideas/parameter-spec.md
     - Look for mockups/ directory with YAML files

  2. If mockup exists:
     - Run design-sync validation automatically
     - Present findings with decision menu
     - BLOCK Stage 2 until one of:
       a) No drift detected (continue)
       b) Acceptable evolution (user confirms)
       c) Drift resolved (user updates brief or mockup)
       d) User explicitly overrides (logged)

  3. If no mockup:
     - Skip design-sync (no visual design to validate)
     - Proceed to Stage 2 directly
  ```

  **Decision menu when drift detected:**
  ```
  ⚠️ Design-brief drift detected

  [Findings from design-sync]

  Cannot proceed to Stage 2 until resolved:

  1. Update brief - Document evolution and continue
  2. Update mockup - Fix mockup to match brief
  3. Override (not recommended) - Accept drift and proceed anyway
  4. Cancel - Stop workflow, fix manually
  5. Other

  Choose (1-5): _
  ```

  **Why mandatory:**
  - Catches misalignments before Stage 2 generates boilerplate
  - Prevents implementing features not in brief
  - Prevents missing features mentioned in brief
  - Avoids 10+ minutes of wasted work on wrong implementation
  - Ensures contracts remain single source of truth

  **Override logging:**
  If user chooses override, log to .validator-overrides.yaml:
  ```yaml
  - timestamp: [ISO-8601]
    validator: design-sync
    stage: pre-stage-2
    severity: [none|attention|critical]
    override-reason: "User proceeded despite drift"
    findings: "[brief summary]"
  ```
</design_sync_gate>

4. **Checkpoint enforcement after EVERY subagent:**

<workflow_loop>
  <stage_iteration from="2" to="6">
    <dispatch_phase>
      Display: "━━━ Stage ${currentStage} ━━━"

      <delegation_enforcement ref="subagent-dispatch-only">
        Invoke subagent via Task tool (NEVER implement directly)
      </delegation_enforcement>

      IF result.status == 'blocked' OR 'error':
        Display error and return (workflow blocked)
    </dispatch_phase>

    <checkpoint_phase>
      <checkpoint_enforcement enforce_order="true">
        <step order="1" required="true" function="commitStage">
          commitStage(pluginName, currentStage, result.description)
          Auto-commit all changes from subagent completion
          VERIFY: git commit succeeded (check exit code)
        </step>

        <step order="2" required="true" function="updateHandoff">
          updateHandoff(pluginName, currentStage + 1, result.completed, result.nextSteps)
          Update .continue-here.md with new stage, timestamp, next_action
          VERIFY: File exists and contains expected stage number
        </step>

        <step order="3" required="true" function="updatePluginStatus">
          updatePluginStatus(pluginName, `🚧 Stage ${currentStage}`)
          Update PLUGINS.md status emoji in BOTH locations atomically
          VERIFY: PLUGINS.md contains new status in full entry section
        </step>

        <step order="3.5" required="true" function="validateRegistryConsistency">
          validateRegistryConsistency(pluginName)
          Verify registry table matches full entry after status update
          VERIFY: Both locations show identical status
          BLOCK: If mismatch detected, present drift resolution menu
        </step>

        <step order="4" required="true" function="updatePluginTimeline">
          updatePluginTimeline(pluginName, currentStage, result.description)
          Append timeline entry to PLUGINS.md
          VERIFY: Timeline entry exists in PLUGINS.md
        </step>

        <step order="5" required="true" function="verifyCheckpoint">
          verifyCheckpoint(pluginName, currentStage)
          Validate all checkpoint steps completed successfully
          BLOCK: If any step failed, present retry menu before continuing
        </step>

        <step order="6" required="true" blocking="true" function="presentDecisionMenu">
          presentDecisionMenu({ stage, completionStatement, pluginName })
          Present numbered decision menu and WAIT for user response
        </step>
      </checkpoint_enforcement>

      <checkpoint_verification>
        **Purpose:** Verify all checkpoint steps completed before presenting decision menu.

        **Implementation:**
        ```
        After steps 1-4 complete, run verification:

        CHECKPOINT VERIFICATION:
        ✓ Step 1: Git commit [commit-hash]
        ✓ Step 2: Handoff updated (.continue-here.md stage: N+1)
        ✓ Step 3: Plugin status updated (PLUGINS.md: 🚧 Stage N)
        ✓ Step 4: Timeline appended (PLUGINS.md: [date] Stage N complete)

        IF all verified:
          Proceed to decision menu

        IF any failed:
          Display failure report:
          ━━━ Checkpoint Incomplete ━━━
          ✗ Step 2: Handoff update failed (file not found)
          ✓ Step 1: Commit succeeded
          ✓ Step 3: Status updated
          ✓ Step 4: Timeline appended

          Cannot proceed - checkpoint integrity required.

          1. Retry failed steps - Attempt automatic fix
          2. Show details - See error messages
          3. Manual fix - I'll fix it myself (pause workflow)
          4. Other
        ```

        **Verification checks:**
        - Step 1: `git log -1 --oneline` contains stage reference
        - Step 2: `.continue-here.md` exists and contains `stage: N+1`
        - Step 3: `PLUGINS.md` contains `**Status:** 🚧 Stage N`
        - Step 4: `PLUGINS.md` timeline has entry dated today for Stage N

        **Why critical:**
        Incomplete checkpoints cause state corruption:
        - Missing handoff → /continue can't resume
        - Missing status → PLUGINS.md out of sync
        - Missing timeline → no audit trail
        - Missing commit → changes lost on crash
      </checkpoint_verification>
    </checkpoint_phase>

    <decision_gate blocking="true">
      WAIT for user response (NEVER auto-proceed)

      <routing>
        IF choice == 'continue' OR choice == 1:
          currentStage++
        ELSE IF choice == 'pause':
          Display: "✓ Workflow paused. Resume anytime with /continue"
          Exit workflow loop
        ELSE:
          handleMenuChoice(choice, pluginName, currentStage)
      </routing>
    </decision_gate>
  </stage_iteration>

  <completion_check>
    IF currentStage > 6:
      Display: "✓ All stages complete!"
      updatePluginStatus(pluginName, '✅ Working')
  </completion_check>
</workflow_loop>

**Usage:**

```javascript
// From /implement command (after planning complete):
runWorkflow(pluginName, 2)

// From /continue command:
const handoff = readHandoffFile(pluginName)
const resumeStage = handoff.stage
runWorkflow(pluginName, resumeStage)
```

---

<reference_files>
  Each stage has a reference file containing subagent prompt templates:

  - [stage-2-foundation.md](references/stage-2-foundation.md) - foundation-agent
  - [stage-3-shell.md](references/stage-3-shell.md) - shell-agent
  - [stage-4-dsp.md](references/stage-4-dsp.md) - dsp-agent
  - [stage-5-gui.md](references/stage-5-gui.md) - gui-agent
  - [stage-6-validation.md](references/stage-6-validation.md) - validator
  - [state-management.md](references/state-management.md) - State machine functions
  - [dispatcher-pattern.md](references/dispatcher-pattern.md) - Routing logic
  - [precondition-checks.md](references/precondition-checks.md) - Contract validation

  <usage_pattern>
    Orchestrator reads reference files → constructs Task invocation → NEVER implements directly
  </usage_pattern>
</reference_files>

---

## Integration Points

**Invoked by:**

- `/implement` command (after plugin-planning completes)
- `context-resume` skill (when resuming implementation stages)
- `/continue` command (for stages 2-6)

**ALWAYS invokes (via Task tool):**

- `foundation-agent` subagent (Stage 2) - REQUIRED, never implement directly
- `shell-agent` subagent (Stage 3) - REQUIRED, never implement directly
- `dsp-agent` subagent (Stage 4) - REQUIRED, never implement directly
- `gui-agent` subagent (Stage 5) - REQUIRED, never implement directly
- `validator` subagent (Stage 6) - Optional, can run directly

**Also invokes:**

- `build-automation` skill (build coordination across stages)
- `plugin-testing` skill (validation after stages 4, 5, 6)
- `plugin-lifecycle` skill (if user chooses to install after Stage 6)

**Reads (contracts from plugin-planning):**

- `architecture.md` (DSP specification from Stage 0)
- `plan.md` (implementation strategy from Stage 1)
- `creative-brief.md` (vision from ideation)
- `parameter-spec.md` (parameter definitions)

**Creates:**

- `.continue-here.md` (handoff file for checkpoints)
- `CHANGELOG.md` (Stage 6)
- `Presets/` directory (Stage 6)

**Updates:**

- PLUGINS.md (status changes after each stage)
- `.continue-here.md` (after each stage completes)

---

## Error Handling

**If contract files missing before Stage 2:**
Block and instruct user to run `/plan [PluginName]` to complete stages 0-1.

**If build fails during subagent execution:**
Subagent returns error. Orchestrator presents 4-option menu:
1. Investigate (deep-research)
2. Show me the code
3. Show full build output
4. Manual fix (resume with /continue)

**If tests fail:**
Present menu with investigation options. Do NOT auto-proceed to next stage.

**If subagent fails to complete:**
Present menu allowing retry, manual intervention, or workflow pause.

**If git staging fails:**
Continue anyway, log warning.

---

## Success Criteria

Workflow is successful when:

- All subagents (stages 2-5) invoked successfully via Task tool
- Plugin compiles without errors at each stage
- All stages completed in sequence (2 → 3 → 4 → 5 → 6)
- Decision menus presented after EVERY stage
- Tests pass (if run)
- PLUGINS.md updated to ✅ Working after Stage 6
- Handoff file updated after each stage
- Git history shows atomic commits for each stage

---

## Decision Menu Protocol

**Use AskUserQuestion tool ONLY when:**
- NEVER in this skill - plugin-workflow is pure orchestration
- All decisions are workflow navigation, not ideation

**Use Inline Numbered Menu for:**
- After EVERY stage completion (checkpoint gates)
- Build failure recovery options
- Test failure investigation options
- Phase completion (for complex plugins)
- Examples:
  - "Stage 3 complete. What's next? 1) Continue to Stage 4..."
  - "Build failed. What should I do? 1) Retry build..."
  - "Phase 4.1 complete. 1) Continue to Phase 4.2..."

**Key difference:** This skill ONLY uses inline menus. No creative decisions, only workflow navigation.

**Inline menu format:**

```
✓ [Stage/Phase completion statement]

What's next?

1. [Continue to next stage/phase] (recommended)
2. [Run tests] - Validate current implementation
3. [Pause workflow] - Save progress and exit
4. [Review changes] - Show git diff
5. Other

Choose (1-5): _
```

ALWAYS wait for user response. NEVER auto-proceed.

---

## Integration Contracts

### Invokes: foundation-agent (Stage 2)

**When:** Stage 2 (Foundation) implementation

**Sends via Task tool:**
```
Implement Stage 2 (Foundation) for [PluginName].

**Contracts:**
- creative-brief.md: [full content]
- architecture.md: [full content]
- plan.md: [full content]

**Required Reading:**
[juce8-critical-patterns.md content]

**Directive:** Create foundation files (PluginProcessor.h/cpp, PluginEditor.h/cpp, CMakeLists.txt).
Return JSON report when complete.
```

**Expects:** JSON report conforming to `.claude/schemas/subagent-report.json`:
```json
{
  "agent": "foundation-agent",
  "status": "success",
  "outputs": {
    "plugin_name": "PluginName",
    "source_files_created": [
      "Source/PluginProcessor.h",
      "Source/PluginProcessor.cpp",
      "Source/PluginEditor.h",
      "Source/PluginEditor.cpp",
      "CMakeLists.txt"
    ]
  },
  "issues": [],
  "ready_for_next_stage": true
}
```

**Error handling:**
- Missing contract → foundation-agent returns status: "failure", orchestrator blocks progression
- File creation error → present retry menu to user
- Schema validation failure → log error, block progression

**Contract:** foundation-agent creates files, returns report. Orchestrator validates schema, commits changes, updates state.

---

### Invokes: shell-agent (Stage 3)

**When:** Stage 3 (Shell) implementation

**Sends via Task tool:**
```
Implement Stage 3 (Shell) for [PluginName].

**Contracts:**
- parameter-spec.md: [full content]
- architecture.md: [full content]

**Required Reading:**
[juce8-critical-patterns.md content]

**Directive:** Implement APVTS with all parameters, state management.
Return JSON report when complete.
```

**Expects:** JSON report conforming to `.claude/schemas/subagent-report.json`

**Error handling:**
- Parameter count mismatch → present investigation menu
- APVTS creation failed → offer retry or manual intervention
- Schema validation failure → block progression

**Contract:** shell-agent modifies files, returns report. Orchestrator validates parameter counts match parameter-spec.md.

---

### Invokes: dsp-agent (Stage 4)

**When:** Stage 4 (DSP) implementation

**Sends via Task tool:**
```
Implement Stage 4 [Phase X] for [PluginName].

**Contracts:**
- architecture.md: [full content]
- parameter-spec.md: [full content]
- plan.md: [full content]

**Required Reading:**
[juce8-critical-patterns.md content]

**Directive:** Implement DSP components and processing chain [for Phase X].
Return JSON report when complete.
```

**Expects:** JSON report conforming to `.claude/schemas/subagent-report.json`

**Error handling:**
- Missing DSP component → dsp-agent reports in issues array
- Real-time safety violation → warn in checkpoint menu
- Phase incomplete → ready_for_next_stage: false, orchestrator blocks

**Contract:** dsp-agent implements DSP, returns report. Orchestrator handles phase checkpoints and state updates.

---

### Invokes: gui-agent (Stage 5)

**When:** Stage 5 (GUI) implementation

**Sends via Task tool:**
```
Implement Stage 5 (GUI) for [PluginName].

**Contracts:**
- parameter-spec.md: [full content]
- Mockup files: [list all v[N]-*.* files]

**Required Reading:**
[juce8-critical-patterns.md content]

**Directive:** Integrate WebView UI with parameter bindings.
Return JSON report when complete.
```

**Expects:** JSON report conforming to `.claude/schemas/subagent-report.json`

**Error handling:**
- Missing mockup → gui-agent returns error, cannot proceed
- Relay/attachment mismatch → validate counts match parameters
- WebView integration failed → present recovery menu

**Contract:** gui-agent integrates UI, returns report. Orchestrator validates relay/attachment counts match parameter-spec.md.

---

### Invokes: validator (Stages 1-5)

**When:** After each stage completion (optional but recommended)

**Sends via Task tool:**
```
Validate Stage [N] completion for [PluginName].

**Stage:** [N]
**Plugin:** [PluginName]
**Contracts:**
- [stage-specific contracts]

**Expected outputs for Stage [N]:**
[stage-specific expected outputs]

Return JSON validation report.
```

**Expects:** JSON report conforming to `.claude/schemas/validator-report.json`

**Error handling:**
- Validation failure → include in checkpoint menu, let user decide
- Override file exists → validator handles suppression
- Schema validation failure → log warning, continue anyway (validation is advisory)

**Contract:** Validator is advisory, not blocking. Orchestrator presents validation results in checkpoint menu. User makes final decision.

---

### Invokes: build-automation (After stages 2-6)

**When:** After stage completion to verify compilation

**Sends via Skill tool:**
```json
{
  "plugin_name": "PluginName",
  "build_type": "Debug",
  "operation": "build"
}
```

**Expects:**
```json
{
  "success": true,
  "build_log": "logs/PluginName/build-timestamp.log",
  "binary_path": "builds/PluginName/Debug/PluginName.vst3",
  "warnings": 0,
  "errors": 0
}
```

**Error handling:**
- Build failure → present recovery menu (retry, investigate, manual fix)
- Missing CMakeLists.txt → check foundation stage completion
- Compiler not found → delegate to system-setup skill

**Contract:** Orchestrator NEVER builds directly. Always delegates to build-automation skill.

---

### Invoked by: context-resume

**Receives:**
- Plugin name (string)
- Stage number (integer 0-6)
- Phase (string or null)
- Orchestration mode (boolean, must be true)
- Handoff context (full .continue-here.md content)

**Returns:**
- Resumes at specified stage/phase
- Executes remaining workflow
- Presents checkpoint menus at each stage
- Updates state files throughout

**Contract:** context-resume is READ-ONLY. Orchestrator owns all state updates. context-resume only loads and routes.

---

### Invoked by: /implement command

**Receives:**
- Plugin name (string)
- Entry stage (typically Stage 2, or wherever workflow paused)

**Returns:**
- Executes workflow from entry stage through Stage 6
- Presents checkpoint menus at each stage
- Updates PLUGINS.md to ✅ Working when complete

**Contract:** Command expands to prompt that invokes this skill. Skill handles full workflow.

---

## Data Formats

### Subagent Reports

**Schema:** `.claude/schemas/subagent-report.json`

**Purpose:** Standardizes stage completion reports from all subagents

**Validation:**

```python
import json
from jsonschema import validate

with open('.claude/schemas/subagent-report.json') as f:
    schema = json.load(f)

validate(instance=report, schema=schema)
```

**Required fields:**
- `agent`: string (foundation-agent, shell-agent, dsp-agent, gui-agent)
- `status`: string (success, failure)
- `outputs`: object (must contain plugin_name)
- `issues`: array (empty on success)
- `ready_for_next_stage`: boolean

See `.claude/schemas/README.md` for complete schema and validation examples.

---

### Validator Reports

**Schema:** `.claude/schemas/validator-report.json`

**Purpose:** Standardizes validation reports from validator agent

**Validation:**

```python
with open('.claude/schemas/validator-report.json') as f:
    schema = json.load(f)

validate(instance=report, schema=schema)
```

**Required fields:**
- `agent`: string (always "validator")
- `stage`: integer (0-6)
- `status`: string (PASS, FAIL)
- `checks`: array of check objects
- `recommendation`: string
- `continue_to_next_stage`: boolean

---

## Error Handling

### Common Errors

**Subagent returns invalid JSON:**
- **Detection:** JSON parse error when reading subagent output
- **Recovery:** Use fallback parsing strategies (extract from code blocks, partial JSON)
- **User action:** If all parsing fails, present raw output and offer manual recovery

**Schema validation failure:**
- **Detection:** jsonschema.ValidationError when validating report
- **Recovery:** Log specific validation error, block progression
- **User action:** Present error details, offer to retry subagent or continue anyway (risky)

**Build failure after stage:**
- **Detection:** build-automation returns success: false
- **Recovery:** Present build log excerpt in checkpoint menu
- **User action:** Choose from: retry build, investigate logs, manual fix, rollback stage

**State file corruption:**
- **Detection:** YAML parse error or missing required fields in .continue-here.md
- **Recovery:** Reconstruct from git log and PLUGINS.md
- **User action:** Verify reconstructed state before continuing

**Checkpoint step failure:**
- **Detection:** Git commit fails, file write fails, etc.
- **Recovery:** Log specific step failure, attempt remaining steps
- **User action:** Present partial checkpoint status, offer manual completion

### Graceful Degradation

**When validator unavailable:**
- Skip validation step
- Log warning in checkpoint
- Continue to present checkpoint menu

**When build-automation unavailable:**
- Fall back to direct cmake/xcodebuild (legacy mode)
- Log warning about using legacy build

**When Required Reading file missing:**
- Log warning
- Continue subagent invocation without Required Reading
- May encounter known issues without pattern protection

### Error Reporting Format

All errors presented include:
1. **What failed** - Specific stage/step that failed
2. **Why it failed** - Error message or root cause
3. **How to fix** - Actionable recovery steps
4. **Alternative** - Other ways to proceed (pause, rollback, manual fix)

Example:
```
ERROR: Stage 3 (Shell) failed

Reason: Parameter count mismatch (expected 5, found 3)
Missing parameters: delayTime, feedback

What should I do?

1. Retry Stage 3 - shell-agent will regenerate APVTS
2. Check parameter-spec.md - Verify parameter definitions
3. Manual fix - Edit PluginProcessor.cpp manually then continue
4. Pause workflow - Save progress and investigate

Choose (1-4): _
```

---

<execution_guidance>
  <critical_reminders>
    <reminder priority="CRITICAL" ref="subagent-dispatch-only">
      NEVER implement stages 2-5 directly - MUST use Task tool to invoke subagents
    </reminder>

    <reminder priority="CRITICAL" ref="stage-completion-checkpoint">
      ALWAYS present decision menu after subagent completes - user MUST confirm next action
    </reminder>

    <reminder priority="HIGH">
      ALWAYS commit after each stage using commitStage() from state-management.md
    </reminder>

    <reminder priority="HIGH">
      ALWAYS update state files (.continue-here.md and PLUGINS.md) after every stage
    </reminder>

    <reminder priority="HIGH">
      ALWAYS inject Required Reading (juce8-critical-patterns.md) to all subagents
    </reminder>
  </critical_reminders>

  <execution_checklist>
    When executing this skill:
    1. Read contracts (architecture.md, plan.md) before starting
    2. Verify preconditions - block if contracts missing
    3. Use Task tool for ALL stages 2-5 - no exceptions
    4. Stage reference files contain subagent prompts, not direct implementation instructions
    5. Decision menus use inline numbered lists, not AskUserQuestion tool
    6. Handoff files preserve orchestration state across sessions
    7. Build failures bubble up from subagents to orchestrator for menu presentation
  </execution_checklist>

  <anti_patterns>
    Common pitfalls to AVOID:

    <anti_pattern severity="CRITICAL" ref="subagent-dispatch-only">
      ❌ Implementing stage logic directly in orchestrator
      ✓ ALWAYS use Task tool to invoke appropriate subagent
    </anti_pattern>

    <anti_pattern severity="CRITICAL" ref="phase-aware-dispatch">
      ❌ Sending "Implement ALL phases" to subagent for Stages 4-5
      ✓ ALWAYS detect phases in plan.md and loop through them one at a time

      This error caused DrumRoulette Stage 5 to fail with compilation errors.
      Phase-aware dispatch (lines 270-578) is MANDATORY for complex plugins.
    </anti_pattern>

    <anti_pattern severity="CRITICAL">
      ❌ Auto-proceeding without user confirmation
      ✓ ALWAYS wait for menu choice after presenting options
    </anti_pattern>

    <anti_pattern severity="HIGH">
      ❌ Skipping phase detection for Stages 4-5 when complexity ≥3
      ✓ Read plan.md to check for phases BEFORE invoking dsp-agent or gui-agent
    </anti_pattern>

    <anti_pattern severity="HIGH">
      ❌ Not updating handoff file after stage completes
      ✓ Update .continue-here.md immediately after subagent returns
    </anti_pattern>

    <anti_pattern severity="HIGH">
      ❌ Skipping decision menu after subagent returns
      ✓ Present context-appropriate menu at every checkpoint
    </anti_pattern>

    <anti_pattern severity="MEDIUM">
      ❌ Proceeding to next stage when tests fail
      ✓ Present investigation menu and wait for user decision
    </anti_pattern>

    <anti_pattern severity="MEDIUM">
      ❌ Not injecting Required Reading to subagents
      ✓ Always pass juce8-critical-patterns.md to prevent repeat mistakes
    </anti_pattern>
  </anti_patterns>
</execution_guidance>
