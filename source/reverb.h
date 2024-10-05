/*
  ==============================================================================

    reverb.h
    Created: 21 Aug 2024 2:00:15pm
    Author:  Jake Morgan

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

//--------------------------------------------------------------------------
//==========================================================================
//
//                          REVERB SUPERCLASS
//
//==========================================================================
//--------------------------------------------------------------------------

// try making 3 different instances of allpass so that the buffer stops exploding
// i think the signal duplicates each time it is used.
class reverb {
    
public:
    //
    reverb();
    //
    ~reverb();
    
    void prepareToPlay(float inSampleRate);
    
    void processBlock(juce::AudioBuffer<float>& inBuffer);
    
    void setParameters(float inReverbSize, float inFeedbackPercent, float dryWetPercent);
    
    // nested class for individual all pass processes
    
    //============================================
    //                  ALLPASS
    //============================================
    class delayChain {
        
    public:
        //
        delayChain();
        //
        ~delayChain();
        
        void prepareToPlay(float inSampleRate);
        
        void processBlock(juce::AudioBuffer<float>& inBuffer);
        
        void setDelayTime(float inSize, int allpassNum);
        
        private:
        //
        int allpassDelayTime;
        
        // set size to samplerate * 4;
        juce::AudioBuffer<float> allpassBuffer;
        
        int writehead = 0;
        int readhead = 0;
        //possibly make this a parameter so that we can modify it constantly

        
    };
    
    //nested feedback portion
    // figure out how to all 4 feedback chains at once
    // for loop? maybe? nested class?
    
    // ===============================================
    //                 feedback
    // ===============================================
    class feedback {
        
    public:
        //
        feedback();
        //
        ~feedback();
        
        void prepareToPlay(float inSampleRate);
        
        void processBlock(juce::AudioBuffer<float>& inBuffer);
        // figure out a good function to do the feedback portion
        
        // =======================================================
        //               four simultaneous feedbacks
        // =======================================================
        class simulFeedback{
            
        public:
            simulFeedback();
            
            ~simulFeedback();
            
            void setDelayTimes(int feedbackNum);
            
            void prepareToPlay(float inSampleRate);
            
            void processBlock(juce::AudioBuffer<float>& inBuffer);
        private:
            // heads
            int readhead = 0;
            int writehead = 0;
            
            // circular buffer
            juce::AudioBuffer<float> feedbackBuffer;
            
            int feedbackDelayTime;
        };
        
    private:
        
        juce::AudioBuffer<float> buff1;
        juce::AudioBuffer<float> buff2;
        juce::AudioBuffer<float> buff3;
        juce::AudioBuffer<float> buff4;
        juce::AudioBuffer<float> buff5;
        
        
        simulFeedback simulFeedback1;
        simulFeedback simulFeedback2;
        simulFeedback simulFeedback3;
        simulFeedback simulFeedback4;
    };
    
private:
    
    
    // this can change.
    const float maxReverbSize = 50.f;
    float sampleRate = 44100.f;
    
    const static int allpassCF1 = 347;
    const static int allpassCF2 = 113;
    const static int allpassCF3 = 37;
    
    const static int feedbackCF1 = 1687;
    const static int feedbackCF2 = 1601;
    const static int feedbackCF3 = 2053;
    const static int feedbackCF4 = 2251;
    
    // previously 0.7f
    constexpr const static float scale = 0.8f;
    
    juce::LinearSmoothedValue<int> smoothedReverbSize;
    juce::LinearSmoothedValue<float> smoothedFeedbackPercent;
    juce::LinearSmoothedValue<float> smoothedDryWetPercent;
    
    
    delayChain allpass1;
    delayChain allpass2;
    delayChain allpass3;
    
    feedback feedback;
    
    juce::AudioBuffer<float> buff1;
    juce::AudioBuffer<float> buff2;
    juce::AudioBuffer<float> buff3;
    juce::AudioBuffer<float> buff4;
    
};
