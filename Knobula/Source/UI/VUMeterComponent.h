/*
  ==============================================================================
    Aetheri - High-Fidelity Mastering EQ
    VUMeterComponent - Vintage-style VU Meter Display
  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_core/juce_core.h>
#include "ColorPalette.h"
#include "../DSP/VUMeter.h"

namespace Aetheri
{
    /**
     * Professional LED-style multi-mode level meter
     * Vertical bar with color-coded segments (green/yellow/red)
     * Supports RMS, Peak, VU, and LUFS modes
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
        void setMode(MeterMode mode) { meterMode = mode; repaint(); }
        MeterMode getMode() const { return meterMode; }
        
    private:
        juce::String label;
        MeterMode meterMode = MeterMode::RMS;
        
        float currentLevel = 0.0f;
        float targetLevel = 0.0f;
        float peakLevel = 0.0f;
        
        // Smooth level for display
        float smoothedLevel = 0.0f;
        float smoothedPeak = 0.0f;
        
        static constexpr int NUM_SEGMENTS = 30;  // Number of LED segments
        static constexpr float GREEN_THRESHOLD = 0.6f;   // Green up to 60%
        static constexpr float YELLOW_THRESHOLD = 0.85f; // Yellow up to 85%
        // Red above 85%
        
        void drawMeterBackground(juce::Graphics& g, juce::Rectangle<float> bounds);
        void drawLEDSegments(juce::Graphics& g, juce::Rectangle<float> bounds);
        void drawScale(juce::Graphics& g, juce::Rectangle<float> bounds);
        juce::String getModeLabel() const;
        
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
        void setMode(MeterMode mode);
        MeterMode getMode() const { return leftMeter.getMode(); }
        
        VUMeterComponent& getLeft() { return leftMeter; }
        VUMeterComponent& getRight() { return rightMeter; }
        
    private:
        juce::String title;
        VUMeterComponent leftMeter;
        VUMeterComponent rightMeter;
        juce::ComboBox modeSelector;
        
        void modeChanged();
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoVUMeterComponent)
    };
}
