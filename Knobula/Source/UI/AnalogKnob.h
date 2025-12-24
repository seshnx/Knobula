/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    AnalogKnob - Custom Rotary Knob Component
  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_core/juce_core.h>
#include "ColorPalette.h"

namespace Aetheri
{
    /**
     * Custom analog-style rotary knob with label and value display
     */
    class AnalogKnob : public juce::Component
    {
    public:
        enum class KnobSize { Large, Medium, Small };
        
        AnalogKnob(const juce::String& labelText, KnobSize size = KnobSize::Large);
        ~AnalogKnob() override = default;
        
        void paint(juce::Graphics& g) override;
        void resized() override;
        
        juce::Slider& getSlider() { return slider; }
        
        void setAccentColor(juce::Colour color);
        void setValueSuffix(const juce::String& suffix) { valueSuffix = suffix; }
        void setShowValue(bool show) { showValue = show; repaint(); }
        void setTooltip(const juce::String& tooltip) { slider.setTooltip(tooltip); }
        
        // Attach to parameter
        void attachToParameter(juce::AudioProcessorValueTreeState& apvts, 
                              const juce::String& paramID);
        
        // Mouse events for double-click reset
        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDoubleClick(const juce::MouseEvent& e) override;
        
    private:
        juce::Slider slider;
        juce::Label nameLabel;
        juce::Label valueLabel;
        
        KnobSize knobSize;
        juce::Colour accentColor;
        juce::String valueSuffix;
        bool showValue = true;
        int normalSensitivity;
        
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
        juce::AudioProcessorValueTreeState* apvts = nullptr;
        juce::String paramID;
        
        void sliderValueChanged();
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalogKnob)
    };
    
    /**
     * Dual-knob component for Main Gain + Fine Trim
     */
    class DualKnobControl : public juce::Component
    {
    public:
        DualKnobControl(const juce::String& bandName, juce::Colour bandColor);
        ~DualKnobControl() override = default;
        
        void paint(juce::Graphics& g) override;
        void resized() override;
        
        AnalogKnob& getMainKnob() { return mainKnob; }
        AnalogKnob& getTrimKnob() { return trimKnob; }
        
        void attachToParameters(juce::AudioProcessorValueTreeState& apvts,
                               const juce::String& mainParamID,
                               const juce::String& trimParamID);
        
    private:
        AnalogKnob mainKnob;
        AnalogKnob trimKnob;
        juce::String bandName;
        juce::Colour bandColor;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DualKnobControl)
    };
}
