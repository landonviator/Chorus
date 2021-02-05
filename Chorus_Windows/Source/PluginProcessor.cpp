/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Chorus_WindowsAudioProcessor::Chorus_WindowsAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    treeState(*this, nullptr, "PARAMETER", createParameterLayout())
#endif
{
}

Chorus_WindowsAudioProcessor::~Chorus_WindowsAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout Chorus_WindowsAudioProcessor::createParameterLayout()
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
const juce::String Chorus_WindowsAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Chorus_WindowsAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool Chorus_WindowsAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool Chorus_WindowsAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double Chorus_WindowsAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Chorus_WindowsAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Chorus_WindowsAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Chorus_WindowsAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String Chorus_WindowsAudioProcessor::getProgramName(int index)
{
    return {};
}

void Chorus_WindowsAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void Chorus_WindowsAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();

    Chorus_WindowsProcessor.prepare(spec);
}

void Chorus_WindowsAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Chorus_WindowsAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
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

void Chorus_WindowsAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> audioBlock{ buffer };

    auto* rawRate = treeState.getRawParameterValue(rateSliderId);
    auto* rawDepth = treeState.getRawParameterValue(depthSliderId);
    float depthScaled = scaleRange(*rawDepth, 0, 100, 0.0f, 1.0f);
    auto* rawCenterDelay = treeState.getRawParameterValue(centerDelaySliderId);
    auto* rawFeedback = treeState.getRawParameterValue(feedbackSliderId);
    float feedbackScaled = scaleRange(*rawFeedback, 0, 95, 0.0f, 0.95f);
    auto* rawMix = treeState.getRawParameterValue(mixSliderId);
    float mixScaled = scaleRange(*rawMix, 0, 100, 0.0f, 1.0f);

    Chorus_WindowsProcessor.setRate(*rawRate);
    Chorus_WindowsProcessor.setDepth(depthScaled);
    Chorus_WindowsProcessor.setCentreDelay(*rawCenterDelay);
    Chorus_WindowsProcessor.setFeedback(feedbackScaled);
    Chorus_WindowsProcessor.setMix(mixScaled);

    Chorus_WindowsProcessor.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
}

float Chorus_WindowsAudioProcessor::scaleRange(const float& input, const float& inputLow, const float& inputHigh, const float& outputLow, const float& outputHigh) {
    return ((input - inputLow) / (inputHigh - inputLow)) * (outputHigh - outputLow) + outputLow;
}

//==============================================================================
bool Chorus_WindowsAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Chorus_WindowsAudioProcessor::createEditor()
{
    return new Chorus_WindowsAudioProcessorEditor(*this);
}

//==============================================================================
void Chorus_WindowsAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, false);
    treeState.state.writeToStream(stream);
}

void Chorus_WindowsAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::ValueTree tree = juce::ValueTree::readFromData(data, size_t(sizeInBytes));
    if (tree.isValid()) {
        treeState.state = tree;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Chorus_WindowsAudioProcessor();
}
