#pragma once

#include "abstract_vector.hpp"

namespace gorc {

    struct size_vector_tag { };

    template <size_t n, typename F = float>
    using size = abstract_vector<n, F, size_vector_tag>;

    template <typename ...F>
    constexpr auto make_size(F ...v)
    {
        return make_abstract_vector<size_vector_tag>(v...);
    }

}
