#include "killed.hpp"

gorc::game::world::events::killed::killed(entity_id thing, entity_id killer)
    : thing(thing), killer(killer) {
    return;
}
