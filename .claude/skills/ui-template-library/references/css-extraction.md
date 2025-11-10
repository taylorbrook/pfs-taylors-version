# CSS Extraction Logic

This document describes how to extract visual system components from HTML mockups to create aesthetic templates.

## Overview

When saving an aesthetic, analyze the HTML/CSS to extract the **visual language** (colors, styling, spacing) while ignoring the **layout specifics** (number of controls, positions, grid structure).

## Extraction Process

### 1. Color Palette Extraction

Extract all unique color values from:
- CSS custom properties (`--color-*` variables)
- Inline styles
- CSS rules for body, containers, controls

**Method:**
```javascript
// Extract CSS custom properties
const rootStyles = getComputedStyle(document.documentElement);
const cssVars = {
    background: rootStyles.getPropertyValue('--bg-color').trim(),
    primary: rootStyles.getPropertyValue('--primary-color').trim(),
    accent: rootStyles.getPropertyValue('--accent-color').trim(),
    text: rootStyles.getPropertyValue('--text-color').trim(),
};

// Extract from computed styles
const bodyBg = getComputedStyle(document.body).backgroundColor;
const controlColor = getComputedStyle(document.querySelector('.control')).backgroundColor;
```

**Color categorization:**
- `background`: Main body/container background
- `surface`: Secondary backgrounds (panels, sections)
- `primary`: Main accent color (active controls, highlights)
- `secondary`: Supporting color
- `accent`: Additional emphasis color
- `text_primary`: Main text color
- `text_secondary`: Muted/secondary text

### 2. Typography Extraction

Extract font families, sizes, and weights from:
- Headings (h1, h2, etc.)
- Body text
- Labels
- Monospace elements (values, codes)

**Method:**
```javascript
const typography = {
    font_family: getComputedStyle(document.body).fontFamily,
    font_sizes: {
        large: getComputedStyle(document.querySelector('h1')).fontSize,
        medium: getComputedStyle(document.body).fontSize,
        small: getComputedStyle(document.querySelector('.label')).fontSize,
    },
    font_weights: {
        bold: parseInt(getComputedStyle(document.querySelector('h1')).fontWeight),
        normal: parseInt(getComputedStyle(document.body).fontWeight),
    },
};
```

### 3. Control Styling Extraction

For each control type present (knob, slider, button, toggle), extract:
- Dimensions (width, height, diameter)
- Colors (background, border, active state)
- Border styles
- Shadow effects
- Interaction states (hover, active)

**Method for rotary knobs:**
```javascript
const knobElement = document.querySelector('.rotary-knob');
if (knobElement) {
    const knobStyles = getComputedStyle(knobElement);
    const controls.rotary_knob = {
        diameter: parseInt(knobStyles.width),
        style: detectKnobStyle(knobElement),  // 'tick-marks', 'smooth', 'vintage'
        tick_count: countTickMarks(knobElement),
        center_dot: hasCenterDot(knobElement),
        border: knobStyles.border,
        background: knobStyles.backgroundColor,
        shadow: knobStyles.boxShadow,
    };
}
```

**Method for vertical sliders:**
```javascript
const sliderElement = document.querySelector('.vertical-slider');
if (sliderElement) {
    const sliderStyles = getComputedStyle(sliderElement);
    controls.vertical_slider = {
        width: parseInt(sliderStyles.width),
        height: parseInt(sliderStyles.height),
        track_color: getTrackColor(sliderElement),
        thumb_color: getThumbColor(sliderElement),
        border: sliderStyles.border,
    };
}
```

**Method for buttons:**
```javascript
const buttonElement = document.querySelector('button, .button');
if (buttonElement) {
    const buttonStyles = getComputedStyle(buttonElement);
    controls.button = {
        padding: buttonStyles.padding,
        background: buttonStyles.backgroundColor,
        text_color: buttonStyles.color,
        border_radius: parseInt(buttonStyles.borderRadius),
        hover_brightness: detectHoverBrightness(buttonElement),
    };
}
```

### 4. Spacing Philosophy Extraction

Analyze gaps between controls to determine spacing system:

**Method:**
```javascript
// Measure gaps between adjacent controls
const controls = document.querySelectorAll('.control');
const gaps = [];
for (let i = 0; i < controls.length - 1; i++) {
    const gap = getGapBetween(controls[i], controls[i + 1]);
    gaps.push(gap);
}

// Determine most common gap (spacing unit)
const avgGap = median(gaps);

// Detect density
const spacing = {
    philosophy: avgGap < 16 ? 'tight' : avgGap < 28 ? 'comfortable' : 'generous',
    between_controls: avgGap,
    section_padding: getSectionPadding(),
    edge_margin: getEdgeMargin(),
    control_label_gap: getLabelGap(),
};
```

### 5. Visual Effects Extraction

Extract decorative effects:
- Background textures or patterns
- Shadow styles and depths
- Border radius values
- Glow/highlight effects

**Method:**
```javascript
const effects = {
    background_texture: detectTexture(document.body),  // 'none', 'subtle-paper', 'noise', etc.
    shadow_depth: detectShadowDepth(),  // 'none', 'subtle', 'medium', 'dramatic'
    border_radius: getMostCommonBorderRadius(),
    glow_on_active: hasActiveGlow(),
};

function detectTexture(element) {
    const bg = getComputedStyle(element).backgroundImage;
    if (bg === 'none') return 'none';
    if (bg.includes('noise')) return 'noise-subtle';
    if (bg.includes('paper')) return 'subtle-paper';
    return 'custom';
}

function detectShadowDepth() {
    const shadows = Array.from(document.querySelectorAll('.control'))
        .map(el => getComputedStyle(el).boxShadow);

    if (shadows.every(s => s === 'none')) return 'none';

    const avgBlur = shadows
        .filter(s => s !== 'none')
        .map(s => parseFloat(s.split(' ')[3]))
        .reduce((sum, val) => sum + val, 0) / shadows.length;

    if (avgBlur < 5) return 'subtle';
    if (avgBlur < 15) return 'medium';
    return 'dramatic';
}
```

### 6. Vibe Inference

Infer aesthetic vibe from visual characteristics:

**Method:**
```javascript
function inferVibe(colors, typography, effects) {
    // Warm colors + textures + serif fonts = vintage
    if (isWarmPalette(colors) && effects.background_texture !== 'none' && isSerifFont(typography)) {
        return 'vintage';
    }

    // Cool colors + flat + sans-serif = modern
    if (isCoolPalette(colors) && effects.shadow_depth === 'subtle' && !effects.background_texture) {
        return 'modern';
    }

    // High contrast + angular + bold = aggressive/industrial
    if (isHighContrast(colors) && effects.shadow_depth === 'dramatic') {
        return 'aggressive';
    }

    // Neutral + minimal shadows + simple = minimal
    if (isNeutralPalette(colors) && effects.shadow_depth === 'none') {
        return 'minimal';
    }

    return 'custom';
}
```

## What NOT to Extract

**Layout-specific information (ignore these):**
- Number of controls
- Control positions (x, y coordinates)
- Grid structure (rows, columns)
- Specific parameter IDs or names
- Window dimensions (except as suggestion)

**Focus on transferable visual characteristics only.**

## Example Extraction

**Input HTML (3-knob compressor):**
```html
<style>
    body {
        background: #2b2015;
        font-family: 'Courier New', monospace;
        color: #f5e6d3;
    }
    .knob {
        width: 70px;
        height: 70px;
        border: 2px solid #d97742;
        border-radius: 50%;
        box-shadow: 0 4px 12px rgba(0, 0, 0, 0.3);
    }
    .controls {
        gap: 32px;
    }
</style>
```

**Extracted Aesthetic:**
```yaml
colors:
  background: "#2b2015"
  primary: "#d97742"
  text_primary: "#f5e6d3"

typography:
  font_family: "'Courier New', monospace"

controls:
  rotary_knob:
    diameter: 70
    border: "2px solid #d97742"
    shadow: "0 4px 12px rgba(0, 0, 0, 0.3)"

spacing:
  between_controls: 32
  philosophy: "generous"

effects:
  shadow_depth: "medium"
  border_radius: 8
```

## Usage in Save Workflow

1. Read HTML mockup file
2. Parse HTML/CSS (using jsdom or similar)
3. Run extraction functions for each category
4. Generate aesthetic.yaml from extracted data
5. Infer vibe and best_for metadata
6. Save to `.claude/aesthetics/[aesthetic-id]/`
