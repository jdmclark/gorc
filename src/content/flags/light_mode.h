#pragma once

#include <cstdint>

namespace gorc {
namespace flags {

enum class light_mode : uint32_t {
	FullyLit						= 0,
	NotLit							= 1,
	diffuse							= 2,
	Gouraud							= 3,
	Gouraud2						= 4,
	Gouraud3						= 5
};

}
}
