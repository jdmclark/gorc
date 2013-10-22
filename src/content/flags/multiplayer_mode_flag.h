#pragma once

namespace gorc {
namespace flags {

enum class MultiplayerModeFlag {
	TeamPlay						= 0x1,
	NoFriendlyFire					= 0x2,
	CtfComputesOwnScore				= 0x4,
	TimeLimit						= 0x8,
	ScoreLimit						= 0x10,
	NoCustomSkins					= 0x20,
	AutoAssignTeams					= 0x100
};

}
}
