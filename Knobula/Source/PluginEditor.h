/*
  ==============================================================================
    Knobula - High-Fidelity Dual-Channel Mastering EQ
    PluginEditor - Main UI with Analog Nebula Theme
    
    Features:
    - Fully resizable interface
    - Vintage analog hardware aesthetic
    - Nebula particle visualizer background
    - VU meters for Input/Output
    - Tube glow effect when Hysteresis is active
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/KnobulaLookAndFeel.h"
#include "UI/NebulaVisualizer.h"
#include "UI/VUMeterComponent.h"
#include "UI/BandControl.h"
#include "UI/HysteresisPanel.h"
#include "UI/TubeGlowEffect.h"
#include "UI/AnalogKnob.h"

class KnobulaAudioProcessorEditor : public juce::AudioProcessorEditor,
                                     public juce::Timer
{
public:
    KnobulaAudioProcessorEditor(KnobulaAudioProcessor&);
    ~KnobulaAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    KnobulaAudioProcessor& audioProcessor;
    
    // Custom look and feel
    Knobula::KnobulaLookAndFeel lookAndFeel;
    
    // Background visualizer
    Knobula::NebulaVisualizer nebulaVisualizer;
    
    // Tube glow overlay
    Knobula::TubeGlowEffect tubeGlow;
    
    // VU Meters
    Knobula::StereoVUMeterComponent inputVU;
    Knobula::StereoVUMeterComponent outputVU;
    
    // EQ Channel Strips (L/R or M/S)
    Knobula::ChannelEQStrip leftChannelStrip;
    Knobula::ChannelEQStrip rightChannelStrip;
    
    // Hysteresis Panel
    Knobula::HysteresisPanel hysteresisPanel;
    
    // Global Controls
    Knobula::AnalogKnob inputGainKnob;
    Knobula::AnalogKnob outputTrimKnob;
    
    // Filter Section
    juce::TextButton hpfButton;
    juce::TextButton lpfButton;
    Knobula::AnalogKnob hpfFreqKnob;
    Knobula::AnalogKnob lpfFreqKnob;
    
    // Stereo Mode / Link
    juce::ComboBox stereoModeSelector;
    juce::TextButton channelLinkButton;
    
    // Parameter attachments for global controls
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputTrimAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> hpfButtonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hpfFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> lpfButtonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lpfFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> stereoModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> channelLinkAttachment;
    
    // Layout helpers
    void setupGlobalControls();
    void setupFilterSection();
    void updateChannelLabels();
    void updateVUMeters();
    void updateNebulaEnergies();
    void updateTubeGlow();

    // Resizing constraints
    juce::ComponentBoundsConstrainer constrainer;
    std::unique_ptr<juce::ResizableCornerComponent> resizer;
    
    static constexpr int DEFAULT_WIDTH = 1000;
    static constexpr int DEFAULT_HEIGHT = 600;
    static constexpr int MIN_WIDTH = 800;
    static constexpr int MIN_HEIGHT = 500;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobulaAudioProcessorEditor)
};
