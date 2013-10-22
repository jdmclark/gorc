#pragma once

#include <cstdint>

namespace gorc {
namespace flags {

enum class collide_type : uint32_t {
	none = 0,
	sphere = 1,
	sphere_unknown = 2,
	face = 3
};

}
}
