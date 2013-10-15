#pragma once

#include <cstdint>

namespace gorc {
namespace flags {

enum class GoalFlag : uint32_t {
	DisplayedInObjectives			= 0x1,
	Accomplished					= 0x2
};

}
}
