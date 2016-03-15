#pragma once

#include "animation.hpp"

namespace gorc {
namespace content {
namespace assets {

class puppet_submode {
public:
    asset_ref<animation> anim;
    flag_set<flags::key_flag> flags;
    int lo_priority;
    int hi_priority;

    puppet_submode(asset_ref<animation> anim,
                   flag_set<flags::key_flag> flags,
                   int lo_priority,
                   int hi_priority);
};

}
}
}
