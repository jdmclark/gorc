#pragma once

#include <cstdint>

namespace gorc {
namespace flags {

enum class thing_flag : uint32_t {
	EmitsLight						= 0x1,
	Dead							= 0x2,
	MagSealed						= 0x4,
	PartOfWorldGeometry				= 0x8,
	Invisible						= 0x10,
	CanStandOn						= 0x40,
	Sighted							= 0x200,	// NOTE: Unknown value.
	CogLinked						= 0x400,
	NoCrush							= 0x800,
	NotInEasy						= 0x1000,
	NotInMedium						= 0x2000,
	NotInHard						= 0x4000,
	NotInMultiplayer				= 0x8000,
	NotInSingleplayer				= 0x10000,
	ObjectSendingPulse				= 0x20000,
	ObjectSendingTimer				= 0x40000,
	Metal							= 0x400000,
	Dirt							= 0x800000,
	NoSounds						= 0x1000000,
	Underwater						= 0x2000000,
	DestroyedIfEntersWater			= 0x8000000,
	DestroyedIfEntersAir			= 0x10000000,
	GeneratesSplash					= 0x20000000
};

}
}
