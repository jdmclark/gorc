#pragma once

#include "game/world/components/actor.hpp"
#include "game/world/components/thing.hpp"
#include "game/world/level_presenter.hpp"
#include "ecs/inner_join_aspect.hpp"

namespace gorc {
namespace game {
namespace world {
namespace aspects {

class actor_controller_aspect : public inner_join_aspect<thing_id, components::actor, components::thing> {
private:
    maybe<scoped_delegate> killed_delegate;
    maybe<scoped_delegate> created_delegate;

public:
    explicit actor_controller_aspect(entity_component_system<thing_id>& cs);

    void update(time_delta, thing_id, components::actor&, components::thing&);
};

}
}
}
}
