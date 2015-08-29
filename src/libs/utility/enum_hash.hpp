#pragma once

#include <type_traits>
#include <functional>
#include <tuple>

namespace gorc {
    template <typename T>
    class enum_hash {
        using UT = typename std::underlying_type<T>::type;

        std::hash<UT> hasher;

    public:
        auto operator()(T value) const
            -> decltype(hasher(static_cast<UT>(value)))
        {
            return hasher(static_cast<UT>(value));
        }
    };
}
