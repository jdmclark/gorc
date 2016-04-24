#pragma once

#include "content/id.hpp"

namespace gorc {
namespace game {
namespace world {
namespace animations {
namespace components {

class surface_animation {
public:
    #include "surface_animation.uid"

    surface_id surface;

    surface_animation(surface_id surface);
};

}
}
}
}
}
