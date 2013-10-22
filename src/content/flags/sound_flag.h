#pragma once

namespace gorc {
namespace flags {

enum class sound_flag {
	Loops = 0x1,
	Voice = 0x2,
	Ambient = 0x4,
	MinimalVolume = 0x10,
	Delay = 0x20,
	ClassOriginDoesNotMove = 0x40,
	ThingOriginMovesWithThing = 0x80,
	priority = 0x100,
	Immediate = 0x200,
	IgnoreIfAlreadyPlaying = 0x400,
	IgnoreIfSoundclassAlreadyPlaying = 0x800,
	VolumeQuickFalloff = 0x2000,
	NotSimultaneous = 0x10000
};

}
}
