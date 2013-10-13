#pragma once

#include "cog/vm/value.h"
#include "script_instance.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Scripts {

class ScriptTimer {
public:
	double Delay;
	int InstanceId;
	Cog::VM::Value Id, Param0, Param1;
};

}
}
}
}
}
