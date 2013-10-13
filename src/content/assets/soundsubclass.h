#pragma once

#include "sound.h"
#include "content/flags/sound_flag.h"
#include "content/flags/sound_subclass_type.h"
#include "framework/flag_set.h"

namespace Gorc {
namespace Content {
namespace Assets {

class SoundSubclass {
public:
	int sound = -1;
	FlagSet<Flags::SoundFlag> flags;
	float min_radius = 1.0f;
	float max_radius = 4.0f;
	float max_volume = 1.0f;
};

}
}
}
