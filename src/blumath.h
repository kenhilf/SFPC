#ifndef _blumath_h_
#define _blumath_h_

#include <assert.h>

template <typename T>
T GetSign(T val)
{
    if (val >= static_cast<T>(0))
    {
        return static_cast<T>(1);
    }
    else
    {
        return static_cast<T>(-1);
    }
}

// Similar to fmodf, but handles negative values correctly
template <typename T>
T FMod(T value, T min, T max)
{
    T const period = max - min;
    assert(period != 0);
    while (value < min) value += period;
    while (value > max) value -= period;
    return value;
}

template <typename T>
T Clamp(T val, T min, T max)
{
    if (val > max)
        val = max;

    if (val < min)
        val = min;

    return val;
}

template <typename T>
T Min(T a, T b) { return a < b? a : b; }

template <typename T>
T Max(T a, T b) { return a > b? a : b; }

#endif //_blumath_h_