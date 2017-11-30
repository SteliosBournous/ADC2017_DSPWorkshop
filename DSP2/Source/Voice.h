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

//==============================================================================
class Voice  : public juce::MPESynthesiserVoice
{
public:
    Voice()
    {
        lfo.initialise ([] (float x) { return std::sin (x); }, 128);
        lfo.setFrequency (3.0f);

        const auto waveform = Oscillator<float>::Waveform::saw;
        processorChain.get<osc1Index>().setWaveform (waveform);
        processorChain.get<osc2Index>().setWaveform (waveform);

        auto& masterGain = processorChain.get<masterGainIndex>();
        masterGain.setGainLinear (0.7f);

        auto& filter = processorChain.get<filterIndex>();
        filter.setMode (juce::dsp::LadderFilter<float>::Mode::LPF24);
        filter.setResonance (0.7f);
        filter.setCutoffFrequencyHz (500.0f);
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        lfo.prepare ({ spec.sampleRate / lfoDownsamplingRatio, spec.maximumBlockSize, 1 });
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

        processorChain.get<osc2Index>().setFrequency (1.01f * freqHz, true);
        processorChain.get<osc2Index>().setLevel (velocity);
    }

    //==============================================================================
    void notePitchbendChanged () override
    {
        const auto freqHz = (float) getCurrentlyPlayingNote().getFrequencyInHertz();
        processorChain.get<osc1Index>().setFrequency (freqHz);
        processorChain.get<osc2Index>().setFrequency (1.01f * freqHz);
    }

    //==============================================================================
    void noteStopped (bool /*allowTailOff*/) override
    {
        clearCurrentNote();
    }

    //==============================================================================
    void notePressureChanged() override {}
    void noteTimbreChanged() override   {}
    void noteKeyStateChanged() override {}

    //==============================================================================
    void renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        for (int i = 0; i < numSamples; ++i)
        {
            if (--lfoProcessingIndex == 0)
            {
                lfoProcessingIndex = lfoDownsamplingRatio;
                const auto lfoOut = lfo.processSample (0.0f);
                const auto cutoffHz = jmap (lfoOut, -1.0f, 1.0f, 100.0f, 4e3f);
                processorChain.get<filterIndex>().setCutoffFrequencyHz (cutoffHz);
            }
        }

        auto block = tempBlock.getSubBlock (0, (size_t) numSamples);
        block.clear();
        juce::dsp::ProcessContextReplacing<float> context (block);
        processorChain.process (context);

        juce::dsp::AudioBlock<float> (outputBuffer)
            .getSubBlock ((size_t) startSample, (size_t) numSamples)
            .add (tempBlock);
    }

private:
    //==============================================================================
    juce::HeapBlock<char> heapBlock;
    juce::dsp::AudioBlock<float> tempBlock;

    enum
    {
        osc1Index,
        osc2Index,
        filterIndex,
        masterGainIndex
    };

    juce::dsp::ProcessorChain<
        Oscillator<float>,
        Oscillator<float>,
        juce::dsp::LadderFilter<float>,
        juce::dsp::Gain<float>> processorChain;

    static constexpr size_t lfoDownsamplingRatio = 128;
    size_t lfoProcessingIndex = lfoDownsamplingRatio;
    juce::dsp::Oscillator<float> lfo;
};
