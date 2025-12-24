/*
  ==============================================================================
    Aetheri - High-Fidelity Mastering EQ
    BandControl Implementation
  ==============================================================================
*/

#include "BandControl.h"

namespace Aetheri
{
    BandControl::BandControl(int index, const juce::String& name)
        : bandIndex(index),
          bandName(name),
          bandColor(Colors::getBandColor(index)),
          gainKnob("GAIN", AnalogKnob::KnobSize::Large),
          trimKnob("TRIM", AnalogKnob::KnobSize::Small),
          freqKnob("FREQ", AnalogKnob::KnobSize::Medium)
    {
        // Setup gain knob
        gainKnob.setAccentColor(bandColor);
        gainKnob.setValueSuffix(" dB");
        addAndMakeVisible(gainKnob);
        
        // Setup trim knob
        trimKnob.setAccentColor(bandColor.darker(0.3f));
        trimKnob.setValueSuffix(" dB");
        addAndMakeVisible(trimKnob);
        
        // Setup frequency knob
        freqKnob.setAccentColor(bandColor.withAlpha(0.7f));
        freqKnob.setValueSuffix(" Hz");
        freqKnob.setTooltip("Frequency: " + name + " band center frequency");
        addAndMakeVisible(freqKnob);
        
        // Add tooltips
        gainKnob.setTooltip("Main Gain: ±10 dB adjustment for " + name + " band");
        trimKnob.setTooltip("Fine Trim: ±1 dB precision adjustment for " + name + " band");
        enableButton.setTooltip("Enable/Disable " + name + " band");
        
        // Curve selector (only visible for LF and HF)
        curveSelector.addItem("Bell", 1);
        curveSelector.addItem("Shelf", 2);
        curveSelector.setSelectedId(1);
        curveSelector.setTooltip("Curve Type: Bell (peaking) or Shelf (shelving) response");
        addChildComponent(curveSelector);  // Hidden by default
        
        // Enable button
        enableButton.setButtonText("ON");
        enableButton.setClickingTogglesState(true);
        enableButton.setToggleState(true, juce::dontSendNotification);
        addAndMakeVisible(enableButton);
        
        // Solo button
        soloButton.setButtonText("S");
        soloButton.setClickingTogglesState(true);
        soloButton.setTooltip("Solo: Isolate this band (only soloed bands will be audible)");
        addAndMakeVisible(soloButton);
        
        // Mute button
        muteButton.setButtonText("M");
        muteButton.setClickingTogglesState(true);
        muteButton.setTooltip("Mute: Bypass this band");
        addAndMakeVisible(muteButton);
        
        // Show curve selector for LF and HF bands
        showCurve = (bandIndex == 0 || bandIndex == 3);
        curveSelector.setVisible(showCurve);
    }
    
    void BandControl::setShowCurveSelector(bool show)
    {
        showCurve = show;
        curveSelector.setVisible(show);
        resized();
    }
    
    void BandControl::attachToParameters(juce::AudioProcessorValueTreeState& apvts, int channel)
    {
        // Attach gain
        gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::bandGain(bandIndex, channel), gainKnob.getSlider());
        
        // Attach trim
        trimAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::bandTrim(bandIndex, channel), trimKnob.getSlider());
        
        // Attach frequency
        freqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, ParamIDs::bandFreq(bandIndex, channel), freqKnob.getSlider());
        
        // Attach curve selector (only for LF and HF)
        if (bandIndex == 0 || bandIndex == 3)
        {
            curveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
                apvts, ParamIDs::bandCurve(bandIndex, channel), curveSelector);
        }
        
        // Attach enable
        enableAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, ParamIDs::bandEnabled(bandIndex, channel), enableButton);
        
        // Attach solo
        soloAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, ParamIDs::bandSolo(bandIndex, channel), soloButton);
        
        // Attach mute
        muteAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, ParamIDs::bandMute(bandIndex, channel), muteButton);
    }
    
    void BandControl::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();
        
        // Background panel - highlight if soloed
        if (soloButton.getToggleState())
        {
            g.setColour(bandColor.withAlpha(0.3f));
        }
        else
        {
            g.setColour(Colors::panelSurface.withAlpha(0.5f));
        }
        g.fillRoundedRectangle(bounds.reduced(2.0f), 6.0f);
        
        // Dimmed if muted
        if (muteButton.getToggleState())
        {
            g.setColour(Colors::panelBackground.withAlpha(0.4f));
            g.fillRoundedRectangle(bounds.reduced(2.0f), 6.0f);
        }
        
        // Band color indicator strip at top
        auto indicator = bounds.removeFromTop(4.0f).reduced(4.0f, 0.0f);
        g.setColour(bandColor);
        g.fillRoundedRectangle(indicator, 2.0f);
        
        // Band name
        g.setColour(bandColor);
        g.setFont(juce::FontOptions().withHeight(14.0f).withStyle("Bold"));
        g.drawText(bandName, bounds.removeFromTop(22.0f), juce::Justification::centred);
    }
    
    void BandControl::resized()
    {
        auto bounds = getLocalBounds().reduced(4);
        
        // Top indicator and name area
        bounds.removeFromTop(28);
        
        // Bottom row: Enable, Solo, Mute buttons
        auto buttonRow = bounds.removeFromBottom(24).reduced(4, 2);
        int buttonWidth = buttonRow.getWidth() / 3;
        enableButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2, 0));
        soloButton.setBounds(buttonRow.removeFromLeft(buttonWidth).reduced(2, 0));
        muteButton.setBounds(buttonRow.reduced(2, 0));
        
        // Curve selector (if visible)
        if (showCurve)
        {
            curveSelector.setBounds(bounds.removeFromBottom(24).reduced(8, 2));
        }
        
        // Divide remaining space for knobs
        int knobHeight = bounds.getHeight() / 3;
        
        // Gain knob (largest)
        gainKnob.setBounds(bounds.removeFromTop(knobHeight + 10));
        
        // Frequency knob
        freqKnob.setBounds(bounds.removeFromTop(knobHeight));
        
        // Trim knob (smallest)
        trimKnob.setBounds(bounds);
    }
    
    //==============================================================================
    // ChannelEQStrip Implementation
    //==============================================================================
    
    ChannelEQStrip::ChannelEQStrip(const juce::String& name)
        : channelName(name)
    {
        // Create all 4 bands
        const juce::String bandNames[] = { "LF", "LMF", "HMF", "HF" };
        
        for (int i = 0; i < 4; ++i)
        {
            bands[i] = std::make_unique<BandControl>(i, bandNames[i]);
            addAndMakeVisible(*bands[i]);
        }
    }
    
    void ChannelEQStrip::attachToParameters(juce::AudioProcessorValueTreeState& apvts, int channel)
    {
        for (int i = 0; i < 4; ++i)
        {
            bands[i]->attachToParameters(apvts, channel);
        }
    }
    
    void ChannelEQStrip::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();
        
        // Channel header
        auto headerArea = bounds.removeFromTop(28.0f);
        
        g.setColour(Colors::panelSurface);
        g.fillRect(headerArea);
        
        g.setColour(Colors::textPrimary);
        g.setFont(juce::FontOptions().withHeight(13.0f).withStyle("Bold"));
        g.drawText(channelName, headerArea, juce::Justification::centred);
        
        // Separator line
        g.setColour(Colors::panelBorder);
        g.drawLine(bounds.getX(), headerArea.getBottom(), 
                   bounds.getRight(), headerArea.getBottom(), 1.0f);
    }
    
    void ChannelEQStrip::resized()
    {
        auto bounds = getLocalBounds();
        bounds.removeFromTop(30);  // Header space
        
        int bandWidth = bounds.getWidth() / 4;
        
        for (int i = 0; i < 4; ++i)
        {
            bands[i]->setBounds(bounds.removeFromLeft(bandWidth));
        }
    }
}
