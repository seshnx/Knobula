/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    TubeGlowEffect Implementation
  ==============================================================================
*/

#include "TubeGlowEffect.h"

namespace Aetheri
{
    TubeGlowEffect::TubeGlowEffect()
    {
        setInterceptsMouseClicks(false, false);
        startTimerHz(20);  // Reduced frequency to prevent glitches
    }
    
    TubeGlowEffect::~TubeGlowEffect()
    {
        stopTimer();
    }
    
    void TubeGlowEffect::setEnabled(bool newEnabled)
    {
        enabled = newEnabled;
        if (!enabled)
        {
            targetIntensity = 0.0f;
        }
    }
    
    void TubeGlowEffect::setIntensity(float intensity)
    {
        targetIntensity = juce::jlimit(0.0f, 1.0f, intensity);
    }
    
    void TubeGlowEffect::timerCallback()
    {
        // Smooth intensity changes
        float smoothFactor = enabled ? 0.1f : 0.05f;
        currentIntensity = currentIntensity * (1.0f - smoothFactor) + targetIntensity * smoothFactor;
        
        // Subtle flickering for organic feel
        if (enabled && currentIntensity > 0.1f)
        {
            flickerPhase += 0.15f;
            if (flickerPhase > juce::MathConstants<float>::twoPi)
                flickerPhase -= juce::MathConstants<float>::twoPi;
            
            // Very subtle flicker (1-3% variation)
            flickerAmount = std::sin(flickerPhase * 2.3f) * 0.01f + 
                           std::sin(flickerPhase * 5.7f) * 0.005f +
                           std::sin(flickerPhase * 0.7f) * 0.015f;
        }
        else
        {
            flickerAmount = 0.0f;
        }
        
        // Only repaint if intensity changed significantly (reduces glitches)
        static float lastIntensity = 0.0f;
        if (std::abs(currentIntensity - lastIntensity) > 0.01f)
        {
            lastIntensity = currentIntensity;
            if (currentIntensity > 0.001f || targetIntensity > 0.001f)
            {
                repaint();
            }
        }
    }
    
    void TubeGlowEffect::paint(juce::Graphics& g)
    {
        if (currentIntensity < 0.001f)
            return;
        
        auto bounds = getLocalBounds().toFloat();
        float intensity = currentIntensity + flickerAmount;
        
        // Vignette-style glow from edges
        float maxAlpha = intensity * 0.15f;
        
        // Top edge glow
        juce::ColourGradient topGlow(
            Colors::tubeGlowOn.withAlpha(maxAlpha),
            juce::Point<float>(bounds.getCentreX(), bounds.getY()),
            Colors::tubeGlowOn.withAlpha(0.0f),
            juce::Point<float>(bounds.getCentreX(), bounds.getY() + bounds.getHeight() * 0.15f),
            false);
        g.setGradientFill(topGlow);
        g.fillRect(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight() * 0.15f);
        
        // Bottom edge glow (stronger, simulating floor reflections)
        juce::ColourGradient bottomGlow(
            Colors::tubeGlowOn.withAlpha(maxAlpha * 1.3f),
            juce::Point<float>(bounds.getCentreX(), bounds.getBottom()),
            Colors::tubeGlowOn.withAlpha(0.0f),
            juce::Point<float>(bounds.getCentreX(), bounds.getBottom() - bounds.getHeight() * 0.2f),
            false);
        g.setGradientFill(bottomGlow);
        g.fillRect(bounds.getX(), bounds.getBottom() - bounds.getHeight() * 0.2f, 
                   bounds.getWidth(), bounds.getHeight() * 0.2f);
        
        // Left edge glow
        juce::ColourGradient leftGlow(
            Colors::tubeGlowOn.withAlpha(maxAlpha * 0.8f),
            juce::Point<float>(bounds.getX(), bounds.getCentreY()),
            Colors::tubeGlowOn.withAlpha(0.0f),
            juce::Point<float>(bounds.getX() + bounds.getWidth() * 0.1f, bounds.getCentreY()),
            false);
        g.setGradientFill(leftGlow);
        g.fillRect(bounds.getX(), bounds.getY(), bounds.getWidth() * 0.1f, bounds.getHeight());
        
        // Right edge glow
        juce::ColourGradient rightGlow(
            Colors::tubeGlowOn.withAlpha(maxAlpha * 0.8f),
            juce::Point<float>(bounds.getRight(), bounds.getCentreY()),
            Colors::tubeGlowOn.withAlpha(0.0f),
            juce::Point<float>(bounds.getRight() - bounds.getWidth() * 0.1f, bounds.getCentreY()),
            false);
        g.setGradientFill(rightGlow);
        g.fillRect(bounds.getRight() - bounds.getWidth() * 0.1f, bounds.getY(), 
                   bounds.getWidth() * 0.1f, bounds.getHeight());
        
        // Corner glows for extra warmth
        float cornerRadius = bounds.getWidth() * 0.15f;
        
        auto drawCornerGlow = [&](float cx, float cy) {
            juce::ColourGradient cornerGlow(
                Colors::tubeGlowBright.withAlpha(maxAlpha * 0.5f),
                juce::Point<float>(cx, cy),
                Colors::tubeGlowOn.withAlpha(0.0f),
                juce::Point<float>(cx + cornerRadius, cy),
                true);
            g.setGradientFill(cornerGlow);
            g.fillEllipse(cx - cornerRadius, cy - cornerRadius, cornerRadius * 2, cornerRadius * 2);
        };
        
        drawCornerGlow(bounds.getX(), bounds.getY());
        drawCornerGlow(bounds.getRight(), bounds.getY());
        drawCornerGlow(bounds.getX(), bounds.getBottom());
        drawCornerGlow(bounds.getRight(), bounds.getBottom());
    }
}
