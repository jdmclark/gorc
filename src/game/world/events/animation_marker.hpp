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

    int thing;
    flags::key_marker_type type;

    animation_marker(int thing, flags::key_marker_type type);
};

}
}
}
}
