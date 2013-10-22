#pragma once

namespace gorc {
namespace flags {

enum class ParticleFlag {
	OutwardExpanding				= 0x1,
	AnimateCel						= 0x2,
	RandomStartCel					= 0x4,
	FadeOutOverTime					= 0x8,
	EmitLight						= 0x10,
	Flipped							= 0x20
};

}
}
