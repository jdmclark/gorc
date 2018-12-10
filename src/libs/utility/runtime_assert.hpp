#pragma once

#include <stdexcept>

namespace gorc {
    template <class ExceptT>
    inline void throw_std_exception_assert(bool e, char const *msg)
    {
        if(!e) {
            throw ExceptT(msg);
        }
    }

    inline void runtime_assert(bool e, char const *msg)
    {
        throw_std_exception_assert<std::runtime_error>(e, msg);
    }

    inline void logic_assert(bool e, char const *msg)
    {
        throw_std_exception_assert<std::logic_error>(e, msg);
    }
}
