/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    VUMeter Implementation
  ==============================================================================
*/

#include "VUMeter.h"

namespace Knobula
{
    void VUMeter::prepare(double newSampleRate)
    {
        sampleRate = newSampleRate;
        
        // VU meter has 300ms integration time
        // Attack and release times are both ~300ms for authentic VU behavior
        float vuTimeConstant = 0.3f;  // 300ms
        vuAttackCoeff = std::exp(-1.0f / (static_cast<float>(sampleRate) * vuTimeConstant));
        vuReleaseCoeff = vuAttackCoeff;  // Same for VU meters
        
        // RMS window size (~100ms)
        rmsWindowSize = static_cast<int>(sampleRate * 0.1);
        
        // Peak hold time (~2 seconds, then decay)
        peakHoldSamples = static_cast<int>(sampleRate * 2.0);
        peakDecayCoeff = std::exp(-1.0f / (static_cast<float>(sampleRate) * 0.5f));  // 500ms decay
        
        reset();
    }
    
    void VUMeter::reset()
    {
        rmsSum = 0.0;
        rmsSampleCount = 0;
        vuLevel = 0.0f;
        peakLevel = 0.0f;
        peakHoldTime = 0.0f;
        currentPeakHold = 0;
    }
    
    void VUMeter::pushSamples(const float* data, int numSamples)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            float sample = data[i];
            float absSample = std::abs(sample);
            
            // RMS accumulation
            rmsSum += static_cast<double>(sample * sample);
            rmsSampleCount++;
            
            // Calculate RMS when window is full
            if (rmsSampleCount >= rmsWindowSize)
            {
                float rmsValue = std::sqrt(static_cast<float>(rmsSum / rmsSampleCount));
                
                // Apply VU ballistics
                if (rmsValue > vuLevel)
                {
                    // Attack (rising)
                    vuLevel = vuLevel * vuAttackCoeff + rmsValue * (1.0f - vuAttackCoeff);
                }
                else
                {
                    // Release (falling)
                    vuLevel = vuLevel * vuReleaseCoeff + rmsValue * (1.0f - vuReleaseCoeff);
                }
                
                // Reset RMS accumulator
                rmsSum = 0.0;
                rmsSampleCount = 0;
            }
            
            // Peak detection
            if (absSample > peakLevel)
            {
                peakLevel = absSample;
                currentPeakHold = 0;
            }
            else
            {
                currentPeakHold++;
                if (currentPeakHold > peakHoldSamples)
                {
                    // Decay peak after hold time
                    peakLevel *= peakDecayCoeff;
                }
            }
        }
    }
    
    float VUMeter::getLevelDB() const
    {
        if (vuLevel < 1e-10f)
            return MIN_DB;
        
        // Convert to dB, with 0 VU calibrated to -3 dBFS (typical mastering reference)
        float db = 20.0f * std::log10(vuLevel / VU_REFERENCE) + 3.0f;
        return std::max(MIN_DB, db);
    }
    
    float VUMeter::getNormalizedLevel() const
    {
        // Normalize so 0 VU (= -3 dBFS) maps to ~0.75, and +3 VU (0 dBFS) maps to 1.0
        float db = getLevelDB();
        
        // Map -40 dB to 0.0, 0 VU to 0.75, +3 VU to 1.0
        if (db < -40.0f)
            return 0.0f;
        
        if (db < 0.0f)
        {
            // -40 to 0 VU maps to 0.0 to 0.75
            return (db + 40.0f) / 40.0f * 0.75f;
        }
        else
        {
            // 0 to +3 VU maps to 0.75 to 1.0
            return 0.75f + (db / 3.0f) * 0.25f;
        }
    }
    
    float VUMeter::getPeakDB() const
    {
        if (peakLevel < 1e-10f)
            return MIN_DB;
        
        return 20.0f * std::log10(peakLevel);
    }
    
    float VUMeter::getNormalizedPeak() const
    {
        float db = getPeakDB();
        
        // Map -40 dB to 0.0, 0 dBFS to 1.0
        if (db < -40.0f)
            return 0.0f;
        
        return (db + 40.0f) / 40.0f;
    }
    
    //==============================================================================
    // StereoVUMeter Implementation
    //==============================================================================
    
    void StereoVUMeter::prepare(double sampleRate)
    {
        for (auto& meter : meters)
        {
            meter.prepare(sampleRate);
        }
    }
    
    void StereoVUMeter::reset()
    {
        for (auto& meter : meters)
        {
            meter.reset();
        }
    }
    
    void StereoVUMeter::pushSamples(const juce::AudioBuffer<float>& buffer)
    {
        int numChannels = std::min(buffer.getNumChannels(), 2);
        int numSamples = buffer.getNumSamples();
        
        for (int ch = 0; ch < numChannels; ++ch)
        {
            meters[ch].pushSamples(buffer.getReadPointer(ch), numSamples);
        }
    }
}
