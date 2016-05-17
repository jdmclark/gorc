#pragma once

#include "content/id.hpp"
#include "libold/content/flags/anim_flag.hpp"
#include "utility/flag_set.hpp"
#include "utility/uid.hpp"

namespace gorc {
namespace game {
namespace world {

class level_model;

namespace animations {
namespace components {

class surface_material {
public:
    uid(3174651231);

    surface_id surface;
    double framerate;
    flag_set<flags::anim_flag> flag;
    double framerate_accumulator = 0.0;

    surface_material(surface_id surface, double framerate, flag_set<flags::anim_flag> flag);
};

}
}
}
}
}
