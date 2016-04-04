#pragma once

#include "libold/base/utility/entity_id.hpp"

namespace gorc {
namespace game {
namespace world {
namespace events {

class touched_thing {
public:
    #include "touched_thing.uid"

    int toucher;
    int touched;

    touched_thing(int toucher, int touched);
};

}
}
}
}
