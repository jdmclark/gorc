#pragma once

#include "base/utility/entity_id.h"

namespace gorc {
namespace game {
namespace world {
namespace events {

class jumped {
public:
    #include "jumped.uid"

    entity_id thing;

    jumped(entity_id thing);
};

}
}
}
}
