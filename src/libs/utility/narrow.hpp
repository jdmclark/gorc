#pragma once

#include <stdexcept>
#include <type_traits>
#include <utility>

namespace gorc {
    template <class T, class U>
    inline constexpr T narrow_cast(U &&u)
    {
        return static_cast<T>(std::forward<U>(u));
    }

    template <class T, class U>
    inline constexpr T narrow(U u)
    {
        auto t = narrow_cast<T>(u);
        if(static_cast<U>(t) != u) {
            throw std::runtime_error("Bad narrow cast");
        }

        return t;
    }
}
