/*
  ==============================================================================
    Aetheri - High-Fidelity Mastering EQ
    HighLowPassFilters - Switchable HPF/LPF (12 dB/oct)
  ==============================================================================
*/

#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_core/juce_core.h>

namespace Aetheri
{
    /**
     * High-Pass Filter with 12 dB/octave slope
     * Butterworth response for flat passband
     */
    class HighPassFilter
    {
    public:
        HighPassFilter() = default;
        
        void prepare(double sampleRate, int samplesPerBlock);
        void reset();
        
        void setEnabled(bool enabled) { isEnabled = enabled; }
        void setFrequency(float freq);
        
        void processBlock(juce::AudioBuffer<float>& buffer);
        float processSample(float input, int channel);
        
        bool getEnabled() const { return isEnabled; }
        float getFrequency() const { return currentFreq; }
        
    private:
        bool isEnabled = false;
        float currentFreq = 30.0f;
        double sampleRate = 44100.0;
        
        juce::SmoothedValue<float> smoothedFreq;
        bool needsUpdate = true;
        
        // 2nd order Butterworth (12 dB/oct) - implemented as cascaded biquad
        struct BiquadState
        {
            double x1 = 0.0, x2 = 0.0;
            double y1 = 0.0, y2 = 0.0;
        };
        
        std::array<BiquadState, 2> channelStates;  // Stereo
        
        // Coefficients
        double b0 = 1.0, b1 = 0.0, b2 = 0.0;
        double a1 = 0.0, a2 = 0.0;
        
        void updateCoefficients();
    };
    
    /**
     * Low-Pass Filter with 12 dB/octave slope
     * Butterworth response for flat passband
     */
    class LowPassFilter
    {
    public:
        LowPassFilter() = default;
        
        void prepare(double sampleRate, int samplesPerBlock);
        void reset();
        
        void setEnabled(bool enabled) { isEnabled = enabled; }
        void setFrequency(float freq);
        
        void processBlock(juce::AudioBuffer<float>& buffer);
        float processSample(float input, int channel);
        
        bool getEnabled() const { return isEnabled; }
        float getFrequency() const { return currentFreq; }
        
    private:
        bool isEnabled = false;
        float currentFreq = 18000.0f;
        double sampleRate = 44100.0;
        
        juce::SmoothedValue<float> smoothedFreq;
        bool needsUpdate = true;
        
        struct BiquadState
        {
            double x1 = 0.0, x2 = 0.0;
            double y1 = 0.0, y2 = 0.0;
        };
        
        std::array<BiquadState, 2> channelStates;
        
        double b0 = 1.0, b1 = 0.0, b2 = 0.0;
        double a1 = 0.0, a2 = 0.0;
        
        void updateCoefficients();
    };
    
    /**
     * Combined HPF/LPF Section
     */
    class FilterSection
    {
    public:
        FilterSection() = default;
        
        void prepare(double sampleRate, int samplesPerBlock);
        void reset();
        
        void setHPFEnabled(bool enabled) { hpf.setEnabled(enabled); }
        void setHPFFrequency(float freq) { hpf.setFrequency(freq); }
        
        void setLPFEnabled(bool enabled) { lpf.setEnabled(enabled); }
        void setLPFFrequency(float freq) { lpf.setFrequency(freq); }
        
        void processBlock(juce::AudioBuffer<float>& buffer);
        
        HighPassFilter& getHPF() { return hpf; }
        LowPassFilter& getLPF() { return lpf; }
        
    private:
        HighPassFilter hpf;
        LowPassFilter lpf;
    };
}
