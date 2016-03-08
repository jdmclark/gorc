#pragma once

#include "libold/base/utility/entity_id.hpp"

namespace gorc {
namespace game {
namespace world {
namespace animations {
namespace components {

class surface_animation {
public:
    #include "surface_animation.uid"

    int surface;

    surface_animation(int surface);
};

}
}
}
}
}
