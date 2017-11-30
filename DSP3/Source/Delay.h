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

template <typename Type, size_t maxNumChannels = 2>
class Delay
{
public:
    //==============================================================================
    Delay()
    {
        setMaxDelayTime (2.0f);
        setDelayTime (0, 0.7f);
        setDelayTime (1, 0.5f);
        setWetLevel (0.8f);
        setFeedback (0.5f);
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        jassert (spec.numChannels <= maxNumChannels);
        sampleRate = (Type) spec.sampleRate;
        updateDelayLineSize();
        updateDelayTime();

        filterCoefs = juce::dsp::IIR::Coefficients<Type>::makeFirstOrderLowPass (sampleRate, Type (1e3));

            // <- 13.2. replace the above low-pass filter with a high-pass filter

        for (auto& f : filters)
        {
            f.prepare (spec);
            f.coefficients = filterCoefs;
        }
    }

    //==============================================================================
    void reset() noexcept
    {
        for (auto& f : filters)
            f.reset();

        for (auto& dline : delayLines)
            dline.clear();
    }

    //==============================================================================
    size_t getNumChannels() const noexcept
    {
        return delayLines.size();
    }

    //==============================================================================
    void setMaxDelayTime (Type newValue)
    {
        jassert (newValue > Type (0));
        maxDelayTime = newValue;
        updateDelayLineSize();
    }

    //==============================================================================
    void setFeedback (Type newValue) noexcept
    {
        jassert (newValue >= Type (0) && newValue <= Type (1));
        feedback = newValue;
    }

    //==============================================================================
    void setWetLevel (Type newValue) noexcept
    {
        jassert (newValue >= Type (0) && newValue <= Type (1));
        wetLevel = newValue;
    }

    //==============================================================================
    void setDelayTime (size_t channel, Type newValue)
    {
        if (channel >= getNumChannels())
        {
            jassertfalse;
            return;
        }

        jassert (newValue >= Type (0));
        delayTimes[channel] = newValue;

        updateDelayTime();
    }

    //==============================================================================
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numSamples = outputBlock.getNumSamples();
        const auto numChannels = outputBlock.getNumChannels();

        jassert (inputBlock.getNumSamples() == numSamples);
        jassert (inputBlock.getNumChannels() == numChannels);

        for (size_t ch = 0; ch < numChannels; ++ch)
        {
            const auto* input = inputBlock.getChannelPointer (ch);
            auto* output = outputBlock.getChannelPointer (ch);
            auto& dline = delayLines[ch];
            const auto delayTime = delayTimesSample[ch];
            auto& filter = filters[ch];

            ignoreUnused (input);
            ignoreUnused (output);
            ignoreUnused (dline);
            ignoreUnused (delayTime);
            ignoreUnused (filter);

            for (size_t i = 0; i < numSamples; ++i)
            {
                    // <- 12.1. get the delayed sample from the delay line

                    // <- 13.1. apply the filter to the delay line output

                    // <- 12.2. get the current input sample from input

                    // <- 12.3. calculate the sample to be pushed to the delay line
                    //          by mixing the input sample with the delay line output
                    //          weighted with the feedback parameter

                    // <- 12.4. push the above sample to the delay line

                    // <- 12.5. calculate the output sample by mixing the input sample
                    //          with the delay line output weighted with the wetLevel
                    //          parameter

                    // <- 12.6. store the output sample to output
            }
        }
    }

private:
    //==============================================================================
    std::array<DelayLine<Type>, maxNumChannels> delayLines;
    std::array<size_t, maxNumChannels> delayTimesSample;
    std::array<Type, maxNumChannels> delayTimes;
    Type feedback       { Type (0) };
    Type wetLevel       { Type (0) };

    std::array<juce::dsp::IIR::Filter<Type>, maxNumChannels> filters;
    typename juce::dsp::IIR::Coefficients<Type>::Ptr filterCoefs;

    Type sampleRate     { Type (44.1e3) };
    Type maxDelayTime   { Type (2) };

    //==============================================================================
    void updateDelayLineSize()
    {
        const auto delayLineSizeSamples = (size_t) std::ceil (maxDelayTime * sampleRate);

        for (auto& dline : delayLines)
            dline.resize (delayLineSizeSamples);
    }

    //==============================================================================
    void updateDelayTime() noexcept
    {
        for (size_t ch = 0; ch < maxNumChannels; ++ch)
            delayTimesSample[ch] = (size_t) juce::roundToInt (delayTimes[ch] * sampleRate);
    }
};
