#include "animation_marker.h"

gorc::game::world::events::animation_marker::animation_marker(entity_id thing, flags::key_marker_type type)
    : thing(thing), type(type) {
    return;
}
