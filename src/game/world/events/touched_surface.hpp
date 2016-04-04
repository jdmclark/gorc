#pragma once

#include "libold/base/utility/entity_id.hpp"

namespace gorc {
namespace game {
namespace world {
namespace events {

class touched_surface {
public:
    #include "touched_surface.uid"

    int toucher;
    int touched;

    touched_surface(int toucher, int touched);
};

}
}
}
}
