#pragma once

#include "math/color.hpp"
#include "math/grid.hpp"
#include "io/path.hpp"

namespace gorc {

    // Check alignment and padding requirements for image datatype
    static_assert(sizeof(color_rgba8) == 4, "pixel vector must be 4 bytes");
    static_assert(alignof(color_rgba8) == 1, "pixel vector must be unaligned");

    using image = grid<color_rgba8>;

    void save_image_to_file(image const &img, path const &);

}
