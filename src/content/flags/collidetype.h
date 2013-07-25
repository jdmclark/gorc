#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class CollideType : uint32_t {
	None = 0,
	Sphere = 1,
	SphereUnknown = 2,
	Face = 3
};

}
}
