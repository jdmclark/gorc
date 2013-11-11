#pragma once

#include "animation.h"

namespace gorc {
namespace content {
namespace assets {

class puppet_submode {
public:
	animation const* animation;
	flag_set<flags::key_flag> flags;
	int lo_priority;
	int hi_priority;
};

}
}
}
