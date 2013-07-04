#pragma once

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

enum class PhysicsCollideClass : unsigned int {
	Wall = 1,
	Adjoin = 2,
	Player = 4,
	Enemy = 8,
	Thing = 16
};

}
}
}
}
