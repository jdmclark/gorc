#include "standing_material_changed.h"

gorc::game::world::events::standing_material_changed::standing_material_changed(entity_id thing, flags::standing_material_type type)
    : thing(thing), type(type) {
    return;
}
