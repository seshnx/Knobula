/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    HysteresisProcessor Implementation
  ==============================================================================
*/

#include "HysteresisProcessor.h"

namespace Aetheri
{
    void HysteresisProcessor::prepare(double newSampleRate, int /*samplesPerBlock*/)
    {
        sampleRate = newSampleRate;
        
        // Smooth parameter changes over 20ms
        tubeAmount.reset(sampleRate, 0.02);
        transformerAmount.reset(sampleRate, 0.02);
        mixAmount.reset(sampleRate, 0.02);
        
        tubeAmount.setCurrentAndTargetValue(0.0f);
        transformerAmount.setCurrentAndTargetValue(0.0f);
        mixAmount.setCurrentAndTargetValue(1.0f);
        
        reset();
    }
    
    void HysteresisProcessor::reset()
    {
        for (auto& state : channelStates)
        {
            state.tubeState = 0.0;
            state.hystState = 0.0;
            state.prevInput = 0.0;
            state.dcX = 0.0;
            state.dcY = 0.0;
        }
    }
    
    void HysteresisProcessor::setTubeHarmonics(float amount)
    {
        // Scale 0-100% to internal range (0-0.7 for subtle to moderate saturation)
        tubeAmount.setTargetValue(amount / 100.0f * 0.7f);
    }
    
    void HysteresisProcessor::setTransformerSaturate(float amount)
    {
        // Scale 0-100% to internal range
        transformerAmount.setTargetValue(amount / 100.0f * 0.6f);
    }
    
    void HysteresisProcessor::setMix(float mixPercent)
    {
        mixAmount.setTargetValue(mixPercent / 100.0f);
    }
    
    float HysteresisProcessor::getGlowIntensity() const
    {
        if (!isEnabled)
            return 0.0f;
        
        // Combine tube and transformer amounts for glow intensity
        float tubeVal = tubeAmount.getCurrentValue();
        float transVal = transformerAmount.getCurrentValue();
        return std::min(1.0f, (tubeVal + transVal) * 1.5f);
    }
    
    float HysteresisProcessor::processTube(float input, ChannelState& state, float amount)
    {
        if (amount < 0.001f)
            return input;
        
        // Pre-emphasis on high frequencies for "air" effect
        float highFreqContent = input - static_cast<float>(state.tubeState);
        state.tubeState = state.tubeState * 0.95 + input * 0.05;  // Simple lowpass for state
        
        // Apply tube saturation (odd harmonics)
        float saturated = softClipOdd(input + highFreqContent * amount * 0.5f);
        
        // Add subtle odd harmonic distortion
        float harmonics = saturated - input;
        
        // Mix based on amount
        return input + harmonics * amount * 2.0f;
    }
    
    float HysteresisProcessor::processTransformer(float input, ChannelState& state, float amount)
    {
        if (amount < 0.001f)
            return input;
        
        // Hysteresis modeling - the output depends on both current input and previous state
        // This creates the "glue" and "thickness" characteristic of transformer saturation
        
        double inputD = static_cast<double>(input);
        double diff = inputD - state.prevInput;
        
        // Hysteresis curve - output lags behind input slightly, creating phase relationships
        double hystCoeff = 0.1 * amount;
        double targetState = inputD;
        
        // If signal is rising, state follows faster; if falling, slower (magnetic lag)
        if (diff > 0)
        {
            state.hystState = state.hystState * (1.0 - hystCoeff * 1.2) + targetState * hystCoeff * 1.2;
        }
        else
        {
            state.hystState = state.hystState * (1.0 - hystCoeff * 0.8) + targetState * hystCoeff * 0.8;
        }
        
        state.prevInput = inputD;
        
        // Apply even-order saturation
        float combined = static_cast<float>(inputD * 0.7 + state.hystState * 0.3);
        float saturated = softClipEven(combined * (1.0f + amount * 0.5f));
        
        // Bass emphasis from transformer core behavior
        float lowFreqBoost = static_cast<float>(state.hystState) * amount * 0.1f;
        
        return saturated + lowFreqBoost;
    }
    
    float HysteresisProcessor::processDCBlock(float input, ChannelState& state)
    {
        // Simple DC blocking filter: y[n] = x[n] - x[n-1] + 0.995 * y[n-1]
        double output = input - state.dcX + 0.995 * state.dcY;
        state.dcX = input;
        state.dcY = output;
        return static_cast<float>(output);
    }
    
    float HysteresisProcessor::processSample(float input, int channel)
    {
        if (!isEnabled || channel < 0 || channel >= 2)
            return input;
        
        auto& state = channelStates[channel];
        
        float tube = tubeAmount.getNextValue();
        float transformer = transformerAmount.getNextValue();
        float mix = mixAmount.getNextValue();
        
        // Process through saturation stages
        float processed = input;
        
        // Tube stage (odd harmonics, high-frequency emphasis)
        processed = processTube(processed, state, tube);
        
        // Transformer stage (even harmonics, low-frequency emphasis, hysteresis)
        processed = processTransformer(processed, state, transformer);
        
        // DC blocking to remove any DC offset introduced by saturation
        processed = processDCBlock(processed, state);
        
        // Dry/wet mix
        return input * (1.0f - mix) + processed * mix;
    }
    
    void HysteresisProcessor::processBlock(juce::AudioBuffer<float>& buffer)
    {
        if (!isEnabled)
            return;
        
        int numChannels = std::min(buffer.getNumChannels(), 2);
        int numSamples = buffer.getNumSamples();
        
        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* data = buffer.getWritePointer(channel);
            
            for (int i = 0; i < numSamples; ++i)
            {
                data[i] = processSample(data[i], channel);
            }
        }
    }
}
