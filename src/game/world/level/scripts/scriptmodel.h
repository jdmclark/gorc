#pragma once

#include "scriptinstance.h"
#include "scriptcontinuation.h"
#include "scripttimer.h"
#include "framework/pool.h"
#include <memory>
#include <vector>
#include <tuple>

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class Thing;

namespace Scripts {

class ScriptModel {
public:
	std::vector<ScriptInstance> Cogs;
	Pool<std::tuple<double, ScriptContinuation>, 8> SleepingCogs;
	Pool<std::tuple<Id<Thing>, ScriptContinuation>, 8> WaitForStopCogs;
	std::stack<ScriptContinuation> RunningCogState;
	Pool<ScriptTimer> Timers;
};

}
}
}
}
}
