#pragma once

#include "content/assets/template.h"
#include "gameplay/thing_controller.h"
#include "physics/object_data.h"
#include "framework/utility/maybe.h"
#include "content/flags/armed_mode.h"
#include "content/flags/jk_flag.h"
#include "content/flags/ai_mode_flag.h"
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

class thing : public content::assets::thing_template {
public:
    gameplay::thing_controller* controller;
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

    maybe<sounds::sound*> current_foley_loop_channel;

    int attached_surface = -1;
    int attached_thing = -1;
    vector<3> prev_attached_thing_position;

    int attached_key_mix = -1;
    int actor_walk_animation = -1;

    int capture_cog = -1;
    float time_alive = 0.0f;

    int parent_thing = -1;

    content::assets::model const* weapon_mesh = nullptr;

    flags::armed_mode armed_mode = flags::armed_mode::unarmed;
    flags::puppet_mode_type puppet_mode = flags::puppet_mode_type::unarmed;

    flag_set<flags::jk_flag> jk_flags;
    flag_set<flags::ai_mode_flag> ai_mode_flags;

    content::assets::material const* saber_side_mat = nullptr;
    content::assets::material const* saber_tip_mat = nullptr;
    float saber_base_rad = 0.003f;
    float saber_tip_rad = 0.001f;
    float saber_length = 0.1f;
    float saber_drawn_length = 0.0f;
    content::assets::thing_template const* saber_wall = nullptr;
    content::assets::thing_template const* saber_blood = nullptr;
    content::assets::thing_template const* saber_saber = nullptr;
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
