#pragma once

#include "libold/base/utility/entity_id.hpp"

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
