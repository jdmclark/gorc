#pragma once

#include "libold/base/utility/entity_id.hpp"
#include "game/flags/standing_material_type.hpp"

namespace gorc {
namespace game {
namespace world {
namespace events {

class standing_material_changed {
public:
    #include "standing_material_changed.uid"

    entity_id thing;
    flags::standing_material_type type;

    standing_material_changed(entity_id thing, flags::standing_material_type type);
};

}
}
}
}
