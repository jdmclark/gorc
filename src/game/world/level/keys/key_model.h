#pragma once

#include "framework/pool.h"
#include "key_mix.h"
#include "key_state.h"

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
