/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "reverb.h"

//==============================================================================
/**
*/
class ReverbTestAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ReverbTestAudioProcessor();
    ~ReverbTestAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorValueTreeState& getVTS(){
        return *dValueTreeState;
    }
    
    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    
    reverb reverb;
    
    std::atomic<float>* size = nullptr;
    std::atomic<float>* dryWet = nullptr;
    std::atomic<float>* feedbackPercent = nullptr;
    std::atomic<float>* cutoff = nullptr;
    std::atomic<float>* q = nullptr;
    std::atomic<float>* gain = nullptr;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState> dValueTreeState;
    
    juce::LinearSmoothedValue<float> smoothedSize;
    juce::LinearSmoothedValue<float> smoothedDryWet;
    juce::LinearSmoothedValue<float> smoothedCutoff;
    juce::LinearSmoothedValue<float> smoothedQ;

    
    void _constructValueTreeState();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReverbTestAudioProcessor)
};
