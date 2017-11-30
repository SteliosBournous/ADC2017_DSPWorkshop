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
        return rawData[(leastRecentIndex + 1 + delayInSamples) % size()];
    }

    void set (size_t delayInSamples, Type newValue) noexcept
    {
        jassert (delayInSamples >= 0 && delayInSamples < size());
        rawData[(leastRecentIndex + 1 + delayInSamples) % size()] = newValue;
    }

    void push (Type valueToAdd) noexcept
    {
        rawData[leastRecentIndex] = valueToAdd;
        leastRecentIndex = leastRecentIndex == 0 ? size() - 1 : leastRecentIndex - 1;
    }

private:
    std::vector<Type> rawData;
    size_t leastRecentIndex = 0;
};
