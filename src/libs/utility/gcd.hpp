#pragma once

#include "mod.hpp"
#include <type_traits>

namespace gorc {
    template <typename T>
    inline constexpr T gcd(T const &a)
    {
        return a;
    }

    template <typename T>
    inline constexpr T gcd(T const &a, T const &b)
    {
        return (b == T(0)) ? a
                           : gcd(b, mod(a, b));
    }

    template <typename T, typename ...TailT>
    inline constexpr T gcd(T const &t0, T const &t1, T const &t2, TailT const &...tail)
    {
        return gcd(t0, gcd(t1, t2, tail...));
    }
}
