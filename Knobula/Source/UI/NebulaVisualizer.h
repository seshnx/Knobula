/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    NebulaVisualizer - Frequency-Reactive Particle Background
    
    Subtle, low-CPU particle visualization that reacts to EQ band energy.
    Stars are color-coded to the four frequency bands:
    - LF = Blue, LMF = Green, HMF = Yellow, HF = Red
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ColorPalette.h"
#include <random>

namespace Knobula
{
    /**
     * A single star particle in the nebula
     */
    struct StarParticle
    {
        float x = 0.0f;           // Position (0-1 normalized)
        float y = 0.0f;
        float size = 1.0f;        // Base size
        float brightness = 1.0f;  // Current brightness
        float baseBrightness = 0.5f;
        float twinklePhase = 0.0f;
        float twinkleSpeed = 0.02f;
        int bandIndex = 0;        // Which frequency band (0-3)
        float depth = 1.0f;       // Parallax depth (0.5-1.5)
    };
    
    /**
     * Nebula Visualizer Component
     * Renders a subtle star field that reacts to frequency band energy
     */
    class NebulaVisualizer : public juce::Component,
                             public juce::Timer
    {
    public:
        static constexpr int MAX_PARTICLES = 120;  // Keep CPU low
        static constexpr int PARTICLES_PER_BAND = 30;
        
        NebulaVisualizer();
        ~NebulaVisualizer() override;
        
        void paint(juce::Graphics& g) override;
        void resized() override;
        void timerCallback() override;
        
        // Update band energy levels (call from audio thread via async)
        void setBandEnergy(int band, float energy);
        
        // Set overall intensity (0-1, for fading in/out)
        void setIntensity(float intensity) { targetIntensity = intensity; }
        
        // Enable/disable animation
        void setAnimating(bool shouldAnimate);
        
    private:
        std::array<StarParticle, MAX_PARTICLES> particles;
        std::array<float, 4> bandEnergies = {0.0f, 0.0f, 0.0f, 0.0f};
        std::array<float, 4> smoothedEnergies = {0.0f, 0.0f, 0.0f, 0.0f};
        
        float currentIntensity = 1.0f;
        float targetIntensity = 1.0f;
        
        // Background nebula clouds
        struct NebulaCloud
        {
            float x, y;
            float radius;
            juce::Colour color;
            float alpha;
        };
        std::array<NebulaCloud, 6> clouds;
        
        std::mt19937 rng;
        
        void initializeParticles();
        void initializeClouds();
        void updateParticles();
        void drawBackground(juce::Graphics& g, juce::Rectangle<float> bounds);
        void drawClouds(juce::Graphics& g, juce::Rectangle<float> bounds);
        void drawStars(juce::Graphics& g, juce::Rectangle<float> bounds);
        
        juce::Colour getParticleColor(const StarParticle& particle) const;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NebulaVisualizer)
    };
}
