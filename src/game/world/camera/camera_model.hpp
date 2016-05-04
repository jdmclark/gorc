#pragma once

#include "camera_state.hpp"
#include "current_camera_state.hpp"
#include "game/flags/camera_state_flag.hpp"
#include "utility/flag_set.hpp"
#include "libold/content/assets/model.hpp"
#include <vector>

namespace gorc {
namespace game {
namespace world {
namespace camera {

class camera_model {
public:
    flag_set<flags::camera_state_flag> state_flags;

    std::vector<camera_state> cameras = {
            camera_state(), camera_state()
    };

    int current_camera = 0;
    current_camera_state current_computed_state;

    maybe<asset_ref<content::assets::model>> pov_model;
    thing_id pov_key_mix_id;

    vector<3> waggle = make_zero_vector<3, float>();
    float waggle_speed = 0.0f;
    float waggle_time = 0.0;
};

}
}
}
}
