#pragma once

#include "base/utility/entity_id.h"

namespace gorc {
namespace game {
namespace world {
namespace sounds {
namespace components {

class foley {
public:
    #include "foley.uid"

    entity_id sound;

    foley(entity_id sound);
};

}
}
}
}
}
