/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReverbTestAudioProcessorEditor::ReverbTestAudioProcessorEditor (ReverbTestAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    sizeSlider.setRange(1,50);
    sizeSlider.setTextBoxStyle(juce::Slider::TextBoxLeft,true, 50, 20);
    sizeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    addAndMakeVisible(sizeSlider);
    sizeSliderAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.getVTS(),"size",sizeSlider));
    
    dryWetSlider.setRange(0, 1.0f);
    dryWetSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, true, 50, 20);
    dryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    addAndMakeVisible(dryWetSlider);
    dryWetSliderAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.getVTS(),"dryWet",dryWetSlider));
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

ReverbTestAudioProcessorEditor::~ReverbTestAudioProcessorEditor()
{
}

//==============================================================================
void ReverbTestAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::peru);
}

void ReverbTestAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    auto knob1 = bounds.removeFromLeft(getWidth()/2);
    auto knob2 = bounds.removeFromRight(getWidth()/2);
    
    sizeSlider.setBounds(knob1);
    dryWetSlider.setBounds(knob2);
    
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}