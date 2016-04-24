#pragma once

#include "libold/base/utility/inner_join_aspect.hpp"
#include "game/world/level_presenter.hpp"
#include "game/world/components/thing.hpp"
#include "game/world/components/weapon.hpp"

namespace gorc {
namespace game {
namespace world {
namespace aspects {

class weapon_controller_aspect : public inner_join_aspect<components::weapon, components::thing> {
private:
    level_presenter &presenter;
    maybe<scoped_delegate> touched_thing_delegate;
    maybe<scoped_delegate> touched_surface_delegate;
    maybe<scoped_delegate> created_delegate;

    void touched_thing(thing_id toucher, thing_id touched_thing_id);
    void touched_surface(thing_id toucher, surface_id surface);

public:
    weapon_controller_aspect(component_system&, level_presenter&);

    virtual void update(gorc::time, thing_id, components::weapon&, components::thing&) override;
};

}
}
}
}
