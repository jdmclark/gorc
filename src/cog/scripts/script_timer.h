#pragma once

#include "cog/vm/value.h"
#include "script_instance.h"

namespace gorc {
namespace cog {
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
