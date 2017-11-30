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

#include "../../Shared/CabIR.h"

template <typename Type>
class CabSimulator
{
public:
    //==============================================================================
    CabSimulator()
    {
            // <- 10.5. get a reference to the Convolution with processorChain.get<>()
            // <- 10.6. load 1024 samples of the impulse response specified in the CabIR namespace
    }

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
            // <- 10.4. prepare the processorChain

        ignoreUnused (spec);
    }

    //==============================================================================
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept
    {
            // <- 10.7. process the processorChain with the given context

        ignoreUnused (context);
    }

    //==============================================================================
    void reset() noexcept
    {
            // <- 10.3. reset the processorChain
    }

private:
    //==============================================================================
    // enum
    // {
            // <- 10.2. add Convolution index
    // };

    // juce::dsp::ProcessorChain<
            // <- 10.1. add juce::dsp::Convolution
    // > processorChain;
};
