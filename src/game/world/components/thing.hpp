#pragma once

#include "libold/content/assets/template.hpp"
#include "game/world/physics/object_data.hpp"
#include "utility/maybe.hpp"
#include "libold/content/flags/jk_flag.hpp"
#include "libold/content/flags/ai_mode_flag.hpp"
#include <memory>

namespace gorc {
namespace game {
namespace world {

namespace sounds {
class sound;
}

namespace keys {
class key_mix;
class key_state;
}

namespace components {

class thing : public content::assets::thing_template {
public:
    #include "thing.uid"

    physics::thing_object_data object_data;

    bool path_moving_paused = false;
    bool rotatepivot_moving = false;
    bool path_moving = false;
    bool rotatepivot_longway = false;
    int current_frame = 0;
    int next_frame = 0;
    int goal_frame = 0;
    float path_move_time = 0.0f;
    float path_move_speed = 0.0f;
    bool is_blocked = false;

    vector<3> ai_move_target;
    vector<3> ai_look_target;
    float ai_move_speed = 1.0;

    maybe<surface_id> attached_surface;
    maybe<thing_id> attached_thing;
    vector<3> prev_attached_thing_position;
    vector<3> attached_thing_velocity;

    int attached_key_mix = -1;

    maybe<cog_id> capture_cog;
    float time_alive = 0.0f;

    maybe<thing_id> parent_thing;

    maybe<asset_ref<content::assets::model>> weapon_mesh;

    flag_set<flags::jk_flag> jk_flags;
    flag_set<flags::ai_mode_flag> ai_mode_flags;

    maybe<asset_ref<material>> saber_side_mat;
    maybe<asset_ref<material>> saber_tip_mat;
    float saber_base_rad = 0.003f;
    float saber_tip_rad = 0.001f;
    float saber_length = 0.1f;
    float saber_drawn_length = 0.0f;
    maybe<content::assets::thing_template const *> saber_wall;
    maybe<content::assets::thing_template const *> saber_blood;
    maybe<content::assets::thing_template const *> saber_saber;
    bool saber_enabled = false;
    float saber_damage = 0.0f;
    float saber_collide_length = 0.0f;
    float saber_unknown = 0.0f;

    thing() = default;
    thing(const content::assets::thing_template& tpl);
};

}

}
}
}
