#pragma once

#include "thing_controller.h"

namespace gorc {
namespace game {
namespace world {
namespace level {

class thing;

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

class filtered_closest_ray_callback;

class character_controller : public thing_controller {
private:
	standing_material get_standing_material(thing& thing);

	void play_running_animation(int thing_id, thing& thing, double speed);
	void play_standing_animation(int thing_id, thing& thing);

	void update_falling(int thing_id, thing& thing, double dt);
	void update_standing_on_surface(int thing_id, thing& thing, double dt);
	void update_standing_on_thing(int thing_id, thing& thing, double dt);
	void jump(int thing_id, thing& thing);
	void jump_from_surface(int thing_id, thing& thing, unsigned int surf_id);
	void jump_from_thing(int thing_id, thing& thing, int jump_thing_id);

public:
	using thing_controller::thing_controller;

	virtual void update(int thing_id, double dt) override;
	virtual void create_controller_data(int thing_id) override;
	virtual void handle_animation_marker(int thing_id, flags::key_marker_type marker) override;

	void play_left_run_footstep(int thing_id);
	void play_right_run_footstep(int thing_id);
};

}
}
}
}
}
