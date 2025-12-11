/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    PassiveEQ - 4-Band Passive Topology EQ (64-bit Internal Processing)
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Parameters.h"

namespace Knobula
{
    /**
     * Single EQ band with passive/parallel topology modeling
     * Features musical, broad Q values and Bell/Shelf switching
     */
    class EQBand
    {
    public:
        enum class CurveType { Bell, Shelf };
        
        EQBand() = default;
        
        void prepare(double sampleRate, int samplesPerBlock);
        void reset();
        
        void setParameters(float frequency, float gainDB, float q, CurveType curve, bool enabled);
        void updateCoefficients();
        
        float processSample(float input);
        void processBlock(float* data, int numSamples);
        
        // For visualization
        float getCurrentGain() const { return currentGainDB; }
        float getCurrentFrequency() const { return currentFreq; }
        bool isEnabled() const { return bandEnabled; }
        
    private:
        // Biquad state (64-bit precision)
        double x1 = 0.0, x2 = 0.0;
        double y1 = 0.0, y2 = 0.0;
        
        // Coefficients
        double b0 = 1.0, b1 = 0.0, b2 = 0.0;
        double a0 = 1.0, a1 = 0.0, a2 = 0.0;
        
        // Smoothed parameters
        juce::SmoothedValue<float> smoothedGain;
        juce::SmoothedValue<float> smoothedFreq;
        
        // Current state
        float currentFreq = 1000.0f;
        float currentGainDB = 0.0f;
        float currentQ = 0.707f;
        CurveType currentCurve = CurveType::Bell;
        bool bandEnabled = true;
        
        double sampleRate = 44100.0;
        bool needsUpdate = true;
        
        void calculateBellCoefficients(double freq, double gainDB, double q);
        void calculateShelfCoefficients(double freq, double gainDB, bool isHighShelf);
    };
    
    /**
     * 4-Band Passive EQ for one channel
     * Models the musical interaction of a passive EQ topology
     */
    class ChannelEQ
    {
    public:
        static constexpr int NumBands = 4;
        
        ChannelEQ() = default;
        
        void prepare(double sampleRate, int samplesPerBlock);
        void reset();
        
        void setBandParameters(int band, float frequency, float gainDB, float trimDB, 
                              EQBand::CurveType curve, bool enabled);
        
        void processBlock(juce::AudioBuffer<float>& buffer, int channel);
        float processSample(float input);
        
        // For visualization - get band energy
        float getBandEnergy(int band) const { return bandEnergies[band]; }
        const EQBand& getBand(int band) const { return bands[band]; }
        
    private:
        std::array<EQBand, NumBands> bands;
        std::array<float, NumBands> bandEnergies = {0.0f, 0.0f, 0.0f, 0.0f};
        
        // Passive topology: bands interact through slight phase/gain coupling
        float passiveCoupling = 0.02f;  // Subtle interaction between bands
    };
    
    /**
     * Dual-Channel 4-Band Passive EQ
     * Supports both L/R and M/S processing modes
     */
    class PassiveEQ
    {
    public:
        PassiveEQ() = default;
        
        void prepare(double sampleRate, int samplesPerBlock);
        void reset();
        
        void setInputGain(float gainDB);
        void setOutputTrim(float gainDB);
        void setStereoMode(bool isMidSide);
        void setChannelLink(bool linked);
        
        // Set parameters for a specific band and channel
        void setBandParameters(int band, int channel, 
                              float frequency, float gainDB, float trimDB,
                              EQBand::CurveType curve, bool enabled);
        
        void processBlock(juce::AudioBuffer<float>& buffer);
        
        // For visualization
        float getBandEnergy(int band, int channel) const;
        float getInputLevel(int channel) const { return inputLevels[channel]; }
        float getOutputLevel(int channel) const { return outputLevels[channel]; }
        
    private:
        std::array<ChannelEQ, 2> channelEQs;
        
        juce::SmoothedValue<float> inputGain;
        juce::SmoothedValue<float> outputTrim;
        
        bool midSideMode = false;
        bool channelsLinked = true;
        
        // Level metering
        std::array<float, 2> inputLevels = {0.0f, 0.0f};
        std::array<float, 2> outputLevels = {0.0f, 0.0f};
        
        double sampleRate = 44100.0;
        
        void encodeToMidSide(juce::AudioBuffer<float>& buffer);
        void decodeFromMidSide(juce::AudioBuffer<float>& buffer);
    };
}
