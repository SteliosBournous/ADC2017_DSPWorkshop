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
#include "../../Shared/WaveguideString.h"

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

        auto& filter = processorChain.get<filterIndex>();
        filter.setResonance (0.8f);
        filter.setDrive (5.0f);
        filter.setMode (juce::dsp::LadderFilter<float>::Mode::LPF24);
        filter.setEnabled (true);

        auto& masterGain = processorChain.get<masterGainIndex>();
        masterGain.setGainLinear (0.7f);
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        lfo.prepare ({ spec.sampleRate / lfoDownsamplingRatio, spec.maximumBlockSize, 1 });

        tempBuffer.setSize ((int) spec.numChannels, (int) spec.maximumBlockSize);
        processorChain.prepare (spec);
    }

    //==============================================================================
    void noteStarted() override
    {
        const auto velocity = getCurrentlyPlayingNote().noteOnVelocity.asUnsignedFloat();
        const auto freqHz = (float) getCurrentlyPlayingNote().getFrequencyInHertz();

        auto& stringModel = processorChain.get<stringIndex>();
        stringModel.setFrequency (freqHz);
        stringModel.trigger (velocity);

        auto& stringModel2 = processorChain.get<string2Index>();
        stringModel2.setFrequency (1.01f * freqHz);
        stringModel2.trigger (velocity);

        processorChain.get<osc1Index>().setFrequency (freqHz, true);
        processorChain.get<osc2Index>().setFrequency (freqHz, true);
    }

    //==============================================================================
    void notePitchbendChanged () override
    {
        const auto freqHz = (float) getCurrentlyPlayingNote().getFrequencyInHertz();
        processorChain.get<osc1Index>().setFrequency (freqHz);
        processorChain.get<osc2Index>().setFrequency (freqHz * 2);
    }

    //==============================================================================
    void noteTimbreChanged() override
    {
        const auto cutoffFreqHz = jmap (getCurrentlyPlayingNote().timbre.asUnsignedFloat(), 20.0f, 5e3f);
        auto newFilterParams = juce::dsp::StateVariableFilter::Parameters<float>();
        processorChain.get<filterIndex>().setCutoffFrequencyHz (cutoffFreqHz);
    }

    //==============================================================================
    void noteStopped (bool /*allowTailOff*/) override
    {
        //clearCurrentNote();
        processorChain.get<osc1Index>().setLevel (0.0f);
        processorChain.get<osc2Index>().setLevel (0.0f);
    }

    //==============================================================================
    void notePressureChanged() override
    {
        const auto pressure = 0.1f * getCurrentlyPlayingNote().pressure.asUnsignedFloat();

        processorChain.get<osc1Index>().setLevel (pressure);
        processorChain.get<osc2Index>().setLevel (pressure);
    }

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

        auto block = juce::dsp::AudioBlock<float> (tempBuffer).getSubBlock ((size_t) 0, (size_t) numSamples);
        block.clear();
        juce::dsp::ProcessContextReplacing<float> context (block);
        processorChain.process (context);

        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            outputBuffer.addFrom (ch, startSample, tempBuffer.getReadPointer (ch), numSamples);

        bool active = false;
        for (int i = 0; i < numSamples; ++i)
        {
            if (tempBuffer.getReadPointer (0)[i] != 0.0f)
            {
                active = true;
                break;
            }
        }

        if (! active)
            clearCurrentNote();
    }

private:
    //==============================================================================
    AudioBuffer<float> tempBuffer { 2, 128 };

    enum
    {
        stringIndex,
        string2Index,
        osc1Index,
        osc2Index,
        filterIndex,
        masterGainIndex
    };

    juce::dsp::ProcessorChain<
        WaveguideString<float>,
        WaveguideString<float>,
        Oscillator<float>,
        Oscillator<float>,
        juce::dsp::LadderFilter<float>,
        juce::dsp::Gain<float>> processorChain;

    static constexpr size_t lfoDownsamplingRatio = 128;
    size_t lfoProcessingIndex = lfoDownsamplingRatio;
    juce::dsp::Oscillator<float> lfo;
};
