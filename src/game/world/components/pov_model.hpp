#pragma once

#include "libold/content/assets/model.hpp"

namespace gorc {
namespace game {
namespace world {
namespace components {

class pov_model {
public:
    asset_ref<content::assets::model> model;

    pov_model(asset_ref<content::assets::model> model);
};

}
}
}
}
