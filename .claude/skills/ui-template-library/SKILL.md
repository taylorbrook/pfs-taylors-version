---
name: ui-template-library
description: Manage aesthetic templates - save visual systems from completed mockups, apply to new plugins with adaptive layouts
allowed-tools:
  - Read
  - Write
  - Bash
preconditions:
  - .claude/aesthetics/ directory exists
---

# ui-template-library Skill

**Purpose:** Manage aesthetic templates for consistent visual design across plugins.

## Core Concept: Aesthetics vs Layouts

**Aesthetic template** = Visual system (colors, typography, control styling, spacing philosophy, effects)
**NOT** = Layout grid or parameter count

**Example:**
- Aesthetic: "Vintage Hardware" (warm colors, brass knobs, textured background)
- Applied to 3-parameter plugin: Horizontal layout, 3 brass knobs
- Applied to 8-parameter plugin: Grid layout 2x4, 8 brass knobs, same colors/styling

The aesthetic system separates **visual design** from **layout structure**. When you save an aesthetic from a 3-knob compressor and apply it to an 8-parameter reverb, you get:
- **Same visual language:** Colors, control styling, typography, spacing philosophy, effects
- **Different layout:** Appropriate grid/arrangement for the parameter count

This allows building a visually cohesive plugin family without rigid layout constraints.

## Operations

1. **Save aesthetic** - Extract visual system from completed mockup
2. **Apply aesthetic** - Generate mockup with aesthetic + appropriate layout for parameter count
3. **List aesthetics** - Show all saved aesthetics with previews
4. **Delete aesthetic** - Remove aesthetic from library
5. **Update aesthetic** - Refine aesthetic from improved mockup

## Directory Structure

```
.claude/aesthetics/
├── manifest.json                    # Registry of all aesthetics
└── [aesthetic-id]/
    ├── aesthetic.yaml               # Visual system specification
    ├── preview.html                 # Visual reference (original mockup)
    └── metadata.json                # Name, description, source plugin
```

## Workflow: Save Aesthetic

**Input:** Path to finalized mockup HTML
**Output:** New aesthetic in `.claude/aesthetics/[aesthetic-id]/`

### Step 1: Parse Mockup HTML

Extract visual system components from the HTML/CSS:

```javascript
// Extract CSS custom properties (if present)
const cssVars = {
    background: getComputedStyle(document.body).getPropertyValue('--bg-color'),
    accent: getComputedStyle(document.body).getPropertyValue('--accent-color'),
    text: getComputedStyle(document.body).getPropertyValue('--text-color'),
    // ...
};

// Extract control styling
const controlStyles = {
    knobType: detectKnobType(),  // rotary, linear, vintage, modern
    sliderStyle: detectSliderStyle(),  // horizontal, vertical, minimal, skeuomorphic
    buttonStyle: detectButtonStyle(),  // toggle, momentary, flat, 3d
    // ...
};

// Extract typography
const typography = {
    heading: getComputedStyle(document.querySelector('h1')).fontFamily,
    body: getComputedStyle(document.body).fontFamily,
    mono: getComputedStyle(document.querySelector('code')).fontFamily,
    // ...
};

// Extract spacing philosophy
const spacing = {
    unit: detectSpacingUnit(),  // 8px, 10px, 12px grid
    density: detectDensity(),  // compact, comfortable, spacious
};

// Extract visual effects
const effects = {
    shadows: detectShadows(),  // none, subtle, dramatic
    borders: detectBorders(),  // none, subtle, prominent
    textures: detectTextures(),  // none, subtle, prominent
};
```

**See:** `.claude/skills/ui-template-library/references/css-extraction.md` for detailed extraction logic.

### Step 2: Generate Aesthetic YAML

Create `aesthetic.yaml` using template from `.claude/skills/ui-template-library/assets/aesthetic-template.yaml`:

```yaml
# aesthetic.yaml
version: 1.0

colors:
  background: "#2a2a2a"
  backgroundAlt: "#1e1e1e"
  accent: "#ff6b35"
  accentHover: "#ff8555"
  text: "#f0e7d8"
  textSecondary: "#a0a0a0"
  border: "#404040"

typography:
  heading:
    family: "Bebas Neue, sans-serif"
    weight: 700
    size: "24px"
    letterSpacing: "0.05em"
  body:
    family: "Roboto, sans-serif"
    weight: 400
    size: "14px"
    letterSpacing: "0"
  mono:
    family: "Roboto Mono, monospace"
    weight: 400
    size: "12px"

controls:
  knob:
    type: rotary-vintage
    size: 60px
    color: "#c8a882"  # brass
    pointerColor: "#ff6b35"
  slider:
    type: vertical-wood
    height: 100px
    width: 30px
    trackColor: "#4a3929"
    thumbColor: "#c8a882"
  button:
    type: toggle-mechanical
    height: 30px
    onColor: "#ff6b35"
    offColor: "#404040"

spacing:
  unit: 10px
  density: comfortable
  controlGap: 20px
  sectionGap: 40px

effects:
  shadows:
    level: subtle
    color: "rgba(0, 0, 0, 0.3)"
    blur: 10px
  borders:
    width: 1px
    color: "#404040"
    style: solid
  textures:
    background: "noise-subtle.png"
    opacity: 0.05
```

### Step 3: Generate Metadata

Create `metadata.json` using template from `.claude/skills/ui-template-library/assets/metadata-template.json`:

```json
{
  "id": "vintage-hardware-001",
  "name": "Vintage Hardware",
  "description": "Warm, retro, skeuomorphic design with brass controls and textured background",
  "sourcePlugin": "TapeAge",
  "sourceVersion": "v3",
  "created": "2025-11-10T10:30:00Z",
  "usedInPlugins": ["TapeAge"],
  "tags": ["vintage", "warm", "skeuomorphic", "brass", "retro"]
}
```

**Aesthetic ID generation:**
- Slugify aesthetic name: "Vintage Hardware" → "vintage-hardware"
- Append counter: "vintage-hardware-001"
- Increment if ID already exists

### Step 4: Copy Preview HTML

Copy finalized mockup HTML as visual reference:

```bash
cp plugins/$PLUGIN_NAME/.ideas/mockups/v$N-ui.html \
   .claude/aesthetics/$AESTHETIC_ID/preview.html
```

### Step 5: Update Manifest

Update `.claude/aesthetics/manifest.json`:

```json
{
  "version": "1.0",
  "aesthetics": [
    {
      "id": "vintage-hardware-001",
      "name": "Vintage Hardware",
      "description": "Warm, retro, skeuomorphic design",
      "sourcePlugin": "TapeAge",
      "created": "2025-11-10T10:30:00Z",
      "tags": ["vintage", "warm", "skeuomorphic"]
    }
  ]
}
```

**Operations:**
1. Read existing manifest (or create if first aesthetic)
2. Append new aesthetic entry
3. Write updated manifest

### Step 6: Confirmation

Display success message with preview path:

```
✅ Aesthetic saved: "Vintage Hardware" (vintage-hardware-001)

Preview: .claude/aesthetics/vintage-hardware-001/preview.html

You can now apply this aesthetic to new plugins using:
/ui-mockup [PluginName] --aesthetic vintage-hardware-001
```

## Workflow: Apply Aesthetic

**Input:**
- Aesthetic ID
- Target plugin name
- Parameter count and types

**Output:** Generated mockup HTML with aesthetic applied + appropriate layout

### Step 1: Load Aesthetic

Read aesthetic.yaml and metadata.json:

```bash
AESTHETIC_DIR=".claude/aesthetics/$AESTHETIC_ID"
AESTHETIC_YAML="$AESTHETIC_DIR/aesthetic.yaml"
METADATA_JSON="$AESTHETIC_DIR/metadata.json"
```

Parse YAML and JSON to extract:
- Color palette
- Typography definitions
- Control styling rules
- Spacing philosophy
- Visual effects

### Step 2: Analyze Target Plugin

Read parameter-spec.md or creative-brief.md:

```bash
PARAM_SPEC="plugins/$PLUGIN_NAME/.ideas/parameter-spec.md"
BRIEF="plugins/$PLUGIN_NAME/.ideas/creative-brief.md"
```

Extract parameter information:
- Parameter count
- Parameter IDs and names
- Parameter types (FLOAT, BOOL, CHOICE)
- Default values and ranges
- Prominence indicators (Mix, Output, etc.)

Example parsed parameters:
```javascript
const parameters = [
    { id: 'GAIN', name: 'Gain', type: 'FLOAT', prominent: false },
    { id: 'TONE', name: 'Tone', type: 'FLOAT', prominent: false },
    { id: 'MIX', name: 'Mix', type: 'FLOAT', prominent: true },
    { id: 'BYPASS', name: 'Bypass', type: 'BOOL', prominent: false },
];
```

### Step 3: Generate Appropriate Layout

Choose layout based on parameter count:

```javascript
function chooseLayout(paramCount) {
    if (paramCount <= 3) {
        return 'horizontal';  // Single row
    } else if (paramCount <= 6) {
        return 'grid-2x3';  // 2 rows, up to 3 columns
    } else if (paramCount <= 9) {
        return 'grid-3x3';  // 3x3 grid
    } else {
        return 'scrollable-grid';  // Larger grid with scrolling
    }
}

const layout = chooseLayout(paramCount);
```

**See:** `.claude/skills/ui-template-library/references/layout-generation.md` for detailed layout logic.

Separate prominent parameters (Mix, Output) from regular parameters:
```javascript
const prominentParams = parameters.filter(isProminentParameter);
const regularParams = parameters.filter(p => !isProminentParameter(p));
```

Generate layout structure:
- Horizontal: Single row
- Grid 2x3: 2 rows, 3 columns
- Grid 3x3: 3 rows, 3 columns
- Sectioned: Main grid + side sliders for prominent params

### Step 4: Generate HTML with Aesthetic

Combine aesthetic + layout + parameters:

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{{PLUGIN_NAME}}</title>
    <style>
        /* Apply aesthetic colors */
        :root {
            --bg-color: {{aesthetic.colors.background}};
            --accent-color: {{aesthetic.colors.accent}};
            --text-color: {{aesthetic.colors.text_primary}};
            --control-gap: {{aesthetic.spacing.controlGap}};
            --section-padding: {{aesthetic.spacing.sectionPadding}};
            /* ... more CSS vars from aesthetic */
        }

        html, body {
            margin: 0;
            padding: 0;
            height: 100%;  /* NOT 100vh - WebView constraint */
            background: var(--bg-color);
            color: var(--text-color);
            font-family: {{aesthetic.typography.body.family}};
            user-select: none;
        }

        /* Apply layout based on parameter count */
        .controls {
            {{generateLayoutCSS(layout, aesthetic)}}
            gap: var(--control-gap);
            padding: var(--section-padding);
        }

        /* Apply control styling from aesthetic */
        {{generateKnobCSS(aesthetic)}}
        {{generateSliderCSS(aesthetic)}}
        {{generateButtonCSS(aesthetic)}}
    </style>
</head>
<body>
    <h1 style="font-family: {{aesthetic.typography.heading.family}};
               font-size: {{aesthetic.typography.heading.size}};">
        {{PLUGIN_NAME}}
    </h1>

    <div class="controls">
        {{#each parameters}}
        {{generateControlHTML(this, aesthetic)}}
        {{/each}}
    </div>

    <script>
        // Parameter binding setup
        {{generateBindingJS(parameters)}}
    </script>
</body>
</html>
```

**See:** `.claude/skills/ui-template-library/references/control-generation.md` for detailed control generation logic.

### Step 5: Save Generated Mockup

Save to target plugin mockups directory:

```bash
mkdir -p plugins/$PLUGIN_NAME/.ideas/mockups/
echo "<!-- Generated from aesthetic: $AESTHETIC_ID -->" > \
    plugins/$PLUGIN_NAME/.ideas/mockups/v1-ui-test.html
cat generated.html >> \
    plugins/$PLUGIN_NAME/.ideas/mockups/v1-ui-test.html
```

**Naming convention:**
- First application: `v1-ui-test.html`
- User can iterate and rename to `v1-ui.html` when finalized
- Subsequent versions: `v2-ui-test.html`, etc.

### Step 6: Update Aesthetic Metadata

Track usage in metadata.json:

```json
{
  "usedInPlugins": ["TapeAge", "NewPlugin"]
}
```

Update the aesthetic's metadata to include the new plugin in `usedInPlugins` array.

### Step 7: Present Decision Menu

Offer next steps:

```
✅ Mockup v1-test generated with "Vintage Hardware" aesthetic

Files created:
- plugins/ReverbPlugin/.ideas/mockups/v1-ui-test.html

Layout: 3x2 grid (6 knobs) + 2 vertical sliders
Colors: Orange/cream/brown (from aesthetic)
Styling: Tick-mark knobs, paper texture, generous spacing

What's next?
1. Preview in browser - Open v1-ui-test.html
2. Iterate design - Adjust colors/styling manually
3. Finalize - Run ui-mockup skill to generate full scaffolding
4. Choose different aesthetic - Try another template
5. Save modifications - If you tweaked the design, save as new aesthetic
```

## Workflow: List Aesthetics

**Input:** None (or optional filter by tag)
**Output:** Table of all aesthetics with metadata

### Step 1: Read Manifest

```bash
MANIFEST=".claude/aesthetics/manifest.json"
```

Parse manifest.json to get all aesthetic entries:

```javascript
const manifest = JSON.parse(fs.readFileSync('.claude/aesthetics/manifest.json'));
const aesthetics = manifest.aesthetics;
```

### Step 2: Display Table

Format aesthetics as markdown table:

```markdown
# Aesthetic Library

| ID | Name | Description | Source Plugin | Created | Used In |
|----|------|-------------|---------------|---------|---------|
| vintage-hardware-001 | Vintage Hardware | Warm, retro design | TapeAge | 2025-11-10 | TapeAge, CompCo |
| modern-minimal-002 | Modern Minimal | Clean, flat design | EQPro | 2025-11-11 | EQPro |
```

For each aesthetic, display:
- ID (aesthetic-id)
- Name
- Description (truncated if > 50 chars)
- Source plugin
- Created date
- Used in plugins (comma-separated)

### Step 3: Show Preview Paths

List preview file locations:

```
Preview files:
- vintage-hardware-001: .claude/aesthetics/vintage-hardware-001/preview.html
- modern-minimal-002: .claude/aesthetics/modern-minimal-002/preview.html
```

**Usage:**
```bash
open .claude/aesthetics/vintage-hardware-001/preview.html  # macOS
```

### Step 4: Decision Menu

Present options:

```
What would you like to do?
1. Apply aesthetic to plugin - Choose aesthetic and target plugin
2. View aesthetic details - Show full aesthetic.yaml
3. Delete aesthetic - Remove from library
4. Exit
```

## Workflow: Delete Aesthetic

**Input:** Aesthetic ID
**Output:** Aesthetic removed from library

### Step 1: Confirm Deletion

Show usage information before deleting:

```
⚠️  Delete aesthetic "Vintage Hardware" (vintage-hardware-001)?

Currently used in 2 plugins:
- TapeAge
- CompCo

Deleting will NOT affect existing plugins (they have copies of the UI).
This only removes the template from the library.

Are you sure? (yes/no)
```

**Important:** Make it clear that deletion doesn't affect existing plugins, only the template library.

### Step 2: Remove Files

If confirmed, remove aesthetic directory:

```bash
rm -rf .claude/aesthetics/$AESTHETIC_ID
```

This removes:
- aesthetic.yaml
- metadata.json
- preview.html
- Directory itself

### Step 3: Update Manifest

Remove entry from manifest.json:

```javascript
// Read manifest
const manifest = JSON.parse(fs.readFileSync('.claude/aesthetics/manifest.json'));

// Filter out deleted aesthetic
manifest.aesthetics = manifest.aesthetics.filter(
    a => a.id !== aestheticId
);

// Write updated manifest
fs.writeFileSync(
    '.claude/aesthetics/manifest.json',
    JSON.stringify(manifest, null, 2)
);
```

### Step 4: Confirmation

Display success message:

```
✅ Aesthetic "Vintage Hardware" deleted from library.

Plugins using this aesthetic are unaffected (they have independent copies).

Remaining aesthetics: 1
```
