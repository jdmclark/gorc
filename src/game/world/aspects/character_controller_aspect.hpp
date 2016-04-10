#pragma once

#include "libold/base/utility/inner_join_aspect.hpp"
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

class character_controller_aspect : public inner_join_aspect<components::character, components::thing> {
private:
    level_presenter &presenter;
    maybe<scoped_delegate> created_delegate;
    maybe<scoped_delegate> killed_delegate;

    flags::standing_material_type get_standing_material(components::thing& thing);

    bool can_stand_on_thing(int surface_thing_id);
    bool can_stand_on_surface(int surface_id);

    maybe<physics::contact> run_falling_sweep(int thing_id, components::thing& thing, double dt);
    maybe<physics::contact> run_walking_sweep(int thing_id, components::thing& thing, double dt);

    void update_falling(int thing_id, components::thing& thing, double dt);
    void update_standing(int thing_id, components::thing& thing, double dt);
    bool step_on_surface(int thing_id, components::thing& thing, int surf_id, const physics::contact& rrcb);
    bool step_on_thing(int thing_id, components::thing& thing, int step_thing_id, const physics::contact& rrcb);
    void land_on_surface(int thing_id, components::thing& thing, int surf_id, const physics::contact& rrcb);
    void land_on_thing(int thing_id, components::thing& thing, int land_thing_id, const physics::contact& rrcb);
    void jump(int thing_id, components::thing& thing);
    void set_is_falling(int thing_id, components::thing& thing);

    void play_left_run_footstep(int thing_id);
    void play_right_run_footstep(int thing_id);
    void play_left_walk_footstep(int thing_id);
    void play_right_walk_footstep(int thing_id);

    void on_killed(int thing_id, components::thing &thing, int killer);

public:
    character_controller_aspect(component_system&, level_presenter&);

    static void create_controller_data(int thing_id, level_presenter&);
    static void remove_controller_data(int thing_id, level_presenter&);

    virtual void update(gorc::time, entity_id, components::character&, components::thing&) override;
};

}
}
}
}
