/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    VUMeter - Slow-moving RMS Level Meter (VU-style ballistics)
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace Knobula
{
    /**
     * VU Meter with classic VU-style ballistics
     * - 300ms integration time (per VU standard)
     * - Slow rise, slow fall
     * - RMS measurement for accurate perceived loudness
     */
    class VUMeter
    {
    public:
        VUMeter() = default;
        
        void prepare(double sampleRate);
        void reset();
        
        void pushSamples(const float* data, int numSamples);
        
        // Get current level in dB (-inf to +3 VU)
        float getLevelDB() const;
        
        // Get normalized level (0.0 to 1.0, where 1.0 = 0 VU)
        float getNormalizedLevel() const;
        
        // Get peak level (for peak indicator)
        float getPeakDB() const;
        float getNormalizedPeak() const;
        
    private:
        double sampleRate = 44100.0;
        
        // RMS calculation
        double rmsSum = 0.0;
        int rmsSampleCount = 0;
        int rmsWindowSize = 4410;  // ~100ms at 44.1kHz
        
        // VU ballistics
        float vuLevel = 0.0f;
        float vuAttackCoeff = 0.0f;   // ~300ms rise time
        float vuReleaseCoeff = 0.0f;  // ~300ms fall time
        
        // Peak detection
        float peakLevel = 0.0f;
        float peakHoldTime = 0.0f;
        float peakDecayCoeff = 0.0f;
        int peakHoldSamples = 0;
        int currentPeakHold = 0;
        
        static constexpr float VU_REFERENCE = 1.0f;  // 0 dBFS = +3 VU
        static constexpr float MIN_DB = -60.0f;
    };
    
    /**
     * Stereo VU Meter pair
     */
    class StereoVUMeter
    {
    public:
        StereoVUMeter() = default;
        
        void prepare(double sampleRate);
        void reset();
        
        void pushSamples(const juce::AudioBuffer<float>& buffer);
        
        VUMeter& getLeft() { return meters[0]; }
        VUMeter& getRight() { return meters[1]; }
        
        const VUMeter& getLeft() const { return meters[0]; }
        const VUMeter& getRight() const { return meters[1]; }
        
    private:
        std::array<VUMeter, 2> meters;
    };
}
