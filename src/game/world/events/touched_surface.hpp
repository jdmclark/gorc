#pragma once

#include "libold/base/utility/entity_id.hpp"

namespace gorc {
namespace game {
namespace world {
namespace events {

class touched_surface {
public:
    #include "touched_surface.uid"

    thing_id toucher;
    surface_id touched;

    touched_surface(thing_id toucher, surface_id touched);
};

}
}
}
}
