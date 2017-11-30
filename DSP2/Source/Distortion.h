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
    Distortion()
    {
            // <- 7.5. get a reference to the Waveshaper with processorChain.get<>()
            // <- 7.6. initialise the Waveshaper to a hard clipper, which limits the
            //         input signal to the [-0.1, 0.1] range

            // <- 8.5. change the Waveshaper's transfer curve to hyperbolic tangent
            // <- 8.6. get a reference to the pre-gain with processorChain.get<>()
            // <- 8.7. set the pre-gain to 30 dB

            // <- 8.8. get a reference to the post-gain with processorChain.get<>()
            // <- 8.9. set the post-gain to -20 dB

            // <- 10.8. set the post-gain to 0 dB after the CabSimulator has 
            //          been integrated
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
            // <- 9.3. get a reference to the filter ProcessorDuplicator with
            //         processorChain.get<>()

            // <- 9.4 assign a new set of coefficients to the ProcessorDuplicator's
            //        state using juce::dsp::IIR::Coefficients::makeFirstOrderHighPass();
            //        the cutoff frequency should be 1 kHz

            // <- 7.4. prepare the processorChain

        ignoreUnused (spec);
    }

    //==============================================================================
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
            // <- 7.7. process the processorChain with the given context

        ignoreUnused (context);
    }

    //==============================================================================
    void reset() noexcept
    {
            // <- 7.3. reset the processorChain
    }

private:
    //==============================================================================
    // enum
    // {
            // <- 9.2. add Filter index
            // <- 8.3. add pre-gain index
            // <- 7.2. add Waveshaper index
            // <- 8.4. add post-gain index
    // };

    using Filter = juce::dsp::IIR::Filter<Type>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<Type>;

    // juce::dsp::ProcessorChain<
            // <- 9.1. add a multi-channel IIR filter using juce::dsp::ProcessorDuplicator,
            //         Filter and FilterCoefs
            // <- 8.1. add a juce::dsp::Gain<Type>
            // <- 7.1. add a juce::dsp::WaveShaper
            // <- 8.2. add a juce::dsp::Gain<Type>
    // > processorChain;
};
