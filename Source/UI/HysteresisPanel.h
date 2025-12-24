/*
  ==============================================================================
    Aetheri - High-Fidelity Mastering EQ
    HysteresisPanel - Analog Saturation Stage Controls
  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_core/juce_core.h>
#include "AnalogKnob.h"
#include "ColorPalette.h"
#include "Parameters.h"

namespace Aetheri
{
    /**
     * Panel for the Analog Hysteresis Stage controls
     * Contains:
     * - Enable/Engage button
     * - Tube Harmonics knob
     * - Transformer Saturate knob
     * - Mix control
     */
    class HysteresisPanel : public juce::Component
    {
    public:
        HysteresisPanel();
        ~HysteresisPanel() override = default;
        
        void paint(juce::Graphics& g) override;
        void resized() override;
        
        void attachToParameters(juce::AudioProcessorValueTreeState& apvts);
        
        // For glow effect
        void setGlowIntensity(float intensity) { glowIntensity = intensity; repaint(); }
        float getGlowIntensity() const { return glowIntensity; }
        bool isEngaged() const { return engageButton.getToggleState(); }
        
    private:
        // Controls
        juce::TextButton engageButton;
        AnalogKnob tubeKnob;
        AnalogKnob transformerKnob;
        AnalogKnob mixKnob;
        
        float glowIntensity = 0.0f;
        
        // Attachments
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> engageAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tubeAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> transformerAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
        
        void drawTubeGlow(juce::Graphics& g, juce::Rectangle<float> bounds);
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HysteresisPanel)
    };
}
