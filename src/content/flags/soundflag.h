#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class SoundFlag : uint32_t {
	Loops = 0x1,
	Voice = 0x2,
	Ambient = 0x4,
	MinimalVolume = 0x10,
	Delay = 0x20,
	ClassOriginDoesNotMove = 0x40,
	ThingOriginMovesWithThing = 0x80,
	Priority = 0x100,
	Immediate = 0x200,
	IgnoreIfAlreadyPlaying = 0x400,
	IgnoreIfSoundclassAlreadyPlaying = 0x800,
	VolumeQuickFalloff = 0x2000,
	NotSimultaneous = 0x10000
};

}
}
