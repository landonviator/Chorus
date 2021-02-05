/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChorusAudioProcessorEditor::ChorusAudioProcessorEditor (ChorusAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    shadowProperties.radius = 15;
    shadowProperties.offset = juce::Point<int> (-2, 6);
    dialShadow.setShadowProperties (shadowProperties);
        
    sliders.reserve(5);
    sliders = {
        &rateSlider, &depthSlider, &centerDelaySlider, &feedbackSlider, &mixSlider
    };
        
    labels.reserve(5);
    labels = {
        &rateLabel, &depthLabel, &centerDelayLabel, &feedbackLabel, &mixLabel
    };
            
    labelTexts.reserve(5);
    labelTexts = {
        rateSliderLabelText, depthSliderLabelText, centerDelaySliderLabelText, feedbackSliderLabelText, mixSliderLabelText
    };
            
        
    for (auto i = 0; i < sliders.size(); i++) {
        addAndMakeVisible(sliders[i]);
        sliders[i]->setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        sliders[i]->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 32);
        sliders[i]->setColour(0x1001400, juce::Colour::fromFloatRGBA(1, 1, 1, 0.5f));
        sliders[i]->setColour(0x1001700, juce::Colour::fromFloatRGBA(1, 1, 1, 0.0f));
        sliders[i]->setColour(0x1001500, juce::Colour::fromFloatRGBA(0, 0, 0, 0.25f));
        sliders[i]->setLookAndFeel(&customDial);
        sliders[i]->setComponentEffect(&dialShadow);
        }
        
    rateSlider.setRange(1, 100, 1);
    depthSlider.setRange(0.0f, 1.0f, 0.01f);
    centerDelaySlider.setRange(1, 100, 1);
    feedbackSlider.setRange(-1.0f, 1.0f, 0.01f);
    mixSlider.setRange(0.0f, 1.0f, 0.01f);
        
    rateSliderAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, rateSliderId, rateSlider);
    depthSliderAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, depthSliderId, depthSlider);
    centerDelaySliderAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, centerDelaySliderId, centerDelaySlider);
    feedbackSliderAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, feedbackSliderId, feedbackSlider);
    mixSliderAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, mixSliderId, mixSlider);
        
    for (auto i = 0; i < labels.size(); i++) {
            addAndMakeVisible(labels[i]);
            labels[i]->setText(labelTexts[i], juce::dontSendNotification);
            labels[i]->setJustificationType(juce::Justification::centred);
            labels[i]->setColour(0x1000281, juce::Colour::fromFloatRGBA(1, 1, 1, 0.25f));
            labels[i]->attachToComponent(sliders[i], false);
        }
        
    addAndMakeVisible(windowBorder);
    windowBorder.setText("Chorus");
    windowBorder.setColour(0x1005400, juce::Colour::fromFloatRGBA(1, 1, 1, 0.25f));
    windowBorder.setColour(0x1005410, juce::Colour::fromFloatRGBA(1, 1, 1, 0.25f));
        
    //Making the window resizable by aspect ratio and setting size
    AudioProcessorEditor::setResizable(true, true);
    AudioProcessorEditor::setResizeLimits(711, 237, 1374, 458);
    AudioProcessorEditor::getConstrainer()->setFixedAspectRatio(3.0);
    setSize (711, 237);
}

ChorusAudioProcessorEditor::~ChorusAudioProcessorEditor()
{
}

//==============================================================================
void ChorusAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromFloatRGBA(0.1f, 0.12f, 0.16f, 1.0));
            
    juce::Rectangle<int> background = AudioProcessorEditor::getLocalBounds();
    g.setGradientFill(juce::ColourGradient::vertical(juce::Colour::fromFloatRGBA(0.18f, 0.20f, 0.24f, 1.0), 300 * .01, juce::Colour::fromFloatRGBA(0.12f, 0.14f, 0.18f, 1.0), 300 * .99));
    g.fillRect(background);
}

void ChorusAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
