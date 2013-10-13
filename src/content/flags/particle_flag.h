#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class ParticleFlag : uint32_t {
	OutwardExpanding				= 0x1,
	AnimateCel						= 0x2,
	RandomStartCel					= 0x4,
	FadeOutOverTime					= 0x8,
	EmitLight						= 0x10,
	Flipped							= 0x20
};

}
}
