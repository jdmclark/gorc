#pragma once

#include "base/utility/entity_id.h"

namespace gorc {
namespace game {
namespace world {
namespace events {

class touched_surface {
public:
    #include "touched_surface.uid"

    entity_id toucher;
    int touched;

    touched_surface(entity_id toucher, int touched);
};

}
}
}
}
