#pragma once

#include <cstdint>

namespace gorc {
namespace flags {

enum class sector_flag : uint32_t {
	NoGravity						= 0x1,
	Underwater						= 0x2,
	CogLinked						= 0x4,
	HasThrust						= 0x8,
	HideOnAutomap					= 0x10,
	NoActorEnter					= 0x20,
	Pit								= 0x40,
	HasCollideBox					= 0x1000,
	ShownOnAutoMap					= 0x4000
};

}
}
