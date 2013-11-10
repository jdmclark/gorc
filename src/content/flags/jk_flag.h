#pragma once

namespace gorc {
namespace flags {

enum class JkFlag {
	HasSaber						= 0x1,
	SaberCollides					= 0x2,
	SaberIgniting					= 0x4,
	SaberShrinking					= 0x8,
	HasTwoSabers					= 0x10,
	PersuasionTwinkling				= 0x20,
	AttackingInSameKeyframe			= 0x40
};

}
}
