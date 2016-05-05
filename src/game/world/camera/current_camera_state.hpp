#pragma once

#include "math/vector.hpp"
#include "content/id.hpp"
#include "libold/content/assets/model.hpp"

namespace gorc {
namespace game {
namespace world {
namespace camera {

class current_camera_state {
public:
    vector<3> position;
    vector<3> look;
    vector<3> up;
    sector_id containing_sector;
    thing_id focus_not_drawn_thing;
    maybe<asset_ref<content::assets::model>> pov_model;
    vector<3> pov_model_offset = make_zero_vector<3, float>();
};

}
}
}
}
