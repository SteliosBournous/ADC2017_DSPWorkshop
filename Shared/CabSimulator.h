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

#include "CabIR.h"

template <typename Type>
class CabSimulator
{
public:
    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        processorChain.prepare (spec);
        processorChain.get<0>().loadImpulseResponse (CabIR::IR_wav, CabIR::IR_wavSize, true, false, 1024);
    }

    //==============================================================================
    template <typename ProcessContext>
    void process (const ProcessContext& context) noexcept { processorChain.process (context); }
    void reset() noexcept                                 { processorChain.reset(); }

private:
    //==============================================================================
    juce::dsp::ProcessorChain<juce::dsp::Convolution> processorChain;
};
