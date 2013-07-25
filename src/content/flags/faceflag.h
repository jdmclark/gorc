#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class FaceFlag : uint32_t {
	Normal							= 0,
	TwoSided						= 1,
	Translucent						= 2
};

}
}
