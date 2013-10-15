#pragma once

#include <cstdint>

namespace gorc {
namespace flags {

enum class texture_mode : uint32_t {
	AffineMapping					= 0,
	PerspectiveMapping				= 1
};

}
}
