#pragma once

#include "game/world/animations/components/slide_ceiling_sky.hpp"
#include "libold/base/utility/inner_join_aspect.hpp"
#include "game/world/level_model.hpp"

namespace gorc {
namespace game {
namespace world {
namespace animations {
namespace aspects {

class update_slide_ceiling_sky_aspect : public inner_join_aspect<components::slide_ceiling_sky> {
private:
    level_model& model;

public:
    update_slide_ceiling_sky_aspect(component_system& cs, level_model& model);

    virtual void update(gorc::time t, entity_id id, components::slide_ceiling_sky& anim) override;
};

}
}
}
}
}
