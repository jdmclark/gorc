#pragma once

#include "abstract_vector.hpp"

namespace gorc {

    struct direction_vector_tag { };

    template <size_t n, typename F = float>
    using vector = abstract_vector<n, F, direction_vector_tag>;

    template <typename ...F>
    constexpr auto make_vector(F ...v)
    {
        return make_abstract_vector<direction_vector_tag>(v...);
    }

    template <size_t n, typename F>
    constexpr auto make_fill_vector(F c)
    {
        return make_fill_abstract_vector<n, F, direction_vector_tag>(c);
    }

    template <size_t n, typename F = float>
    constexpr auto make_zero_vector()
    {
        return make_fill_abstract_vector<n, F, direction_vector_tag>(F(0));
    }

    // Operators
    direction_vector_tag operator+(direction_vector_tag, direction_vector_tag);
    direction_vector_tag operator-(direction_vector_tag, direction_vector_tag);

}
