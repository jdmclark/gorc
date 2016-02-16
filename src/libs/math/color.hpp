#pragma once

#include "vector.hpp"

namespace gorc {

    struct color_vector_tag { };

    using color = abstract_vector<4, float, color_vector_tag>;
    using color_rgb = abstract_vector<3, float, color_vector_tag>;
    using color_rgba8 = abstract_vector<4, uint8_t, color_vector_tag>;
    using color_rgb8 = abstract_vector<3, uint8_t, color_vector_tag>;

    constexpr color make_color(float r, float g, float b, float a)
    {
        return color(r, g, b, a);
    }

}
