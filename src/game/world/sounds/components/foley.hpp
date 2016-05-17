#pragma once

#include "content/id.hpp"
#include "utility/uid.hpp"

namespace gorc {
namespace game {
namespace world {
namespace sounds {
namespace components {

class foley {
public:
    uid(3826752884);

    thing_id sound;

    foley(thing_id sound);
};

}
}
}
}
}
