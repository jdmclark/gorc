#pragma once

#include "base/utility/inner_join_aspect.h"
#include "game/world/level_presenter.h"
#include "game/world/components/thing.h"
#include "game/world/components/puppet_animations.h"

namespace gorc {
namespace game {
namespace world {
namespace aspects {

class puppet_animation_aspect : public inner_join_aspect<components::puppet_animations,
                                                         components::thing> {
private:
    level_presenter &presenter;

    void set_walk_animation(components::thing&,
                            components::puppet_animations&,
                            flags::puppet_submode_type,
                            float speed);
    void set_walk_animation_speed(components::puppet_animations&,
                                  float speed);
    bool is_walk_animation_mode(components::puppet_animations&,
                                flags::puppet_submode_type);

    void update_standing_animation(components::thing&, components::puppet_animations&);

public:
    puppet_animation_aspect(component_system &cs, level_presenter &presenter);

    virtual void update(time, entity_id, components::puppet_animations&, components::thing&) override;
};

}
}
}
}
