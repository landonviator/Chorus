/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChorusAudioProcessor::ChorusAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
treeState (*this, nullptr, "PARAMETER", createParameterLayout())
#endif
{
}

ChorusAudioProcessor::~ChorusAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout ChorusAudioProcessor::createParameterLayout()
{
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    params.reserve(5);
    
    
    auto rateParam = std::make_unique<juce::AudioParameterInt>(rateSliderId, rateSliderName, 1, 99, 50);
    auto depthParam = std::make_unique<juce::AudioParameterInt>(depthSliderId, depthSliderName, 0, 100, 0);
    auto centerDelayParam = std::make_unique<juce::AudioParameterInt>(centerDelaySliderId, centerDelaySliderName, 1, 99, 50);
    auto feedbackParam = std::make_unique<juce::AudioParameterInt>(feedbackSliderId, feedbackSliderName, 0, 95, 0);
    auto mixParam = std::make_unique<juce::AudioParameterInt>(mixSliderId, mixSliderName, 0, 100, 0);

    params.push_back(std::move(rateParam));
    params.push_back(std::move(depthParam));
    params.push_back(std::move(centerDelayParam));
    params.push_back(std::move(feedbackParam));
    params.push_back(std::move(mixParam));
    
    
    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String ChorusAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ChorusAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ChorusAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ChorusAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ChorusAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ChorusAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ChorusAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ChorusAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ChorusAudioProcessor::getProgramName (int index)
{
    return {};
}

void ChorusAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ChorusAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
    chorusProcessor.prepare(spec);
}

void ChorusAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ChorusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ChorusAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> audioBlock {buffer};
    
    auto* rawRate = treeState.getRawParameterValue(rateSliderId);
    auto* rawDepth = treeState.getRawParameterValue(depthSliderId);
    float depthScaled = scaleRange(*rawDepth, 0, 100, 0.0f, 1.0f);
    auto* rawCenterDelay = treeState.getRawParameterValue(centerDelaySliderId);
    auto* rawFeedback = treeState.getRawParameterValue(feedbackSliderId);
    float feedbackScaled = scaleRange(*rawFeedback, 0, 95, 0.0f, 0.95f);
    auto* rawMix = treeState.getRawParameterValue(mixSliderId);
    float mixScaled = scaleRange(*rawMix, 0, 100, 0.0f, 1.0f);
    
    chorusProcessor.setRate(*rawRate);
    chorusProcessor.setDepth(depthScaled);
    chorusProcessor.setCentreDelay(*rawCenterDelay);
    chorusProcessor.setFeedback(feedbackScaled);
    chorusProcessor.setMix(mixScaled);
    
    chorusProcessor.process(juce::dsp::ProcessContextReplacing<float> (audioBlock));
}

float ChorusAudioProcessor::scaleRange(const float &input, const float &inputLow, const float &inputHigh, const float &outputLow, const float &outputHigh){
    return ((input - inputLow) / (inputHigh - inputLow)) * (outputHigh - outputLow) + outputLow;
}

//==============================================================================
bool ChorusAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ChorusAudioProcessor::createEditor()
{
    return new ChorusAudioProcessorEditor (*this);
}

//==============================================================================
void ChorusAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, false);
            treeState.state.writeToStream (stream);
}

void ChorusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::ValueTree tree = juce::ValueTree::readFromData (data, size_t (sizeInBytes));
            if (tree.isValid()) {
                treeState.state = tree;
            }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChorusAudioProcessor();
}
