# Knobula - High-Fidelity Dual-Channel Mastering EQ
# CMake Build Configuration for JUCE Plugin
cmake_minimum_required(VERSION 3.22)

project(Knobula VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE - Configure this path to your JUCE installation
# Option 1: Set JUCE_DIR environment variable
# Option 2: Use FetchContent to download JUCE
include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.9
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(JUCE)

# Plugin Configuration
juce_add_plugin(Knobula
    COMPANY_NAME "SeshNx"
    BUNDLE_ID "com.seshnx.knobula"
    PLUGIN_MANUFACTURER_CODE Knob
    PLUGIN_CODE Knob
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Knobula"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

# Source files - DSP
set(DSP_SOURCES
    Source/DSP/PassiveEQ.h
    Source/DSP/PassiveEQ.cpp
    Source/DSP/HysteresisProcessor.h
    Source/DSP/HysteresisProcessor.cpp
    Source/DSP/VUMeter.h
    Source/DSP/VUMeter.cpp
    Source/DSP/MidSideProcessor.h
    Source/DSP/MidSideProcessor.cpp
    Source/DSP/HighLowPassFilters.h
    Source/DSP/HighLowPassFilters.cpp
)

# Source files - UI
set(UI_SOURCES
    Source/UI/KnobulaLookAndFeel.h
    Source/UI/KnobulaLookAndFeel.cpp
    Source/UI/AnalogKnob.h
    Source/UI/AnalogKnob.cpp
    Source/UI/VUMeterComponent.h
    Source/UI/VUMeterComponent.cpp
    Source/UI/NebulaVisualizer.h
    Source/UI/NebulaVisualizer.cpp
    Source/UI/BandControl.h
    Source/UI/BandControl.cpp
    Source/UI/HysteresisPanel.h
    Source/UI/HysteresisPanel.cpp
    Source/UI/TubeGlowEffect.h
    Source/UI/TubeGlowEffect.cpp
)

# Source files - Utils
set(UTIL_SOURCES
    Source/Utils/Parameters.h
    Source/Utils/Parameters.cpp
    Source/Utils/ColorPalette.h
)

# Main plugin sources
set(PLUGIN_SOURCES
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
)

# Combine all sources
target_sources(Knobula
    PRIVATE
        ${DSP_SOURCES}
        ${UI_SOURCES}
        ${UTIL_SOURCES}
        ${PLUGIN_SOURCES}
)

# Compiler definitions
target_compile_definitions(Knobula
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

# Link with JUCE modules
target_link_libraries(Knobula
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_extra
        juce::juce_opengl
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Include directories
target_include_directories(Knobula
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/DSP
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/UI
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Utils
)
