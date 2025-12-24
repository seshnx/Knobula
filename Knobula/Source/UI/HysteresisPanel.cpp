/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    HysteresisPanel Implementation
  ==============================================================================
*/

#include "HysteresisPanel.h"

namespace Aetheri
{
    HysteresisPanel::HysteresisPanel()
        : tubeKnob("TUBE", AnalogKnob::KnobSize::Medium),
          transformerKnob("XFMR", AnalogKnob::KnobSize::Medium),
          mixKnob("MIX", AnalogKnob::KnobSize::Small)
    {
        // Engage button
        engageButton.setButtonText("ENGAGE");
        engageButton.setClickingTogglesState(true);
        engageButton.setColour(juce::TextButton::buttonOnColourId, Colors::tubeGlowOn);
        addAndMakeVisible(engageButton);
        
        // Tube harmonics knob (warm orange color)
        tubeKnob.setAccentColor(juce::Colour(0xFFFF9944));
        tubeKnob.setValueSuffix("%");
        addAndMakeVisible(tubeKnob);
        
        // Transformer saturate knob (deep amber)
        transformerKnob.setAccentColor(juce::Colour(0xFFCC7722));
        transformerKnob.setValueSuffix("%");
        addAndMakeVisible(transformerKnob);
        
        // Mix knob
        mixKnob.setAccentColor(Colors::textSecondary);
        mixKnob.setValueSuffix("%");
        addAndMakeVisible(mixKnob);
    }
    
    void HysteresisPanel::attachToParameters(juce::AudioProcessorValueTreeState& apvts)
    {
        engageAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, ParamIDs::hystEnabled, engageButton);
        
        tubeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::tubeHarmonics, tubeKnob.getSlider());
        
        transformerAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::transformerSat, transformerKnob.getSlider());
        
        mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::hystMix, mixKnob.getSlider());
    }
    
    void HysteresisPanel::drawTubeGlow(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        if (glowIntensity < 0.01f || !isEngaged())
            return;
        
        // Create warm amber glow effect
        auto centre = bounds.getCentre();
        float maxRadius = bounds.getWidth() * 0.4f;
        
        // Multiple glow layers for soft effect
        for (int i = 3; i >= 0; --i)
        {
            float radius = maxRadius * (1.0f - i * 0.2f);
            float alpha = glowIntensity * (0.1f - i * 0.02f);
            
            juce::ColourGradient glow(
                Colors::tubeGlowBright.withAlpha(alpha),
                juce::Point<float>(centre.x, centre.y),
                Colors::tubeGlowOn.withAlpha(0.0f),
                juce::Point<float>(centre.x + radius, centre.y),
                true);
            
            g.setGradientFill(glow);
            g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2, radius * 2);
        }
    }
    
    void HysteresisPanel::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();
        
        // Panel background with subtle gradient
        juce::ColourGradient bgGrad(
            Colors::panelSurface.brighter(0.05f), juce::Point<float>(bounds.getX(), bounds.getY()),
            Colors::panelSurface.darker(0.05f), juce::Point<float>(bounds.getX(), bounds.getBottom()), false);
        g.setGradientFill(bgGrad);
        g.fillRoundedRectangle(bounds.reduced(2.0f), 8.0f);
        
        // Draw tube glow if engaged
        drawTubeGlow(g, bounds);
        
        // Panel border
        g.setColour(Colors::panelBorder);
        g.drawRoundedRectangle(bounds.reduced(2.0f), 8.0f, 1.0f);
        
        // Title
        auto titleArea = bounds.removeFromTop(24.0f);
        g.setColour(Colors::tubeGlowOn.interpolatedWith(Colors::textPrimary, 0.5f));
        g.setFont(juce::FontOptions().withHeight(12.0f).withStyle("Bold"));
        g.drawText("HYSTERESIS", titleArea, juce::Justification::centred);
        
        // Decorative line
        g.setColour(Colors::tubeGlowOn.withAlpha(0.3f));
        g.fillRect(bounds.getCentreX() - 30.0f, titleArea.getBottom() - 2.0f, 60.0f, 2.0f);
    }
    
    void HysteresisPanel::resized()
    {
        auto bounds = getLocalBounds().reduced(4);
        
        // Title area
        bounds.removeFromTop(26);
        
        // Engage button at top
        engageButton.setBounds(bounds.removeFromTop(28).reduced(12, 2));
        
        bounds.removeFromTop(8);  // Spacing
        
        // Knobs in a row
        int knobWidth = bounds.getWidth() / 3;
        int knobHeight = bounds.getHeight() - 40;
        
        auto knobArea = bounds.removeFromTop(knobHeight);
        tubeKnob.setBounds(knobArea.removeFromLeft(knobWidth));
        transformerKnob.setBounds(knobArea.removeFromLeft(knobWidth));
        mixKnob.setBounds(knobArea);
    }
}
