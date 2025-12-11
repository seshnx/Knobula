/*
  ==============================================================================
    Knobula - High-Fidelity Dual-Channel Mastering EQ
    PluginProcessor Implementation
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

KnobulaAudioProcessor::KnobulaAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, juce::Identifier("KnobulaParameters"),
                 Knobula::createParameterLayout())
{
    // Cache parameter pointers for efficient access
    inputGainParam = parameters.getRawParameterValue(Knobula::ParamIDs::inputGain);
    outputTrimParam = parameters.getRawParameterValue(Knobula::ParamIDs::outputTrim);
    stereoModeParam = parameters.getRawParameterValue(Knobula::ParamIDs::stereoMode);
    channelLinkParam = parameters.getRawParameterValue(Knobula::ParamIDs::channelLink);
    
    hpfEnabledParam = parameters.getRawParameterValue(Knobula::ParamIDs::hpfEnabled);
    hpfFreqParam = parameters.getRawParameterValue(Knobula::ParamIDs::hpfFreq);
    lpfEnabledParam = parameters.getRawParameterValue(Knobula::ParamIDs::lpfEnabled);
    lpfFreqParam = parameters.getRawParameterValue(Knobula::ParamIDs::lpfFreq);
    
    hystEnabledParam = parameters.getRawParameterValue(Knobula::ParamIDs::hystEnabled);
    tubeHarmonicsParam = parameters.getRawParameterValue(Knobula::ParamIDs::tubeHarmonics);
    transformerSatParam = parameters.getRawParameterValue(Knobula::ParamIDs::transformerSat);
    hystMixParam = parameters.getRawParameterValue(Knobula::ParamIDs::hystMix);
    
    // Cache band parameters
    for (int band = 0; band < 4; ++band)
    {
        for (int ch = 0; ch < 2; ++ch)
        {
            bandGainParams[band][ch] = parameters.getRawParameterValue(
                Knobula::ParamIDs::bandGain(band, ch));
            bandTrimParams[band][ch] = parameters.getRawParameterValue(
                Knobula::ParamIDs::bandTrim(band, ch));
            bandFreqParams[band][ch] = parameters.getRawParameterValue(
                Knobula::ParamIDs::bandFreq(band, ch));
            bandEnabledParams[band][ch] = parameters.getRawParameterValue(
                Knobula::ParamIDs::bandEnabled(band, ch));
        }
    }
    
    // Curve params only for LF (band 0) and HF (band 3)
    for (int ch = 0; ch < 2; ++ch)
    {
        bandCurveParams[0][ch] = parameters.getRawParameterValue(
            Knobula::ParamIDs::bandCurve(0, ch));
        bandCurveParams[1][ch] = parameters.getRawParameterValue(
            Knobula::ParamIDs::bandCurve(3, ch));
    }
}

KnobulaAudioProcessor::~KnobulaAudioProcessor()
{
}

const juce::String KnobulaAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool KnobulaAudioProcessor::acceptsMidi() const
{
    return false;
}

bool KnobulaAudioProcessor::producesMidi() const
{
    return false;
}

bool KnobulaAudioProcessor::isMidiEffect() const
{
    return false;
}

double KnobulaAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int KnobulaAudioProcessor::getNumPrograms()
{
    return 1;
}

int KnobulaAudioProcessor::getCurrentProgram()
{
    return 0;
}

void KnobulaAudioProcessor::setCurrentProgram(int /*index*/)
{
}

const juce::String KnobulaAudioProcessor::getProgramName(int /*index*/)
{
    return {};
}

void KnobulaAudioProcessor::changeProgramName(int /*index*/, const juce::String& /*newName*/)
{
}

void KnobulaAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare all DSP processors
    passiveEQ.prepare(sampleRate, samplesPerBlock);
    hysteresis.prepare(sampleRate, samplesPerBlock);
    filterSection.prepare(sampleRate, samplesPerBlock);
    
    // Prepare VU meters
    inputVU.prepare(sampleRate);
    outputVU.prepare(sampleRate);
}

void KnobulaAudioProcessor::releaseResources()
{
    passiveEQ.reset();
    hysteresis.reset();
    filterSection.reset();
    inputVU.reset();
    outputVU.reset();
}

bool KnobulaAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Only support stereo
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void KnobulaAudioProcessor::updateDSPFromParameters()
{
    // Input/Output gains
    passiveEQ.setInputGain(inputGainParam->load());
    passiveEQ.setOutputTrim(outputTrimParam->load());
    
    // Stereo mode
    bool isMidSide = stereoModeParam->load() > 0.5f;
    passiveEQ.setStereoMode(isMidSide);
    passiveEQ.setChannelLink(channelLinkParam->load() > 0.5f);
    
    // HPF/LPF
    filterSection.setHPFEnabled(hpfEnabledParam->load() > 0.5f);
    filterSection.setHPFFrequency(hpfFreqParam->load());
    filterSection.setLPFEnabled(lpfEnabledParam->load() > 0.5f);
    filterSection.setLPFFrequency(lpfFreqParam->load());
    
    // Hysteresis
    hysteresis.setEnabled(hystEnabledParam->load() > 0.5f);
    hysteresis.setTubeHarmonics(tubeHarmonicsParam->load());
    hysteresis.setTransformerSaturate(transformerSatParam->load());
    hysteresis.setMix(hystMixParam->load());
    
    // EQ Bands
    for (int band = 0; band < 4; ++band)
    {
        for (int ch = 0; ch < 2; ++ch)
        {
            float gain = bandGainParams[band][ch]->load();
            float trim = bandTrimParams[band][ch]->load();
            float freq = bandFreqParams[band][ch]->load();
            bool enabled = bandEnabledParams[band][ch]->load() > 0.5f;
            
            Knobula::EQBand::CurveType curve = Knobula::EQBand::CurveType::Bell;
            
            // Get curve type for LF (band 0) and HF (band 3)
            if (band == 0)
            {
                curve = bandCurveParams[0][ch]->load() > 0.5f ? 
                        Knobula::EQBand::CurveType::Shelf : 
                        Knobula::EQBand::CurveType::Bell;
            }
            else if (band == 3)
            {
                curve = bandCurveParams[1][ch]->load() > 0.5f ? 
                        Knobula::EQBand::CurveType::Shelf : 
                        Knobula::EQBand::CurveType::Bell;
            }
            
            passiveEQ.setBandParameters(band, ch, freq, gain, trim, curve, enabled);
        }
    }
}

void KnobulaAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                          juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;
    
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear unused output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    if (buffer.getNumChannels() < 2)
        return;

    // Update DSP parameters
    updateDSPFromParameters();
    
    // Measure input levels
    inputVU.pushSamples(buffer);
    
    // Processing chain:
    // 1. HPF/LPF Filters
    filterSection.processBlock(buffer);
    
    // 2. Passive EQ (includes input gain, M/S encoding, and output trim)
    passiveEQ.processBlock(buffer);
    
    // 3. Hysteresis Stage (parallel to EQ, applied after)
    hysteresis.processBlock(buffer);
    
    // Measure output levels
    outputVU.pushSamples(buffer);
}

bool KnobulaAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* KnobulaAudioProcessor::createEditor()
{
    return new KnobulaAudioProcessorEditor(*this);
}

void KnobulaAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void KnobulaAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
    {
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}

float KnobulaAudioProcessor::getInputLevel(int channel) const
{
    return passiveEQ.getInputLevel(channel);
}

float KnobulaAudioProcessor::getOutputLevel(int channel) const
{
    return passiveEQ.getOutputLevel(channel);
}

float KnobulaAudioProcessor::getBandEnergy(int band, int channel) const
{
    return passiveEQ.getBandEnergy(band, channel);
}

float KnobulaAudioProcessor::getHysteresisGlowIntensity() const
{
    return hysteresis.getGlowIntensity();
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KnobulaAudioProcessor();
}
