#pragma once

#include "base/utility/entity_id.h"

namespace gorc {
namespace game {
namespace world {
namespace events {

class landed {
public:
    #include "landed.uid"

    entity_id thing;

    landed(entity_id thing);
};

}
}
}
}
