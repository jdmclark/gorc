#pragma once

#include "content/flags/adjoinflag.h"

namespace Gorc {
namespace Content {
namespace Assets {

class LevelAdjoin {
public:
	FlagSet<Flags::AdjoinFlag> Flags;
	size_t Mirror;
	float Distance;
};

}
}
}
