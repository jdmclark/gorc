#pragma once

#include "scriptcontinuation.h"
#include "scripttimerstate.h"
#include "scripttimer.h"
#include "cog/instance.h"
#include "framework/pool.h"
#include <memory>
#include <vector>
#include <tuple>

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Scripts {

class ScriptModel {
public:
	std::vector<std::tuple<std::unique_ptr<Cog::Instance>, ScriptTimerState>> Cogs;
	Pool<std::tuple<double, ScriptContinuation>, 8> SleepingCogs;
	std::stack<ScriptContinuation> RunningCogState;
	Pool<ScriptTimer> Timers;
};

}
}
}
}
}
