#pragma once

#include "content/assets/template.h"
#include "gameplay/thing_controller.h"
#include "physics/object_data.h"
#include "framework/utility/maybe.h"
#include <memory>

namespace gorc {
namespace game {
namespace world {
namespace level {

namespace sounds {
class sound;
}

namespace keys {
class key_mix;
class key_state;
}

class thing : public content::assets::thing_template {
public:
	gameplay::thing_controller* controller;
	physics::thing_object_data object_data;

	bool path_moving = false;
	int current_frame = 0;
	int next_frame = 0;
	int goal_frame = 0;
	float path_move_speed = 0.0f;
	bool is_blocked = false;

	maybe<sounds::sound*> current_foley_loop_channel;

	int attached_surface = -1;
	int attached_thing = -1;
	vector<3> prev_attached_thing_position;

	int attached_key_mix = -1;
	int actor_walk_animation = -1;

	int capture_cog = -1;
	float time_alive = 0.0f;

	thing() = default;
	thing(const content::assets::thing_template& tpl);
};

}
}
}
}
