/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    HysteresisProcessor - Analog Saturation/Modeling Stage
    
    Features:
    - Tube Harmonics: High-frequency, odd-order harmonics (air/sheen)
    - Transformer Saturate: Low-frequency, even-order harmonics (thickness/glue)
  ==============================================================================
*/

#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_core/juce_core.h>

namespace Aetheri
{
    /**
     * Analog Hysteresis Stage
     * Provides dual-character saturation with independent tube and transformer controls
     */
    class HysteresisProcessor
    {
    public:
        HysteresisProcessor() = default;
        
        void prepare(double sampleRate, int samplesPerBlock);
        void reset();
        
        void setEnabled(bool enabled) { isEnabled = enabled; }
        void setTubeHarmonics(float amount);    // 0-100%
        void setTransformerSaturate(float amount);  // 0-100%
        void setMix(float mixPercent);  // 0-100%
        
        void processBlock(juce::AudioBuffer<float>& buffer);
        float processSample(float input, int channel);
        
        bool getEnabled() const { return isEnabled; }
        float getGlowIntensity() const;  // For UI glow effect
        
    private:
        bool isEnabled = false;
        
        // Smoothed parameters
        juce::SmoothedValue<float> tubeAmount;
        juce::SmoothedValue<float> transformerAmount;
        juce::SmoothedValue<float> mixAmount;
        
        double sampleRate = 44100.0;
        
        // State for each channel
        struct ChannelState
        {
            // Tube stage state
            double tubeState = 0.0;
            
            // Transformer stage state (hysteresis)
            double hystState = 0.0;
            double prevInput = 0.0;
            
            // DC blocking
            double dcX = 0.0;
            double dcY = 0.0;
        };
        
        std::array<ChannelState, 2> channelStates;
        
        // Tube saturation (odd harmonics - asymmetric soft clipping)
        float processTube(float input, ChannelState& state, float amount);
        
        // Transformer saturation (even harmonics - symmetric saturation with hysteresis)
        float processTransformer(float input, ChannelState& state, float amount);
        
        // DC blocking filter
        float processDCBlock(float input, ChannelState& state);
        
        // Soft clipping functions
        static float softClipOdd(float x);   // Odd harmonics (tanh-like)
        static float softClipEven(float x);  // Even harmonics (parabolic)
    };
    
    //==============================================================================
    // Inline implementations for saturation functions
    //==============================================================================
    
    inline float HysteresisProcessor::softClipOdd(float x)
    {
        // Asymmetric soft clipping for odd harmonics
        // Uses a combination of tanh and polynomial
        if (std::abs(x) < 0.5f)
        {
            return x * (1.0f - 0.15f * x * x);
        }
        return std::tanh(x * 1.2f) * 0.9f;
    }
    
    inline float HysteresisProcessor::softClipEven(float x)
    {
        // Symmetric saturation with even harmonic emphasis
        // Soft knee compression curve
        float absX = std::abs(x);
        if (absX < 0.3f)
        {
            return x;
        }
        else if (absX < 0.8f)
        {
            // Add subtle even harmonics
            float sign = (x > 0.0f) ? 1.0f : -1.0f;
            return sign * (0.3f + (absX - 0.3f) * 0.8f + 0.1f * x * x);
        }
        else
        {
            // Soft limiting
            float sign = (x > 0.0f) ? 1.0f : -1.0f;
            return sign * (0.7f + std::tanh((absX - 0.8f) * 2.0f) * 0.25f);
        }
    }
}
