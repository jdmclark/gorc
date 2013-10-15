#pragma once

#include <cstdint>

namespace gorc {
namespace flags {

enum class collide_type : uint32_t {
	none = 0,
	sphere = 1,
	SphereUnknown = 2,
	Face = 3
};

}
}
