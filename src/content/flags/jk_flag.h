#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class JkFlag : uint32_t {
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
