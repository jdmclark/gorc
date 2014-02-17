#pragma once

#include "cog/scripts/script_model.h"

namespace gorc {
namespace game {
namespace world {
namespace scripts {

class script_model : public gorc::cog::scripts::script_model {
public:
    pool<std::tuple<int, gorc::cog::scripts::script_continuation>, 8> wait_for_stop_cogs;
};

}
}
}
}
