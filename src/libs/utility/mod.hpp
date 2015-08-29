#pragma once

namespace gorc {
    template <typename T>
    inline constexpr T normalize_remainder(T const &rem, T const &mod)
    {
        return (rem >= T(0)) ? rem : (rem + mod);
    }

    template <typename T>
    inline constexpr T mod(T const &lhs, T const &mod)
    {
        return normalize_remainder(lhs % mod, mod);
    }
}
