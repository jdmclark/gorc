#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class GeometryMode : uint32_t {
	NotDrawn						= 0,
	Points							= 1,
	Wireframe						= 2,
	Solid							= 3,
	Textured						= 4,
	Full							= 5
};

}
}
