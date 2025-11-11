<objective>
Update ALL pluginval references in the plugin-freedom-system to use the full macOS application path instead of assuming it's in PATH.
This ensures pluginval validation works reliably regardless of whether users have created symlinks or modified their PATH.
</objective>

<context>
The system currently uses `which pluginval` to check installation and bare `pluginval` commands to run validation.
This fails when pluginval.app is installed (via Homebrew cask or manual download) but not symlinked to PATH.
The actual binary exists at: /Applications/pluginval.app/Contents/MacOS/pluginval
This is a JUCE plugin development system that runs validation as part of the testing workflow.
</context>

<requirements>
1. Find EVERY file that invokes or checks for pluginval
2. Update command invocations from `pluginval` to `/Applications/pluginval.app/Contents/MacOS/pluginval`
3. Update installation checks from `which pluginval` to checking if the .app exists
4. Ensure fallback behavior if pluginval.app doesn't exist at expected location
5. Maintain backward compatibility for users who have pluginval in PATH
6. Update documentation to reflect new path expectations
</requirements>

<files_to_update>
Primary files that execute pluginval (MUST update):
- .claude/skills/plugin-testing/SKILL.md
- .claude/skills/plugin-testing/references/pluginval-guide.md

Secondary files to check and update if needed:
- .claude/agents/validator.md (if it runs pluginval)
- .claude/skills/plugin-workflow/references/stage-6-validation.md (if it has commands)
- .claude/commands/test.md (if it has direct commands)
- Any other file that might programmatically invoke pluginval
</files_to_update>

<implementation_strategy>
1. First, search comprehensively for ALL pluginval references:
   - Grep for "pluginval" in .claude/ directory
   - Grep for "pluginval" in scripts/ directory
   - Check for any bash scripts that might invoke it

2. For installation checks, replace:
   ```bash
   which pluginval
   ```
   With:
   ```bash
   # Check for pluginval.app first, then fall back to PATH
   if [ -x "/Applications/pluginval.app/Contents/MacOS/pluginval" ]; then
       PLUGINVAL_PATH="/Applications/pluginval.app/Contents/MacOS/pluginval"
   elif command -v pluginval >/dev/null 2>&1; then
       PLUGINVAL_PATH="pluginval"
   else
       echo "Pluginval not found"
       exit 1
   fi
   ```

3. For execution, replace bare `pluginval` with:
   ```bash
   "${PLUGINVAL_PATH:-/Applications/pluginval.app/Contents/MacOS/pluginval}" --validate ...
   ```

4. Update error messages to guide users:
   ```
   Pluginval not found. Please install:
   - Via Homebrew: brew install --cask pluginval
   - Or download from: https://github.com/Tracktion/pluginval/releases
   Expected location: /Applications/pluginval.app
   ```
</implementation_strategy>

<verification>
After making changes:
1. Grep for bare "pluginval" commands (should only appear in documentation/comments)
2. Verify all execution paths use either $PLUGINVAL_PATH or full path
3. Test that installation detection works with both .app and PATH scenarios
4. Confirm error messages are helpful when pluginval is missing
5. Check that no references were missed in auxiliary files
</verification>

<success_criteria>
- Every pluginval invocation uses full path or variable containing full path
- Installation checks work for both .app bundle and PATH installations
- Clear error messages guide users when pluginval is missing
- No bare "pluginval" commands remain in executable contexts
- Documentation reflects the new path strategy
</success_criteria>