#pragma once

#include <cstdint>

namespace Gorc {
namespace Flags {

enum class ThingType : uint32_t {
	Free							= 0,
	Camera							= 1,
	Actor							= 2,
	Weapon							= 3,
	Debris							= 4,
	Item							= 5,
	Explosion						= 6,
	Cog								= 7,
	Ghost							= 8,
	Corpse							= 9,
	Player							= 10,
	Particle						= 11
};

}
}
