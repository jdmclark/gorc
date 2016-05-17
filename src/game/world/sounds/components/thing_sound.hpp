#pragma once

#include "content/id.hpp"
#include "utility/uid.hpp"

namespace gorc {
namespace game {
namespace world {
namespace sounds {
namespace components {

class thing_sound {
public:
    uid(1373383292);

    thing_id sound;

    thing_sound(thing_id sound);
};

}
}
}
}
}
