/*
  ==============================================================================
    Aetheri - High-Fidelity Mastering EQ
    PhaseCorrelationMeter - Stereo Phase Correlation Display
  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_core/juce_core.h>

namespace Aetheri
{
    /**
     * Phase correlation meter for stereo signals
     * Shows correlation from -1 (out of phase) to +1 (in phase)
     */
    class PhaseCorrelationMeter : public juce::Component,
                                   public juce::Timer
    {
    public:
        PhaseCorrelationMeter();
        ~PhaseCorrelationMeter() override;
        
        void paint(juce::Graphics& g) override;
        void resized() override;
        void timerCallback() override;
        
        // Update with audio data
        void updateCorrelation(float correlation);
        
    private:
        float currentCorrelation = 0.0f;
        float smoothedCorrelation = 0.0f;
        
        juce::Colour getCorrelationColor(float corr) const;
    };
}

