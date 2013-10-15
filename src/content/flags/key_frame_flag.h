#pragma once

#include <cstdint>

namespace gorc {
namespace flags {

enum class KeyframeFlag : uint32_t {
	PositionChanges = 0x1,
	OrientationChanges = 0x2
};

}
}
