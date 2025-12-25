/*
  ==============================================================================
    Aetheri - High-Fidelity Mastering EQ
    Parameter Definitions Implementation
  ==============================================================================
*/

#include "Parameters.h"

namespace Aetheri
{
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
        
        // Input/Output Gain Stage
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::inputGain, 1),
            "Input Gain",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("dB")));
        
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::outputTrim, 1),
            "Output Trim",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("dB")));
        
        // Global Controls
        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(ParamIDs::stereoMode, 1),
            "Stereo Mode",
            juce::StringArray{"L/R", "M/S"},
            0));
        
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(ParamIDs::channelLink, 1),
            "Channel Link",
            true));
        
        // High-Pass Filter
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(ParamIDs::hpfEnabled, 1),
            "HPF Enable",
            false));
        
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::hpfFreq, 1),
            "HPF Frequency",
            juce::NormalisableRange<float>(FreqRanges::hpfMin, FreqRanges::hpfMax, 1.0f, 0.3f),
            30.0f,
            juce::AudioParameterFloatAttributes().withLabel("Hz")));
        
        // Low-Pass Filter
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(ParamIDs::lpfEnabled, 1),
            "LPF Enable",
            false));
        
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::lpfFreq, 1),
            "LPF Frequency",
            juce::NormalisableRange<float>(FreqRanges::lpfMin, FreqRanges::lpfMax, 1.0f, 0.3f),
            18000.0f,
            juce::AudioParameterFloatAttributes().withLabel("Hz")));
        
        // EQ Bands (4 bands x 2 channels)
        for (int band = 0; band < 4; ++band)
        {
            for (int channel = 0; channel < 2; ++channel)
            {
                juce::String bandName = BandDefaults::getBandName(band);
                juce::String chName = (channel == 0) ? "L" : "R";
                
                // Main Gain (±10dB)
                params.push_back(std::make_unique<juce::AudioParameterFloat>(
                    juce::ParameterID(ParamIDs::bandGain(band, channel), 1),
                    bandName + " Gain " + chName,
                    juce::NormalisableRange<float>(-10.0f, 10.0f, 0.1f),
                    0.0f,
                    juce::AudioParameterFloatAttributes().withLabel("dB")));
                
                // Fine Trim (±1dB)
                params.push_back(std::make_unique<juce::AudioParameterFloat>(
                    juce::ParameterID(ParamIDs::bandTrim(band, channel), 1),
                    bandName + " Trim " + chName,
                    juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f),
                    0.0f,
                    juce::AudioParameterFloatAttributes().withLabel("dB")));
                
                // Frequency
                params.push_back(std::make_unique<juce::AudioParameterFloat>(
                    juce::ParameterID(ParamIDs::bandFreq(band, channel), 1),
                    bandName + " Freq " + chName,
                    juce::NormalisableRange<float>(FreqRanges::getMinFreq(band), 
                                                   FreqRanges::getMaxFreq(band), 
                                                   1.0f, 0.3f),
                    BandDefaults::getDefaultFreq(band),
                    juce::AudioParameterFloatAttributes().withLabel("Hz")));
                
                // Curve Type (only for LF and HF bands: Bell/Shelf)
                if (band == 0 || band == 3)
                {
                    params.push_back(std::make_unique<juce::AudioParameterChoice>(
                        juce::ParameterID(ParamIDs::bandCurve(band, channel), 1),
                        bandName + " Curve " + chName,
                        juce::StringArray{"Bell", "Shelf"},
                        0));  // Default to Bell
                }
                
                // Band Enable
                params.push_back(std::make_unique<juce::AudioParameterBool>(
                    juce::ParameterID(ParamIDs::bandEnabled(band, channel), 1),
                    bandName + " Enable " + chName,
                    true));
                
                // Band Solo
                params.push_back(std::make_unique<juce::AudioParameterBool>(
                    juce::ParameterID(ParamIDs::bandSolo(band, channel), 1),
                    bandName + " Solo " + chName,
                    false));
                
                // Band Mute
                params.push_back(std::make_unique<juce::AudioParameterBool>(
                    juce::ParameterID(ParamIDs::bandMute(band, channel), 1),
                    bandName + " Mute " + chName,
                    false));
            }
        }
        
        // Hysteresis Stage
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(ParamIDs::hystEnabled, 1),
            "Hysteresis Enable",
            false));
        
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::tubeHarmonics, 1),
            "Tube Harmonics",
            juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("%")));
        
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::transformerSat, 1),
            "Transformer Saturate",
            juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel("%")));
        
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ParamIDs::hystMix, 1),
            "Hysteresis Mix",
            juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
            100.0f,
            juce::AudioParameterFloatAttributes().withLabel("%")));
        
        // Processing Options
        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(ParamIDs::oversampling, 1),
            "Oversampling",
            juce::StringArray{"1x", "2x", "4x"},
            0));  // Default to 1x
        
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(ParamIDs::autoGainComp, 1),
            "Auto Gain Compensation",
            false));  // Default off

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(ParamIDs::bypass, 1),
            "Bypass",
            false));  // Default off

        return { params.begin(), params.end() };
    }
}
