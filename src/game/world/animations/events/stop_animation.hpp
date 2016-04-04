#pragma once

#include "libold/base/utility/entity_id.hpp"

namespace gorc {
namespace game {
namespace world {
namespace animations {
namespace events {

class stop_animation {
public:
    #include "stop_animation.uid"

    int animation;

    stop_animation(int animation);
};

}
}
}
}
}
