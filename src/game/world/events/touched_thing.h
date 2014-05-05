#pragma once

#include "base/utility/entity_id.h"

namespace gorc {
namespace game {
namespace world {
namespace events {

class touched_thing {
public:
    #include "touched_thing.uid"

    entity_id toucher;
    entity_id touched;

    touched_thing(entity_id toucher, entity_id touched);
};

}
}
}
}
