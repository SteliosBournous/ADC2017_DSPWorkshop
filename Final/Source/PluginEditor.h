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

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "../../Shared/ScopeComponent.h"

//==============================================================================
class DspworkshopAdc2017AudioProcessorEditor  : public AudioProcessorEditor
{
public:
    DspworkshopAdc2017AudioProcessorEditor (DspworkshopAdc2017AudioProcessor&);
    ~DspworkshopAdc2017AudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    DspworkshopAdc2017AudioProcessor& processor;
    MidiKeyboardState midiKeyboardState;
    MidiKeyboardComponent midiKeyboardComponent { midiKeyboardState, MidiKeyboardComponent::horizontalKeyboard };
    ScopeComponent<float> scopeComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DspworkshopAdc2017AudioProcessorEditor)
};
