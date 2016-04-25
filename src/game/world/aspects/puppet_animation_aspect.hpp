#pragma once

#include "libold/base/utility/inner_join_aspect.hpp"
#include "game/world/level_presenter.hpp"
#include "game/world/components/thing.hpp"
#include "game/world/components/puppet_animations.hpp"

namespace gorc {
namespace game {
namespace world {
namespace aspects {

class puppet_animation_aspect : public inner_join_aspect<components::puppet_animations,
                                                         components::thing> {
private:
    level_presenter &presenter;
    maybe<scoped_delegate> created_delegate;
    maybe<scoped_delegate> armed_mode_delegate;
    maybe<scoped_delegate> jumped_delegate;
    maybe<scoped_delegate> landed_delegate;
    maybe<scoped_delegate> killed_delegate;

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
    puppet_animation_aspect(entity_component_system &cs, level_presenter &presenter);

    virtual void update(time_delta, thing_id, components::puppet_animations&, components::thing&) override;
};

}
}
}
}
