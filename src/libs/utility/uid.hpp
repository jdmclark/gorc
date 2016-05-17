#pragma once

#include <cstdint>

#define uid(x) enum class UID : uint32_t { UID = x##UL }

namespace gorc {
    template <typename T>
    uint32_t uid_of()
    {
        return static_cast<uint32_t>(T::UID::UID);
    }

    template <typename T>
    uint32_t uid_of(T const &)
    {
        return static_cast<uint32_t>(T::UID::UID);
    }
}
