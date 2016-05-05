#pragma once

#include "libold/content/assets/model.hpp"

namespace gorc {
namespace game {
namespace world {
namespace components {

class pov_model {
public:
    maybe<asset_ref<content::assets::model>> model = nothing;

    vector<3> waggle = make_zero_vector<3, float>();
    float waggle_speed = 0.0f;
    float waggle_time = 0.0f;
};

}
}
}
}
