---
name: plugin-packaging
description: Create branded PKG installers for plugin distribution
allowed-tools:
  - Bash
  - Read
  - Write
preconditions:
  - Plugin status must be 📦 Installed
  - VST3 and AU binaries exist in system folders
---

# plugin-packaging Skill

**Purpose:** Create professional PKG installers for sharing plugins with others.

## Overview

Generates macOS PKG installers with:
- Branded Welcome/ReadMe/Conclusion screens (TÂCHES branding)
- Automated installation to user's plugin folders
- Gatekeeper bypass instructions
- User-friendly distribution package

## Workflow

<critical_sequence enforcement="strict" blocking="true">

### 1. Verify Prerequisites

Check plugin is ready for packaging:
- Read PLUGINS.md, verify status is 📦 Installed
- Verify VST3 exists: `~/Library/Audio/Plug-Ins/VST3/[ProductName].vst3`
- Verify AU exists: `~/Library/Audio/Plug-Ins/Components/[ProductName].component`

**Blocking:** If not installed, guide user to run `/install-plugin [Name]` first.

### 2. Extract Plugin Metadata

Gather information for branding files:
- Read PLUGINS.md entry for plugin:
  - Version number
  - Description
  - Parameter list (name, range, defaults)
  - Use cases
- Read `plugins/[Name]/CMakeLists.txt` for PRODUCT_NAME
- Store metadata for template population

### 3. Create Branding Files

Generate three branding text files using templates from `assets/`:

**Welcome.txt:**
- Plugin name with TÂCHES branding
- Brief intro (1-2 sentences)
- What's being installed (VST3, AU, parameter count)
- "Click Continue to begin"

**ReadMe.txt:**
- Full feature list
- Parameter descriptions with ranges
- Installation location details
- Gatekeeper bypass instructions (step-by-step)
- System requirements
- Support contact info

**Conclusion.txt:**
- Installation success message
- Where to find plugin in DAW
- Quick start preset suggestions (3-5 settings)
- Thank you message with TÂCHES signature

See `references/branding-templates.md` for complete template structure.

### 4. Build Base Package

Create foundational PKG with installation logic:

**4a. Create temp directory structure:**
```bash
mkdir -p /tmp/[Name]-installer/payload/[Name]
mkdir -p /tmp/[Name]-installer/scripts
```

**4b. Copy binaries to payload:**
```bash
cp -R ~/Library/Audio/Plug-Ins/VST3/[ProductName].vst3 /tmp/[Name]-installer/payload/[Name]/
cp -R ~/Library/Audio/Plug-Ins/Components/[ProductName].component /tmp/[Name]-installer/payload/[Name]/
```

**4c. Create postinstall script:**
- Script gets actual user (not root during install)
- Creates plugin directories if needed
- Copies plugins from /tmp to user's ~/Library
- Sets correct ownership and permissions
- Cleans up temp files

See `references/pkg-creation.md` Section 4c for complete script.

**4d. Run pkgbuild:**
```bash
pkgbuild --root payload \
         --scripts scripts \
         --identifier com.taches.[pluginname] \
         --version [X.Y.Z] \
         --install-location /tmp \
         [Name]-Installer.pkg
```

### 5. Build Branded Installer

Wrap base package with branding:

**5a. Create Distribution.xml:**
- Title: "[PluginName] by TÂCHES"
- Organization: com.taches
- Reference branding files (Welcome/ReadMe/Conclusion)
- Reference base PKG

See `references/pkg-creation.md` Section 5a for complete XML structure.

**5b. Run productbuild:**
```bash
productbuild --distribution Distribution.xml \
             --resources resources \
             --package-path . \
             "[Name]-by-TACHES.pkg"
```

Result: Branded PKG with custom installer screens.

### 6. Output Distribution Package

Finalize and present to user:

**6a. Create dist directory:**
```bash
mkdir -p plugins/[Name]/dist
```

**6b. Copy installer:**
```bash
cp /tmp/[Name]-installer/[Name]-by-TACHES.pkg plugins/[Name]/dist/
```

**6c. Generate install-readme.txt:**
- File list (what to send)
- Installation steps
- Gatekeeper bypass instructions
- Troubleshooting tips

See `references/distribution-guide.md` for complete README template.

**6d. Display summary:**
```
✓ [PluginName] packaged successfully

Created: plugins/[Name]/dist/[Name]-by-TACHES.pkg (X.X MB)

Distribution package includes:
- [Name]-by-TACHES.pkg (branded installer)
- install-readme.txt (installation guide)

Send both files to your friend.
```

</critical_sequence>

---

## Integration Points

**Invoked by:**
- `/package [PluginName]` command
- Natural language: "Create installer for TapeAge", "Package GainKnob"

**Invokes:**
- None (terminal skill - does not invoke other skills)

**Reads:**
- `PLUGINS.md` → Plugin metadata
- `plugins/[Name]/CMakeLists.txt` → PRODUCT_NAME extraction
- `~/Library/Audio/Plug-Ins/VST3/[Product].vst3` → Source binary
- `~/Library/Audio/Plug-Ins/Components/[Product].component` → Source binary

**Creates:**
- `plugins/[Name]/dist/[Name]-by-TACHES.pkg` → Branded installer
- `plugins/[Name]/dist/install-readme.txt` → Installation guide

**Updates:**
- None (packaging doesn't modify plugin state)

---

## Decision Menu

<decision_gate type="checkpoint" enforcement="strict">

After successful packaging, present this menu and WAIT for user response:

```
✓ [PluginName] packaged successfully

Created: plugins/[Name]/dist/[Name]-by-TACHES.pkg (X.X MB)

What's next?
1. Test installer (recommended) → Verify PKG works correctly
2. Package another plugin → /package [OtherName]
3. View installation guide → Show install-readme.txt contents
4. Share distribution files → Instructions for sending to friend
5. Other

Choose (1-5): _
```

**Option handlers:**
1. **Test installer** → Guide user to test on different user account or provide testing checklist
2. **Package another** → Prompt for plugin name, re-invoke skill
3. **View guide** → Display install-readme.txt with `cat` command
4. **Share instructions** → Explain what files to send, how to compress if needed
5. **Other** → Open-ended response

</decision_gate>

---

## Error Handling

Common error scenarios:

**Plugin not installed:**
- Error: "Cannot package [Name] - plugin not installed"
- Solution: Guide to run `/install-plugin [Name]` first

**Binaries not found:**
- Error: "VST3 or AU not found in system folders"
- Solution: Verify installation, check PRODUCT_NAME matches

**pkgbuild/productbuild failed:**
- Error: Display build tool error message
- Solution: Check permissions, disk space, tool installation

**Brand name missing:**
- Error: "TÂCHES branding not found in templates"
- Solution: Verify assets/ directory has template files

See `references/error-handling.md` for complete error catalog.

---

## Success Criteria

Packaging succeeds when:
- ✅ Base PKG created with postinstall script
- ✅ Branded PKG created with Welcome/ReadMe/Conclusion screens
- ✅ Installer file copied to `plugins/[Name]/dist/`
- ✅ Installation guide generated
- ✅ File sizes reported (PKG should be 3-5 MB typically)
- ✅ User knows what files to share

**NOT required for success:**
- Actually testing installer (recommended but not blocking)
- Signing/notarization (future enhancement)
- Multi-format packages (DMG, ZIP - future modes)

---

## Notes for Claude

**When executing this skill:**

1. Always verify plugin is installed first (check both VST3 and AU exist)
2. Extract PRODUCT_NAME carefully (may contain spaces, use quotes)
3. Populate branding templates with actual plugin metadata (not placeholders)
4. Postinstall script must handle user detection (can't assume /Users/[name])
5. Clean up temp files after success (`rm -rf /tmp/[Name]-installer`)
6. Report file size to user (helpful for sharing over email/Dropbox)
7. Present decision menu and wait (checkpoint protocol)

**Branding consistency:**
- Always use "TÂCHES" in title, welcome, conclusion
- Format: "[PluginName] by TÂCHES"
- Maintain professional tone (helpful, not sales-y)
- Include Gatekeeper steps in ReadMe (critical for unsigned plugins)

**Future enhancements:**
- DMG packaging (drag-and-drop style)
- ZIP packaging (simplest, no installer)
- Code signing integration (if developer account added)
- Windows installer support (NSIS/Inno Setup)
- Multi-plugin packages (bundle multiple plugins)
