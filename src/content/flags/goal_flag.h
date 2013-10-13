#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class GoalFlag : uint32_t {
	DisplayedInObjectives			= 0x1,
	Accomplished					= 0x2
};

}
}
