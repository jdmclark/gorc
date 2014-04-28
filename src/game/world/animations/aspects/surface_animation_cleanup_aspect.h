#pragma once

#include "base/utility/component_system.h"
#include "game/world/animations/components/surface_animation.h"

namespace gorc {
namespace game {
namespace world {
namespace animations {
namespace aspects {

class surface_animation_cleanup_aspect : public aspect {
public:
    surface_animation_cleanup_aspect(component_system& cs);
};

}
}
}
}
}
