/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    PassiveEQ Implementation
  ==============================================================================
*/

#include "PassiveEQ.h"

namespace Knobula
{
    //==============================================================================
    // EQBand Implementation
    //==============================================================================
    
    void EQBand::prepare(double newSampleRate, int /*samplesPerBlock*/)
    {
        sampleRate = newSampleRate;
        smoothedGain.reset(sampleRate, 0.05);  // 50ms smoothing
        smoothedFreq.reset(sampleRate, 0.05);
        reset();
    }
    
    void EQBand::reset()
    {
        x1 = x2 = y1 = y2 = 0.0;
    }
    
    void EQBand::setParameters(float frequency, float gainDB, float q, CurveType curve, bool enabled)
    {
        bandEnabled = enabled;
        currentQ = q;
        currentCurve = curve;
        
        if (frequency != currentFreq || gainDB != currentGainDB)
        {
            smoothedFreq.setTargetValue(frequency);
            smoothedGain.setTargetValue(gainDB);
            currentFreq = frequency;
            currentGainDB = gainDB;
            needsUpdate = true;
        }
    }
    
    void EQBand::updateCoefficients()
    {
        if (!needsUpdate && !smoothedFreq.isSmoothing() && !smoothedGain.isSmoothing())
            return;
        
        float freq = smoothedFreq.getNextValue();
        float gain = smoothedGain.getNextValue();
        
        if (currentCurve == CurveType::Bell)
        {
            calculateBellCoefficients(freq, gain, currentQ);
        }
        else
        {
            // For Shelf curves: LF uses low shelf, HF uses high shelf
            bool isHighShelf = (freq > 2000.0f);
            calculateShelfCoefficients(freq, gain, isHighShelf);
        }
        
        needsUpdate = smoothedFreq.isSmoothing() || smoothedGain.isSmoothing();
    }
    
    void EQBand::calculateBellCoefficients(double freq, double gainDB, double q)
    {
        if (std::abs(gainDB) < 0.01)
        {
            // Bypass - unity gain
            b0 = 1.0; b1 = 0.0; b2 = 0.0;
            a0 = 1.0; a1 = 0.0; a2 = 0.0;
            return;
        }
        
        double A = std::pow(10.0, gainDB / 40.0);
        double omega = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
        double sinOmega = std::sin(omega);
        double cosOmega = std::cos(omega);
        double alpha = sinOmega / (2.0 * q);
        
        // Peak/Bell filter coefficients
        b0 = 1.0 + alpha * A;
        b1 = -2.0 * cosOmega;
        b2 = 1.0 - alpha * A;
        a0 = 1.0 + alpha / A;
        a1 = -2.0 * cosOmega;
        a2 = 1.0 - alpha / A;
        
        // Normalize
        b0 /= a0; b1 /= a0; b2 /= a0;
        a1 /= a0; a2 /= a0; a0 = 1.0;
    }
    
    void EQBand::calculateShelfCoefficients(double freq, double gainDB, bool isHighShelf)
    {
        if (std::abs(gainDB) < 0.01)
        {
            b0 = 1.0; b1 = 0.0; b2 = 0.0;
            a0 = 1.0; a1 = 0.0; a2 = 0.0;
            return;
        }
        
        double A = std::pow(10.0, gainDB / 40.0);
        double omega = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
        double sinOmega = std::sin(omega);
        double cosOmega = std::cos(omega);
        double S = 1.0;  // Shelf slope
        double alpha = sinOmega / 2.0 * std::sqrt((A + 1.0 / A) * (1.0 / S - 1.0) + 2.0);
        double sqrtA_alpha = 2.0 * std::sqrt(A) * alpha;
        
        if (isHighShelf)
        {
            // High shelf
            b0 = A * ((A + 1.0) + (A - 1.0) * cosOmega + sqrtA_alpha);
            b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * cosOmega);
            b2 = A * ((A + 1.0) + (A - 1.0) * cosOmega - sqrtA_alpha);
            a0 = (A + 1.0) - (A - 1.0) * cosOmega + sqrtA_alpha;
            a1 = 2.0 * ((A - 1.0) - (A + 1.0) * cosOmega);
            a2 = (A + 1.0) - (A - 1.0) * cosOmega - sqrtA_alpha;
        }
        else
        {
            // Low shelf
            b0 = A * ((A + 1.0) - (A - 1.0) * cosOmega + sqrtA_alpha);
            b1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * cosOmega);
            b2 = A * ((A + 1.0) - (A - 1.0) * cosOmega - sqrtA_alpha);
            a0 = (A + 1.0) + (A - 1.0) * cosOmega + sqrtA_alpha;
            a1 = -2.0 * ((A - 1.0) + (A + 1.0) * cosOmega);
            a2 = (A + 1.0) + (A - 1.0) * cosOmega - sqrtA_alpha;
        }
        
        // Normalize
        b0 /= a0; b1 /= a0; b2 /= a0;
        a1 /= a0; a2 /= a0; a0 = 1.0;
    }
    
    float EQBand::processSample(float input)
    {
        if (!bandEnabled)
            return input;
        
        updateCoefficients();
        
        // Direct Form II Transposed Biquad
        double inputD = static_cast<double>(input);
        double output = b0 * inputD + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
        
        // Update state
        x2 = x1;
        x1 = inputD;
        y2 = y1;
        y1 = output;
        
        return static_cast<float>(output);
    }
    
    void EQBand::processBlock(float* data, int numSamples)
    {
        if (!bandEnabled)
            return;
        
        for (int i = 0; i < numSamples; ++i)
        {
            data[i] = processSample(data[i]);
        }
    }
    
    //==============================================================================
    // ChannelEQ Implementation
    //==============================================================================
    
    void ChannelEQ::prepare(double sampleRate, int samplesPerBlock)
    {
        for (int i = 0; i < NumBands; ++i)
        {
            bands[i].prepare(sampleRate, samplesPerBlock);
        }
        bandEnergies.fill(0.0f);
    }
    
    void ChannelEQ::reset()
    {
        for (auto& band : bands)
        {
            band.reset();
        }
        bandEnergies.fill(0.0f);
    }
    
    void ChannelEQ::setBandParameters(int band, float frequency, float gainDB, float trimDB,
                                       EQBand::CurveType curve, bool enabled)
    {
        if (band >= 0 && band < NumBands)
        {
            float totalGain = gainDB + trimDB;
            float q = BandDefaults::getQ(band);
            bands[band].setParameters(frequency, totalGain, q, curve, enabled);
        }
    }
    
    float ChannelEQ::processSample(float input)
    {
        float output = input;
        
        // Process through all bands in series (passive topology)
        for (int i = 0; i < NumBands; ++i)
        {
            float bandOutput = bands[i].processSample(output);
            
            // Update energy measurement with envelope following
            float energy = std::abs(bandOutput - output);
            bandEnergies[i] = bandEnergies[i] * 0.99f + energy * 0.01f;
            
            output = bandOutput;
        }
        
        return output;
    }
    
    void ChannelEQ::processBlock(juce::AudioBuffer<float>& buffer, int channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        int numSamples = buffer.getNumSamples();
        
        for (int i = 0; i < numSamples; ++i)
        {
            channelData[i] = processSample(channelData[i]);
        }
    }
    
    //==============================================================================
    // PassiveEQ Implementation
    //==============================================================================
    
    void PassiveEQ::prepare(double newSampleRate, int samplesPerBlock)
    {
        sampleRate = newSampleRate;
        
        for (auto& eq : channelEQs)
        {
            eq.prepare(sampleRate, samplesPerBlock);
        }
        
        inputGain.reset(sampleRate, 0.02);
        outputTrim.reset(sampleRate, 0.02);
        
        inputGain.setCurrentAndTargetValue(1.0f);
        outputTrim.setCurrentAndTargetValue(1.0f);
        
        inputLevels.fill(0.0f);
        outputLevels.fill(0.0f);
    }
    
    void PassiveEQ::reset()
    {
        for (auto& eq : channelEQs)
        {
            eq.reset();
        }
        inputLevels.fill(0.0f);
        outputLevels.fill(0.0f);
    }
    
    void PassiveEQ::setInputGain(float gainDB)
    {
        inputGain.setTargetValue(juce::Decibels::decibelsToGain(gainDB));
    }
    
    void PassiveEQ::setOutputTrim(float gainDB)
    {
        outputTrim.setTargetValue(juce::Decibels::decibelsToGain(gainDB));
    }
    
    void PassiveEQ::setStereoMode(bool isMidSide)
    {
        midSideMode = isMidSide;
    }
    
    void PassiveEQ::setChannelLink(bool linked)
    {
        channelsLinked = linked;
    }
    
    void PassiveEQ::setBandParameters(int band, int channel,
                                       float frequency, float gainDB, float trimDB,
                                       EQBand::CurveType curve, bool enabled)
    {
        if (channel >= 0 && channel < 2)
        {
            channelEQs[channel].setBandParameters(band, frequency, gainDB, trimDB, curve, enabled);
            
            // If linked, apply same parameters to other channel
            if (channelsLinked && channel == 0)
            {
                channelEQs[1].setBandParameters(band, frequency, gainDB, trimDB, curve, enabled);
            }
        }
    }
    
    void PassiveEQ::processBlock(juce::AudioBuffer<float>& buffer)
    {
        if (buffer.getNumChannels() < 2)
            return;
        
        int numSamples = buffer.getNumSamples();
        
        // Apply input gain and measure input levels
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            float maxLevel = 0.0f;
            
            for (int i = 0; i < numSamples; ++i)
            {
                data[i] *= inputGain.getNextValue();
                maxLevel = std::max(maxLevel, std::abs(data[i]));
            }
            
            // Slow-moving level for VU (RMS-like)
            inputLevels[ch] = inputLevels[ch] * 0.95f + maxLevel * 0.05f;
        }
        
        // Encode to M/S if needed
        if (midSideMode)
        {
            encodeToMidSide(buffer);
        }
        
        // Process EQ for each channel
        for (int ch = 0; ch < 2; ++ch)
        {
            channelEQs[ch].processBlock(buffer, ch);
        }
        
        // Decode from M/S if needed
        if (midSideMode)
        {
            decodeFromMidSide(buffer);
        }
        
        // Apply output trim and measure output levels
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            float maxLevel = 0.0f;
            
            for (int i = 0; i < numSamples; ++i)
            {
                data[i] *= outputTrim.getNextValue();
                maxLevel = std::max(maxLevel, std::abs(data[i]));
            }
            
            outputLevels[ch] = outputLevels[ch] * 0.95f + maxLevel * 0.05f;
        }
    }
    
    float PassiveEQ::getBandEnergy(int band, int channel) const
    {
        if (channel >= 0 && channel < 2)
        {
            return channelEQs[channel].getBandEnergy(band);
        }
        return 0.0f;
    }
    
    void PassiveEQ::encodeToMidSide(juce::AudioBuffer<float>& buffer)
    {
        auto* left = buffer.getWritePointer(0);
        auto* right = buffer.getWritePointer(1);
        int numSamples = buffer.getNumSamples();
        
        for (int i = 0; i < numSamples; ++i)
        {
            float l = left[i];
            float r = right[i];
            
            left[i] = (l + r) * 0.5f;   // Mid
            right[i] = (l - r) * 0.5f;  // Side
        }
    }
    
    void PassiveEQ::decodeFromMidSide(juce::AudioBuffer<float>& buffer)
    {
        auto* mid = buffer.getWritePointer(0);
        auto* side = buffer.getWritePointer(1);
        int numSamples = buffer.getNumSamples();
        
        for (int i = 0; i < numSamples; ++i)
        {
            float m = mid[i];
            float s = side[i];
            
            mid[i] = m + s;   // Left
            side[i] = m - s;  // Right
        }
    }
}
