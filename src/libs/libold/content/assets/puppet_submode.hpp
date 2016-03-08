#pragma once

#include "animation.hpp"

namespace gorc {
namespace content {
namespace assets {

class puppet_submode {
public:
    animation const* anim;
    flag_set<flags::key_flag> flags;
    int lo_priority;
    int hi_priority;
};

}
}
}
