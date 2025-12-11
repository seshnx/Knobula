# Knobula - Build Instructions

## Quick Start

### Prerequisites
- CMake 3.22+
- C++17 compiler (Clang, GCC 9+, MSVC 2019+)
- Git (for JUCE download)

### Build Commands

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release -j$(nproc)
```

### Output Locations
After building, plugins are located in:
- `build/Knobula_artefacts/Release/VST3/Knobula.vst3`
- `build/Knobula_artefacts/Release/AU/Knobula.component` (macOS)
- `build/Knobula_artefacts/Release/Standalone/Knobula`

## Development

### IDE Setup

#### Visual Studio Code
```bash
cmake -B build -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

#### Xcode (macOS)
```bash
cmake -B build -G Xcode
open build/Knobula.xcodeproj
```

#### Visual Studio (Windows)
```bash
cmake -B build -G "Visual Studio 17 2022"
start build/Knobula.sln
```

### Debug Build
```bash
cmake -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug
```

## Architecture Overview

```
┌─────────────────────────────────────────────────────────┐
│                    PluginProcessor                       │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────┐  │
│  │ FilterSection│  │  PassiveEQ  │  │HysteresisProcessor│ │
│  │  (HPF/LPF)  │→ │  (4-Band)   │→ │  (Tube+XFMR)   │  │
│  └─────────────┘  └─────────────┘  └─────────────────┘  │
│         ↓                ↓                  ↓            │
│  ┌─────────────────────────────────────────────────┐    │
│  │              VU Metering (Input/Output)          │    │
│  └─────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────┘
                           ↓
┌─────────────────────────────────────────────────────────┐
│                     PluginEditor                         │
│  ┌─────────────────────────────────────────────────┐    │
│  │              NebulaVisualizer (Background)       │    │
│  └─────────────────────────────────────────────────┘    │
│  ┌──────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌────┐│
│  │VU In │ │Channel L │ │Hysteresis│ │Channel R │ │VUOut││
│  │Meter │ │EQ Strip  │ │  Panel   │ │EQ Strip  │ │Meter││
│  └──────┘ └──────────┘ └──────────┘ └──────────┘ └────┘│
│  ┌─────────────────────────────────────────────────┐    │
│  │              TubeGlowEffect (Overlay)            │    │
│  └─────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────┘
```

## Key DSP Classes

### PassiveEQ
64-bit biquad filter implementation with:
- Bell and Shelf filter types
- Per-band frequency, gain, and Q control
- M/S encoding/decoding
- Smoothed parameter changes

### HysteresisProcessor
Dual-character analog saturation:
- Tube stage: Odd-order harmonics via asymmetric soft clipping
- Transformer stage: Even-order harmonics with magnetic hysteresis modeling
- DC blocking and mix control

### VUMeter
Authentic VU ballistics:
- 300ms integration time
- Spring-damper physics for needle movement
- Peak hold with decay

## UI Components

| Component | Description |
|-----------|-------------|
| `NebulaVisualizer` | Frequency-reactive particle system |
| `VUMeterComponent` | Vintage needle-style meter |
| `AnalogKnob` | Custom rotary slider with glow |
| `BandControl` | Complete EQ band with all controls |
| `HysteresisPanel` | Saturation stage controls |
| `TubeGlowEffect` | Warm amber overlay effect |

## Color Theme

```cpp
// Band Colors (Nebula)
bandLF   = 0xFF4488FF  // Blue
bandLMF  = 0xFF44DD88  // Green  
bandHMF  = 0xFFDDCC44  // Yellow
bandHF   = 0xFFFF6644  // Red/Orange

// Panel Colors
panelBackground = 0xFF1A1A1E  // Deep charcoal
panelSurface    = 0xFF242428  // Slightly lighter
tubeGlowOn      = 0x44FF9944  // Warm amber
```
