#pragma once

#include "gcd.hpp"
#include "abs.hpp"

namespace gorc {
    template <typename T>
    inline constexpr T lcm(T const &a)
    {
        return a;
    }

    template <typename T>
    inline constexpr T lcm(T const &a, T const &b)
    {
        return abs(a * b) / gcd(a, b);
    }

    template <typename T, typename ...TailT>
    inline constexpr T lcm(T const &t0, T const &t1, T const &t2, TailT const &...tail)
    {
        return lcm(t0, lcm(t1, t2, tail...));
    }
}
