/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SpecEQAudioProcessor::SpecEQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

SpecEQAudioProcessor::~SpecEQAudioProcessor()
{
}

//==============================================================================
const juce::String SpecEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SpecEQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SpecEQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SpecEQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SpecEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SpecEQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SpecEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SpecEQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SpecEQAudioProcessor::getProgramName (int index)
{
    return {};
}

void SpecEQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SpecEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    updateFilters();


    
}

void SpecEQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SpecEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SpecEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples()); 

    updateFilters();

   

   

    juce::dsp::AudioBlock<float> block(buffer);
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);
     
    

}

//==============================================================================
bool SpecEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SpecEQAudioProcessor::createEditor()
{
    //return new SpecEQAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void SpecEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SpecEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SpecEQAudioProcessor();
}
juce::AudioProcessorValueTreeState::ParameterLayout SpecEQAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;


    layout.add(std::make_unique<juce::AudioParameterFloat>("LowCutFreq","LowCutFreq",juce::NormalisableRange<float>(20.0f,20000.0f,1.0f,0.25f),20.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("HighCutFreq", "HighCutFreq", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f), 20000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakFreq", "PeakFreq", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f), 750.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakGain", "PeakGain", juce::NormalisableRange<float>(-24.0f, 24.0f, 0.5f, 0.25f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("PeakQuality", "PeakQuality", juce::NormalisableRange<float>(0.1f, 10.0f, 0.05f, 0.25f), 1.0f));
    juce::StringArray stringArray;
    for (int i = 0; i < 4; ++i)
    {
        juce::String str;
        str << (12 + i * 12);
        str << " db/Oct ";
        stringArray.add(str);
    }
    layout.add(std::make_unique<juce::AudioParameterChoice>("LowCutSlope", "LowCutSlope", stringArray, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("HighCutSlope", "HighCutSlope", stringArray, 0));
    return layout;
}
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings;
    settings.lowCutFreq = apvts.getRawParameterValue("LowCutFreq")->load();
    settings.highCutFreq = apvts.getRawParameterValue("HighCutFreq")->load();
    settings.peakFreq = apvts.getRawParameterValue("PeakFreq")->load();
    settings.peakGainInDecibles = apvts.getRawParameterValue("PeakGain")->load();
    settings.peakQuality = apvts.getRawParameterValue("PeakQuality")->load();
    settings.lowCutSlope = static_cast<Slope>(apvts.getRawParameterValue("LowCutSlope")->load());
    settings.highCutSlope = static_cast<Slope>(apvts.getRawParameterValue("HighCutSlope")->load());

    return settings;
}

void SpecEQAudioProcessor::updatePeakFilter(const ChainSettings& chainSettings)
{
    auto peakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), chainSettings.peakFreq, chainSettings.peakQuality, juce::Decibels::decibelsToGain(chainSettings.peakGainInDecibles));
    *leftChain.get<ChainPositions::Peak>().coefficients = *peakCoefficients;
    *rightChain.get<ChainPositions::Peak>().coefficients = *peakCoefficients;
    updateCoefficients(leftChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
    updateCoefficients(rightChain.get<ChainPositions::Peak>().coefficients, peakCoefficients);
}
void SpecEQAudioProcessor::updateCoefficients(Coefficients& old, const Coefficients& replacements)
{
    *old = *replacements;

}
void SpecEQAudioProcessor::updateLowCutFilters(const ChainSettings& chainSettings)
{
    auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq,
        getSampleRate(),
        2 * (chainSettings.lowCutSlope + 1));
    auto& leftLowCut = leftChain.get<ChainPositions::LowCut>();
    updateCutFilter(leftLowCut, lowCutCoefficients, chainSettings.lowCutSlope);

    auto& rightLowCut = rightChain.get<ChainPositions::LowCut>();
    updateCutFilter(rightLowCut, lowCutCoefficients, chainSettings.lowCutSlope);
}
void SpecEQAudioProcessor::updateHighCutFilters(const ChainSettings& chainSettings)
{
    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.highCutFreq,
        getSampleRate(),
        2 * (chainSettings.highCutSlope + 1));

    auto& leftHighCut = leftChain.get<ChainPositions::HighCut>();
    updateCutFilter(leftHighCut, highCutCoefficients, chainSettings.highCutSlope);

    auto& rightHighCut = rightChain.get<ChainPositions::HighCut>();
    updateCutFilter(rightHighCut, highCutCoefficients, chainSettings.highCutSlope);

}
void SpecEQAudioProcessor::updateFilters()
{
    auto chainSettings = getChainSettings(apvts);
    updateLowCutFilters(chainSettings);
    updatePeakFilter(chainSettings);
    updateHighCutFilters(chainSettings);
}