/*
  ==============================================================================
    Aetheri - High-Fidelity Mastering EQ
    Preset Definitions Implementation
  ==============================================================================
*/

#include "Presets.h"
#include "Parameters.h"

namespace Aetheri
{
    std::vector<Preset> PresetManager::factoryPresets;
    
    // Helper function to reset all parameters to defaults
    static void resetAllParameters(juce::AudioProcessorValueTreeState& apvts)
    {
        auto resetParam = [&apvts](const juce::String& id) {
            if (auto* param = apvts.getParameter(id))
                param->setValueNotifyingHost(param->getDefaultValue());
        };
        
        resetParam(ParamIDs::inputGain);
        resetParam(ParamIDs::outputTrim);
        resetParam(ParamIDs::stereoMode);
        resetParam(ParamIDs::channelLink);
        resetParam(ParamIDs::hpfEnabled);
        resetParam(ParamIDs::hpfFreq);
        resetParam(ParamIDs::lpfEnabled);
        resetParam(ParamIDs::lpfFreq);
        resetParam(ParamIDs::hystEnabled);
        resetParam(ParamIDs::tubeHarmonics);
        resetParam(ParamIDs::transformerSat);
        resetParam(ParamIDs::hystMix);
        
        for (int band = 0; band < 4; ++band)
        {
            for (int ch = 0; ch < 2; ++ch)
            {
                resetParam(ParamIDs::bandGain(band, ch));
                resetParam(ParamIDs::bandTrim(band, ch));
                resetParam(ParamIDs::bandFreq(band, ch));
                resetParam(ParamIDs::bandEnabled(band, ch));
                if (band == 0 || band == 3)
                    resetParam(ParamIDs::bandCurve(band, ch));
            }
        }
    }
    
    void PresetManager::initializePresets()
    {
        if (!factoryPresets.empty())
            return;
            
        factoryPresets.clear();
        
        // Flat - All bands at 0dB, no processing
        factoryPresets.emplace_back("Flat", "Neutral response, all bands at 0dB",
            [](juce::AudioProcessorValueTreeState& apvts) {
                resetAllParameters(apvts);
            });
        
        // Vocal Presence - Boost around 2-5kHz
        factoryPresets.emplace_back("Vocal Presence", "Boost vocal clarity in 2-5kHz range",
            [](juce::AudioProcessorValueTreeState& apvts) {
                resetAllParameters(apvts);
                
                // Boost HMF band (band 2) around 3kHz
                if (auto* freq = apvts.getParameter(ParamIDs::bandFreq(2, 0)))
                    freq->setValueNotifyingHost(freq->convertTo0to1(3000.0f));
                if (auto* gain = apvts.getParameter(ParamIDs::bandGain(2, 0)))
                    gain->setValueNotifyingHost(gain->convertTo0to1(3.0f));
                if (auto* enabled = apvts.getParameter(ParamIDs::bandEnabled(2, 0)))
                    enabled->setValueNotifyingHost(1.0f);
            });
        
        // Bass Boost - Enhance low frequencies
        factoryPresets.emplace_back("Bass Boost", "Enhance low frequencies with shelf",
            [](juce::AudioProcessorValueTreeState& apvts) {
                resetAllParameters(apvts);
                
                // LF band as shelf, boost at 80Hz
                if (auto* freq = apvts.getParameter(ParamIDs::bandFreq(0, 0)))
                    freq->setValueNotifyingHost(freq->convertTo0to1(80.0f));
                if (auto* gain = apvts.getParameter(ParamIDs::bandGain(0, 0)))
                    gain->setValueNotifyingHost(gain->convertTo0to1(4.0f));
                if (auto* curve = apvts.getParameter(ParamIDs::bandCurve(0, 0)))
                    curve->setValueNotifyingHost(1.0f);  // Shelf
                if (auto* enabled = apvts.getParameter(ParamIDs::bandEnabled(0, 0)))
                    enabled->setValueNotifyingHost(1.0f);
            });
        
        // Air - High frequency enhancement
        factoryPresets.emplace_back("Air", "Add high-frequency sheen and presence",
            [](juce::AudioProcessorValueTreeState& apvts) {
                resetAllParameters(apvts);
                
                // HF band as shelf, boost at 10kHz
                if (auto* freq = apvts.getParameter(ParamIDs::bandFreq(3, 0)))
                    freq->setValueNotifyingHost(freq->convertTo0to1(10000.0f));
                if (auto* gain = apvts.getParameter(ParamIDs::bandGain(3, 0)))
                    gain->setValueNotifyingHost(gain->convertTo0to1(3.0f));
                if (auto* curve = apvts.getParameter(ParamIDs::bandCurve(3, 0)))
                    curve->setValueNotifyingHost(1.0f);  // Shelf
                if (auto* enabled = apvts.getParameter(ParamIDs::bandEnabled(3, 0)))
                    enabled->setValueNotifyingHost(1.0f);
            });
        
        // Warmth - Tube saturation with low-mid boost
        factoryPresets.emplace_back("Warmth", "Tube harmonics with low-mid enhancement",
            [](juce::AudioProcessorValueTreeState& apvts) {
                resetAllParameters(apvts);
                
                // Enable hysteresis with tube harmonics
                if (auto* hystEnabled = apvts.getParameter(ParamIDs::hystEnabled))
                    hystEnabled->setValueNotifyingHost(1.0f);
                if (auto* tube = apvts.getParameter(ParamIDs::tubeHarmonics))
                    tube->setValueNotifyingHost(tube->convertTo0to1(40.0f));
                if (auto* xfmr = apvts.getParameter(ParamIDs::transformerSat))
                    xfmr->setValueNotifyingHost(xfmr->convertTo0to1(30.0f));
                
                // Slight LMF boost
                if (auto* freq = apvts.getParameter(ParamIDs::bandFreq(1, 0)))
                    freq->setValueNotifyingHost(freq->convertTo0to1(400.0f));
                if (auto* gain = apvts.getParameter(ParamIDs::bandGain(1, 0)))
                    gain->setValueNotifyingHost(gain->convertTo0to1(2.0f));
                if (auto* enabled = apvts.getParameter(ParamIDs::bandEnabled(1, 0)))
                    enabled->setValueNotifyingHost(1.0f);
            });
        
        // Clean Mastering - Subtle broad-band EQ
        factoryPresets.emplace_back("Clean Mastering", "Subtle mastering curve for transparency",
            [](juce::AudioProcessorValueTreeState& apvts) {
                resetAllParameters(apvts);
                
                // Gentle LF shelf boost
                if (auto* freq = apvts.getParameter(ParamIDs::bandFreq(0, 0)))
                    freq->setValueNotifyingHost(freq->convertTo0to1(60.0f));
                if (auto* gain = apvts.getParameter(ParamIDs::bandGain(0, 0)))
                    gain->setValueNotifyingHost(gain->convertTo0to1(1.5f));
                if (auto* curve = apvts.getParameter(ParamIDs::bandCurve(0, 0)))
                    curve->setValueNotifyingHost(1.0f);
                if (auto* enabled = apvts.getParameter(ParamIDs::bandEnabled(0, 0)))
                    enabled->setValueNotifyingHost(1.0f);
                
                // Gentle HF shelf boost
                if (auto* freq = apvts.getParameter(ParamIDs::bandFreq(3, 0)))
                    freq->setValueNotifyingHost(freq->convertTo0to1(12000.0f));
                if (auto* gain = apvts.getParameter(ParamIDs::bandGain(3, 0)))
                    gain->setValueNotifyingHost(gain->convertTo0to1(1.5f));
                if (auto* curve = apvts.getParameter(ParamIDs::bandCurve(3, 0)))
                    curve->setValueNotifyingHost(1.0f);
                if (auto* enabled = apvts.getParameter(ParamIDs::bandEnabled(3, 0)))
                    enabled->setValueNotifyingHost(1.0f);
            });
        
        // High-Pass Clean - Remove low-end mud
        factoryPresets.emplace_back("High-Pass Clean", "Remove low-end mud with HPF",
            [](juce::AudioProcessorValueTreeState& apvts) {
                resetAllParameters(apvts);
                
                // Enable HPF at 40Hz
                if (auto* hpfEnabled = apvts.getParameter(ParamIDs::hpfEnabled))
                    hpfEnabled->setValueNotifyingHost(1.0f);
                if (auto* hpfFreq = apvts.getParameter(ParamIDs::hpfFreq))
                    hpfFreq->setValueNotifyingHost(hpfFreq->convertTo0to1(40.0f));
            });
    }
    
    std::vector<Preset> PresetManager::createFactoryPresets()
    {
        initializePresets();
        return factoryPresets;
    }
    
    void PresetManager::applyPreset(juce::AudioProcessorValueTreeState& apvts, int presetIndex)
    {
        initializePresets();
        if (presetIndex >= 0 && presetIndex < static_cast<int>(factoryPresets.size()))
        {
            factoryPresets[presetIndex].applyFunction(apvts);
        }
    }
    
    juce::String PresetManager::getPresetName(int index)
    {
        initializePresets();
        if (index >= 0 && index < static_cast<int>(factoryPresets.size()))
            return factoryPresets[index].name;
        return {};
    }
    
    juce::String PresetManager::getPresetDescription(int index)
    {
        initializePresets();
        if (index >= 0 && index < static_cast<int>(factoryPresets.size()))
            return factoryPresets[index].description;
        return {};
    }
    
    int PresetManager::getNumFactoryPresets()
    {
        initializePresets();
        return static_cast<int>(factoryPresets.size());
    }
}

