#pragma once

#include "key_mix_level_state.h"

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
