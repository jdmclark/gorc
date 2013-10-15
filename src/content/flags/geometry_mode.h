#pragma once

#include <cstdint>

namespace gorc {
namespace flags {

enum class geometry_mode : uint32_t {
	NotDrawn						= 0,
	Points							= 1,
	Wireframe						= 2,
	Solid							= 3,
	textured						= 4,
	Full							= 5
};

}
}
