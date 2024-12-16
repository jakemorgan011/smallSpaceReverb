/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReverbTestAudioProcessor::ReverbTestAudioProcessor()
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
    _constructValueTreeState();
    size = dValueTreeState->getRawParameterValue("size");
    dryWet = dValueTreeState->getRawParameterValue("dryWet");
    q = dValueTreeState->getRawParameterValue("q");
    cutoff = dValueTreeState->getRawParameterValue("cutoff");
    gain = dValueTreeState->getRawParameterValue("gain");
}

ReverbTestAudioProcessor::~ReverbTestAudioProcessor()
{
}

//==============================================================================
const juce::String ReverbTestAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ReverbTestAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ReverbTestAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ReverbTestAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ReverbTestAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ReverbTestAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ReverbTestAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ReverbTestAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ReverbTestAudioProcessor::getProgramName (int index)
{
    return {};
}

void ReverbTestAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ReverbTestAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    reverb.prepareToPlay(sampleRate, samplesPerBlock);
}

void ReverbTestAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ReverbTestAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ReverbTestAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    
    
    reverb.setParameters(*size, 0, *dryWet);
    //highpass.setParameters(*cutoff, *q);
    reverb.setFilterParameters(*cutoff, *q, *gain);
    
    //highpass.processBlock(buffer);
    reverb.processBlock(buffer);
    
}

//==============================================================================
bool ReverbTestAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ReverbTestAudioProcessor::createEditor()
{
    return new ReverbTestAudioProcessorEditor (*this);
}

//==============================================================================
void ReverbTestAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = dValueTreeState->copyState();
    std::unique_ptr<juce::XmlElement>xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ReverbTestAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    dValueTreeState->replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverbTestAudioProcessor();
}

void ReverbTestAudioProcessor::_constructValueTreeState(){
    dValueTreeState.reset(new juce::AudioProcessorValueTreeState(*this, nullptr, juce::Identifier("reverb"),{
        std::make_unique<juce::AudioParameterInt>(juce::ParameterID("size",1),"Size",1,50,25),
        std::make_unique<juce::AudioParameterFloat> (juce::ParameterID("dryWet",1),"Dry/Wet",0.0f,1.0f,0.5f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("cutoff",1),"shelf", 0.0f,1.f,0.f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("q",1),"Q", 0.f,1.f,0.f),
        std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("gain", 1),"gain", 0.f,1.f,0.f)
    }));
}
