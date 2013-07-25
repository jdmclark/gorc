#pragma once

#include "sound.h"
#include "content/flags/soundflag.h"
#include "content/flags/soundsubclasstype.h"
#include "framework/flagset.h"

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
