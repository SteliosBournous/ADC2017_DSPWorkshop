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
class Distortion
{
public:
    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        processorChain.template get<filterIndex>().state = FilterCoefs::makeHighPass (Type (spec.sampleRate), Type (300));

        processorChain.prepare (spec);
        processorChain.template get<preGainIndex>().setGainDecibels (Type (60));
        processorChain.template get<waveshaperIndex>().functionToUse = std::tanh;
    }

    //==============================================================================
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept { processorChain.process (context); }
    void reset() noexcept                                 { processorChain.reset(); }

private:
    //==============================================================================
    using Filter = juce::dsp::IIR::Filter<Type>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<Type>;

    enum
    {
        preGainIndex,
        filterIndex,
        waveshaperIndex
    };

    juce::dsp::ProcessorChain<
        juce::dsp::Gain<Type>,
        juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>,
        juce::dsp::WaveShaper<Type>
    > processorChain;
};
