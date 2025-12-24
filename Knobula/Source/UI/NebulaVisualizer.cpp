/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    NebulaVisualizer Implementation - Standard JUCE Graphics
    Organic nebula cloud visualization with color blending
  ==============================================================================
*/

#include "NebulaVisualizer.h"
#include <juce_graphics/juce_graphics.h>
#include <cmath>

namespace Aetheri
{
    NebulaVisualizer::NebulaVisualizer()
        : rng(std::random_device{}())
    {
        setOpaque(false);  // Transparent background
        initializeClusters();
        initializeParticles();
    }
    
    NebulaVisualizer::~NebulaVisualizer()
    {
        stopTimer();
    }
    
    void NebulaVisualizer::initializeClusters()
    {
        std::uniform_real_distribution<float> posDist(0.1f, 0.9f);
        std::uniform_real_distribution<float> radiusDist(0.15f, 0.35f);
        std::uniform_real_distribution<float> driftDist(-0.0002f, 0.0002f);
        std::uniform_real_distribution<float> densityDist(0.6f, 1.0f);
        
        for (int i = 0; i < NUM_CLUSTERS; ++i)
        {
            auto& cluster = clusters[i];
            cluster.centerX = posDist(rng);
            cluster.centerY = posDist(rng);
            cluster.radius = radiusDist(rng);
            cluster.density = densityDist(rng);
            cluster.brightness = 0.4f + (rng() % 60) * 0.01f;
            cluster.driftX = driftDist(rng);
            cluster.driftY = driftDist(rng);
            cluster.bandIndex = i % 4;  // Distribute across bands
            cluster.age = static_cast<float>(rng() % 1000) / 1000.0f;
        }
    }
    
    void NebulaVisualizer::initializeParticles()
    {
        std::uniform_real_distribution<float> offsetDist(-1.0f, 1.0f);
        std::uniform_real_distribution<float> sizeDist(0.8f, 2.5f);
        std::uniform_real_distribution<float> brightDist(0.2f, 0.6f);
        std::uniform_real_distribution<float> phaseDist(0.0f, juce::MathConstants<float>::twoPi);
        std::uniform_real_distribution<float> speedDist(0.005f, 0.015f);
        
        for (int clusterIdx = 0; clusterIdx < NUM_CLUSTERS; ++clusterIdx)
        {
            const auto& cluster = clusters[clusterIdx];
            auto& clusterParticles = particles[clusterIdx];
            
            for (int i = 0; i < PARTICLES_PER_CLUSTER; ++i)
            {
                auto& p = clusterParticles[i];
                
                // Distribute particles in a Gaussian-like pattern around cluster center
                float angle = static_cast<float>(i) / PARTICLES_PER_CLUSTER * juce::MathConstants<float>::twoPi;
                float distance = std::abs(offsetDist(rng)) * cluster.radius * 0.8f;
                
                p.x = std::cos(angle) * distance;
                p.y = std::sin(angle) * distance;
                p.size = sizeDist(rng);
                p.baseBrightness = brightDist(rng);
                p.brightness = p.baseBrightness;
                p.phase = phaseDist(rng);
                p.speed = speedDist(rng);
            }
        }
    }
    
    void NebulaVisualizer::setAnimating(bool shouldAnimate)
    {
        if (shouldAnimate && !isTimerRunning())
        {
            startTimerHz(30);  // 30 FPS is sufficient
        }
        else if (!shouldAnimate && isTimerRunning())
        {
            stopTimer();
        }
    }
    
    void NebulaVisualizer::setBandEnergy(int band, float energy)
    {
        if (band >= 0 && band < 4)
        {
            bandEnergies[band] = juce::jlimit(0.0f, 1.0f, energy);
        }
    }
    
    void NebulaVisualizer::timerCallback()
    {
        // Smooth band energies
        bool needsRepaint = false;
        for (int i = 0; i < 4; ++i)
        {
            float oldEnergy = smoothedEnergies[i];
            smoothedEnergies[i] = smoothedEnergies[i] * 0.85f + bandEnergies[i] * 0.15f;
            if (std::abs(smoothedEnergies[i] - oldEnergy) > 0.01f)
                needsRepaint = true;
        }
        
        // Smooth intensity
        float oldIntensity = currentIntensity;
        currentIntensity = currentIntensity * 0.95f + targetIntensity * 0.05f;
        if (std::abs(currentIntensity - oldIntensity) > 0.01f)
            needsRepaint = true;
        
        // Update clusters and particles
        updateClusters();
        updateParticles();
        
        // Only repaint if something changed
        if (needsRepaint)
            repaint();
    }
    
    void NebulaVisualizer::updateClusters()
    {
        for (auto& cluster : clusters)
        {
            // Drift clusters slowly
            cluster.centerX += cluster.driftX;
            cluster.centerY += cluster.driftY;
            
            // Wrap around edges
            if (cluster.centerX < 0.0f) cluster.centerX += 1.0f;
            if (cluster.centerX > 1.0f) cluster.centerX -= 1.0f;
            if (cluster.centerY < 0.0f) cluster.centerY += 1.0f;
            if (cluster.centerY > 1.0f) cluster.centerY -= 1.0f;
            
            // Age clusters for subtle evolution
            cluster.age += 0.001f;
            if (cluster.age > 1.0f) cluster.age -= 1.0f;
            
            // Update brightness based on band energy
            if (cluster.bandIndex >= 0 && cluster.bandIndex < 4)
            {
                float energyBoost = smoothedEnergies[cluster.bandIndex] * 0.5f;
                cluster.brightness = 0.4f + energyBoost;
            }
        }
    }
    
    void NebulaVisualizer::updateParticles()
    {
        for (int clusterIdx = 0; clusterIdx < NUM_CLUSTERS; ++clusterIdx)
        {
            const auto& cluster = clusters[clusterIdx];
            auto& clusterParticles = particles[clusterIdx];
            
            for (auto& p : clusterParticles)
            {
                // Update animation phase
                p.phase += p.speed;
                if (p.phase > juce::MathConstants<float>::twoPi)
                    p.phase -= juce::MathConstants<float>::twoPi;
                
                // Gentle pulsing animation
                float pulse = 0.5f + 0.5f * std::sin(p.phase);
                
                // Calculate brightness based on cluster and energy
                float energyBoost = 0.0f;
                if (cluster.bandIndex >= 0 && cluster.bandIndex < 4)
                {
                    energyBoost = smoothedEnergies[cluster.bandIndex] * 0.4f;
                }
                
                p.brightness = p.baseBrightness * pulse * cluster.brightness + energyBoost;
                p.brightness = juce::jlimit(0.0f, 1.0f, p.brightness * currentIntensity);
            }
        }
    }
    
    void NebulaVisualizer::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();
        
        drawBackground(g, bounds);
        drawNebula(g, bounds);
    }
    
    void NebulaVisualizer::drawBackground(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        // Deep space gradient background
        juce::ColourGradient gradient(
            Colors::nebulaBackground.withAlpha(0.4f * currentIntensity),
            juce::Point<float>(bounds.getCentreX(), bounds.getY()),
            Colors::nebulaBackground.darker(0.3f).withAlpha(0.2f * currentIntensity),
            juce::Point<float>(bounds.getCentreX(), bounds.getBottom()),
            false);
        
        g.setGradientFill(gradient);
        g.fillRect(bounds);
        
        // Subtle radial vignette
        juce::ColourGradient vignette(
            juce::Colours::transparentBlack,
            bounds.getCentreX(), bounds.getCentreY(),
            juce::Colours::black.withAlpha(0.2f * currentIntensity),
            bounds.getX(), bounds.getCentreY(),
            true);
        
        g.setGradientFill(vignette);
        g.fillRect(bounds);
    }
    
    void NebulaVisualizer::drawNebula(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        // Draw each cluster as an organic nebula cloud
        for (int clusterIdx = 0; clusterIdx < NUM_CLUSTERS; ++clusterIdx)
        {
            const auto& cluster = clusters[clusterIdx];
            const auto& clusterParticles = particles[clusterIdx];
            
            float clusterX = bounds.getX() + cluster.centerX * bounds.getWidth();
            float clusterY = bounds.getY() + cluster.centerY * bounds.getHeight();
            float clusterRadius = cluster.radius * std::min(bounds.getWidth(), bounds.getHeight());
            
            juce::Colour clusterColor = getClusterColor(cluster);
            
            // Draw nebula cloud using layered radial gradients for organic look
            for (int layer = 0; layer < 3; ++layer)
            {
                float layerRadius = clusterRadius * (1.0f - layer * 0.3f);
                float layerAlpha = (0.15f - layer * 0.04f) * cluster.brightness * currentIntensity * cluster.density;
                
                if (layerAlpha < 0.01f)
                    continue;
                
                juce::ColourGradient cloudGrad(
                    clusterColor.withAlpha(layerAlpha),
                    clusterX, clusterY,
                    clusterColor.withAlpha(0.0f),
                    clusterX + layerRadius, clusterY,
                    true);
                
                g.setGradientFill(cloudGrad);
                g.fillEllipse(clusterX - layerRadius, clusterY - layerRadius, 
                             layerRadius * 2, layerRadius * 2);
            }
            
            // Draw individual particles for detail and sparkle
            for (const auto& p : clusterParticles)
            {
                if (p.brightness < 0.05f)
                    continue;
                
                float x = clusterX + p.x * clusterRadius;
                float y = clusterY + p.y * clusterRadius;
                float size = p.size * p.brightness * 1.5f;
                
                if (size < 0.3f)
                    continue;
                
                // Use cluster color with particle brightness
                juce::Colour particleColor = clusterColor.withAlpha(p.brightness * currentIntensity * 0.6f);
                g.setColour(particleColor);
                
                // Draw as soft circle
                g.fillEllipse(x - size, y - size, size * 2, size * 2);
                
                // Add subtle glow for brighter particles
                if (p.brightness > 0.4f)
                {
                    g.setColour(particleColor.withAlpha(p.brightness * currentIntensity * 0.2f));
                    g.fillEllipse(x - size * 1.8f, y - size * 1.8f, size * 3.6f, size * 3.6f);
                }
            }
        }
    }
    
    juce::Colour NebulaVisualizer::getClusterColor(const NebulaCluster& cluster) const
    {
        // Base band color
        juce::Colour bandColor = Colors::getBandColor(cluster.bandIndex);
        
        // Blend with neighboring band colors for more organic look
        juce::Colour neighborColor = Colors::getBandColor((cluster.bandIndex + 1) % 4);
        float blendAmount = 0.3f + std::sin(cluster.age * juce::MathConstants<float>::twoPi) * 0.2f;
        blendAmount = juce::jlimit(0.0f, 1.0f, blendAmount);
        
        juce::Colour blendedColor = bandColor.interpolatedWith(neighborColor, blendAmount);
        
        // Add energy influence
        float energyInfluence = 0.0f;
        if (cluster.bandIndex >= 0 && cluster.bandIndex < 4)
        {
            energyInfluence = smoothedEnergies[cluster.bandIndex] * 0.4f;
        }
        
        // Brighten based on energy
        return blendedColor.brighter(energyInfluence);
    }
    
    void NebulaVisualizer::resized()
    {
        // Clusters use normalized coordinates, no update needed
    }
}
