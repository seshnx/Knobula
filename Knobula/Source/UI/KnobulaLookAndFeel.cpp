/*
  ==============================================================================
    Knobula - High-Fidelity Mastering EQ
    KnobulaLookAndFeel Implementation
  ==============================================================================
*/

#include "KnobulaLookAndFeel.h"

namespace Knobula
{
    KnobulaLookAndFeel::KnobulaLookAndFeel()
    {
        // Set up fonts
        mainFont = juce::Font("Arial", 14.0f, juce::Font::plain);
        labelFont = juce::Font("Arial", 11.0f, juce::Font::bold);
        valueFont = juce::Font("Arial", 10.0f, juce::Font::plain);
        
        // Set default colors
        setColour(juce::Slider::rotarySliderFillColourId, Colors::bandLMF);
        setColour(juce::Slider::rotarySliderOutlineColourId, Colors::panelBorder);
        setColour(juce::Slider::thumbColourId, Colors::knobPointer);
        
        setColour(juce::Label::textColourId, Colors::textPrimary);
        setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        
        setColour(juce::TextButton::buttonColourId, Colors::buttonOff);
        setColour(juce::TextButton::buttonOnColourId, Colors::buttonOn);
        setColour(juce::TextButton::textColourOffId, Colors::textPrimary);
        setColour(juce::TextButton::textColourOnId, Colors::textPrimary);
        
        setColour(juce::ComboBox::backgroundColourId, Colors::panelSurface);
        setColour(juce::ComboBox::outlineColourId, Colors::panelBorder);
        setColour(juce::ComboBox::textColourId, Colors::textPrimary);
        
        setColour(juce::PopupMenu::backgroundColourId, Colors::panelSurface);
        setColour(juce::PopupMenu::textColourId, Colors::textPrimary);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, Colors::buttonOn);
    }
    
    void KnobulaLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPosProportional, float rotaryStartAngle,
                                              float rotaryEndAngle, juce::Slider& slider)
    {
        auto bounds = juce::Rectangle<float>(static_cast<float>(x), static_cast<float>(y),
                                             static_cast<float>(width), static_cast<float>(height));
        
        // Get accent color from slider properties or use default
        juce::Colour accentColor = slider.findColour(juce::Slider::rotarySliderFillColourId);
        
        bool isSmall = (width < 60);
        drawLargeKnob(g, bounds, sliderPosProportional, accentColor, isSmall);
    }
    
    void KnobulaLookAndFeel::drawLargeKnob(juce::Graphics& g, juce::Rectangle<float> bounds,
                                           float value, juce::Colour accentColor, bool isSmall)
    {
        auto centre = bounds.getCentre();
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.42f;
        
        // Outer ring shadow
        g.setColour(juce::Colours::black.withAlpha(0.4f));
        g.fillEllipse(centre.x - radius - 2, centre.y - radius + 2, radius * 2 + 4, radius * 2 + 4);
        
        // Outer ring (brushed metal effect)
        juce::ColourGradient outerGrad(Colors::knobRing.brighter(0.2f), centre.x - radius, centre.y - radius,
                                        Colors::knobRing.darker(0.2f), centre.x + radius, centre.y + radius, false);
        g.setGradientFill(outerGrad);
        g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2, radius * 2);
        
        // Knob body
        auto bodyRadius = radius * 0.85f;
        juce::ColourGradient bodyGrad(Colors::knobBody.brighter(0.15f), centre.x, centre.y - bodyRadius,
                                       Colors::knobBody.darker(0.1f), centre.x, centre.y + bodyRadius, false);
        g.setGradientFill(bodyGrad);
        g.fillEllipse(centre.x - bodyRadius, centre.y - bodyRadius, bodyRadius * 2, bodyRadius * 2);
        
        // Subtle inner highlight
        g.setColour(Colors::knobHighlight.withAlpha(0.3f));
        g.drawEllipse(centre.x - bodyRadius + 1, centre.y - bodyRadius + 1, 
                      bodyRadius * 2 - 2, bodyRadius * 2 - 2, 1.0f);
        
        // Value arc (colored)
        if (!isSmall)
        {
            auto arcRadius = radius * 1.08f;
            float startAngle = juce::MathConstants<float>::pi * 1.25f;
            float endAngle = juce::MathConstants<float>::pi * 2.75f;
            float valueAngle = startAngle + (endAngle - startAngle) * value;
            
            juce::Path arcPath;
            arcPath.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f, 
                                  startAngle, valueAngle, true);
            
            g.setColour(accentColor.withAlpha(0.8f));
            g.strokePath(arcPath, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, 
                                                        juce::PathStrokeType::rounded));
        }
        
        // Pointer/indicator
        float pointerAngle = juce::MathConstants<float>::pi * 1.25f + 
                            (juce::MathConstants<float>::pi * 1.5f * value);
        float pointerLength = bodyRadius * 0.7f;
        float pointerWidth = isSmall ? 2.0f : 3.0f;
        
        juce::Path pointer;
        pointer.addRoundedRectangle(-pointerWidth * 0.5f, -pointerLength, 
                                     pointerWidth, pointerLength * 0.9f, 1.0f);
        
        g.setColour(Colors::knobPointer);
        g.fillPath(pointer, juce::AffineTransform::rotation(pointerAngle).translated(centre));
        
        // Center cap
        auto capRadius = bodyRadius * 0.2f;
        juce::ColourGradient capGrad(Colors::metalAccent.brighter(0.2f), centre.x, centre.y - capRadius,
                                      Colors::metalAccent.darker(0.2f), centre.x, centre.y + capRadius, false);
        g.setGradientFill(capGrad);
        g.fillEllipse(centre.x - capRadius, centre.y - capRadius, capRadius * 2, capRadius * 2);
    }
    
    void KnobulaLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                                   const juce::Colour& backgroundColour,
                                                   bool shouldDrawButtonAsHighlighted,
                                                   bool shouldDrawButtonAsDown)
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
        
        juce::Colour baseColour = button.getToggleState() ? Colors::buttonOn : Colors::buttonOff;
        
        if (shouldDrawButtonAsDown)
            baseColour = baseColour.darker(0.2f);
        else if (shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.1f);
        
        // Draw rounded rectangle
        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, 4.0f);
        
        // Border
        g.setColour(Colors::panelBorder);
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
    }
    
    void KnobulaLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                              bool shouldDrawButtonAsHighlighted,
                                              bool shouldDrawButtonAsDown)
    {
        auto bounds = button.getLocalBounds().toFloat();
        auto toggleArea = bounds.removeFromLeft(bounds.getHeight()).reduced(4.0f);
        
        // Toggle background
        g.setColour(Colors::panelSurface);
        g.fillRoundedRectangle(toggleArea, 3.0f);
        
        // Toggle indicator
        if (button.getToggleState())
        {
            g.setColour(Colors::buttonOn);
            g.fillRoundedRectangle(toggleArea.reduced(2.0f), 2.0f);
        }
        
        // Border
        g.setColour(Colors::panelBorder);
        g.drawRoundedRectangle(toggleArea, 3.0f, 1.0f);
        
        // Text
        g.setColour(Colors::textPrimary);
        g.setFont(labelFont);
        g.drawText(button.getButtonText(), bounds.reduced(4.0f, 0.0f), 
                   juce::Justification::centredLeft);
    }
    
    void KnobulaLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
    {
        g.fillAll(label.findColour(juce::Label::backgroundColourId));
        
        auto textArea = label.getBorderSize().subtractedFrom(label.getLocalBounds());
        
        g.setColour(label.findColour(juce::Label::textColourId));
        g.setFont(getLabelFont(label));
        g.drawText(label.getText(), textArea, label.getJustificationType(), true);
    }
    
    void KnobulaLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                          int buttonX, int buttonY, int buttonW, int buttonH,
                                          juce::ComboBox& box)
    {
        auto bounds = juce::Rectangle<float>(0, 0, static_cast<float>(width), static_cast<float>(height));
        
        g.setColour(Colors::panelSurface);
        g.fillRoundedRectangle(bounds, 4.0f);
        
        g.setColour(Colors::panelBorder);
        g.drawRoundedRectangle(bounds.reduced(0.5f), 4.0f, 1.0f);
        
        // Arrow
        juce::Path arrow;
        float arrowSize = 6.0f;
        float arrowX = static_cast<float>(width) - 12.0f;
        float arrowY = static_cast<float>(height) * 0.5f;
        
        arrow.addTriangle(arrowX - arrowSize, arrowY - arrowSize * 0.4f,
                         arrowX + arrowSize, arrowY - arrowSize * 0.4f,
                         arrowX, arrowY + arrowSize * 0.6f);
        
        g.setColour(Colors::textSecondary);
        g.fillPath(arrow);
    }
    
    juce::Font KnobulaLookAndFeel::getLabelFont(juce::Label& /*label*/)
    {
        return labelFont;
    }
    
    void KnobulaLookAndFeel::drawPanelSection(juce::Graphics& g, juce::Rectangle<float> bounds, 
                                              const juce::String& title)
    {
        // Background
        g.setColour(Colors::panelSurface);
        g.fillRoundedRectangle(bounds, 6.0f);
        
        // Border
        g.setColour(Colors::panelBorder);
        g.drawRoundedRectangle(bounds.reduced(0.5f), 6.0f, 1.0f);
        
        // Title
        if (title.isNotEmpty())
        {
            g.setColour(Colors::textSecondary);
            g.setFont(juce::Font("Arial", 10.0f, juce::Font::bold));
            g.drawText(title, bounds.removeFromTop(20.0f).reduced(8.0f, 2.0f), 
                       juce::Justification::centredLeft);
        }
    }
    
    void KnobulaLookAndFeel::drawBrushedMetal(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        // Base metal color
        g.setColour(Colors::metalAccent);
        g.fillRect(bounds);
        
        // Add subtle horizontal lines for brushed effect
        juce::Random rng;
        g.setColour(juce::Colours::white.withAlpha(0.03f));
        
        for (float y = bounds.getY(); y < bounds.getBottom(); y += 2.0f)
        {
            if (rng.nextFloat() > 0.5f)
            {
                g.drawHorizontalLine(static_cast<int>(y), bounds.getX(), bounds.getRight());
            }
        }
    }
}
