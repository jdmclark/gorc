#pragma once

#include "base/math/vector.h"
#include "base/utility/flag_set.h"
#include "game/flags/camera_state_flag.h"

namespace gorc {
namespace game {
namespace world {
namespace camera {

class camera_state {
public:
    int focus;
    bool draw_focus;
    bool draw_pov_model;

    vector<3> base_offset;

    vector<3> pos_offset;
    float pos_reset_speed;

    vector<3> angle_offset;
    float angle_reset_speed;
};

}
}
}
}
