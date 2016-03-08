#include "taken.hpp"

gorc::game::world::events::taken::taken(entity_id taken_thing, entity_id taker)
    : taken_thing(taken_thing), taker(taker) {
    return;
}
