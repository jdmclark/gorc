#include "class_sound.h"

gorc::game::world::events::class_sound::class_sound(entity_id thing, flags::sound_subclass_type type)
    : thing(thing), type(type) {
    return;
}
