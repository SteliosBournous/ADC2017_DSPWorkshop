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

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DspworkshopAdc2017AudioProcessorEditor::DspworkshopAdc2017AudioProcessorEditor (DspworkshopAdc2017AudioProcessor& p)
    : AudioProcessorEditor (&p),
      processor (p),
      scopeComponent (processor.getAudioBufferQueue())
{
    addAndMakeVisible (midiKeyboardComponent);
    addAndMakeVisible (scopeComponent);

    setSize (400, 300);

    Rectangle<int> area (getLocalBounds());
    scopeComponent.setTopLeftPosition (0, 80);
    scopeComponent.setSize (area.getWidth(), area.getHeight() - 100);

    midiKeyboardComponent.setMidiChannel (2);
    midiKeyboardState.addListener (& processor.getMidiMessageCollector());
}

//==============================================================================
DspworkshopAdc2017AudioProcessorEditor::~DspworkshopAdc2017AudioProcessorEditor()
{
    midiKeyboardState.removeListener (& processor.getMidiMessageCollector());
}

//==============================================================================
void DspworkshopAdc2017AudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

//==============================================================================
void DspworkshopAdc2017AudioProcessorEditor::resized()
{
    Rectangle<int> area (getLocalBounds());
    midiKeyboardComponent.setBounds (area.removeFromTop (80).reduced (8));
}
