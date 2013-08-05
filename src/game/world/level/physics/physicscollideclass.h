#pragma once

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

enum class PhysicsCollideClass : unsigned int {
	Wall 	= 0x0001,
	Floor 	= 0x0002,
	Adjoin 	= 0x0004,
	Player 	= 0x0008,
	Enemy 	= 0x0010,
	Thing 	= 0x0020
};

}
}
}
}
