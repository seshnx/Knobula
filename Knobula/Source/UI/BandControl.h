/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    BandControl - Full EQ Band Control Panel
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "AnalogKnob.h"
#include "ColorPalette.h"
#include "Parameters.h"

namespace Knobula
{
    /**
     * Complete EQ Band control with:
     * - Main Gain knob (large)
     * - Fine Trim knob (small)
     * - Frequency knob
     * - Curve selector (Bell/Shelf) for LF and HF
     * - Enable button
     */
    class BandControl : public juce::Component
    {
    public:
        BandControl(int bandIndex, const juce::String& bandName);
        ~BandControl() override = default;
        
        void paint(juce::Graphics& g) override;
        void resized() override;
        
        void attachToParameters(juce::AudioProcessorValueTreeState& apvts, int channel);
        
        int getBandIndex() const { return bandIndex; }
        juce::Colour getBandColor() const { return bandColor; }
        
        // Enable/disable curve selector (only for LF and HF)
        void setShowCurveSelector(bool show);
        
    private:
        int bandIndex;
        juce::String bandName;
        juce::Colour bandColor;
        
        // Main controls
        AnalogKnob gainKnob;
        AnalogKnob trimKnob;
        AnalogKnob freqKnob;
        
        // Curve selector (Bell/Shelf)
        juce::ComboBox curveSelector;
        bool showCurve = false;
        
        // Enable button
        juce::TextButton enableButton;
        
        // Parameter attachments
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> trimAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> curveAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enableAttachment;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BandControl)
    };
    
    /**
     * Complete channel EQ strip with all 4 bands
     */
    class ChannelEQStrip : public juce::Component
    {
    public:
        ChannelEQStrip(const juce::String& channelName);
        ~ChannelEQStrip() override = default;
        
        void paint(juce::Graphics& g) override;
        void resized() override;
        
        void attachToParameters(juce::AudioProcessorValueTreeState& apvts, int channel);
        
        BandControl& getBand(int index) { return *bands[index]; }
        
    private:
        juce::String channelName;
        std::array<std::unique_ptr<BandControl>, 4> bands;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChannelEQStrip)
    };
}
