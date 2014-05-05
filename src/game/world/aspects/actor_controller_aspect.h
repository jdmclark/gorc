#pragma once

#include "base/utility/inner_join_aspect.h"
#include "game/world/components/actor.h"
#include "game/world/components/thing.h"

namespace gorc {
namespace game {
namespace world {
namespace aspects {

class actor_controller_aspect : public inner_join_aspect<components::actor, components::thing> {
public:
    actor_controller_aspect(component_system& cs);

    void update(time, entity_id, components::actor&, components::thing&);
};

}
}
}
}
