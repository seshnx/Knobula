/*
  ==============================================================================
    Knobula - High-Fidelity Dual-Channel Mastering EQ
    PluginProcessor - Audio Processing Core
    
    Features:
    - 4-Band Passive Topology EQ (64-bit internal processing)
    - Analog Hysteresis Stage (Tube + Transformer saturation)
    - Dual-Channel (L/R or M/S) Processing
    - HPF/LPF with 12 dB/oct slope
    - VU Metering
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "DSP/PassiveEQ.h"
#include "DSP/HysteresisProcessor.h"
#include "DSP/VUMeter.h"
#include "DSP/HighLowPassFilters.h"
#include "Utils/Parameters.h"

class KnobulaAudioProcessor : public juce::AudioProcessor
{
public:
    KnobulaAudioProcessor();
    ~KnobulaAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameter access
    juce::AudioProcessorValueTreeState& getParameters() { return parameters; }
    
    // Metering access (for UI)
    float getInputLevel(int channel) const;
    float getOutputLevel(int channel) const;
    float getBandEnergy(int band, int channel) const;
    
    // VU Meters
    const Knobula::StereoVUMeter& getInputVU() const { return inputVU; }
    const Knobula::StereoVUMeter& getOutputVU() const { return outputVU; }
    
    // Hysteresis glow intensity
    float getHysteresisGlowIntensity() const;

private:
    // Parameter tree state
    juce::AudioProcessorValueTreeState parameters;
    
    // DSP Processors
    Knobula::PassiveEQ passiveEQ;
    Knobula::HysteresisProcessor hysteresis;
    Knobula::FilterSection filterSection;
    
    // Metering
    Knobula::StereoVUMeter inputVU;
    Knobula::StereoVUMeter outputVU;
    
    // Cached parameter values
    std::atomic<float>* inputGainParam = nullptr;
    std::atomic<float>* outputTrimParam = nullptr;
    std::atomic<float>* stereoModeParam = nullptr;
    std::atomic<float>* channelLinkParam = nullptr;
    std::atomic<float>* hpfEnabledParam = nullptr;
    std::atomic<float>* hpfFreqParam = nullptr;
    std::atomic<float>* lpfEnabledParam = nullptr;
    std::atomic<float>* lpfFreqParam = nullptr;
    std::atomic<float>* hystEnabledParam = nullptr;
    std::atomic<float>* tubeHarmonicsParam = nullptr;
    std::atomic<float>* transformerSatParam = nullptr;
    std::atomic<float>* hystMixParam = nullptr;
    
    // Band parameters [band][channel]
    std::array<std::array<std::atomic<float>*, 2>, 4> bandGainParams;
    std::array<std::array<std::atomic<float>*, 2>, 4> bandTrimParams;
    std::array<std::array<std::atomic<float>*, 2>, 4> bandFreqParams;
    std::array<std::array<std::atomic<float>*, 2>, 4> bandEnabledParams;
    std::array<std::array<std::atomic<float>*, 2>, 2> bandCurveParams;  // Only LF and HF
    
    void updateDSPFromParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobulaAudioProcessor)
};
