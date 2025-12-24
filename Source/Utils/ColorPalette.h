/*
  ==============================================================================
    Aetheri - High-Fidelity Mastering EQ
    Color Palette - Analog Nebula Theme
  ==============================================================================
*/

#pragma once
#include <juce_graphics/juce_graphics.h>
#include <juce_core/juce_core.h>

// Core colour namespace
namespace Aetheri
{
    namespace Colors
    {
        // Main Panel Colors (Vintage Analog)
        const juce::Colour panelBackground      { 0xFF1A1A1E };  // Deep charcoal
        const juce::Colour panelSurface         { 0xFF242428 };  // Slightly lighter
        const juce::Colour panelBorder          { 0xFF3A3A40 };  // Subtle border
        const juce::Colour metalAccent          { 0xFF4A4A52 };  // Brushed metal look
        
        // Knob Colors
        const juce::Colour knobBody             { 0xFF2A2A30 };  // Dark knob base
        const juce::Colour knobRing             { 0xFF3C3C44 };  // Ring around knob
        const juce::Colour knobPointer          { 0xFFE0D0B8 };  // Cream pointer
        const juce::Colour knobHighlight        { 0xFF505058 };  // Highlight edge
        
        // Frequency Band Colors (Nebula reactive)
        const juce::Colour bandLF               { 0xFF4488FF };  // Blue - Low Frequency
        const juce::Colour bandLMF              { 0xFF44DD88 };  // Green - Low-Mid Frequency
        const juce::Colour bandHMF              { 0xFFDDCC44 };  // Yellow - High-Mid Frequency
        const juce::Colour bandHF               { 0xFFFF6644 };  // Red/Orange - High Frequency
        
        // VU Meter Colors
        const juce::Colour vuBackground         { 0xFF1E1E22 };  // Meter background
        const juce::Colour vuGreen              { 0xFF44CC66 };  // Safe level
        const juce::Colour vuYellow             { 0xFFCCBB44 };  // Warning level
        const juce::Colour vuRed                { 0xFFCC4444 };  // Peak level
        const juce::Colour vuNeedle             { 0xFFE8D8C8 };  // Needle color
        
        // Hysteresis / Tube Glow
        const juce::Colour tubeGlowOff          { 0x00000000 };  // Transparent when off
        const juce::Colour tubeGlowOn           { 0x44FF9944 };  // Warm amber glow
        const juce::Colour tubeGlowBright       { 0x88FFAA55 };  // Bright glow center
        
        // Nebula Visualizer
        const juce::Colour nebulaBackground     { 0xFF0A0A0E };  // Deep space black
        const juce::Colour nebulaDust           { 0x11FFFFFF };  // Faint star dust
        const juce::Colour nebulaStarBase       { 0xFFFFFFFF };  // Base star color
        
        // Text Colors
        const juce::Colour textPrimary          { 0xFFE0D8CC };  // Main label text
        const juce::Colour textSecondary        { 0xFFA0988C };  // Secondary text
        const juce::Colour textValue            { 0xFFCCEEFF };  // Value display
        
        // Button/Switch Colors
        const juce::Colour buttonOff            { 0xFF2A2A30 };  // Off state
        const juce::Colour buttonOn             { 0xFF44AA66 };  // On state
        const juce::Colour buttonHover          { 0xFF3A3A44 };  // Hover state
        
        // Get band color by index
        inline juce::Colour getBandColor(int bandIndex)
        {
            switch (bandIndex)
            {
                case 0: return bandLF;
                case 1: return bandLMF;
                case 2: return bandHMF;
                case 3: return bandHF;
                default: return bandLF;
            }
        }
        
        // Get interpolated VU color based on level (0.0 - 1.0)
        inline juce::Colour getVUColor(float level)
        {
            if (level < 0.6f)
                return vuGreen;
            else if (level < 0.85f)
                return vuGreen.interpolatedWith(vuYellow, (level - 0.6f) / 0.25f);
            else
                return vuYellow.interpolatedWith(vuRed, (level - 0.85f) / 0.15f);
        }
    }
}
