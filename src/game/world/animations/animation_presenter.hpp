#pragma once

#include "libold/content/flags/anim_flag.hpp"
#include "utility/flag_set.hpp"
#include "math/vector.hpp"
#include "libold/base/utility/pool.hpp"
#include "libold/base/utility/time.hpp"
#include "cog/script/verb_table.hpp"
#include "libold/base/utility/entity_id.hpp"

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

    int surface_anim(int surface, float rate, flag_set<flags::anim_flag> flags);
    int get_surface_anim(int surface);
    void stop_surface_anim(int surface);

    void stop_anim(int anim);

    int get_surface_cel(int surface);
    void set_surface_cel(int surface, int cel);

    int slide_ceiling_sky(float u_speed, float v_speed);
    int slide_surface(int surface_id, const vector<3>& direction);

    int surface_light_anim(int surface_id, float start_light, float end_light, float change_time);

    static void register_verbs(cog::verb_table&, level_state&);
};

}
}
}
}
