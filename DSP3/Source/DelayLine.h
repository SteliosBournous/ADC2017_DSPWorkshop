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
class DelayLine
{
public:
    void clear() noexcept           { std::fill (rawData.begin(), rawData.end(), Type (0)); }
    size_t size() const noexcept    { return rawData.size(); }
    void resize (size_t newValue)   { rawData.resize (newValue); leastRecentIndex = 0; }
    Type back() const noexcept      { return rawData[leastRecentIndex]; }

    Type get (size_t delayInSamples) const noexcept
    {
        jassert (delayInSamples >= 0 && delayInSamples < size());
        ignoreUnused (delayInSamples);

        // <- 11.3. calculate the index of the delayed sample (for delayInSamples == 0 this should be the index of the most recent sample)
        // <- 11.4. return the delayed sample
        return Type (0);
    }

    /** Set the specified sample in the delay line */
    void set (size_t delayInSamples, Type newValue) noexcept
    {
        jassert (delayInSamples >= 0 && delayInSamples < size());
        ignoreUnused (delayInSamples);
        ignoreUnused (newValue);

        // <- 11.5. calculate the index of the sample at the specified position
        // <- 11.6. overwrite the specified sample in the delay line with newValue
    }

    /** Adds a new value to the delay line, overwriting the least recently added sample */
    void push (Type valueToAdd) noexcept
    {
        ignoreUnused (valueToAdd);

        // <- 11.1. overwrite the least recent element with the new value
        // <- 11.2. update leastRecentIndex
    }

private:
    std::vector<Type> rawData;
    size_t leastRecentIndex = 0;
};
