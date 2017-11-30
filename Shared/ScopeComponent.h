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

//==============================================================================
template <typename SampleType>
class ScopeComponent  : public juce::Component,
                        private Timer
{
public:
    using Queue = AudioBufferQueue<SampleType>;

    //==============================================================================
    ScopeComponent (Queue& queueToUse)
        : audioBufferQueue (queueToUse)
    {
        sampleData.fill (SampleType (0));
        setFramesPerSecond (30);
    }

    //==============================================================================
    void setFramesPerSecond (int framesPerSecond)
    {
        jassert (framesPerSecond > 0 && framesPerSecond < 1000);
        startTimerHz (framesPerSecond);
    }

    //==============================================================================
    void paint (Graphics& g) override
    {
        g.fillAll (juce::Colours::black);
        g.setColour (juce::Colours::white);

        Rectangle<int> area (getLocalBounds());
        const auto h = (SampleType) area.getHeight();
        const auto w = (SampleType) area.getWidth();

        // Oscilloscope
        const auto scopeRect = Rectangle<SampleType> { SampleType (0), SampleType (0), w, h / 2 };
        plot (sampleData.data(), sampleData.size(), g, scopeRect, SampleType (1), h / 4);

        // Spectrum
        const auto spectrumRect = Rectangle<SampleType> { SampleType (0), h / 2, w, h / 2 };
        plot (spectrumData.data(), spectrumData.size() / 4, g, spectrumRect);
    }

    //==============================================================================
    void resized() override
    {
    }

private:
    //==============================================================================
    Queue& audioBufferQueue;
    std::array<SampleType, Queue::bufferSize> sampleData;

    juce::dsp::FFT fft { Queue::order };
    using WindowFun = juce::dsp::WindowingFunction<SampleType>;
    WindowFun windowFun { (size_t) fft.getSize(), WindowFun::hann };
    std::array<SampleType, 2 * Queue::bufferSize> spectrumData;

    //==============================================================================
    void timerCallback() override
    {
        audioBufferQueue.pop (sampleData.data());
        FloatVectorOperations::copy (spectrumData.data(), sampleData.data(), (int) sampleData.size());

        const auto fftSize = (size_t) fft.getSize();

        jassert (spectrumData.size() == 2 * fftSize);
        windowFun.multiplyWithWindowingTable (spectrumData.data(), fftSize);
        fft.performFrequencyOnlyForwardTransform (spectrumData.data());

        static constexpr auto mindB = SampleType (-160);
        static constexpr auto maxdB = SampleType (0);

        for (auto& s : spectrumData)
            s = jmap (jlimit (mindB, maxdB, juce::Decibels::gainToDecibels (s) - juce::Decibels::gainToDecibels (SampleType (fftSize))), mindB, maxdB, SampleType (0), SampleType (1));

        repaint();
    }

    //==============================================================================
    static void plot (const SampleType* data,
                      size_t numSamples,
                      Graphics& g,
                      juce::Rectangle<SampleType> rect,
                      SampleType scaler = SampleType (1),
                      SampleType offset = SampleType (0))
    {
        const auto w = rect.getWidth();
        const auto h = rect.getHeight();
        const auto right = rect.getRight();

        const auto center = rect.getBottom() - offset;
        const auto gain = h * scaler;

        for (size_t i = 1; i < numSamples; ++i)
            g.drawLine ({ jmap (SampleType (i - 1), SampleType (0), SampleType (numSamples - 1), SampleType (right - w), SampleType (right)),
                          center - gain * data[i - 1],
                          jmap (SampleType (i), SampleType (0), SampleType (numSamples - 1), SampleType (right - w), SampleType (right)),
                          center - gain * data[i] });
    }
};
