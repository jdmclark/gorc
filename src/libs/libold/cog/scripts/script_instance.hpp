#pragma once

#include "script_timer_state.hpp"
#include "libold/cog/instance.hpp"
#include <tuple>
#include <memory>

namespace gorc {
namespace cog {
namespace scripts {

using script_instance = std::tuple<std::unique_ptr<cog::instance>, script_timer_state>;

}
}
}
