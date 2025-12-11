/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    VUMeterComponent - Vintage-style VU Meter Display
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ColorPalette.h"
#include "VUMeter.h"

namespace Knobula
{
    /**
     * Single VU Meter display (vintage needle-style)
     */
    class VUMeterComponent : public juce::Component,
                             public juce::Timer
    {
    public:
        VUMeterComponent(const juce::String& label = "");
        ~VUMeterComponent() override;
        
        void paint(juce::Graphics& g) override;
        void resized() override;
        void timerCallback() override;
        
        void setLevel(float normalizedLevel);
        void setPeakLevel(float normalizedPeak);
        void setLabel(const juce::String& newLabel) { label = newLabel; repaint(); }
        
    private:
        juce::String label;
        
        float currentLevel = 0.0f;
        float targetLevel = 0.0f;
        float peakLevel = 0.0f;
        
        // Needle animation
        float needleAngle = 0.0f;
        float needleVelocity = 0.0f;
        
        void drawMeterBackground(juce::Graphics& g, juce::Rectangle<float> bounds);
        void drawNeedle(juce::Graphics& g, juce::Rectangle<float> bounds);
        void drawScale(juce::Graphics& g, juce::Rectangle<float> bounds);
        
        static constexpr float MIN_ANGLE = -0.4f * juce::MathConstants<float>::pi;
        static constexpr float MAX_ANGLE = 0.4f * juce::MathConstants<float>::pi;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VUMeterComponent)
    };
    
    /**
     * Stereo VU Meter pair (Input or Output)
     */
    class StereoVUMeterComponent : public juce::Component
    {
    public:
        StereoVUMeterComponent(const juce::String& title);
        ~StereoVUMeterComponent() override = default;
        
        void paint(juce::Graphics& g) override;
        void resized() override;
        
        void setLevels(float leftLevel, float rightLevel);
        void setPeakLevels(float leftPeak, float rightPeak);
        
        VUMeterComponent& getLeft() { return leftMeter; }
        VUMeterComponent& getRight() { return rightMeter; }
        
    private:
        juce::String title;
        VUMeterComponent leftMeter;
        VUMeterComponent rightMeter;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoVUMeterComponent)
    };
}
