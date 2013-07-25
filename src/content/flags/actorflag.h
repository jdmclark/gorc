#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class ActorFlag : uint32_t {
	PitchingChangesHeadPitch 		= 0x1,
	PlayersFieldlight				= 0x4,
	Invulnerability					= 0x8,
	PlayersHeadCentered				= 0x10,
	ExplodesWhenKilled				= 0x20,
	WillNotDrown					= 0x40,
	Invisibility					= 0x80,
	Droid							= 0x100,
	Boss							= 0x200,
	Deaf							= 0x400,
	Blind							= 0x800,
	BlockingAttacks					= 0x2000,
	Screaming						= 0x10000,
	PartiallyImmobilized			= 0x40000,
	CannotShootUnderwater			= 0x80000,
	CannotBeAutoTargeted			= 0x100000,
	TotallyImmobilized				= 0x200000,
	FallingToDeath					= 0x400000,
	NoDisplayHud					= 0x800000,
	DamagesAtFullRate				= 0x1000000,
	CanSeeInDark					= 0x2000000,
	CausesGreenCornerBrackets		= 0x80000000
};

}
}
