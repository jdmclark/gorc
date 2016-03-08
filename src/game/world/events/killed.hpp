#pragma once

#include "libold/base/utility/entity_id.hpp"

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
