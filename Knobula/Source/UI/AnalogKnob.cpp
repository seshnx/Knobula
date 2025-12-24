/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    AnalogKnob Implementation
  ==============================================================================
*/

#include "AnalogKnob.h"

namespace Aetheri
{
    AnalogKnob::AnalogKnob(const juce::String& labelText, KnobSize size)
        : knobSize(size), accentColor(Colors::bandLMF), normalSensitivity(250)
    {
        // Setup slider
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setColour(juce::Slider::rotarySliderFillColourId, accentColor);
        slider.setMouseDragSensitivity(normalSensitivity);
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
    
    void AnalogKnob::attachToParameter(juce::AudioProcessorValueTreeState& state,
                                       const juce::String& pID)
    {
        this->apvts = &state;
        this->paramID = pID;
        attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, pID, slider);
    }
    
    void AnalogKnob::sliderValueChanged()
    {
        if (showValue)
        {
            double value = slider.getValue();
            juce::String text;
            
            // Smart formatting: kHz for frequencies >= 1000 Hz
            if (valueSuffix == " Hz" && value >= 1000.0)
            {
                text = juce::String(value / 1000.0, 2) + " kHz";
            }
            else
            {
                // Determine decimal places based on value
                int decimals = (std::abs(value) < 10.0) ? 1 : 0;
                text = juce::String(value, decimals) + valueSuffix;
            }
            
            valueLabel.setText(text, juce::dontSendNotification);
        }
    }
    
    void AnalogKnob::paint(juce::Graphics& /*g*/)
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
    
    void AnalogKnob::mouseDown(const juce::MouseEvent& e)
    {
        // Fine adjustment: Shift = fine (3x sensitivity), Ctrl/Cmd = coarse (1/3 sensitivity)
        if (e.mods.isShiftDown())
        {
            slider.setMouseDragSensitivity(normalSensitivity * 3);  // Fine adjustment
        }
        else if (e.mods.isCommandDown() || e.mods.isCtrlDown())
        {
            slider.setMouseDragSensitivity(normalSensitivity / 3);  // Coarse adjustment
        }
        else
        {
            slider.setMouseDragSensitivity(normalSensitivity);  // Normal
        }
        
        Component::mouseDown(e);
    }
    
    void AnalogKnob::mouseDoubleClick(const juce::MouseEvent& /*e*/)
    {
        // Reset to default value
        if (apvts && paramID.isNotEmpty())
        {
            auto* param = apvts->getParameter(paramID);
            if (param)
            {
                param->setValueNotifyingHost(param->getDefaultValue());
                return;
            }
        }
        
        // Fallback: set slider to middle of range
        auto range = slider.getRange();
        slider.setValue(range.getStart() + range.getLength() * 0.5, juce::sendNotificationSync);
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
        g.setFont(juce::FontOptions().withHeight(12.0f).withStyle("Bold"));
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
