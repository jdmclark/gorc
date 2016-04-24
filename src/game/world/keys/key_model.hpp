#pragma once

#include "libold/base/utility/pool.hpp"
#include "key_mix.hpp"
#include "key_state.hpp"

namespace gorc {
namespace game {
namespace world {
namespace keys {

class key_model {
public:
    oldpool<key_mix> mixes;
    oldpool<key_state> keys;
    unsigned int key_creation_counter = 0;
};

}
}
}
}
