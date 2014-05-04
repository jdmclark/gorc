#pragma once

#include "key_mix_level_state.h"

namespace gorc {
namespace game {
namespace world {

namespace components {
class thing;
}

namespace keys {

class key_mix {
public:
    int attached_thing;
    key_mix_level_state high, low, body;
};

}
}
}
}
