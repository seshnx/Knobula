/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    NebulaVisualizer - Frequency-Reactive Particle Background
    
    Subtle, low-CPU particle visualization that reacts to EQ band energy.
    Stars are color-coded to the four frequency bands:
    - LF = Blue, LMF = Green, HMF = Yellow, HF = Red
    
    Uses standard JUCE Graphics (Direct2D/Metal in JUCE 8) for optimal performance
  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_core/juce_core.h>
#include "ColorPalette.h"
#include <random>
#include <array>

namespace Aetheri
{
    /**
     * A nebula cloud cluster - groups of particles that form organic shapes
     */
    struct NebulaCluster
    {
        float centerX = 0.0f;      // Cluster center (0-1 normalized)
        float centerY = 0.0f;
        float radius = 0.2f;      // Cluster radius
        float density = 1.0f;     // Particle density in cluster
        float brightness = 1.0f;  // Overall cluster brightness
        float driftX = 0.0f;      // Drift velocity
        float driftY = 0.0f;
        int bandIndex = 0;        // Which frequency band (0-3)
        float age = 0.0f;         // Cluster age for evolution
    };
    
    /**
     * A single particle in a nebula cluster
     */
    struct NebulaParticle
    {
        float x = 0.0f;           // Position relative to cluster center
        float y = 0.0f;
        float size = 1.0f;        // Particle size
        float brightness = 1.0f;  // Current brightness
        float baseBrightness = 0.3f;
        float phase = 0.0f;       // Animation phase
        float speed = 0.01f;      // Animation speed
    };
    
    /**
     * Nebula Visualizer Component using standard JUCE Graphics
     * Optimized for performance with reduced particle count
     */
    class NebulaVisualizer : public juce::Component,
                             public juce::Timer
    {
    public:
        static constexpr int NUM_CLUSTERS = 8;     // Number of nebula cloud clusters
        static constexpr int PARTICLES_PER_CLUSTER = 40;  // Particles per cluster
        static constexpr int MAX_PARTICLES = NUM_CLUSTERS * PARTICLES_PER_CLUSTER;
        
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
        
        // VU Meter data (not used, kept for compatibility)
        void setVUMeterData(float /*inputLeft*/, float /*inputRight*/, float /*inputLeftPeak*/, float /*inputRightPeak*/,
                           float /*outputLeft*/, float /*outputRight*/, float /*outputLeftPeak*/, float /*outputRightPeak*/) {}
        
    private:
        std::array<NebulaCluster, NUM_CLUSTERS> clusters;
        std::array<std::array<NebulaParticle, PARTICLES_PER_CLUSTER>, NUM_CLUSTERS> particles;
        std::array<float, 4> bandEnergies = {0.0f, 0.0f, 0.0f, 0.0f};
        std::array<float, 4> smoothedEnergies = {0.0f, 0.0f, 0.0f, 0.0f};
        
        float currentIntensity = 1.0f;
        float targetIntensity = 1.0f;
        
        std::mt19937 rng;
        
        void initializeClusters();
        void initializeParticles();
        void updateClusters();
        void updateParticles();
        void drawBackground(juce::Graphics& g, juce::Rectangle<float> bounds);
        void drawNebula(juce::Graphics& g, juce::Rectangle<float> bounds);
        juce::Colour getClusterColor(const NebulaCluster& cluster) const;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NebulaVisualizer)
    };
}
