/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    MidSideProcessor - M/S Encoding/Decoding
  ==============================================================================
*/

#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_core/juce_core.h>

namespace Aetheri
{
    /**
     * Mid/Side Encoding and Decoding
     * Converts stereo L/R to M/S and back
     */
    class MidSideProcessor
    {
    public:
        MidSideProcessor() = default;
        
        // Encode L/R to M/S in-place
        static void encodeToMidSide(juce::AudioBuffer<float>& buffer)
        {
            if (buffer.getNumChannels() < 2)
                return;
            
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
        
        // Decode M/S to L/R in-place
        static void decodeFromMidSide(juce::AudioBuffer<float>& buffer)
        {
            if (buffer.getNumChannels() < 2)
                return;
            
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
        
        // Sample-wise versions
        static inline void encodeSample(float& left, float& right)
        {
            float l = left;
            float r = right;
            left = (l + r) * 0.5f;
            right = (l - r) * 0.5f;
        }
        
        static inline void decodeSample(float& mid, float& side)
        {
            float m = mid;
            float s = side;
            mid = m + s;
            side = m - s;
        }
    };
}
