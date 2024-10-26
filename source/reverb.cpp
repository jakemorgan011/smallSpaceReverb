/*
  ==============================================================================

    reverb.cpp
    Created: 21 Aug 2024 2:00:15pm
    Author:  Jake Morgan

  ==============================================================================
*/


#include "reverb.h"

reverb::reverb(){
    smoothedReverbSize.setCurrentAndTargetValue(25);
    smoothedFeedbackPercent.setCurrentAndTargetValue(0.5f);
    smoothedDryWetPercent.setCurrentAndTargetValue(0.5f);
    
}
reverb::delayChain::delayChain(){
    
}

reverb::~reverb(){
}

reverb::delayChain::~delayChain(){
    
}

// -----------------------------------
// ===================================
//          REVERB SUPERCLASS
// ===================================
// -----------------------------------

void reverb::prepareToPlay(float inSampleRate, int samplesPerBlock){
    
    sampleRate = inSampleRate;
    
    /// filter setup
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = inSampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;
    
    hpfL.prepare(spec);
    hpfR.prepare(spec);
    
    calculateIIR(0.7, 0.7);
    
    /// regular dsp stuff.
    
    smoothedReverbSize.reset(inSampleRate, 0.01f);
    smoothedFeedbackPercent.reset(inSampleRate, 0.01f);
    smoothedDryWetPercent.reset(0.01f);
    
    allpass1.prepareToPlay(sampleRate);
    allpass2.prepareToPlay(sampleRate);
    allpass3.prepareToPlay(sampleRate);
    
    feedback.prepareToPlay(sampleRate);
    
}

void reverb::processBlock(juce::AudioBuffer<float>& inBuffer){
    
    int reverbSize = smoothedReverbSize.getNextValue();
    
    float* in_channel_left = inBuffer.getWritePointer(0);
    float* in_channel_right = inBuffer.getWritePointer(1);
    
    wetBuff.makeCopyOf(inBuffer);
    
    float* wet_channel_left = wetBuff.getWritePointer(0);
    float* wet_channel_right = wetBuff.getWritePointer(1);
    
    int num_samples = wetBuff.getNumSamples();
    
    for(int i = 0; i < num_samples; i++){
        wet_channel_left[i] = hpfL.processSample(wet_channel_left[i]);
        wet_channel_right[i] = hpfR.processSample(wet_channel_right[i]);
    }
    
    allpass1.setDelayTime(reverbSize, 1);
    allpass2.setDelayTime(reverbSize, 2);
    allpass3.setDelayTime(reverbSize, 3);
    
    // not exploding (good)
    // no tails however.
    allpass1.processBlock(wetBuff);
    allpass2.processBlock(wetBuff);
    allpass3.processBlock(wetBuff);
    //post allpass goes to feedback section. may need to be tweaked for longer tails. currently small room
    
    feedback.processBlock(wetBuff);
    
    // dry wet calculation;
    
    float dry_wet = smoothedDryWetPercent.getNextValue();
    
    int num_samples_inBuff = inBuffer.getNumSamples();
    for(int i = 0; i < num_samples_inBuff; i++){
        
        in_channel_left[i] = ((in_channel_left[i] * (1-dry_wet)) + (wet_channel_left[i] * (dry_wet)));
        in_channel_right[i] = ((in_channel_right[i] * (1-dry_wet)) + (wet_channel_right[i] * (dry_wet)));
    }
    
    
    
    
}

void reverb::setParameters(float inReverbSize, float inFeedbackPercent, float dryWetPercent){
    smoothedReverbSize = inReverbSize;
    smoothedDryWetPercent = dryWetPercent;
    // not using feedbackpercent yet
}

// ----------------------------------
// ==================================
//          ALLPASS(subclass)
// ==================================
// ----------------------------------

void reverb::delayChain::prepareToPlay(float inSampleRate){
   
    allpassBuffer.setSize(2, inSampleRate * 4);
    
}

void reverb::delayChain::processBlock(juce::AudioBuffer<float>& inBuffer){
    int num_samples = inBuffer.getNumSamples();
    
    //dry channel
    float* in_channel_left = inBuffer.getWritePointer(0);
    float* in_channel_right = inBuffer.getWritePointer(1);
    
    float* circular_channel_left = allpassBuffer.getWritePointer(0);
    float* circular_channel_right = allpassBuffer.getWritePointer(1);
    
    for(int i = 0; i < num_samples; i++){
        //
        float input_left = in_channel_left[i];
        float input_right = in_channel_right[i];
        
        //
        in_channel_left[i] = 0.f;
        in_channel_left[i] = 0.f;
        
        // no feedback here because this formula uses a feedback mixer at the end
        // make sure you multiply the output by 0.7 at the end of the function
        circular_channel_left[writehead] = input_left;
        circular_channel_right[writehead] = input_right;
        
        // no dry wet here because we leave that for the superclass Reverb
        //readhead calculation
        readhead = writehead - allpassDelayTime;
        if(readhead < 0){
            readhead += allpassBuffer.getNumSamples();
        }
        
        // multipying delay channel by -1 so that the speakers don't explode
        float delay_out_left = circular_channel_left[readhead] * (-1);
        float delay_out_right = circular_channel_right[readhead] * (-1);
        
        //float delay_out_left = circular_channel_left[readhead];
        //float delay_out_right = circular_channel_right[readhead];
        
        in_channel_left[i] = (input_left + delay_out_left)* scale;
        in_channel_right[i] = (input_right + delay_out_right)* scale;
        
        writehead++;
        if(writehead >= allpassBuffer.getNumSamples()){
            writehead = 0;
        }
        // allpass dsp complete.
    }
    
}

void reverb::delayChain::setDelayTime(float inSize, int allpassNum){
    if (allpassNum == 1){
        allpassDelayTime = inSize * allpassCF1;
    }
    if (allpassNum == 2){
        allpassDelayTime = inSize * allpassCF2;
    }
    if (allpassNum == 3){
        allpassDelayTime = inSize * allpassCF3;
    }
    else{
        std::cout << "allpassNum_error";
    }

}

// -----------------------------------
// ===================================
//          FEEDBACK(subclass)
// ===================================
// -----------------------------------

reverb::feedback::feedback(){
   
    simulFeedback1.setDelayTimes(1);
    simulFeedback2.setDelayTimes(2);
    simulFeedback3.setDelayTimes(3);
    simulFeedback4.setDelayTimes(4);
    
}

reverb::feedback::~feedback(){
    
}

void reverb::feedback::prepareToPlay(float inSampleRate){
    
    buff1.setSize(2, inSampleRate * 4);
    buff2.setSize(2, inSampleRate * 4);
    buff3.setSize(2, inSampleRate * 4);
    buff4.setSize(2, inSampleRate * 4);
    buff5.setSize(2, inSampleRate * 4);
    
    simulFeedback1.prepareToPlay(inSampleRate);
    simulFeedback2.prepareToPlay(inSampleRate);
    simulFeedback3.prepareToPlay(inSampleRate);
    simulFeedback4.prepareToPlay(inSampleRate);
    
}

void reverb::feedback::processBlock(juce::AudioBuffer<float>& inBuffer){
    
    
    int numSamples = inBuffer.getNumSamples();
    
    // needs to be simultaneous make buffer 4 variables
    buff1.makeCopyOf(inBuffer);
    buff2.makeCopyOf(inBuffer);
    buff3.makeCopyOf(inBuffer);
    buff4.makeCopyOf(inBuffer);
    
    
    simulFeedback1.processBlock(buff1);
    simulFeedback2.processBlock(buff2);
    simulFeedback3.processBlock(buff3);
    simulFeedback4.processBlock(buff4);
    
    // main buffer.
    float* inChannelLeft = inBuffer.getWritePointer(0);
    float* inChannelRight = inBuffer.getWritePointer(1);
    
    // this will be the final mixed feedback.
    float* mainOutLeft = buff5.getWritePointer(0);
    float* mainOutRight = buff5.getWritePointer(1);
    
    // simultaneous feedback delays
    float* buff1Left = buff1.getWritePointer(0);
    float* buff1Right = buff1.getWritePointer(1);
    
    float* buff2Left = buff2.getWritePointer(0);
    float* buff2Right = buff2.getWritePointer(1);
    
    float* buff3Left = buff3.getWritePointer(0);
    float* buff3Right = buff3.getWritePointer(1);
    
    float* buff4Left = buff4.getWritePointer(0);
    float* buff4Right = buff4.getWritePointer(1);
    
    for(int i = 0; i < numSamples; i++){
        
        //left channel
        mainOutLeft[i] = (buff1Left[i] + buff3Left[i] + buff1Right[i] + buff3Right[i]);
        mainOutLeft[i] += ((buff1Left[i] + buff3Left[i] + buff1Right[i] + buff3Right[i])-(buff2Left[i] + buff2Right[i] + buff4Left[i] + buff4Right[i]))* -1 * (0.01f);
        
        //right channel
        mainOutRight[i] = (buff1Left[i] + buff3Left[i] + buff1Right[i] + buff3Right[i])-(buff2Left[i] + buff2Right[i] + buff4Left[i] + buff4Right[i]);
        mainOutRight[i] += ((buff1Left[i] + buff3Left[i] + buff1Right[i] + buff3Right[i])+(buff2Left[i] + buff2Right[i] + buff4Left[i] + buff4Right[i]))* -1 * (0.01f);
        
        inChannelRight[i] = (mainOutRight[i]);
        inChannelLeft[i] = (mainOutLeft[i]);
    }
    
}

// ----------------------------------------
// ========================================
//        SIMULFEEDBACK(subsubclass)
// ========================================
// ----------------------------------------

reverb::feedback::simulFeedback::simulFeedback(){
    
}

reverb::feedback::simulFeedback::~simulFeedback(){
    
}

void reverb::feedback::simulFeedback::setDelayTimes(int feedbackNumber){
    if (feedbackNumber == 1){
        feedbackDelayTime = feedbackCF1;
    }
    if (feedbackNumber == 2){
        feedbackDelayTime = feedbackCF2;
    }
    if (feedbackNumber == 3){
        feedbackDelayTime = feedbackCF3;
    }
    if (feedbackNumber == 4){
        feedbackDelayTime = feedbackCF4;
    }
}

void reverb::feedback::simulFeedback::prepareToPlay(float inSampleRate){
    
    feedbackBuffer.setSize(2, inSampleRate * 4);
    
}

void reverb::feedback::simulFeedback::processBlock(juce::AudioBuffer<float>& inBuffer){
    int numSamples = inBuffer.getNumSamples();
    
    float* in_channel_left = inBuffer.getWritePointer(0);
    float* in_channel_right = inBuffer.getWritePointer(1);
    
    float* circular_channel_left = feedbackBuffer.getWritePointer(0);
    float* circular_channel_right = feedbackBuffer.getWritePointer(1);
    
    for(int i = 0; i < numSamples; i++){
        //
        float input_left = in_channel_left[i];
        float input_right = in_channel_right[i];
        
        //
        in_channel_left[i] = 0.f;
        in_channel_left[i] = 0.f;
        
        // no feedback here because this formula uses a feedback mixer at the end
        // make sure you multiply the output by 0.7 at the end of the function
        circular_channel_left[writehead] = input_left;
        circular_channel_right[writehead] = input_right;
        
        // no dry wet here because we leave that for the superclass Reverb
        //readhead calculation
        readhead = writehead - feedbackDelayTime;
        if(readhead < 0){
            readhead += feedbackBuffer.getNumSamples();
        }
        
        // multipying delay channel by -1 so that the speakers don't explode
        float delay_out_left = circular_channel_left[readhead] * (-1);
        float delay_out_right = circular_channel_right[readhead] * (-1);
        
        in_channel_left[i] = (input_left + delay_out_left)* scale;
        in_channel_right[i] = (input_right + delay_out_right)* scale;
        
        
        writehead++;
        if(writehead >= feedbackBuffer.getNumSamples()){
            writehead = 0;
        }
    }
}



// ==================================
///             filters
// ==================================


float reverb::normalizeFreq(float inFreq){
    
    return logf(juce::jlimit(0.001f, 21950.f, inFreq) / 10.f) / logf(2200.f);
}

float reverb::freqFromNormalized(float inNorm){
    
    return juce::jlimit(0.001f, 21950.f, 10 * powf(2200.f, inNorm));
}

void reverb::calculateIIR(float inFilterFreq, float inFilterQ){
    float freq = freqFromNormalized(inFilterFreq);
    float Q = (powf(60, inFilterQ)- 0.95f)/50.f;
    
    hpfL.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, freq, Q);
    hpfR.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, freq, Q);
}

void reverb::setFilterParameters(float inFilterFreq, float inFilterQ){
    
    if(inFilterFreq != lastFilterFreq || inFilterQ != lastFilterQ){
        
        hpfL.reset();
        hpfR.reset();
        
        calculateIIR(inFilterFreq, inFilterQ);
    }
    lastFilterFreq = inFilterFreq;
    lastFilterFreq = inFilterFreq;
}

