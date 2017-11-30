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

template <typename Type>
class Oscillator
{
public:
    //==============================================================================
    Oscillator()
    {
        setWaveform (Waveform::sine);

        auto& gain = processorChain.template get<gainIndex>();
        gain.setRampDurationSeconds (3e-2);
        gain.setGainLinear (Type (0));
    }

    //==============================================================================
    enum class Waveform { sine, saw };

    void setWaveform (Waveform waveform)
    {
        switch (waveform)
        {
        case Waveform::sine: processorChain.template get<oscIndex>().initialise ([] (Type x) { return std::sin (x); }, 128);                                                      break;
        case Waveform::saw:  processorChain.template get<oscIndex>().initialise ([] (Type x) { return jmap (x, Type (-double_Pi), Type (float_Pi), Type (-1), Type (1)); }, 128); break;
        default:             jassertfalse;                                                                                                                                        break;
        }
    }

    //==============================================================================
    void setFrequency (Type newValue, bool force = false) { processorChain.template get<oscIndex>().setFrequency (newValue, force); }
    void setLevel (Type newValue)                         { processorChain.template get<gainIndex>().setGainLinear (newValue); }
    void reset() noexcept                                 { processorChain.reset(); }

    //==============================================================================
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        auto&& outBlock = context.getOutputBlock();
        auto blockToUse = tempBlock.getSubBlock (0, outBlock.getNumSamples());
        juce::dsp::ProcessContextReplacing<float> tempContext (blockToUse);
        processorChain.process (tempContext);

        outBlock.copy (context.getInputBlock()).add (blockToUse);
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        tempBlock = juce::dsp::AudioBlock<float> (heapBlock, spec.numChannels, spec.maximumBlockSize);
        processorChain.prepare (spec);
    }

private:
    //==============================================================================
    juce::HeapBlock<char> heapBlock;
    juce::dsp::AudioBlock<float> tempBlock;

    enum
    {
        oscIndex,
        gainIndex,
    };

    juce::dsp::ProcessorChain<
        juce::dsp::Oscillator<Type>,
        juce::dsp::Gain<Type>
    > processorChain;
};
