#pragma once

#include "content/asset.hpp"
#include "libold/content/loaders/sprite_loader.hpp"
#include "libold/content/flags/geometry_mode.hpp"
#include "libold/content/flags/light_mode.hpp"
#include "libold/content/flags/texture_mode.hpp"
#include "math/vector.hpp"

#include "jk/content/material.hpp"

namespace gorc {
namespace content {
namespace assets {

class sprite : public asset {
public:
    static fourcc const type;

    asset_ref<material> mat;
    int sprite_type;
    float width;
    float height;
    flags::geometry_mode geometry_mode;
    flags::light_mode light_mode;
    flags::texture_mode texture_mode;
    float extra_light;
    vector<3> offset;

    sprite(asset_ref<material> mat,
           int sprite_type,
           float width,
           float height,
           flags::geometry_mode geometry_mode,
           flags::light_mode light_mode,
           flags::texture_mode texture_mode,
           float extra_light,
           vector<3> const &offset);
};

}
}
}
