#pragma once

#include "thing_controller.h"
#include "game/world/physics/contact.h"
#include "base/utility/maybe.h"
#include "content/flags/puppet_mode_type.h"
#include "content/flags/puppet_submode_type.h"
#include "content/flags/sound_subclass_type.h"

namespace gorc {
namespace game {
namespace world {

namespace components {
class thing;
}

namespace gameplay {

enum class standing_material {
    none,
    hard,
    dirt,
    metal,
    shallow_water,
    deep_water,
    very_deep_water
};

class character_controller : public thing_controller {
private:
    flags::puppet_mode_type get_puppet_mode(components::thing& thing);
    standing_material get_standing_material(components::thing& thing);

    flags::sound_subclass_type get_left_run_subclass_type(int thing_id);
    flags::sound_subclass_type get_right_run_subclass_type(int thing_id);
    flags::sound_subclass_type get_left_walk_subclass_type(int thing_id);
    flags::sound_subclass_type get_right_walk_subclass_type(int thing_id);
    flags::sound_subclass_type get_jump_subclass_type(int thing_id);
    flags::sound_subclass_type get_land_subclass_type(int thing_id);

    void set_walk_animation(components::thing& thing, flags::puppet_submode_type type, float speed);
    bool is_walk_animation_mode(components::thing& thing, flags::puppet_submode_type type);
    void set_walk_animation_speed(components::thing& thing, float speed);
    void play_falling_animation(int thing_id, components::thing& thing);
    void play_standing_animation(int thing_id, components::thing& thing);

    maybe<physics::contact> run_falling_sweep(int thing_id, components::thing& thing, double dt);
    maybe<physics::contact> run_walking_sweep(int thing_id, components::thing& thing, double dt);

    void update_falling(int thing_id, components::thing& thing, double dt);
    void update_standing(int thing_id, components::thing& thing, double dt);
    bool step_on_surface(int thing_id, components::thing& thing, unsigned int surf_id, const physics::contact& rrcb);
    bool step_on_thing(int thing_id, components::thing& thing, int step_thing_id, const physics::contact& rrcb);
    void land_on_surface(int thing_id, components::thing& thing, unsigned int surf_id, const physics::contact& rrcb);
    void land_on_thing(int thing_id, components::thing& thing, int land_thing_id, const physics::contact& rrcb);
    void jump(int thing_id, components::thing& thing);
    void jump_from_surface(int thing_id, components::thing& thing, unsigned int surf_id);
    void jump_from_thing(int thing_id, components::thing& thing, int jump_thing_id);
    void set_is_falling(int thing_id, components::thing& thing);

    void play_left_run_footstep(int thing_id);
    void play_right_run_footstep(int thing_id);
    void play_left_walk_footstep(int thing_id);
    void play_right_walk_footstep(int thing_id);

public:
    using thing_controller::thing_controller;

    virtual void update(int thing_id, double dt) override;
    virtual void create_controller_data(int thing_id) override;
    virtual void remove_controller_data(int thing_id) override;
    virtual void handle_animation_marker(int thing_id, flags::key_marker_type marker) override;
};

}
}
}
}
