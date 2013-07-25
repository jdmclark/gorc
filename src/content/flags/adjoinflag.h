#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class AdjoinFlag : uint32_t {
	Visible 						= 0x1,
	AllowMovement 					= 0x2,
	AllowSound 						= 0x4,
	AllowPlayerOnly 				= 0x8,
	AllowAiOnly 					= 0x10
};

}
}
