#pragma once

#include "game/world/animations/components/slide_surface.hpp"
#include "libold/base/utility/inner_join_aspect.hpp"
#include "game/world/level_model.hpp"

namespace gorc {
namespace game {
namespace world {
namespace animations {
namespace aspects {

class update_slide_surface_aspect : public inner_join_aspect<components::slide_surface> {
private:
    level_model& model;
    maybe<scoped_delegate> stop_animation_delegate;

public:
    update_slide_surface_aspect(component_system& cs, level_model& model);

    virtual void update(time_delta t, thing_id id, components::slide_surface& anim) override;
};

}
}
}
}
}
