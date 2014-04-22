#pragma once

#include "animation.h"
#include "base/utility/pool.h"
#include <vector>
#include <memory>

namespace gorc {
namespace game {
namespace world {
namespace animations {

class animation_model {
public:
    pool<pool_ptr<animation>> animations;
};

}
}
}
}
