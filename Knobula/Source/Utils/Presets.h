/*
  ==============================================================================
    Aetheri - High-Fidelity Mastering EQ
    Preset Definitions and Management
  ==============================================================================
*/

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>
#include <vector>
#include <memory>
#include <functional>

namespace Aetheri
{
    /**
     * Preset data structure
     */
    struct Preset
    {
        juce::String name;
        juce::String description;
        std::function<void(juce::AudioProcessorValueTreeState&)> applyFunction;
        
        Preset(const juce::String& n, const juce::String& d, 
               std::function<void(juce::AudioProcessorValueTreeState&)> f)
            : name(n), description(d), applyFunction(f) {}
    };
    
    /**
     * Factory presets for Aetheri
     */
    class PresetManager
    {
    public:
        static std::vector<Preset> createFactoryPresets();
        static void applyPreset(juce::AudioProcessorValueTreeState& apvts, int presetIndex);
        static int getNumFactoryPresets();
        static juce::String getPresetName(int index);
        static juce::String getPresetDescription(int index);
        
    private:
        static std::vector<Preset> factoryPresets;
        static void initializePresets();
    };
}

