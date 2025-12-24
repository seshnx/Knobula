/*
  ==============================================================================
    Aetheri - High-Fidelity Mastering EQ
    Parameter Definitions and Management
  ==============================================================================
*/

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

// Core parameter namespace
namespace Aetheri
{
    // Parameter IDs
    namespace ParamIDs
    {
        // Input/Output Stage
        inline const juce::String inputGain     { "inputGain" };
        inline const juce::String outputTrim    { "outputTrim" };
        
        // Global Controls
        inline const juce::String stereoMode    { "stereoMode" };      // 0 = L/R, 1 = M/S
        inline const juce::String channelLink   { "channelLink" };     // Link both channels
        
        // High/Low Pass Filters
        inline const juce::String hpfEnabled    { "hpfEnabled" };
        inline const juce::String hpfFreq       { "hpfFreq" };
        inline const juce::String lpfEnabled    { "lpfEnabled" };
        inline const juce::String lpfFreq       { "lpfFreq" };
        
        // EQ Band Parameters - using format: band{0-3}_{param}_{channel}
        // Channels: L=0, R=1 (or M=0, S=1 in M/S mode)
        inline juce::String bandGain(int band, int channel)     { return "band" + juce::String(band) + "_gain_" + juce::String(channel); }
        inline juce::String bandTrim(int band, int channel)     { return "band" + juce::String(band) + "_trim_" + juce::String(channel); }
        inline juce::String bandFreq(int band, int channel)     { return "band" + juce::String(band) + "_freq_" + juce::String(channel); }
        inline juce::String bandCurve(int band, int channel)    { return "band" + juce::String(band) + "_curve_" + juce::String(channel); }  // 0=Bell, 1=Shelf
        inline juce::String bandEnabled(int band, int channel)  { return "band" + juce::String(band) + "_enabled_" + juce::String(channel); }
        inline juce::String bandSolo(int band, int channel)     { return "band" + juce::String(band) + "_solo_" + juce::String(channel); }
        inline juce::String bandMute(int band, int channel)    { return "band" + juce::String(band) + "_mute_" + juce::String(channel); }
        
        // Hysteresis Stage
        inline const juce::String hystEnabled       { "hystEnabled" };
        inline const juce::String tubeHarmonics     { "tubeHarmonics" };
        inline const juce::String transformerSat    { "transformerSat" };
        inline const juce::String hystMix           { "hystMix" };
        
        // Processing Options
        inline const juce::String oversampling      { "oversampling" };  // 0=1x, 1=2x, 2=4x
        inline const juce::String autoGainComp      { "autoGainComp" };  // Auto-gain compensation
    }
    
    // Band frequency defaults (Hz)
    namespace BandDefaults
    {
        constexpr float lfFreq  = 80.0f;
        constexpr float lmfFreq = 400.0f;
        constexpr float hmfFreq = 2500.0f;
        constexpr float hfFreq  = 8000.0f;
        
        // Fixed Q values for musical, broad response
        constexpr float lfQ  = 0.6f;   // Very broad for low end warmth
        constexpr float lmfQ = 0.8f;   // Broad for body
        constexpr float hmfQ = 0.9f;   // Slightly narrower for presence
        constexpr float hfQ  = 0.7f;   // Broad for air
        
        inline float getDefaultFreq(int band)
        {
            switch (band)
            {
                case 0: return lfFreq;
                case 1: return lmfFreq;
                case 2: return hmfFreq;
                case 3: return hfFreq;
                default: return 1000.0f;
            }
        }
        
        inline float getQ(int band)
        {
            switch (band)
            {
                case 0: return lfQ;
                case 1: return lmfQ;
                case 2: return hmfQ;
                case 3: return hfQ;
                default: return 0.707f;
            }
        }
        
        inline juce::String getBandName(int band)
        {
            switch (band)
            {
                case 0: return "LF";
                case 1: return "LMF";
                case 2: return "HMF";
                case 3: return "HF";
                default: return "Band";
            }
        }
    }
    
    // Frequency ranges for bands
    namespace FreqRanges
    {
        constexpr float lfMin = 20.0f,   lfMax = 300.0f;
        constexpr float lmfMin = 100.0f, lmfMax = 1500.0f;
        constexpr float hmfMin = 500.0f, hmfMax = 8000.0f;
        constexpr float hfMin = 2000.0f, hfMax = 20000.0f;
        
        constexpr float hpfMin = 20.0f,  hpfMax = 500.0f;
        constexpr float lpfMin = 2000.0f, lpfMax = 20000.0f;
        
        inline float getMinFreq(int band)
        {
            switch (band)
            {
                case 0: return lfMin;
                case 1: return lmfMin;
                case 2: return hmfMin;
                case 3: return hfMin;
                default: return 20.0f;
            }
        }
        
        inline float getMaxFreq(int band)
        {
            switch (band)
            {
                case 0: return lfMax;
                case 1: return lmfMax;
                case 2: return hmfMax;
                case 3: return hfMax;
                default: return 20000.0f;
            }
        }
    }
    
    // Create all parameters for the plugin
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
}
