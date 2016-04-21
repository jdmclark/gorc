#pragma once

#include "math/vector.hpp"
#include "math/box.hpp"
#include "libold/base/content/assets/sound.hpp"
#include "libold/content/flags/sector_flag.hpp"
#include "utility/maybe.hpp"
#include "jk/content/colormap.hpp"

namespace gorc {
namespace content {
namespace assets {

class level_sector {
public:
    sector_id number;
    flag_set<flags::sector_flag> flags;
    float ambient_light;
    float extra_light;
    int colormap_id;
    maybe<asset_ref<colormap>> cmp;
    color_rgb tint;
    box<3> bounding_box;
    box<3> collide_box;
    maybe<asset_ref<sound>> ambient_sound;
    float ambient_sound_volume = 0.0f;
    vector<3> center;
    float radius;
    std::vector<size_t> vertices;
    int first_surface;
    int surface_count;
    vector<3> thrust;
};

}
}
}
