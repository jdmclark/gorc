#pragma once

#include <cstdint>

namespace gorc {
namespace flags {

enum class physics_flag : uint32_t {
	HasGravity						= 0x1,
	UsesThrustToMove				= 0x2,
	AlignOrientationWithSurface		= 0x10,
	BounceOffSurface				= 0x20,
	SticksToFloor					= 0x40,
	SticksToWall					= 0x80,
	UsesRotationalVelocity			= 0x200,
	BanksWhenTurning				= 0x400,
	UsesAngularThrustToRotate		= 0x1000,
	CanFly							= 0x2000,
	AffectedByBlastForce			= 0x4000,
	IsCrouching						= 0x10000,
	DoNotRotateVelocity				= 0x20000,
	PartialGravity					= 0x40000,
	TreadingWater					= 0x100000,
	NotAffectedByThrust				= 0x400000
};

}
}
