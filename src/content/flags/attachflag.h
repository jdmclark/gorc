#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class AttachFlag : uint32_t {
	AttachedToWorldSurface			= 0x1,
	AttachedToThingFace				= 0x2,
	AttachedToThing					= 0x4,
	NoMoveRelativeToAttachedThing	= 0x8
};


}
}
