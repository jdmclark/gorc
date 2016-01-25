#pragma once

#include "abstract_vector.hpp"
#include "vector.hpp"

namespace gorc {

    struct position_vector_tag { };

    template <size_t n, typename F = float>
    using point = abstract_vector<n, F, position_vector_tag>;

    template <typename ...F>
    constexpr auto make_point(F ...v)
    {
        return make_abstract_vector<position_vector_tag>(v...);
    }

    template <size_t n, typename F = float>
    constexpr auto make_origin_point()
    {
        return make_fill_abstract_vector<n, F, position_vector_tag>(F(0));
    }

    // Operators
    position_vector_tag operator+(position_vector_tag, direction_vector_tag);
    direction_vector_tag operator-(position_vector_tag, position_vector_tag);

}
