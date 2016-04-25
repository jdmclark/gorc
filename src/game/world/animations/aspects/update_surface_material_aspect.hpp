#pragma once

#include "game/world/animations/components/surface_material.hpp"
#include "libold/base/utility/inner_join_aspect.hpp"
#include "game/world/level_model.hpp"

namespace gorc {
namespace game {
namespace world {
namespace animations {
namespace aspects {

class update_surface_material_aspect : public inner_join_aspect<components::surface_material> {
private:
    level_model& model;

public:
    update_surface_material_aspect(entity_component_system& cs, level_model& model);

    virtual void update(time_delta t, thing_id id, components::surface_material& anim) override;
};

}
}
}
}
}
