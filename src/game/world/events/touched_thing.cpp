#include "touched_thing.hpp"

gorc::game::world::events::touched_thing::touched_thing(int toucher, int touched)
    : toucher(toucher), touched(touched) {
    return;
}
