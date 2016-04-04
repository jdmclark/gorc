#include "taken.hpp"

gorc::game::world::events::taken::taken(int taken_thing, int taker)
    : taken_thing(taken_thing), taker(taker) {
    return;
}
