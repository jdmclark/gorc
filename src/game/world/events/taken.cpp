#include "taken.hpp"

gorc::game::world::events::taken::taken(thing_id taken_thing, thing_id taker)
    : taken_thing(taken_thing), taker(taker) {
    return;
}
