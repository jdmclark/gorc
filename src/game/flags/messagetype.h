#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class MessageType {
	Nothing = 0,
	System = 1,
	Thing = 3,
	Sector = 5,
	Surface = 6,
	Cog = 9
};

}
}
