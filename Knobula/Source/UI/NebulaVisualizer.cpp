/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    NebulaVisualizer Implementation
  ==============================================================================
*/

#include "NebulaVisualizer.h"

namespace Knobula
{
    NebulaVisualizer::NebulaVisualizer()
        : rng(std::random_device{}())
    {
        initializeParticles();
        initializeClouds();
        startTimerHz(24);  // 24 FPS for low CPU overhead
    }
    
    NebulaVisualizer::~NebulaVisualizer()
    {
        stopTimer();
    }
    
    void NebulaVisualizer::initializeParticles()
    {
        std::uniform_real_distribution<float> posDist(0.0f, 1.0f);
        std::uniform_real_distribution<float> sizeDist(0.5f, 2.5f);
        std::uniform_real_distribution<float> brightDist(0.2f, 0.8f);
        std::uniform_real_distribution<float> phaseDist(0.0f, juce::MathConstants<float>::twoPi);
        std::uniform_real_distribution<float> speedDist(0.005f, 0.03f);
        std::uniform_real_distribution<float> depthDist(0.5f, 1.5f);
        
        for (int i = 0; i < MAX_PARTICLES; ++i)
        {
            auto& p = particles[i];
            p.x = posDist(rng);
            p.y = posDist(rng);
            p.size = sizeDist(rng);
            p.baseBrightness = brightDist(rng);
            p.brightness = p.baseBrightness;
            p.twinklePhase = phaseDist(rng);
            p.twinkleSpeed = speedDist(rng);
            p.bandIndex = i / PARTICLES_PER_BAND;  // Distribute across bands
            p.depth = depthDist(rng);
        }
    }
    
    void NebulaVisualizer::initializeClouds()
    {
        std::uniform_real_distribution<float> posDist(0.1f, 0.9f);
        std::uniform_real_distribution<float> radiusDist(0.15f, 0.35f);
        
        juce::Colour cloudColors[] = {
            Colors::bandLF.withAlpha(0.03f),
            Colors::bandLMF.withAlpha(0.02f),
            Colors::bandHMF.withAlpha(0.02f),
            Colors::bandHF.withAlpha(0.03f),
            Colors::nebulaDust.withAlpha(0.02f),
            Colors::nebulaDust.withAlpha(0.015f)
        };
        
        for (int i = 0; i < 6; ++i)
        {
            clouds[i].x = posDist(rng);
            clouds[i].y = posDist(rng);
            clouds[i].radius = radiusDist(rng);
            clouds[i].color = cloudColors[i];
            clouds[i].alpha = 0.02f + (rng() % 100) * 0.0002f;
        }
    }
    
    void NebulaVisualizer::setAnimating(bool shouldAnimate)
    {
        if (shouldAnimate && !isTimerRunning())
        {
            startTimerHz(24);
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
        for (int i = 0; i < 4; ++i)
        {
            smoothedEnergies[i] = smoothedEnergies[i] * 0.85f + bandEnergies[i] * 0.15f;
        }
        
        // Smooth intensity
        currentIntensity = currentIntensity * 0.95f + targetIntensity * 0.05f;
        
        // Update particles
        updateParticles();
        
        repaint();
    }
    
    void NebulaVisualizer::updateParticles()
    {
        for (auto& p : particles)
        {
            // Update twinkle
            p.twinklePhase += p.twinkleSpeed;
            if (p.twinklePhase > juce::MathConstants<float>::twoPi)
                p.twinklePhase -= juce::MathConstants<float>::twoPi;
            
            // Base twinkle
            float twinkle = 0.5f + 0.5f * std::sin(p.twinklePhase);
            
            // Energy boost from corresponding band
            float energyBoost = 0.0f;
            if (p.bandIndex >= 0 && p.bandIndex < 4)
            {
                energyBoost = smoothedEnergies[p.bandIndex] * 3.0f;
            }
            
            // Calculate final brightness
            p.brightness = p.baseBrightness * twinkle + energyBoost;
            p.brightness = juce::jlimit(0.0f, 1.0f, p.brightness * currentIntensity);
            
            // Very subtle drift
            p.y += 0.0001f * p.depth;
            if (p.y > 1.0f) p.y -= 1.0f;
        }
    }
    
    void NebulaVisualizer::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();
        
        // Draw layered background
        drawBackground(g, bounds);
        drawClouds(g, bounds);
        drawStars(g, bounds);
    }
    
    void NebulaVisualizer::drawBackground(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        // Deep space gradient
        juce::ColourGradient spaceGrad(
            Colors::nebulaBackground,
            bounds.getCentreX(), bounds.getY(),
            Colors::nebulaBackground.darker(0.3f),
            bounds.getCentreX(), bounds.getBottom(),
            false);
        
        g.setGradientFill(spaceGrad);
        g.fillRect(bounds);
        
        // Subtle radial vignette
        juce::ColourGradient vignette(
            juce::Colours::transparentBlack,
            bounds.getCentreX(), bounds.getCentreY(),
            juce::Colours::black.withAlpha(0.3f),
            bounds.getX(), bounds.getCentreY(),
            true);
        
        g.setGradientFill(vignette);
        g.fillRect(bounds);
    }
    
    void NebulaVisualizer::drawClouds(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        for (const auto& cloud : clouds)
        {
            float cx = bounds.getX() + cloud.x * bounds.getWidth();
            float cy = bounds.getY() + cloud.y * bounds.getHeight();
            float radius = cloud.radius * std::min(bounds.getWidth(), bounds.getHeight());
            
            // Soft radial gradient for each cloud
            juce::ColourGradient cloudGrad(
                cloud.color.withMultipliedAlpha(currentIntensity),
                cx, cy,
                cloud.color.withAlpha(0.0f),
                cx + radius, cy,
                true);
            
            g.setGradientFill(cloudGrad);
            g.fillEllipse(cx - radius, cy - radius, radius * 2, radius * 2);
        }
    }
    
    void NebulaVisualizer::drawStars(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        for (const auto& p : particles)
        {
            if (p.brightness < 0.01f)
                continue;
            
            float x = bounds.getX() + p.x * bounds.getWidth();
            float y = bounds.getY() + p.y * bounds.getHeight();
            
            juce::Colour starColor = getParticleColor(p);
            float size = p.size * (0.8f + p.brightness * 0.5f);
            
            // Draw star core
            g.setColour(starColor.withAlpha(p.brightness * 0.9f));
            g.fillEllipse(x - size * 0.5f, y - size * 0.5f, size, size);
            
            // Draw soft glow for brighter stars
            if (p.brightness > 0.5f)
            {
                float glowSize = size * 2.5f;
                juce::ColourGradient glow(
                    starColor.withAlpha(p.brightness * 0.3f),
                    x, y,
                    starColor.withAlpha(0.0f),
                    x + glowSize * 0.5f, y,
                    true);
                
                g.setGradientFill(glow);
                g.fillEllipse(x - glowSize * 0.5f, y - glowSize * 0.5f, glowSize, glowSize);
            }
            
            // Draw cross flare for very bright stars
            if (p.brightness > 0.8f)
            {
                float flareLength = size * 3.0f * p.brightness;
                g.setColour(starColor.withAlpha(p.brightness * 0.4f));
                g.drawLine(x - flareLength, y, x + flareLength, y, 0.5f);
                g.drawLine(x, y - flareLength, x, y + flareLength, 0.5f);
            }
        }
    }
    
    juce::Colour NebulaVisualizer::getParticleColor(const StarParticle& particle) const
    {
        // Base white with band color tint
        juce::Colour bandColor = Colors::getBandColor(particle.bandIndex);
        
        // Blend white with band color based on energy
        float energyInfluence = 0.3f;
        if (particle.bandIndex >= 0 && particle.bandIndex < 4)
        {
            energyInfluence += smoothedEnergies[particle.bandIndex] * 0.5f;
        }
        
        return Colors::nebulaStarBase.interpolatedWith(bandColor, energyInfluence);
    }
    
    void NebulaVisualizer::resized()
    {
        // Particles use normalized coordinates, no update needed
    }
}
