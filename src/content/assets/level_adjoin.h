#pragma once

#include "flags.h"

namespace Gorc {
namespace Content {
namespace Assets {

class LevelAdjoin {
public:
	FlagSet<SurfaceAdjoinFlag> Flags;
	size_t Mirror;
	float Distance;
};

}
}
}
