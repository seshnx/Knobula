/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    HighLowPassFilters Implementation
  ==============================================================================
*/

#include "HighLowPassFilters.h"

namespace Knobula
{
    //==============================================================================
    // HighPassFilter Implementation
    //==============================================================================
    
    void HighPassFilter::prepare(double newSampleRate, int /*samplesPerBlock*/)
    {
        sampleRate = newSampleRate;
        smoothedFreq.reset(sampleRate, 0.02);
        smoothedFreq.setCurrentAndTargetValue(currentFreq);
        reset();
    }
    
    void HighPassFilter::reset()
    {
        for (auto& state : channelStates)
        {
            state.x1 = state.x2 = 0.0;
            state.y1 = state.y2 = 0.0;
        }
    }
    
    void HighPassFilter::setFrequency(float freq)
    {
        if (freq != currentFreq)
        {
            currentFreq = freq;
            smoothedFreq.setTargetValue(freq);
            needsUpdate = true;
        }
    }
    
    void HighPassFilter::updateCoefficients()
    {
        if (!needsUpdate && !smoothedFreq.isSmoothing())
            return;
        
        float freq = smoothedFreq.getNextValue();
        
        // Butterworth 2nd order high-pass
        double omega = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
        double cosOmega = std::cos(omega);
        double sinOmega = std::sin(omega);
        double alpha = sinOmega / (2.0 * 0.707107);  // Q = 1/sqrt(2) for Butterworth
        
        double a0 = 1.0 + alpha;
        
        b0 = (1.0 + cosOmega) / 2.0 / a0;
        b1 = -(1.0 + cosOmega) / a0;
        b2 = (1.0 + cosOmega) / 2.0 / a0;
        a1 = -2.0 * cosOmega / a0;
        a2 = (1.0 - alpha) / a0;
        
        needsUpdate = smoothedFreq.isSmoothing();
    }
    
    float HighPassFilter::processSample(float input, int channel)
    {
        if (!isEnabled || channel < 0 || channel >= 2)
            return input;
        
        updateCoefficients();
        
        auto& state = channelStates[channel];
        double inputD = static_cast<double>(input);
        
        double output = b0 * inputD + b1 * state.x1 + b2 * state.x2 
                       - a1 * state.y1 - a2 * state.y2;
        
        state.x2 = state.x1;
        state.x1 = inputD;
        state.y2 = state.y1;
        state.y1 = output;
        
        return static_cast<float>(output);
    }
    
    void HighPassFilter::processBlock(juce::AudioBuffer<float>& buffer)
    {
        if (!isEnabled)
            return;
        
        int numChannels = std::min(buffer.getNumChannels(), 2);
        int numSamples = buffer.getNumSamples();
        
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i)
            {
                data[i] = processSample(data[i], ch);
            }
        }
    }
    
    //==============================================================================
    // LowPassFilter Implementation
    //==============================================================================
    
    void LowPassFilter::prepare(double newSampleRate, int /*samplesPerBlock*/)
    {
        sampleRate = newSampleRate;
        smoothedFreq.reset(sampleRate, 0.02);
        smoothedFreq.setCurrentAndTargetValue(currentFreq);
        reset();
    }
    
    void LowPassFilter::reset()
    {
        for (auto& state : channelStates)
        {
            state.x1 = state.x2 = 0.0;
            state.y1 = state.y2 = 0.0;
        }
    }
    
    void LowPassFilter::setFrequency(float freq)
    {
        if (freq != currentFreq)
        {
            currentFreq = freq;
            smoothedFreq.setTargetValue(freq);
            needsUpdate = true;
        }
    }
    
    void LowPassFilter::updateCoefficients()
    {
        if (!needsUpdate && !smoothedFreq.isSmoothing())
            return;
        
        float freq = smoothedFreq.getNextValue();
        
        // Butterworth 2nd order low-pass
        double omega = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
        double cosOmega = std::cos(omega);
        double sinOmega = std::sin(omega);
        double alpha = sinOmega / (2.0 * 0.707107);
        
        double a0 = 1.0 + alpha;
        
        b0 = (1.0 - cosOmega) / 2.0 / a0;
        b1 = (1.0 - cosOmega) / a0;
        b2 = (1.0 - cosOmega) / 2.0 / a0;
        a1 = -2.0 * cosOmega / a0;
        a2 = (1.0 - alpha) / a0;
        
        needsUpdate = smoothedFreq.isSmoothing();
    }
    
    float LowPassFilter::processSample(float input, int channel)
    {
        if (!isEnabled || channel < 0 || channel >= 2)
            return input;
        
        updateCoefficients();
        
        auto& state = channelStates[channel];
        double inputD = static_cast<double>(input);
        
        double output = b0 * inputD + b1 * state.x1 + b2 * state.x2 
                       - a1 * state.y1 - a2 * state.y2;
        
        state.x2 = state.x1;
        state.x1 = inputD;
        state.y2 = state.y1;
        state.y1 = output;
        
        return static_cast<float>(output);
    }
    
    void LowPassFilter::processBlock(juce::AudioBuffer<float>& buffer)
    {
        if (!isEnabled)
            return;
        
        int numChannels = std::min(buffer.getNumChannels(), 2);
        int numSamples = buffer.getNumSamples();
        
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i)
            {
                data[i] = processSample(data[i], ch);
            }
        }
    }
    
    //==============================================================================
    // FilterSection Implementation
    //==============================================================================
    
    void FilterSection::prepare(double sampleRate, int samplesPerBlock)
    {
        hpf.prepare(sampleRate, samplesPerBlock);
        lpf.prepare(sampleRate, samplesPerBlock);
    }
    
    void FilterSection::reset()
    {
        hpf.reset();
        lpf.reset();
    }
    
    void FilterSection::processBlock(juce::AudioBuffer<float>& buffer)
    {
        // HPF first, then LPF
        hpf.processBlock(buffer);
        lpf.processBlock(buffer);
    }
}
