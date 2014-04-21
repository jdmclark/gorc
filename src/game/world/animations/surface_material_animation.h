#pragma once

#include "animation.h"
#include "framework/utility/pool.h"
#include "content/flags/anim_flag.h"
#include "framework/utility/flag_set.h"

namespace gorc {
namespace game {
namespace world {

class level_model;

namespace animations {

class surface_material_animation : public animation {
private:
    level_model& model;
    unsigned int surface;
    double framerate;
    flag_set<flags::AnimFlag> flag;
    double framerate_accumulator = 0.0;
    size_t num_cels;

public:
    surface_material_animation(level_model& model, unsigned int surface, double framerate, flag_set<flags::AnimFlag> flag, int anim_num);

    void update(double dt);
    void stop();
};

}
}
}
}
