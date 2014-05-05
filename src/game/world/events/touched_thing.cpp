#include "touched_thing.h"

gorc::game::world::events::touched_thing::touched_thing(entity_id toucher, entity_id touched)
    : toucher(toucher), touched(touched) {
    return;
}
