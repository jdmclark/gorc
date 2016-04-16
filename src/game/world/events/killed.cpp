#include "killed.hpp"

gorc::game::world::events::killed::killed(thing_id thing, thing_id killer)
    : thing(thing), killer(killer) {
    return;
}
