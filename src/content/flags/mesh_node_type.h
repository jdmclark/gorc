#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class MeshNodeType : unsigned int {
	LowerBody		= 0x000F0,
	UpperBody		= 0x0000F,

	Torso 			= 0x00001,
	LeftArm 		= 0x00002,
	RightArm		= 0x00004,
	Head			= 0x00008,
	Hip				= 0x00010,
	LeftLeg			= 0x00020,
	RightLeg		= 0x00040,
};

}
}
