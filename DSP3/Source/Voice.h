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

#include "../../Shared/Oscillator.h"
#include "WaveguideString.h"

//==============================================================================
class Voice  : public juce::MPESynthesiserVoice
{
public:
    Voice()
    {
        const auto waveform = Oscillator<float>::Waveform::saw;
        processorChain.get<osc1Index>().setWaveform (waveform);

        auto& masterGain = processorChain.get<masterGainIndex>();
        masterGain.setGainLinear (0.7f);
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        tempBlock = juce::dsp::AudioBlock<float> (heapBlock, spec.numChannels, spec.maximumBlockSize);
        processorChain.prepare (spec);
    }

    //==============================================================================
    void noteStarted() override
    {
        const auto velocity = getCurrentlyPlayingNote().noteOnVelocity.asUnsignedFloat();
        const auto freqHz = (float) getCurrentlyPlayingNote().getFrequencyInHertz();

        processorChain.get<osc1Index>().setFrequency (freqHz, true);

        processorChain.get<osc1Index>().setLevel (velocity);
            // <- 14.11. bypass the oscillator

            // <- 14.12. get a reference to the WaveguideString with processorChain.get<>()
            // <- 14.13. set the frequency of the WaveguideString
            // <- 14.14. trigger the WaveguideString
    }

    //==============================================================================
    void notePitchbendChanged () override
    {
        const auto freqHz = (float) getCurrentlyPlayingNote().getFrequencyInHertz();
        processorChain.get<osc1Index>().setFrequency (freqHz);
    }

    //==============================================================================
    void noteStopped (bool /*allowTailOff*/) override
    {
        processorChain.get<osc1Index>().setLevel (0.0f);
    }

    //==============================================================================
    void notePressureChanged() override {}
    void noteTimbreChanged() override   {}
    void noteKeyStateChanged() override {}

    //==============================================================================
    void renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        auto block = tempBlock.getSubBlock (0, (size_t) numSamples);
        block.clear();
        juce::dsp::ProcessContextReplacing<float> context (block);
        processorChain.process (context);

        // silence detector
        bool active = false;
        for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
        {
            auto* channelPtr = block.getChannelPointer (ch);

            for (int i = 0; i < numSamples; ++i)
            {
                if (channelPtr[i] != 0.0f)
                {
                    active = true;
                    break;
                }
            }
        }

        if (active)
        {
            juce::dsp::AudioBlock<float> (outputBuffer)
                .getSubBlock ((size_t) startSample, (size_t) numSamples)
                .add (tempBlock);
        }
        else
        {
            clearCurrentNote();
        }
    }

private:
    //==============================================================================
    juce::HeapBlock<char> heapBlock;
    juce::dsp::AudioBlock<float> tempBlock;

    enum
    {
        osc1Index,
                        // <- 14.10. add WaveguideString index
        masterGainIndex
    };

    juce::dsp::ProcessorChain<
        Oscillator<float>,
                        // <- 14.9. add a WaveguideString
        juce::dsp::Gain<float>
    > processorChain;
};
