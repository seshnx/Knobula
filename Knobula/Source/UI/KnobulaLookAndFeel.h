/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    KnobulaLookAndFeel - Vintage Analog Hardware Theme
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ColorPalette.h"

namespace Knobula
{
    /**
     * Custom LookAndFeel for the Knobula plugin
     * Vintage analog hardware aesthetic with Sci-Fi undertones
     */
    class KnobulaLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        KnobulaLookAndFeel();
        ~KnobulaLookAndFeel() override = default;
        
        // Slider/Knob drawing
        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                             float sliderPosProportional, float rotaryStartAngle,
                             float rotaryEndAngle, juce::Slider& slider) override;
        
        // Large chunky knob style
        void drawLargeKnob(juce::Graphics& g, juce::Rectangle<float> bounds,
                          float value, juce::Colour accentColor, bool isSmall = false);
        
        // Button drawing
        void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                  const juce::Colour& backgroundColour,
                                  bool shouldDrawButtonAsHighlighted,
                                  bool shouldDrawButtonAsDown) override;
        
        void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                             bool shouldDrawButtonAsHighlighted,
                             bool shouldDrawButtonAsDown) override;
        
        // Label drawing
        void drawLabel(juce::Graphics& g, juce::Label& label) override;
        
        // Combo box
        void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                         int buttonX, int buttonY, int buttonW, int buttonH,
                         juce::ComboBox& box) override;
        
        // Font
        juce::Font getLabelFont(juce::Label& label) override;
        
        // Helper for drawing panel sections
        static void drawPanelSection(juce::Graphics& g, juce::Rectangle<float> bounds, 
                                    const juce::String& title = "");
        
        // Draw brushed metal texture
        static void drawBrushedMetal(juce::Graphics& g, juce::Rectangle<float> bounds);
        
    private:
        juce::Font mainFont;
        juce::Font labelFont;
        juce::Font valueFont;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobulaLookAndFeel)
    };
}
