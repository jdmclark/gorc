#pragma once

#include "framework/pool.h"
#include "keymix.h"
#include "keystate.h"

namespace Gorc {
namespace Game {
namespace World {
namespace Level {
namespace Keys {

class KeyModel {
public:
	Pool<KeyMix> Mixes;
	Pool<KeyState> Keys;
};

}
}
}
}
}
