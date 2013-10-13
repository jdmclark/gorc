#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class MoveType : uint32_t {
	None,
	Physics,
	Path
};

}
}
