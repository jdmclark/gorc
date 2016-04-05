#pragma once

#include "key_mix_level_state.hpp"
#include "content/id.hpp"

namespace gorc {
namespace game {
namespace world {

namespace components {
class thing;
}

namespace keys {

class key_mix {
public:
    thing_id attached_thing;
    key_mix_level_state high, low, body;
};

}
}
}
}
