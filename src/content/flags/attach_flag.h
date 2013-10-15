#pragma once

#include <cstdint>

namespace gorc {
namespace flags {

enum class attach_flag : uint32_t {
	AttachedToWorldSurface			= 0x1,
	AttachedToThingFace				= 0x2,
	AttachedToThing					= 0x4,
	NoMoveRelativeToAttachedThing	= 0x8
};


}
}
