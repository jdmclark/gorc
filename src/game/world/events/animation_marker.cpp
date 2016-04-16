#include "animation_marker.hpp"

gorc::game::world::events::animation_marker::animation_marker(thing_id thing, flags::key_marker_type type)
    : thing(thing), type(type) {
    return;
}
