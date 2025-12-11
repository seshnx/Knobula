/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    VUMeterComponent Implementation
  ==============================================================================
*/

#include "VUMeterComponent.h"

namespace Knobula
{
    VUMeterComponent::VUMeterComponent(const juce::String& labelText)
        : label(labelText)
    {
        startTimerHz(30);  // 30 FPS for smooth animation
    }
    
    VUMeterComponent::~VUMeterComponent()
    {
        stopTimer();
    }
    
    void VUMeterComponent::setLevel(float normalizedLevel)
    {
        targetLevel = juce::jlimit(0.0f, 1.0f, normalizedLevel);
    }
    
    void VUMeterComponent::setPeakLevel(float normalizedPeak)
    {
        peakLevel = juce::jlimit(0.0f, 1.0f, normalizedPeak);
    }
    
    void VUMeterComponent::timerCallback()
    {
        // Smooth needle movement with physics simulation
        float targetAngle = MIN_ANGLE + (MAX_ANGLE - MIN_ANGLE) * targetLevel;
        
        // Spring-damper system for natural VU meter movement
        float spring = 0.15f;  // Slow response
        float damping = 0.4f;  // Heavy damping
        
        float force = (targetAngle - needleAngle) * spring;
        needleVelocity += force;
        needleVelocity *= (1.0f - damping);
        needleAngle += needleVelocity;
        
        // Update smoothed level
        currentLevel = currentLevel * 0.9f + targetLevel * 0.1f;
        
        repaint();
    }
    
    void VUMeterComponent::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        
        drawMeterBackground(g, bounds);
        drawScale(g, bounds);
        drawNeedle(g, bounds);
        
        // Label
        if (label.isNotEmpty())
        {
            g.setColour(Colors::textSecondary);
            g.setFont(juce::Font("Arial", 10.0f, juce::Font::bold));
            g.drawText(label, bounds.removeFromBottom(14.0f), juce::Justification::centred);
        }
    }
    
    void VUMeterComponent::drawMeterBackground(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        // Vintage meter background
        g.setColour(Colors::vuBackground);
        g.fillRoundedRectangle(bounds, 4.0f);
        
        // Subtle gradient overlay
        juce::ColourGradient grad(juce::Colours::white.withAlpha(0.05f), bounds.getCentreX(), bounds.getY(),
                                   juce::Colours::black.withAlpha(0.1f), bounds.getCentreX(), bounds.getBottom(), false);
        g.setGradientFill(grad);
        g.fillRoundedRectangle(bounds, 4.0f);
        
        // Border
        g.setColour(Colors::panelBorder);
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
    }
    
    void VUMeterComponent::drawScale(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        auto meterArea = bounds.reduced(8.0f);
        auto centre = juce::Point<float>(meterArea.getCentreX(), meterArea.getBottom() - 10.0f);
        float radius = meterArea.getHeight() * 0.7f;
        
        // Draw scale arc background
        juce::Path scaleArc;
        scaleArc.addCentredArc(centre.x, centre.y, radius, radius, 0.0f, MIN_ANGLE, MAX_ANGLE, true);
        g.setColour(Colors::panelBorder.withAlpha(0.5f));
        g.strokePath(scaleArc, juce::PathStrokeType(2.0f));
        
        // Scale markings
        g.setFont(juce::Font("Arial", 8.0f, juce::Font::plain));
        
        // VU scale: -20, -10, -7, -5, -3, 0, +1, +2, +3
        std::vector<std::pair<float, juce::String>> markers = {
            {0.0f, "-20"}, {0.25f, "-10"}, {0.4f, "-7"}, {0.5f, "-5"},
            {0.6f, "-3"}, {0.75f, "0"}, {0.85f, "+1"}, {0.95f, "+2"}, {1.0f, "+3"}
        };
        
        for (auto& [pos, text] : markers)
        {
            float angle = MIN_ANGLE + (MAX_ANGLE - MIN_ANGLE) * pos;
            float tickRadius = radius + 5.0f;
            float textRadius = radius + 15.0f;
            
            float tickX = centre.x + std::sin(angle) * tickRadius;
            float tickY = centre.y - std::cos(angle) * tickRadius;
            float innerX = centre.x + std::sin(angle) * (radius - 3.0f);
            float innerY = centre.y - std::cos(angle) * (radius - 3.0f);
            
            // Tick marks
            g.setColour(pos >= 0.75f ? Colors::vuRed : Colors::textSecondary);
            g.drawLine(innerX, innerY, tickX, tickY, pos >= 0.75f ? 1.5f : 1.0f);
            
            // Scale numbers (only major ones)
            if (text == "-20" || text == "-10" || text == "0" || text == "+3")
            {
                float textX = centre.x + std::sin(angle) * textRadius;
                float textY = centre.y - std::cos(angle) * textRadius;
                
                g.setColour(pos >= 0.75f ? Colors::vuRed : Colors::textPrimary);
                g.drawText(text, static_cast<int>(textX) - 12, static_cast<int>(textY) - 5, 24, 10, 
                           juce::Justification::centred);
            }
        }
        
        // Green/Yellow/Red zones on the arc
        float greenEnd = MIN_ANGLE + (MAX_ANGLE - MIN_ANGLE) * 0.6f;
        float yellowEnd = MIN_ANGLE + (MAX_ANGLE - MIN_ANGLE) * 0.85f;
        
        juce::Path greenArc, yellowArc, redArc;
        greenArc.addCentredArc(centre.x, centre.y, radius - 8.0f, radius - 8.0f, 0.0f, MIN_ANGLE, greenEnd, true);
        yellowArc.addCentredArc(centre.x, centre.y, radius - 8.0f, radius - 8.0f, 0.0f, greenEnd, yellowEnd, true);
        redArc.addCentredArc(centre.x, centre.y, radius - 8.0f, radius - 8.0f, 0.0f, yellowEnd, MAX_ANGLE, true);
        
        g.setColour(Colors::vuGreen.withAlpha(0.4f));
        g.strokePath(greenArc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        
        g.setColour(Colors::vuYellow.withAlpha(0.4f));
        g.strokePath(yellowArc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        
        g.setColour(Colors::vuRed.withAlpha(0.4f));
        g.strokePath(redArc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
    
    void VUMeterComponent::drawNeedle(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        auto meterArea = bounds.reduced(8.0f);
        auto centre = juce::Point<float>(meterArea.getCentreX(), meterArea.getBottom() - 10.0f);
        float needleLength = meterArea.getHeight() * 0.65f;
        
        // Needle shadow
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        juce::Path shadowPath;
        shadowPath.addRectangle(-1.5f, -needleLength, 3.0f, needleLength);
        g.fillPath(shadowPath, juce::AffineTransform::rotation(needleAngle)
                                    .translated(centre.x + 2, centre.y + 2));
        
        // Main needle
        g.setColour(Colors::vuNeedle);
        juce::Path needlePath;
        needlePath.addRectangle(-1.0f, -needleLength, 2.0f, needleLength);
        g.fillPath(needlePath, juce::AffineTransform::rotation(needleAngle).translated(centre));
        
        // Needle tip (red)
        float tipLength = 8.0f;
        g.setColour(Colors::vuRed);
        juce::Path tipPath;
        tipPath.addTriangle(-3.0f, -needleLength, 3.0f, -needleLength, 0.0f, -needleLength - tipLength);
        g.fillPath(tipPath, juce::AffineTransform::rotation(needleAngle).translated(centre));
        
        // Centre pivot
        g.setColour(Colors::metalAccent);
        g.fillEllipse(centre.x - 5.0f, centre.y - 5.0f, 10.0f, 10.0f);
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.drawEllipse(centre.x - 5.0f, centre.y - 5.0f, 10.0f, 10.0f, 1.0f);
    }
    
    void VUMeterComponent::resized()
    {
        // Layout handled in paint
    }
    
    //==============================================================================
    // StereoVUMeterComponent Implementation
    //==============================================================================
    
    StereoVUMeterComponent::StereoVUMeterComponent(const juce::String& titleText)
        : title(titleText), leftMeter("L"), rightMeter("R")
    {
        addAndMakeVisible(leftMeter);
        addAndMakeVisible(rightMeter);
    }
    
    void StereoVUMeterComponent::setLevels(float leftLevel, float rightLevel)
    {
        leftMeter.setLevel(leftLevel);
        rightMeter.setLevel(rightLevel);
    }
    
    void StereoVUMeterComponent::setPeakLevels(float leftPeak, float rightPeak)
    {
        leftMeter.setPeakLevel(leftPeak);
        rightMeter.setPeakLevel(rightPeak);
    }
    
    void StereoVUMeterComponent::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();
        
        // Title
        g.setColour(Colors::textPrimary);
        g.setFont(juce::Font("Arial", 11.0f, juce::Font::bold));
        g.drawText(title, bounds.removeFromTop(18.0f), juce::Justification::centred);
    }
    
    void StereoVUMeterComponent::resized()
    {
        auto bounds = getLocalBounds();
        bounds.removeFromTop(20);  // Title space
        
        int meterWidth = bounds.getWidth() / 2;
        leftMeter.setBounds(bounds.removeFromLeft(meterWidth));
        rightMeter.setBounds(bounds);
    }
}
