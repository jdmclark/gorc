#pragma once

#include <cstdint>

namespace Gorc {
namespace Cog {
namespace Symbols {

enum class SymbolType : uint8_t {
	Ai,
	Cog,
	Flex,
	Float,
	Int,
	Keyframe,
	Material,
	Message,
	Model,
	Sector,
	Sound,
	String,
	Surface,
	Template,
	Thing,
	Vector
};

}
}
}
