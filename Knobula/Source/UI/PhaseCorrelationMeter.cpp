/*
  ==============================================================================
    Aetheri - High-Fidelity Mastering EQ
    PhaseCorrelationMeter Implementation
  ==============================================================================
*/

#include "PhaseCorrelationMeter.h"
#include "ColorPalette.h"

namespace Aetheri
{
    PhaseCorrelationMeter::PhaseCorrelationMeter()
    {
        startTimerHz(30);  // Update at 30 FPS
    }
    
    PhaseCorrelationMeter::~PhaseCorrelationMeter()
    {
        stopTimer();
    }
    
    void PhaseCorrelationMeter::updateCorrelation(float correlation)
    {
        currentCorrelation = juce::jlimit(-1.0f, 1.0f, correlation);
    }
    
    void PhaseCorrelationMeter::timerCallback()
    {
        // Smooth correlation value
        smoothedCorrelation = smoothedCorrelation * 0.9f + currentCorrelation * 0.1f;
        repaint();
    }
    
    juce::Colour PhaseCorrelationMeter::getCorrelationColor(float corr) const
    {
        // Green for positive (in phase), Red for negative (out of phase)
        if (corr > 0.0f)
        {
            return juce::Colour::fromFloatRGBA(0.0f, 1.0f, 0.0f, 0.8f);
        }
        else if (corr < -0.5f)
        {
            return juce::Colour::fromFloatRGBA(1.0f, 0.0f, 0.0f, 0.8f);
        }
        else
        {
            // Yellow/Orange for middle range
            return juce::Colour::fromFloatRGBA(1.0f, 0.7f, 0.0f, 0.8f);
        }
    }
    
    void PhaseCorrelationMeter::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();
        
        // Background
        g.setColour(Colors::panelSurface);
        g.fillRoundedRectangle(bounds, 4.0f);
        
        // Border
        g.setColour(Colors::panelBorder);
        g.drawRoundedRectangle(bounds.reduced(0.5f), 4.0f, 1.0f);
        
        // Center line (0 correlation)
        g.setColour(Colors::textSecondary.withAlpha(0.5f));
        float centerX = bounds.getCentreX();
        g.drawLine(centerX, bounds.getY() + 4.0f, centerX, bounds.getBottom() - 4.0f, 1.0f);
        
        // Correlation indicator
        float corr = smoothedCorrelation;
        float indicatorWidth = bounds.getWidth() * 0.5f * std::abs(corr);
        float indicatorX = corr > 0.0f ? centerX : centerX - indicatorWidth;
        
        g.setColour(getCorrelationColor(corr));
        g.fillRoundedRectangle(indicatorX, bounds.getY() + 2.0f, indicatorWidth, bounds.getHeight() - 4.0f, 2.0f);
        
        // Label
        g.setColour(Colors::textPrimary);
        g.setFont(juce::FontOptions().withHeight(10.0f));
        g.drawText("PHASE", bounds.removeFromTop(14.0f), juce::Justification::centred);
        
        // Value display
        juce::String corrText = juce::String(corr, 2);
        g.setFont(juce::FontOptions().withHeight(9.0f));
        g.drawText(corrText, bounds.removeFromBottom(12.0f), juce::Justification::centred);
    }
    
    void PhaseCorrelationMeter::resized()
    {
        // Component handles its own layout
    }
}

