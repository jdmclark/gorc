#pragma once

#include "key_mix_level_state.hpp"
#include "content/id.hpp"
#include "utility/uid.hpp"

namespace gorc {
namespace game {
namespace world {
namespace keys {

class key_mix {
public:
    uid(1237354);

    key_mix_level_state high, low, body;

    key_mix();
    virtual ~key_mix();
};

}
}
}
}
