#pragma once

#include "sound.h"
#include "flags.h"

namespace Gorc {
namespace Content {
namespace Assets {

class SoundSubclass {
public:
	Sound const* sound = nullptr;
	FlagSet<SoundFlag> flags;
	float min_radius = 1.0f;
	float max_radius = 4.0f;
	float max_volume = 1.0f;
};

}
}
}
