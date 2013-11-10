#pragma once

namespace gorc {
namespace flags {

enum class AiModeFlag {
	MovingTowardDestination			= 0x1,
	AttackingTarget					= 0x2,
	SearchingForTarget				= 0x4,
	TurningToFaceTarget				= 0x8,
	HasMoveTarget					= 0x10,
	HasFireTarget					= 0x20,
	Ready							= 0x200,
	HasLineOfSight					= 0x400,
	Fleeing							= 0x800,
	Passive							= 0x1000,
	Disabled						= 0x2000
};

}
}
