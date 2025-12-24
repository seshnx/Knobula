/*
  ==============================================================================
    Aetheri - High-Fidelity Mastering EQ
    TubeGlowEffect - Warm Amber Glow Overlay
  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_core/juce_core.h>
#include "ColorPalette.h"

namespace Aetheri
{
    /**
     * Overlay component that adds a warm tube glow effect to the UI
     * when the Hysteresis stage is active
     */
    class TubeGlowEffect : public juce::Component,
                           public juce::Timer
    {
    public:
        TubeGlowEffect();
        ~TubeGlowEffect() override;
        
        void paint(juce::Graphics& g) override;
        void timerCallback() override;
        
        void setEnabled(bool enabled);
        void setIntensity(float intensity);  // 0.0 - 1.0
        
        bool isEnabled() const { return enabled; }
        
    private:
        bool enabled = false;
        float targetIntensity = 0.0f;
        float currentIntensity = 0.0f;
        
        // Subtle flickering animation
        float flickerPhase = 0.0f;
        float flickerAmount = 0.0f;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TubeGlowEffect)
    };
}
