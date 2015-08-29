#pragma once

namespace gorc {

    template <typename T>
    inline constexpr T abs(T const &value)
    {
        return (value >= T(0)) ? value : -value;
    }
}
