#pragma once

#include "libold/content/flags/anim_flag.hpp"
#include "utility/flag_set.hpp"
#include "math/vector.hpp"
#include "libold/base/utility/pool.hpp"
#include "libold/base/utility/time.hpp"
#include "jk/cog/script/verb_table.hpp"
#include "content/id.hpp"

namespace gorc {
namespace game {

class level_state;

namespace world {

class level_model;

namespace animations {

class animation_presenter {
private:
    level_model* levelModel;

public:
    void start(level_model& levelModel);

    thing_id surface_anim(surface_id surface, float rate, flag_set<flags::anim_flag> flags);
    thing_id get_surface_anim(surface_id surface);
    void stop_surface_anim(surface_id surface);

    void stop_anim(thing_id anim);

    int get_surface_cel(surface_id surface);
    void set_surface_cel(surface_id surface, int cel);

    thing_id slide_ceiling_sky(float u_speed, float v_speed);
    thing_id slide_surface(surface_id surface_id, const vector<3>& direction);

    thing_id surface_light_anim(surface_id surface_id, float start_light, float end_light, float change_time);

    static void register_verbs(cog::verb_table&, level_state&);
};

}
}
}
}
