#pragma once

#include "cog/vm/value.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Scripts {

class ScriptTimer {
public:
	double Delay;
	unsigned int InstanceId;
	Cog::VM::Value Id, Param0, Param1;
};

}
}
}
}
}
