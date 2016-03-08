#pragma once

#include "libold/base/utility/entity_id.hpp"
#include "libold/content/flags/key_marker_type.hpp"

namespace gorc {
namespace game {
namespace world {
namespace events {

class animation_marker {
public:
    #include "animation_marker.uid"

    entity_id thing;
    flags::key_marker_type type;

    animation_marker(entity_id thing, flags::key_marker_type type);
};

}
}
}
}
