#pragma once

namespace gorc {
inline namespace utility {

template <typename T> class enum_hasher {
    using UT = typename std::underlying_type<T>::type;

    std::hash<UT> hasher;

public:
    size_t operator()(T value) const {
        return hasher(static_cast<UT>(value));
    }
};

}
}
