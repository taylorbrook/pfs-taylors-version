# Changelog

All notable changes to LushVerb will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [1.0.3] - 2025-11-12

### Fixed

- Shimmer FFT pitch shifter producing no audible output (three fundamental DSP errors)
  - **Root cause 1:** Backwards pitch shift algorithm - used `i / 2` instead of `i * 2`, creating octave DOWN pattern with 75% of spectrum as zeros
  - **Root cause 2:** Double normalization - divided by fftSize (2048) when JUCE IFFT already normalizes internally, causing ~60dB attenuation
  - **Root cause 3:** Missing window compensation - Hann window applied twice (analysis + synthesis) with 50% overlap required 2.0x gain factor
  - **Impact:** Shimmer parameter had zero effect at any setting (0-100%) - no pitch-shifted harmonics audible
  - **Solution:**
    1. Fixed pitch shift direction: `sourceIndex = i * 2` (double bin index for octave up)
    2. Removed fftSize division from overlap-add (line 223)
    3. Added 2.0x window compensation factor for Hann 50% overlap
  - **Affected components:** PluginProcessor.cpp:207 (pitch shift algorithm), PluginProcessor.cpp:223 (normalization + windowing)

### Technical

- DSP changes:
  - Line 207: Changed `i / 2` to `i * 2` - correctly maps lower frequencies to higher bins for octave up
  - Line 223: Removed `/ (float)fftSize`, added `* 2.0f` compensation
  - FFT pitch shifter now produces audible +1 octave harmonic content
  - Signal flow verified: Input → Shimmer (+1 octave) → Reverb → Modulation → Dry/Wet Mix
  - Research sources: JUCE forum (FFT normalization), DSP theory (phase vocoder pitch shifting)

### Testing

- Manual verification: Load LushVerb, play audio, adjust shimmer 0% → 100%
- Expected behavior: 0% = no shimmer, 50% = subtle octave layer, 100% = prominent shimmer harmonics
- Test cases: Drums (transient response), vocals (sustained tones), synth pads (harmonic content)
- No preset compatibility issues (parameter ranges unchanged)

## [1.0.2] - 2025-11-12

### Fixed

- Shimmer parameter incorrect mix behavior causing silence at 100%
  - Root cause: Used crossfade (replace) instead of parallel add - `dry * (1-amt) + shimmer * amt` replaced entire signal with pitch-shifted component at 100%
  - Impact: Shimmer knob at 0% did nothing (correct), but at 100% removed all audio (went silent)
  - Solution: Changed to additive parallel mix - `dry + shimmer * amt` - now shimmer ADDS octave-up layer to original signal
  - Affected component: PluginProcessor.cpp:181 (shimmer mix algorithm in processBlock)
  - Verified against industry standards: Sound On Sound tutorials, Eventide/Valhalla shimmer reverb architecture research

### Technical

- DSP changes:
  - Line 181: Changed from crossfade to parallel add
  - New behavior: 0% = dry only, 50% = dry + 50% shimmer, 100% = dry + full shimmer octave
  - Shimmer routing verified: Pre-reverb placement correct (matches architecture.md specification)
  - No gain compensation added (shimmer signal already normalized by FFT windowing)

### Testing

- Regression testing: Compare shimmer behavior at 0%, 50%, 100% settings
- Expected: Dry signal always present, shimmer adds octave-up layer proportionally
- No preset compatibility issues (parameter ranges unchanged)

## [1.0.1] - 2025-11-11

### Fixed

- WebView knobs frozen issue - knobs now respond to mouse drag
  - Root cause: ES6 module export/import mismatch in index.html
  - Impact: All four parameter knobs were non-interactive (couldn't rotate or update)
  - Symptoms: Knobs displayed correctly but didn't respond to drag, DAW automation worked but UI didn't reflect changes

### Technical

- UI changes:
  - Added `type="module"` to script tags in index.html
  - Added ES6 import statement: `import { getSliderState } from './js/juce/index.js'`
  - Changed API call from `window.__JUCE__.backend.getSliderState()` to direct `getSliderState()` call
  - Fixed JavaScript module loading to enable proper JUCE WebView integration

### Testing

- Manual verification: Knob rotation, drag sensitivity, DAW automation reflection
- No regression - presets and parameter values unaffected

## [1.0.0] - 2025-11-11

### Added
- Initial release of LushVerb
- Core reverb engine with SIZE (0.5-20s) and DAMPING (0-100%) parameters
- Shimmer pitch shifter (+1 octave) with SHIMMER parameter (0-100%)
- Built-in modulation system (dual LFO at 0.3Hz and 0.5Hz for lush depth)
- MIX parameter for dry/wet blending (0-100%)
- WebView UI with industrial 19" rack aesthetic
- 4 interactive rotary knobs with parameter value displays
- Real-time LED output meter with ballistic motion (green/yellow/red zones)
- Factory presets: Default, Small Room, Large Hall, Shimmer Pad, Dark Ambient, Bright Plate, Instant Inspiration
- VST3, AU, and Standalone formats

### Technical Details
- FFT-based phase vocoder (2048-point, 4x overlap) for high-quality shimmer
- Modern juce::dsp::Reverb algorithm with built-in modulation
- Lagrange 3rd-order interpolation for smooth pitch modulation
- 500×300px window size
- Latency: ~46ms (2048 samples at 44.1kHz due to shimmer FFT processing)
- JUCE 8 codebase with modern API patterns
