/*
  ==============================================================================

   This file is part of the ADC 2017 DSP Workshop demo project.
   Copyright (c) 2017 - ROLI Ltd.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

#pragma once

#include "Voice.h"

//==============================================================================
class AudioEngine  : public juce::MPESynthesiser
{
public:
    static constexpr size_t maxNumVoices = 4;

    //==============================================================================
    AudioEngine()
    {
        for (size_t i = 0; i < maxNumVoices; ++i)
            addVoice (new Voice);

        setVoiceStealingEnabled (true);
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec) noexcept
    {
        setCurrentPlaybackSampleRate (spec.sampleRate);

        for (auto* v : voices)
            dynamic_cast<Voice*> (v)->prepare (spec);

            // <- 6.3. prepare the fxChain
    }

private:
    //==============================================================================
    void renderNextSubBlock (AudioBuffer<float>& outputAudio, int startSample, int numSamples) override
    {
        MPESynthesiser::renderNextSubBlock (outputAudio, startSample, numSamples);

            // <- 6.4. wrap outputAudio in a juce::dsp::AudioBlock
            // <- 6.5. create another juce::dsp::AudioBlock which refers to the section
            //         of the previous block defined by startSample and numSamples
            // <- 6.6. wrap the above AudioBlock in a juce::dsp::ProcessContextReplacing
            // <- 6.7. process the fxChain with the above context
    }

    // enum
    // {
            // <- 6.2. add Reverb index
    // };

    // juce::dsp::ProcessorChain<
            // <- 6.1. add juce::dsp::Reverb
    // > fxChain;
};
