#pragma once

#include <cstdint>

namespace gorc {
namespace flags {

enum class face_flag : uint32_t {
	normal							= 0,
	TwoSided						= 1,
	Translucent						= 2
};

}
}
