#pragma once

#include "content/id.hpp"
#include "utility/uid.hpp"

namespace gorc {
namespace game {
namespace world {
namespace sounds {
namespace components {

class voice {
public:
    uid(78126341);

    thing_id sound;

    voice(thing_id sound);
};

}
}
}
}
}
