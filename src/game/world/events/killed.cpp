#include "killed.hpp"

gorc::game::world::events::killed::killed(int thing, int killer)
    : thing(thing), killer(killer) {
    return;
}
