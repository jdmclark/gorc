#pragma once

#include "libold/base/utility/inner_join_aspect.hpp"
#include "game/world/components/thing.hpp"
#include "game/world/level_presenter.hpp"

namespace gorc {
namespace game {
namespace world {
namespace aspects {

class thing_controller_aspect : public inner_join_aspect<components::thing> {
private:
    level_presenter& presenter;
    maybe<scoped_delegate> touched_surface_delegate;
    maybe<scoped_delegate> touched_thing_delegate;

public:
    thing_controller_aspect(component_system& cs, level_presenter& presenter);

    void update(gorc::time, entity_id, components::thing&);

    void on_touched_surface(int thing, int touched_surface);
    void on_touched_thing(int thing, int touched_thing);
};

}
}
}
}
