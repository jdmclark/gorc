#pragma once

#include "libold/cog/vm/value.hpp"
#include "script_instance.hpp"

namespace gorc {
namespace cog {
namespace scripts {

class script_timer {
public:
    double delay;
    int instance_id;
    cog::vm::value id, param0, param1;
};

}
}
}
