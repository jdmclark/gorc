#pragma once

#include <cstdint>

namespace gorc {
namespace cog {
namespace symbols {

enum class symbol_type : uint8_t {
	ai,
	cog,
	flex,
	floating,
	integer,
	keyframe,
	material,
	message,
	model,
	sector,
	sound,
	string,
	surface,
	thing_template,
	thing,
	vector
};

}
}
}
