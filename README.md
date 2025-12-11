# Knobula

## High-Fidelity Dual-Channel Mastering EQ

Knobula is a professional VST3/AU audio plugin built with JUCE, featuring a 4-band passive topology EQ with an integrated analog hysteresis stage. Designed for mastering applications, it combines high-fidelity 64-bit internal processing with a stunning "Analog Nebula" visual theme.

![Knobula Mastering EQ](docs/knobula-preview.png)

---

## Features

### 🎛️ 4-Band Passive Topology EQ
- **Dual-Channel Processing**: Independent L/R or M/S processing modes
- **Per-Band Controls**:
  - Main Gain (±10dB) - Large chunky knobs for primary adjustments
  - Fine Trim (±1dB) - Precision gain trimming for mastering
  - Frequency control with musical ranges per band
  - Bell/Shelf curve selection (LF and HF bands)
- **Musical Q Values**: Fixed, broad Q values optimized for mastering (0.6-0.9)
- **64-bit Internal Processing**: Maximum precision and headroom

### 🌟 Analog Hysteresis Stage
- **Tube Harmonics**: High-frequency, odd-order harmonic generation for air and sheen
- **Transformer Saturate**: Low-frequency, even-order harmonic generation for thickness and glue
- **Switchable Engage/Bypass**: Clean signal path when disabled
- **Mix Control**: Blend dry and saturated signals

### 📊 Professional Metering
- **VU Meters**: Large, slow-moving RMS meters with vintage needle-style display
- **Input & Output Metering**: Monitor gain staging at all times
- **Peak Indicators**: Visual peak detection with hold and decay

### 🎨 Analog Nebula UI Theme
- **Vintage Aesthetic**: Dark panel with large, chunky analog-style knobs
- **Nebula Visualizer**: Subtle, low-CPU particle background that reacts to frequency band energy
  - LF = Blue stars, LMF = Green, HMF = Yellow, HF = Red
- **Tube Glow Effect**: Warm amber glow overlay when Hysteresis is engaged
- **Fully Resizable**: Scale the interface to your preference (800x500 to 1600x1000)

### 🔧 Global Controls
- **Input Gain**: Pre-EQ gain staging (±12dB)
- **Output Trim**: Final output level adjustment (±12dB)
- **High-Pass Filter**: Switchable 12dB/oct HPF (20-500Hz)
- **Low-Pass Filter**: Switchable 12dB/oct LPF (2-20kHz)
- **Stereo Mode**: L/R or M/S processing selection
- **Channel Link**: Link both channels for stereo operation

---

## Frequency Bands

| Band | Name | Default Frequency | Range | Q Value |
|------|------|------------------|-------|---------|
| 1 | LF (Low Frequency) | 80 Hz | 20-300 Hz | 0.6 (very broad) |
| 2 | LMF (Low-Mid Frequency) | 400 Hz | 100-1500 Hz | 0.8 (broad) |
| 3 | HMF (High-Mid Frequency) | 2.5 kHz | 500-8000 Hz | 0.9 (slightly narrow) |
| 4 | HF (High Frequency) | 8 kHz | 2-20 kHz | 0.7 (broad) |

---

## Building

### Prerequisites
- CMake 3.22 or higher
- C++17 compatible compiler
- JUCE 7.0.9 (automatically downloaded via FetchContent)

### Build Steps

```bash
# Navigate to the project directory
cd Knobula

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the plugin
cmake --build . --config Release

# The built plugins will be in:
# - VST3: build/Knobula_artefacts/Release/VST3/
# - AU: build/Knobula_artefacts/Release/AU/ (macOS only)
# - Standalone: build/Knobula_artefacts/Release/Standalone/
```

### Supported Platforms
- **Windows**: VST3, Standalone
- **macOS**: VST3, AU, Standalone
- **Linux**: VST3, Standalone

---

## Project Structure

```
Knobula/
├── CMakeLists.txt              # Build configuration
├── Source/
│   ├── PluginProcessor.h/cpp   # Main audio processing
│   ├── PluginEditor.h/cpp      # Main UI editor
│   ├── DSP/
│   │   ├── PassiveEQ.h/cpp           # 4-band passive EQ core
│   │   ├── HysteresisProcessor.h/cpp # Analog saturation stage
│   │   ├── VUMeter.h/cpp             # VU level metering
│   │   ├── HighLowPassFilters.h/cpp  # HPF/LPF filters
│   │   └── MidSideProcessor.h/cpp    # M/S encoding/decoding
│   ├── UI/
│   │   ├── KnobulaLookAndFeel.h/cpp  # Custom visual theme
│   │   ├── AnalogKnob.h/cpp          # Custom rotary knobs
│   │   ├── VUMeterComponent.h/cpp    # Vintage VU meter display
│   │   ├── NebulaVisualizer.h/cpp    # Particle background
│   │   ├── BandControl.h/cpp         # EQ band controls
│   │   ├── HysteresisPanel.h/cpp     # Saturation controls
│   │   └── TubeGlowEffect.h/cpp      # Glow overlay effect
│   └── Utils/
│       ├── Parameters.h/cpp          # Parameter definitions
│       └── ColorPalette.h            # Color theme constants
```

---

## Technical Specifications

### Audio Processing
- **Sample Rates**: 44.1 kHz to 192 kHz
- **Internal Precision**: 64-bit floating point
- **Latency**: Zero latency (IIR filters)
- **CPU Optimization**: Smoothed parameters, efficient DSP algorithms

### UI Performance
- **Nebula Visualizer**: 24 FPS, 120 particles, minimal CPU overhead
- **VU Meters**: 30 FPS, physics-based needle animation
- **Resizing**: Maintains aspect ratio and layout proportions

### Parameter Smoothing
- All parameters feature 20-50ms smoothing for click-free adjustments
- VU meters use 300ms integration time per VU standard

---

## License

This project is provided as a framework/template for audio plugin development. 
See LICENSE file for details.

---

## Credits

Built with [JUCE](https://juce.com/) - the cross-platform C++ framework for audio applications.

**Knobula Audio** - High-Fidelity Audio Tools
