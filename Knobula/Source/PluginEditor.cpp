/*
  ==============================================================================
    Knobula - High-Fidelity Dual-Channel Mastering EQ
    PluginEditor Implementation
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

KnobulaAudioProcessorEditor::KnobulaAudioProcessorEditor(KnobulaAudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      inputVU("INPUT"),
      outputVU("OUTPUT"),
      leftChannelStrip("LEFT / MID"),
      rightChannelStrip("RIGHT / SIDE"),
      inputGainKnob("INPUT", Knobula::AnalogKnob::KnobSize::Medium),
      outputTrimKnob("OUTPUT", Knobula::AnalogKnob::KnobSize::Medium),
      hpfFreqKnob("HPF", Knobula::AnalogKnob::KnobSize::Small),
      lpfFreqKnob("LPF", Knobula::AnalogKnob::KnobSize::Small)
{
    // Set custom look and feel
    setLookAndFeel(&lookAndFeel);
    
    // Background nebula visualizer (goes behind everything)
    addAndMakeVisible(nebulaVisualizer);
    
    // Main components on top of nebula
    addAndMakeVisible(inputVU);
    addAndMakeVisible(outputVU);
    addAndMakeVisible(leftChannelStrip);
    addAndMakeVisible(rightChannelStrip);
    addAndMakeVisible(hysteresisPanel);
    
    // Global controls
    setupGlobalControls();
    
    // Filter section
    setupFilterSection();
    
    // Tube glow overlay (on top of everything)
    addAndMakeVisible(tubeGlow);
    tubeGlow.toFront(false);
    
    // Attach EQ bands to parameters
    leftChannelStrip.attachToParameters(audioProcessor.getParameters(), 0);
    rightChannelStrip.attachToParameters(audioProcessor.getParameters(), 1);
    
    // Attach hysteresis panel
    hysteresisPanel.attachToParameters(audioProcessor.getParameters());
    
    // Resizing setup
    constrainer.setMinimumSize(MIN_WIDTH, MIN_HEIGHT);
    constrainer.setMaximumSize(1600, 1000);
    
    resizer = std::make_unique<juce::ResizableCornerComponent>(this, &constrainer);
    addAndMakeVisible(*resizer);
    
    setSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    setResizable(true, true);
    
    // Start timer for UI updates
    startTimerHz(30);
}

KnobulaAudioProcessorEditor::~KnobulaAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void KnobulaAudioProcessorEditor::setupGlobalControls()
{
    // Input gain
    inputGainKnob.setValueSuffix(" dB");
    inputGainKnob.setAccentColor(Knobula::Colors::textValue);
    addAndMakeVisible(inputGainKnob);
    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), Knobula::ParamIDs::inputGain, inputGainKnob.getSlider());
    
    // Output trim
    outputTrimKnob.setValueSuffix(" dB");
    outputTrimKnob.setAccentColor(Knobula::Colors::textValue);
    addAndMakeVisible(outputTrimKnob);
    outputTrimAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), Knobula::ParamIDs::outputTrim, outputTrimKnob.getSlider());
    
    // Stereo mode selector
    stereoModeSelector.addItem("L/R", 1);
    stereoModeSelector.addItem("M/S", 2);
    addAndMakeVisible(stereoModeSelector);
    stereoModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getParameters(), Knobula::ParamIDs::stereoMode, stereoModeSelector);
    
    // Channel link button
    channelLinkButton.setButtonText("LINK");
    channelLinkButton.setClickingTogglesState(true);
    addAndMakeVisible(channelLinkButton);
    channelLinkAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getParameters(), Knobula::ParamIDs::channelLink, channelLinkButton);
}

void KnobulaAudioProcessorEditor::setupFilterSection()
{
    // HPF button
    hpfButton.setButtonText("HPF");
    hpfButton.setClickingTogglesState(true);
    addAndMakeVisible(hpfButton);
    hpfButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getParameters(), Knobula::ParamIDs::hpfEnabled, hpfButton);
    
    // HPF frequency knob
    hpfFreqKnob.setValueSuffix(" Hz");
    hpfFreqKnob.setAccentColor(Knobula::Colors::bandLF);
    addAndMakeVisible(hpfFreqKnob);
    hpfFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), Knobula::ParamIDs::hpfFreq, hpfFreqKnob.getSlider());
    
    // LPF button
    lpfButton.setButtonText("LPF");
    lpfButton.setClickingTogglesState(true);
    addAndMakeVisible(lpfButton);
    lpfButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getParameters(), Knobula::ParamIDs::lpfEnabled, lpfButton);
    
    // LPF frequency knob
    lpfFreqKnob.setValueSuffix(" Hz");
    lpfFreqKnob.setAccentColor(Knobula::Colors::bandHF);
    addAndMakeVisible(lpfFreqKnob);
    lpfFreqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), Knobula::ParamIDs::lpfFreq, lpfFreqKnob.getSlider());
}

void KnobulaAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Nebula visualizer draws the main background
    // Just draw any overlays here
    
    // Header bar
    auto headerArea = getLocalBounds().removeFromTop(40).toFloat();
    g.setColour(Knobula::Colors::panelSurface.withAlpha(0.9f));
    g.fillRect(headerArea);
    
    g.setColour(Knobula::Colors::textPrimary);
    g.setFont(juce::Font("Arial", 22.0f, juce::Font::bold));
    g.drawText("KNOBULA", headerArea.reduced(15.0f, 0.0f), juce::Justification::centredLeft);
    
    g.setColour(Knobula::Colors::textSecondary);
    g.setFont(juce::Font("Arial", 10.0f, juce::Font::plain));
    g.drawText("High-Fidelity Mastering EQ", headerArea.reduced(15.0f, 0.0f).translated(120.0f, 4.0f),
               juce::Justification::centredLeft);
    
    // Section dividers
    g.setColour(Knobula::Colors::panelBorder);
    g.drawLine(0.0f, headerArea.getBottom(), static_cast<float>(getWidth()), headerArea.getBottom(), 1.0f);
}

void KnobulaAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Nebula visualizer fills the entire background
    nebulaVisualizer.setBounds(bounds);
    
    // Tube glow overlay fills entire area
    tubeGlow.setBounds(bounds);
    
    // Header area
    bounds.removeFromTop(44);
    
    // Padding
    bounds.reduce(10, 10);
    
    // Resizer in corner
    resizer->setBounds(bounds.getRight() - 16, bounds.getBottom() - 16, 16, 16);
    
    // Layout: VU | Filters | Input | EQ Left | Hysteresis | EQ Right | Output | VU
    
    int vuWidth = 100;
    int filterWidth = 80;
    int gainKnobWidth = 70;
    int hysteresisWidth = 180;
    int stereoControlHeight = 50;
    
    // Input VU meter
    auto inputVUArea = bounds.removeFromLeft(vuWidth);
    inputVU.setBounds(inputVUArea);
    
    bounds.removeFromLeft(5);  // Spacing
    
    // Filter section (HPF)
    auto hpfArea = bounds.removeFromLeft(filterWidth);
    hpfButton.setBounds(hpfArea.removeFromTop(28).reduced(4, 2));
    hpfFreqKnob.setBounds(hpfArea);
    
    bounds.removeFromLeft(5);
    
    // Input gain knob
    auto inputGainArea = bounds.removeFromLeft(gainKnobWidth);
    inputGainKnob.setBounds(inputGainArea);
    
    bounds.removeFromLeft(10);
    
    // Output VU meter (from right)
    auto outputVUArea = bounds.removeFromRight(vuWidth);
    outputVU.setBounds(outputVUArea);
    
    bounds.removeFromRight(5);
    
    // LPF section (from right)
    auto lpfArea = bounds.removeFromRight(filterWidth);
    lpfButton.setBounds(lpfArea.removeFromTop(28).reduced(4, 2));
    lpfFreqKnob.setBounds(lpfArea);
    
    bounds.removeFromRight(5);
    
    // Output trim knob
    auto outputTrimArea = bounds.removeFromRight(gainKnobWidth);
    outputTrimKnob.setBounds(outputTrimArea);
    
    bounds.removeFromRight(10);
    
    // Stereo controls at bottom-center
    auto stereoArea = bounds.removeFromBottom(stereoControlHeight);
    auto stereoCenter = stereoArea.withWidth(200).withX(bounds.getCentreX() - 100);
    stereoModeSelector.setBounds(stereoCenter.removeFromLeft(80).reduced(4, 10));
    channelLinkButton.setBounds(stereoCenter.removeFromLeft(80).reduced(4, 10));
    
    bounds.removeFromBottom(5);
    
    // Center section: EQ Left | Hysteresis | EQ Right
    int remainingWidth = bounds.getWidth();
    int eqStripWidth = (remainingWidth - hysteresisWidth) / 2;
    
    leftChannelStrip.setBounds(bounds.removeFromLeft(eqStripWidth));
    
    bounds.removeFromLeft(5);
    hysteresisPanel.setBounds(bounds.removeFromLeft(hysteresisWidth - 10));
    bounds.removeFromLeft(5);
    
    rightChannelStrip.setBounds(bounds);
}

void KnobulaAudioProcessorEditor::timerCallback()
{
    updateVUMeters();
    updateNebulaEnergies();
    updateTubeGlow();
    updateChannelLabels();
}

void KnobulaAudioProcessorEditor::updateVUMeters()
{
    // Update input VU meters
    const auto& inVU = audioProcessor.getInputVU();
    inputVU.setLevels(inVU.getLeft().getNormalizedLevel(),
                      inVU.getRight().getNormalizedLevel());
    inputVU.setPeakLevels(inVU.getLeft().getNormalizedPeak(),
                          inVU.getRight().getNormalizedPeak());
    
    // Update output VU meters
    const auto& outVU = audioProcessor.getOutputVU();
    outputVU.setLevels(outVU.getLeft().getNormalizedLevel(),
                       outVU.getRight().getNormalizedLevel());
    outputVU.setPeakLevels(outVU.getLeft().getNormalizedPeak(),
                           outVU.getRight().getNormalizedPeak());
}

void KnobulaAudioProcessorEditor::updateNebulaEnergies()
{
    // Update nebula visualizer with band energies (average of both channels)
    for (int band = 0; band < 4; ++band)
    {
        float energy = (audioProcessor.getBandEnergy(band, 0) + 
                       audioProcessor.getBandEnergy(band, 1)) * 0.5f;
        nebulaVisualizer.setBandEnergy(band, energy * 10.0f);  // Scale for visibility
    }
}

void KnobulaAudioProcessorEditor::updateTubeGlow()
{
    float glowIntensity = audioProcessor.getHysteresisGlowIntensity();
    
    bool hystEnabled = audioProcessor.getParameters().getRawParameterValue(
        Knobula::ParamIDs::hystEnabled)->load() > 0.5f;
    
    tubeGlow.setEnabled(hystEnabled);
    tubeGlow.setIntensity(glowIntensity);
    hysteresisPanel.setGlowIntensity(glowIntensity);
}

void KnobulaAudioProcessorEditor::updateChannelLabels()
{
    // Update channel labels based on stereo mode
    // This would update the strip labels (L/R vs M/S)
    // Currently handled in constructor, could be made dynamic
}
