#pragma once

#include "game/world/animations/components/surface_light.h"
#include "base/utility/inner_join_aspect.h"
#include "game/world/level_model.h"

namespace gorc {
namespace game {
namespace world {
namespace animations {
namespace aspects {

class update_surface_light_aspect : public inner_join_aspect<components::surface_light> {
private:
    level_model& model;

public:
    update_surface_light_aspect(component_system& cs, level_model& model);

    virtual void update(time t, entity_id id, components::surface_light& anim) override;
};

}
}
}
}
}
