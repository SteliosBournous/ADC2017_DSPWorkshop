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
    void prepare (const juce::dsp::ProcessSpec& /*spec*/)
    {
        /////////////////////////////////////
        // TODO: initialise the signal chain
        /////////////////////////////////////
    }

    //==============================================================================
    template <typename ProcessContext>
    void process (const ProcessContext& /*context*/) noexcept
    {
        //////////////////////////////////
        // TODO: process the signal chain
        //////////////////////////////////
    }

    //==============================================================================
    void reset() noexcept
    {
        //////////////////////////////
        // TODO: reset internal state
        //////////////////////////////
    }

private:
    //==============================================================================
    //////////////////////////////
    // TODO: add member variables
    //////////////////////////////
};
