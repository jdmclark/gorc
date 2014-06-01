#pragma once

#include "base/utility/entity_id.h"

namespace gorc {
namespace game {
namespace world {
namespace events {

class killed {
public:
    #include "killed.uid"

    entity_id thing;
    entity_id killer;

    killed(entity_id thing, entity_id killer);
};

}
}
}
}
