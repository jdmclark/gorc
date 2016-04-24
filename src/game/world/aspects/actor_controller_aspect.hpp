#pragma once

#include "libold/base/utility/inner_join_aspect.hpp"
#include "game/world/components/actor.hpp"
#include "game/world/components/thing.hpp"
#include "game/world/level_presenter.hpp"

namespace gorc {
namespace game {
namespace world {
namespace aspects {

class actor_controller_aspect : public inner_join_aspect<components::actor, components::thing> {
private:
    maybe<scoped_delegate> killed_delegate;
    maybe<scoped_delegate> created_delegate;

public:
    explicit actor_controller_aspect(component_system& cs);

    void update(gorc::time, thing_id, components::actor&, components::thing&);
};

}
}
}
}
