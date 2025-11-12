# Final Fix Plan: /reset-to-ideation

**Source Analysis:** command-analysis/final-fixes-analysis.md
**Command File:** .claude/commands/reset-to-ideation.md
**Fixes to Apply:** 2 (both path corrections)
**Estimated Time:** 2 minutes

---

## Fix 1: Broken Path - Confirmation Example Reference

**Category:** Critical Bug (Documentation Links)
**Priority:** Must-Fix
**Line:** 109

### Current State
```markdown
See [plugin-lifecycle assets/reset-confirmation-example.txt](../.skills/plugin-lifecycle/assets/reset-confirmation-example.txt) for sample output.
```

### Desired State
```markdown
See [plugin-lifecycle assets/reset-confirmation-example.txt](.claude/skills/plugin-lifecycle/assets/reset-confirmation-example.txt) for sample output.
```

### Operation
**Type:** REPLACE

**Find:** `../.skills/plugin-lifecycle/assets/reset-confirmation-example.txt`
**Replace with:** `.claude/skills/plugin-lifecycle/assets/reset-confirmation-example.txt`

### Verification
- [ ] Link path uses `.claude/skills/` prefix (not `../.skills/`)
- [ ] Reference is on line 109
- [ ] Line count unchanged (126 lines)
- [ ] Markdown link syntax intact

### Token Impact
**Before:** ~1 token
**After:** ~1 token
**Change:** 0 tokens (same length, different path)

---

## Fix 2: Broken Path - Success Example Reference

**Category:** Critical Bug (Documentation Links)
**Priority:** Must-Fix
**Line:** 120

### Current State
```markdown
See [plugin-lifecycle assets/reset-success-example.txt](../.skills/plugin-lifecycle/assets/reset-success-example.txt) for sample output.
```

### Desired State
```markdown
See [plugin-lifecycle assets/reset-success-example.txt](.claude/skills/plugin-lifecycle/assets/reset-success-example.txt) for sample output.
```

### Operation
**Type:** REPLACE

**Find:** `../.skills/plugin-lifecycle/assets/reset-success-example.txt`
**Replace with:** `.claude/skills/plugin-lifecycle/assets/reset-success-example.txt`

### Verification
- [ ] Link path uses `.claude/skills/` prefix (not `../.skills/`)
- [ ] Reference is on line 120
- [ ] Line count unchanged (126 lines)
- [ ] Markdown link syntax intact

### Token Impact
**Before:** ~1 token
**After:** ~1 token
**Change:** 0 tokens (same length, different path)

---

## Execution Order

Fixes can be applied in any order (no dependencies).

**Recommended approach:** Apply both replacements in a single Edit operation using replace_all:

1. Replace all instances of `../.skills/` with `.claude/skills/`

This ensures we catch both references and any others we might have missed.

---

## Line Number Adjustments

Not applicable - fixes don't change line count, no drift concerns.

---

## File Operations Summary

**Files to modify:**
- `.claude/commands/reset-to-ideation.md`

**Files to create:**
- None

**Files to verify:**
- `.claude/skills/plugin-lifecycle/assets/reset-confirmation-example.txt` (should exist)
- `.claude/skills/plugin-lifecycle/assets/reset-success-example.txt` (should exist)

---

## Verification Checklist

After applying all fixes:
- [ ] Command file is valid markdown
- [ ] Both references use `.claude/skills/` prefix
- [ ] Line count is 126 (unchanged)
- [ ] No other instances of `../.skills/` pattern
- [ ] Links would resolve if clicked (paths are correct)
- [ ] No functionality regressions

---

## Rollback Information

**Backup location:** `.claude/commands/.backup-20251112-[timestamp]/reset-to-ideation.md`

**Rollback command:**
```bash
# Create backup first
cp .claude/commands/reset-to-ideation.md .claude/commands/.backup-20251112-$(date +%H%M%S)/reset-to-ideation.md

# If rollback needed
cp .claude/commands/.backup-20251112-[timestamp]/reset-to-ideation.md .claude/commands/reset-to-ideation.md
```

---

## Expected Outcome

**Before:**
- Lines: 126
- Tokens: ~1100
- Issues: 2 broken documentation reference paths

**After:**
- Lines: 126
- Tokens: ~1100
- Issues: None

**Grade Change:** Yellow → Green (fixes documentation link accuracy)

---

## Implementation Commands

### Option A: Replace All (Recommended)
```bash
# Use Edit tool with replace_all=true
old_string: ../.skills/
new_string: .claude/skills/
file_path: .claude/commands/reset-to-ideation.md
replace_all: true
```

### Option B: Individual Replacements
```bash
# Fix 1
Edit line 109: replace path
# Fix 2
Edit line 120: replace path
```

**Recommendation:** Use Option A to catch all instances safely.

---

## Post-Fix Verification Commands

```bash
# Verify no more broken paths
grep -n '\.\.\/\.skills' .claude/commands/reset-to-ideation.md
# (should return nothing)

# Verify correct paths
grep -n '\.claude/skills/' .claude/commands/reset-to-ideation.md
# (should show both lines)

# Verify target files exist
ls -la .claude/skills/plugin-lifecycle/assets/reset-*.txt
# (should list both files)

# Verify line count
wc -l .claude/commands/reset-to-ideation.md
# (should show 126)
```

---

## QA Audit

After completion, run:
```bash
@prompts/008-qa-audit-refactored-command.md @.claude/commands/reset-to-ideation.md
```

**Expected Result:** Green grade with all checks passing
