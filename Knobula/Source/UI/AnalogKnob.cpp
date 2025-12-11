/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    AnalogKnob Implementation
  ==============================================================================
*/

#include "AnalogKnob.h"

namespace Knobula
{
    AnalogKnob::AnalogKnob(const juce::String& labelText, KnobSize size)
        : knobSize(size), accentColor(Colors::bandLMF)
    {
        // Setup slider
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setColour(juce::Slider::rotarySliderFillColourId, accentColor);
        addAndMakeVisible(slider);
        
        // Setup name label
        nameLabel.setText(labelText, juce::dontSendNotification);
        nameLabel.setJustificationType(juce::Justification::centred);
        nameLabel.setColour(juce::Label::textColourId, Colors::textSecondary);
        addAndMakeVisible(nameLabel);
        
        // Setup value label
        valueLabel.setJustificationType(juce::Justification::centred);
        valueLabel.setColour(juce::Label::textColourId, Colors::textValue);
        addAndMakeVisible(valueLabel);
        
        // Listen to slider changes
        slider.onValueChange = [this] { sliderValueChanged(); };
    }
    
    void AnalogKnob::setAccentColor(juce::Colour color)
    {
        accentColor = color;
        slider.setColour(juce::Slider::rotarySliderFillColourId, color);
        repaint();
    }
    
    void AnalogKnob::attachToParameter(juce::AudioProcessorValueTreeState& apvts,
                                       const juce::String& paramID)
    {
        attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, paramID, slider);
    }
    
    void AnalogKnob::sliderValueChanged()
    {
        if (showValue)
        {
            juce::String text = juce::String(slider.getValue(), 1) + valueSuffix;
            valueLabel.setText(text, juce::dontSendNotification);
        }
    }
    
    void AnalogKnob::paint(juce::Graphics& g)
    {
        // Optional: draw subtle background behind knob area
    }
    
    void AnalogKnob::resized()
    {
        auto bounds = getLocalBounds();
        
        int labelHeight = 16;
        int valueHeight = 14;
        
        // Name label at top
        nameLabel.setBounds(bounds.removeFromTop(labelHeight));
        
        // Value at bottom
        if (showValue)
        {
            valueLabel.setBounds(bounds.removeFromBottom(valueHeight));
        }
        
        // Slider takes remaining space
        slider.setBounds(bounds.reduced(2));
    }
    
    //==============================================================================
    // DualKnobControl Implementation
    //==============================================================================
    
    DualKnobControl::DualKnobControl(const juce::String& name, juce::Colour color)
        : mainKnob("GAIN", AnalogKnob::KnobSize::Large),
          trimKnob("TRIM", AnalogKnob::KnobSize::Small),
          bandName(name),
          bandColor(color)
    {
        mainKnob.setAccentColor(color);
        mainKnob.setValueSuffix(" dB");
        addAndMakeVisible(mainKnob);
        
        trimKnob.setAccentColor(color.darker(0.3f));
        trimKnob.setValueSuffix(" dB");
        addAndMakeVisible(trimKnob);
    }
    
    void DualKnobControl::attachToParameters(juce::AudioProcessorValueTreeState& apvts,
                                             const juce::String& mainParamID,
                                             const juce::String& trimParamID)
    {
        mainKnob.attachToParameter(apvts, mainParamID);
        trimKnob.attachToParameter(apvts, trimParamID);
    }
    
    void DualKnobControl::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();
        
        // Draw band indicator/title
        g.setColour(bandColor);
        auto titleArea = bounds.removeFromTop(20.0f);
        g.setFont(juce::Font("Arial", 12.0f, juce::Font::bold));
        g.drawText(bandName, titleArea, juce::Justification::centred);
        
        // Draw subtle underline
        g.setColour(bandColor.withAlpha(0.5f));
        g.fillRect(titleArea.getCentreX() - 15.0f, titleArea.getBottom() - 2.0f, 30.0f, 2.0f);
    }
    
    void DualKnobControl::resized()
    {
        auto bounds = getLocalBounds();
        
        // Title space at top
        bounds.removeFromTop(24);
        
        // Main knob takes most of the space
        int mainKnobHeight = static_cast<int>(bounds.getHeight() * 0.65f);
        mainKnob.setBounds(bounds.removeFromTop(mainKnobHeight));
        
        // Small trim knob below
        trimKnob.setBounds(bounds.reduced(10, 0));
    }
}
