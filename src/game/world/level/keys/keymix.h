#pragma once

#include "keymixlevelstate.h"
#include "framework/id.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {

class Thing;

namespace Keys {

class KeyMix {
public:
	Id<Thing> AttachedThing;
	KeyMixLevelState High, Low, Body;
};

}
}
}
}
}
