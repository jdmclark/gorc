#pragma once

#include "ecs/inner_join_aspect.hpp"
#include "game/world/level_presenter.hpp"
#include "game/world/components/thing.hpp"
#include "game/world/components/character.hpp"

#include "game/world/physics/contact.hpp"
#include "utility/maybe.hpp"
#include "libold/content/flags/puppet_mode_type.hpp"
#include "libold/content/flags/puppet_submode_type.hpp"
#include "libold/content/flags/sound_subclass_type.hpp"

#include "game/flags/standing_material_type.hpp"

namespace gorc {
namespace game {
namespace world {
namespace aspects {

class character_controller_aspect : public inner_join_aspect<thing_id, components::character, components::thing> {
private:
    level_presenter &presenter;
    maybe<scoped_delegate> created_delegate;
    maybe<scoped_delegate> killed_delegate;

    flags::standing_material_type get_standing_material(components::thing& thing);

    bool can_stand_on_thing(thing_id surface_thing_id);
    bool can_stand_on_surface(surface_id);

    maybe<physics::contact> run_falling_sweep(thing_id, components::thing& thing, double dt);
    maybe<physics::contact> run_walking_sweep(thing_id, components::thing& thing, double dt);

    void update_falling(thing_id, components::thing& thing, double dt);
    void update_standing(thing_id, components::thing& thing, double dt);
    bool step_on_surface(thing_id, components::thing& thing, surface_id surf_id, const physics::contact& rrcb);
    bool step_on_thing(thing_id, components::thing& thing, thing_id step_thing_id, const physics::contact& rrcb);
    void land_on_surface(thing_id, components::thing& thing, surface_id surf_id, const physics::contact& rrcb);
    void land_on_thing(thing_id, components::thing& thing, thing_id land_thing_id, const physics::contact& rrcb);
    void jump(thing_id, components::thing& thing);
    void set_is_falling(thing_id, components::thing& thing);

    void play_left_run_footstep(thing_id);
    void play_right_run_footstep(thing_id);
    void play_left_walk_footstep(thing_id);
    void play_right_walk_footstep(thing_id);

    void on_killed(thing_id, components::thing &thing, thing_id killer);

public:
    character_controller_aspect(entity_component_system<thing_id>&, level_presenter&);

    static void create_controller_data(thing_id, level_presenter&);
    static void remove_controller_data(thing_id, level_presenter&);

    virtual void update(time_delta, thing_id, components::character&, components::thing&) override;
};

}
}
}
}
