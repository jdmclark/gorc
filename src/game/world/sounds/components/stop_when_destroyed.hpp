#pragma once

#include "content/id.hpp"
#include "utility/uid.hpp"

namespace gorc {
namespace game {
namespace world {
namespace sounds {
namespace components {

class stop_when_destroyed {
public:
    uid(1309740388);

    thing_id sound;

    stop_when_destroyed(thing_id sound);
};

}
}
}
}
}
