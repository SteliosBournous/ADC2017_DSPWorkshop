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
#include "../../Shared/Distortion.h"
#include "../../Shared/CabSimulator.h"
#include "../../Shared/Delay.h"

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
        fxChain.prepare (spec);

        for (auto* v : voices)
            dynamic_cast<Voice*> (v)->prepare (spec);
    }

private:
    //==============================================================================
    enum
    {
        distortionIndex,
        cabSimIndex,
        delayIndex,
        reverbIndex
    };

    juce::dsp::ProcessorChain<Distortion<float>,
                              CabSimulator<float>,
                              Delay<float>,
                              juce::dsp::Reverb> fxChain;

    //==============================================================================
    void renderNextSubBlock (AudioBuffer<float>& outputAudio, int startSample, int numSamples) override
    {
        juce::MPESynthesiser::renderNextSubBlock (outputAudio, startSample, numSamples);

        auto block = juce::dsp::AudioBlock<float> (outputAudio).getSubBlock ((size_t) startSample, (size_t) numSamples);
        juce::dsp::ProcessContextReplacing<float> context (block);
        fxChain.process (context);
    }
};
