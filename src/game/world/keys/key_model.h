#pragma once

#include "framework/utility/pool.h"
#include "key_mix.h"
#include "key_state.h"

namespace gorc {
namespace game {
namespace world {
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
