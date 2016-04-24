#pragma once

#include "content/id.hpp"

namespace gorc {
namespace game {
namespace world {
namespace sounds {
namespace components {

class foley {
public:
    #include "foley.uid"

    thing_id sound;

    foley(thing_id sound);
};

}
}
}
}
}
