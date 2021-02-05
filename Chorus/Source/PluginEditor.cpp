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
    shadowProperties.radius = 24;
    shadowProperties.offset = juce::Point<int> (-1, 3);
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
        sliders[i]->setColour(0x1001400, juce::Colour::fromFloatRGBA(1, 1, 1, 0.25f));
        sliders[i]->setColour(0x1001700, juce::Colour::fromFloatRGBA(1, 1, 1, 0.0f));
        sliders[i]->setColour(0x1001500, juce::Colour::fromFloatRGBA(0, 0, 0, 0.25f));
        sliders[i]->setLookAndFeel(&customDial);
        sliders[i]->setComponentEffect(&dialShadow);
        }
        
    rateSlider.setRange(1, 100, 1);
    rateSlider.setTextValueSuffix(" Ms");
    depthSlider.setRange(0.0f, 1.0f, 0.01f);
    depthSlider.setTextValueSuffix(" %");
    centerDelaySlider.setRange(1, 100, 1);
    centerDelaySlider.setTextValueSuffix(" Ms");
    feedbackSlider.setRange(-1.0f, 1.0f, 0.01f);
    feedbackSlider.setTextValueSuffix(" %");
    mixSlider.setRange(0.0f, 1.0f, 0.01f);
    mixSlider.setTextValueSuffix(" %");
        
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
    AudioProcessorEditor::setResizeLimits(591, 197, 1182, 394);
    AudioProcessorEditor::getConstrainer()->setFixedAspectRatio(3.0);
    setSize (888, 296);
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
    //Master bounds object
    juce::Rectangle<int> bounds = getLocalBounds();

    //first column of gui
    juce::FlexBox flexboxColumnOne;
    flexboxColumnOne.flexDirection = juce::FlexBox::Direction::column;
    flexboxColumnOne.flexWrap = juce::FlexBox::Wrap::noWrap;
    flexboxColumnOne.alignContent = juce::FlexBox::AlignContent::stretch;

    juce::Array<juce::FlexItem> itemArrayColumnOne;
    itemArrayColumnOne.add(juce::FlexItem(bounds.getWidth() / 6, bounds.getHeight() / 2, rateSlider).withMargin(juce::FlexItem::Margin(bounds.getHeight() * .25, 0, 0, 0)));

    flexboxColumnOne.items = itemArrayColumnOne;
    flexboxColumnOne.performLayout(bounds.removeFromLeft(bounds.getWidth() / 5));
    /* ============================================================================ */

    //second column of gui
    juce::FlexBox flexboxColumnTwo;
    flexboxColumnTwo.flexDirection = juce::FlexBox::Direction::column;
    flexboxColumnTwo.flexWrap = juce::FlexBox::Wrap::noWrap;
    flexboxColumnTwo.alignContent = juce::FlexBox::AlignContent::stretch;

    juce::Array<juce::FlexItem> itemArrayColumnTwo;
    itemArrayColumnTwo.add(juce::FlexItem(bounds.getWidth() / 6, bounds.getHeight() / 2, depthSlider).withMargin(juce::FlexItem::Margin(bounds.getHeight() * .25, 0, 0, 0)));

    flexboxColumnTwo.items = itemArrayColumnTwo;
    flexboxColumnTwo.performLayout(bounds.removeFromLeft(bounds.getWidth() / 4));
    /* ============================================================================ */

    //third column of gui
    juce::FlexBox flexboxColumnThree;
    flexboxColumnThree.flexDirection = juce::FlexBox::Direction::column;
    flexboxColumnThree.flexWrap = juce::FlexBox::Wrap::noWrap;
    flexboxColumnThree.alignContent = juce::FlexBox::AlignContent::stretch;

    juce::Array<juce::FlexItem> itemArrayColumnThree;
    itemArrayColumnThree.add(juce::FlexItem(bounds.getWidth() / 6, bounds.getHeight() / 2, centerDelaySlider).withMargin(juce::FlexItem::Margin(bounds.getHeight() * .25, 0, 0, 0)));

    flexboxColumnThree.items = itemArrayColumnThree;
    flexboxColumnThree.performLayout(bounds.removeFromLeft(bounds.getWidth() / 3));
    /* ============================================================================ */

    //fourth column of gui
    juce::FlexBox flexboxColumnFour;
    flexboxColumnFour.flexDirection = juce::FlexBox::Direction::column;
    flexboxColumnFour.flexWrap = juce::FlexBox::Wrap::noWrap;
    flexboxColumnFour.alignContent = juce::FlexBox::AlignContent::stretch;

    juce::Array<juce::FlexItem> itemArrayColumnFour;
    itemArrayColumnFour.add(juce::FlexItem(bounds.getWidth() / 6, bounds.getHeight() / 2, feedbackSlider).withMargin(juce::FlexItem::Margin(bounds.getHeight() * .25, 0, 0, 0)));

    flexboxColumnFour.items = itemArrayColumnFour;
    flexboxColumnFour.performLayout(bounds.removeFromLeft(bounds.getWidth() / 2));
    /* ============================================================================ */

    //fifth column of gui
    juce::FlexBox flexboxColumnFive;
    flexboxColumnFive.flexDirection = juce::FlexBox::Direction::column;
    flexboxColumnFive.flexWrap = juce::FlexBox::Wrap::noWrap;
    flexboxColumnFive.alignContent = juce::FlexBox::AlignContent::stretch;

    juce::Array<juce::FlexItem> itemArrayColumnFive;
    itemArrayColumnFive.add(juce::FlexItem(bounds.getWidth() / 6, bounds.getHeight() / 2, mixSlider).withMargin(juce::FlexItem::Margin(bounds.getHeight() * .25, 0, 0, 0)));

    flexboxColumnFive.items = itemArrayColumnFive;
    flexboxColumnFive.performLayout(bounds.removeFromLeft(bounds.getWidth()));
    /* ============================================================================ */

    windowBorder.setBounds(AudioProcessorEditor::getWidth() * .01, AudioProcessorEditor::getHeight() * 0.04, AudioProcessorEditor::getWidth() * .98, AudioProcessorEditor::getHeight() * .90);
}
