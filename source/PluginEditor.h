/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ReverbTestAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ReverbTestAudioProcessorEditor (ReverbTestAudioProcessor&);
    ~ReverbTestAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    juce::Slider sizeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sizeSliderAttachment;
    
    juce::Slider dryWetSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetSliderAttachment;
    
    ReverbTestAudioProcessor& audioProcessor;
    
    // we need this::::
    void _updateTempoSync();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbTestAudioProcessorEditor)
};
