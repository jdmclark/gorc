#pragma once

#include "game/world/animations/components/slide_surface.hpp"
#include "game/world/level_model.hpp"
#include "ecs/inner_join_aspect.hpp"

namespace gorc {
namespace game {
namespace world {
namespace animations {
namespace aspects {

class update_slide_surface_aspect : public inner_join_aspect<thing_id, components::slide_surface> {
private:
    level_model& model;
    maybe<scoped_delegate> stop_animation_delegate;

public:
    update_slide_surface_aspect(entity_component_system<thing_id>& cs, level_model& model);

    virtual void update(time_delta t, thing_id id, components::slide_surface& anim) override;
};

}
}
}
}
}
