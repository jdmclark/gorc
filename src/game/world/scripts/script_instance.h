#pragma once

#include "script_timer_state.h"
#include "cog/instance.h"
#include <tuple>
#include <memory>

namespace gorc {
namespace game {
namespace world {
namespace scripts {

using script_instance = std::tuple<std::unique_ptr<cog::instance>, script_timer_state>;

}
}
}
}
