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

#include "DelayLine.h"

//==============================================================================
template <typename Type>
class WaveguideString
{
public:
    //==============================================================================
    WaveguideString()
    {
        setTriggerPosition (Type (0.2));
        setPickupPosition (Type (0.8));
        setDecayTime (Type (0.5));
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        sampleRateHz = (Type) spec.sampleRate;
        tempBlock = juce::dsp::AudioBlock<float> (heapBlock, spec.numChannels, spec.maximumBlockSize);
        filter.prepare (spec);
        updateParameters();
    }

    //==============================================================================
    void reset() noexcept
    {
        forwardDelayLine.clear();
        backwardDelayLine.clear();
    }

    //==============================================================================
    void setFrequency (Type newValueHz)
    {
        freqHz = newValueHz;
        updateParameters();
    }

    //==============================================================================
    void setPickupPosition (Type newValue)
    {
        jassert (newValue >= Type (0) && newValue <= Type (1));
        pickupPos = newValue;
        updateParameters();
    }

    //==============================================================================
    void setTriggerPosition (Type newValue)
    {
        jassert (newValue >= Type (0) && newValue <= Type (1));
        triggerPos = newValue;
        updateParameters();
    }

    //==============================================================================
    void setDecayTime (Type newValue) noexcept
    {
        jassert (newValue >= Type (0) && newValue <= Type (1));
        decayTime = newValue;
        updateParameters();
    }

    //==============================================================================
    void trigger (Type velocity) noexcept
    {
        jassert (velocity >= Type (0) && velocity <= Type (1));

        for (size_t i = 0; i <= forwardTriggerIndex; ++i)
        {
            const auto value = jmap (Type (i), Type (0), Type (forwardTriggerIndex), Type (0), velocity / 2);
            forwardDelayLine.set (i, value);
            backwardDelayLine.set (getDelayLineLength() - 1 - i, value);
        }

        for (size_t i = forwardTriggerIndex; i < getDelayLineLength(); ++i)
        {
            const auto value = jmap (Type (i), Type (forwardTriggerIndex), Type (getDelayLineLength() - 1), velocity / 2, Type (0));
            forwardDelayLine.set (i, value);
            backwardDelayLine.set (getDelayLineLength() - 1 - i, value);
        }
    }

    //==============================================================================
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        auto&& outBlock = context.getOutputBlock();
        const auto numSamples = outBlock.getNumSamples();
        auto* dst = tempBlock.getChannelPointer (0);

        for (size_t i = 0; i < numSamples; ++i)
            dst[i] = processSample();

        for (size_t ch = 1; ch < tempBlock.getNumChannels(); ++ch)
            FloatVectorOperations::copy (tempBlock.getChannelPointer (ch),
                                         tempBlock.getChannelPointer (0),
                                         (int) numSamples);

        outBlock.copy (context.getInputBlock()).add (tempBlock.getSubBlock (0, outBlock.getNumSamples()));
    }

private:
    //==============================================================================
    DelayLine<Type> forwardDelayLine;
    DelayLine<Type> backwardDelayLine;
    juce::dsp::IIR::Filter<Type> filter;

    juce::HeapBlock<char> heapBlock;
    juce::dsp::AudioBlock<float> tempBlock;

    size_t forwardPickupIndex   { 0 };
    size_t backwardPickupIndex  { 0 };
    size_t forwardTriggerIndex  { 0 };
    Type decayCoef;

    Type sampleRateHz           { Type (1e3) };
    Type freqHz                 { Type (1) };
    Type pickupPos              { Type (0) };
    Type triggerPos             { Type (0) };
    Type decayTime              { Type (0) };

    //==============================================================================
    size_t getDelayLineLength() const noexcept { return forwardDelayLine.size(); }

    //==============================================================================
    Type processSample() noexcept
    {
        const auto forwardOut = forwardDelayLine.back();
        const auto backwardOut = backwardDelayLine.back();

        forwardDelayLine.push (-backwardOut);
        backwardDelayLine.push (-decayCoef * filter.processSample (forwardOut));

        return forwardDelayLine.get (forwardPickupIndex) + backwardDelayLine.get (backwardPickupIndex);
    }

    //==============================================================================
    void updateParameters()
    {
        const auto length = (size_t) juce::roundToInt (sampleRateHz / freqHz);
        forwardDelayLine.resize (length);
        backwardDelayLine.resize (length);

        forwardPickupIndex = (size_t) juce::roundToInt (jmap (pickupPos, Type (0), Type (length / 2 - 1)));
        backwardPickupIndex = length - 1 - forwardPickupIndex;

        forwardTriggerIndex = (size_t) juce::roundToInt (jmap (triggerPos, Type (0), Type (length / 2 - 1)));

        filter.coefficients = juce::dsp::IIR::Coefficients<Type>::makeFirstOrderLowPass (sampleRateHz, 4 * freqHz);
        //filter.coefficients = juce::dsp::IIR::Coefficients<Type>::makeFirstOrderHighPass (sampleRateHz, freqHz / 4);

        decayCoef = jmap (decayTime, std::pow (Type (0.999), Type (length)), std::pow (Type (0.99999), Type (length)));

        reset();
    }
};
