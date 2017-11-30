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

#define BENCHMARK_ENABLED   0

//==============================================================================
DspworkshopAdc2017AudioProcessor::DspworkshopAdc2017AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
#if BENCHMARK_ENABLED
    using Type = float;

    static constexpr size_t numChannels = 1;
    static constexpr size_t samplesPerBlock = 1024;
    static constexpr size_t numBlocks = 10000;

    struct BenchmarkData
    {
        BenchmarkData()
        {
            juce::dsp::ProcessSpec spec { 44.1e3f, samplesPerBlock, numChannels };
            osc.prepare (spec);
            osc.initialise ([] (Type x) { return std::sin (x); }, 128);
            osc.setFrequency (440.0f);
        }

        void process (size_t index)
        {
            juce::dsp::ProcessContextReplacing<Type> context (block);

            double t;
            {
                ScopedTimeMeasurement m (t);
                index == 0 ? osc.process (context)
                           : osc.process2 (context);
            }
            timeAcc += t;
        }

        HeapBlock<char> heapBlock;
        juce::dsp::AudioBlock<Type> block  { heapBlock, numChannels, samplesPerBlock };
        juce::dsp::Oscillator<Type> osc;
        double timeAcc { 0.0 };
    } benchmarkData[2];

    for (size_t i = 0; i < numBlocks; ++i)
    {
        for (size_t j = 0; j < 2; ++j)
            benchmarkData[j].process (j);

        for (size_t j = 0; j < samplesPerBlock; ++j)
        {
            const auto x = benchmarkData[0].block.getChannelPointer(0)[j];
            const auto y = benchmarkData[1].block.getChannelPointer(0)[j];

            if (x != y)
                Logger::writeToLog ("BENCHMARK DATA ERROR");
        }
    }

    Logger::writeToLog (String ("BENCHMARK T1 / T2 = ") + String (benchmarkData[0].timeAcc / benchmarkData[1].timeAcc));
#endif
}

DspworkshopAdc2017AudioProcessor::~DspworkshopAdc2017AudioProcessor()
{
}

//==============================================================================
const String DspworkshopAdc2017AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DspworkshopAdc2017AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DspworkshopAdc2017AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DspworkshopAdc2017AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DspworkshopAdc2017AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DspworkshopAdc2017AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DspworkshopAdc2017AudioProcessor::getCurrentProgram()
{
    return 0;
}

void DspworkshopAdc2017AudioProcessor::setCurrentProgram (int /*index*/)
{
}

const String DspworkshopAdc2017AudioProcessor::getProgramName (int /*index*/)
{
    return {};
}

void DspworkshopAdc2017AudioProcessor::changeProgramName (int /*index*/, const String& /*newName*/)
{
}

//==============================================================================
void DspworkshopAdc2017AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    audioEngine.prepare ({ sampleRate, (uint32) samplesPerBlock, 2 });
    midiMessageCollector.reset (sampleRate);
}

void DspworkshopAdc2017AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DspworkshopAdc2017AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DspworkshopAdc2017AudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    midiMessageCollector.removeNextBlockOfMessages (midiMessages, buffer.getNumSamples());

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    audioEngine.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());
    scopeDataCollector.process (buffer.getReadPointer (0), (size_t) buffer.getNumSamples());
}

//==============================================================================
bool DspworkshopAdc2017AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DspworkshopAdc2017AudioProcessor::createEditor()
{
    return new DspworkshopAdc2017AudioProcessorEditor (*this);
}

//==============================================================================
void DspworkshopAdc2017AudioProcessor::getStateInformation (MemoryBlock& /*destData*/)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DspworkshopAdc2017AudioProcessor::setStateInformation (const void* /*data*/, int /*sizeInBytes*/)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DspworkshopAdc2017AudioProcessor();
}
