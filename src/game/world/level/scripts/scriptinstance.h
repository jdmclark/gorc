#pragma once

#include "scripttimerstate.h"
#include "cog/instance.h"
#include <tuple>
#include <memory>

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Scripts {

using ScriptInstance = std::tuple<std::unique_ptr<Cog::Instance>, ScriptTimerState>;

}
}
}
}
}
