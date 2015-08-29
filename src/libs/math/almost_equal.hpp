#pragma once

#include <limits>
#include <cmath>
#include <type_traits>

namespace gorc {

    template <typename F>
    bool almost_equal(typename std::enable_if<std::is_integral<F>::value, F>::type x, F y)
    {
        return x == y;
    }

    template <typename F>
    bool almost_equal(typename std::enable_if<!std::is_integral<F>::value, F>::type x,
                      F y,
                      F ulp = F(2) * std::numeric_limits<F>::round_error())
    {
        auto absxmy = std::abs(x - y);
        return (absxmy < std::numeric_limits<F>::epsilon() * std::abs(x + y) * ulp) ||
               (absxmy < std::numeric_limits<F>::min());
    }

}
