#pragma once

#include "math/vector.hpp"

namespace gorc {
namespace game {
namespace world {
namespace camera {

class current_camera_state {
public:
    vector<3> position;
    vector<3> look;
    vector<3> up;
    int containing_sector;
    int focus_not_drawn_thing;
    bool draw_pov_model;
    vector<3> pov_model_offset = make_zero_vector<3, float>();
};

}
}
}
}
