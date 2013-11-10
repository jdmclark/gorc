#pragma once

#include "cog/vm/value.h"
#include "script_instance.h"

namespace gorc {
namespace game {
namespace world {
namespace level {
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
}
}
