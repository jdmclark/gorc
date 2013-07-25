#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class LightMode : uint32_t {
	FullyLit						= 0,
	NotLit							= 1,
	Diffuse							= 2,
	Gouraud							= 3,
	Gouraud2						= 4,
	Gouraud3						= 5
};

}
}
