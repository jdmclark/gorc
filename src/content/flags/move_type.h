#pragma once

#include <cstdint>

namespace gorc {
namespace flags {

enum class move_type : uint32_t {
	none,
	physics,
	Path
};

}
}
