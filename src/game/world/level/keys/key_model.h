#pragma once

#include "framework/pool.h"
#include "key_mix.h"
#include "key_state.h"

namespace gorc {
namespace game {
namespace world {
namespace level {
namespace keys {

class key_model {
public:
	pool<key_mix> mixes;
	pool<key_state> keys;
};

}
}
}
}
}
