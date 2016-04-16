#include "standing_material_changed.hpp"

gorc::game::world::events::standing_material_changed::standing_material_changed(thing_id thing, flags::standing_material_type type)
    : thing(thing), type(type) {
    return;
}
