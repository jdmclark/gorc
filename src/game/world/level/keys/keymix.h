#pragma once

#include "keymixlevelstate.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class Thing;

namespace Keys {

class KeyMix {
public:
	int AttachedThing;
	KeyMixLevelState High, Low, Body;
};

}
}
}
}
}
