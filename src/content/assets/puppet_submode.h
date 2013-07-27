#pragma once

#include "animation.h"

namespace Gorc {
namespace Content {
namespace Assets {

class PuppetSubmode {
public:
	Animation const* Animation;
	unsigned int Flags;
	unsigned int LoPriority;
	unsigned int HiPriority;
};

}
}
}
