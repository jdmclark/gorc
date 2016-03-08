#pragma once

#include "script_instance.hpp"
#include "script_continuation.hpp"
#include "script_timer.hpp"
#include "libold/base/utility/pool.hpp"
#include <memory>
#include <vector>
#include <tuple>
#include <unordered_map>

namespace gorc {
namespace cog {
namespace scripts {

class script_model {
public:
    std::vector<script_instance> cogs;
    std::unordered_map<cog::script const*, int> global_script_instances;
    pool<std::tuple<double, script_continuation>, 8> sleeping_cogs;
    std::stack<script_continuation> running_cog_state;
    pool<script_timer> timers;
};

}
}
}
