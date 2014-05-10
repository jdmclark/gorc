#pragma once

#include "base/utility/inner_join_aspect.h"
#include "game/world/level_presenter.h"
#include "game/world/components/thing.h"
#include "game/world/components/weapon.h"

namespace gorc {
namespace game {
namespace world {
namespace aspects {

class weapon_controller_aspect : public inner_join_aspect<components::weapon, components::thing> {
private:
    level_presenter &presenter;

    void touched_thing(entity_id thing_id, entity_id touched_thing_id);
    void touched_surface(entity_id thing_id, int surface);

public:
    weapon_controller_aspect(component_system&, level_presenter&);

    virtual void update(time, entity_id, components::weapon&, components::thing&) override;
};

}
}
}
}
