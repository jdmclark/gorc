#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class TextureMode : uint32_t {
	AffineMapping					= 0,
	PerspectiveMapping				= 1
};

}
}
