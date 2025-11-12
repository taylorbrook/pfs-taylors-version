# Changelog

All notable changes to LushVerb will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

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
