#include "touched_thing.hpp"

gorc::game::world::events::touched_thing::touched_thing(thing_id toucher, thing_id touched)
    : toucher(toucher), touched(touched) {
    return;
}
