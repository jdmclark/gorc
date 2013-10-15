#pragma once

#include "animation.h"
#include "content/flags/anim_flag.h"
#include "framework/flag_set.h"
#include "framework/math/vector.h"
#include "framework/pool.h"

namespace gorc {

namespace cog {
namespace verbs {

class verb_table;
}
}

namespace game {

class components;

namespace world {
namespace level {

class level_model;

namespace animations {

class animation_model;

class animation_presenter {
private:
	level_model* levelModel;
	animation_model* model;

public:
	void start(level_model& levelModel, animation_model& model);
	void update(double dt);

	int surface_anim(int surface, float rate, flag_set<flags::AnimFlag> flags);
	int get_surface_anim(int surface);

	void stop_anim(int anim);

	int get_surface_cel(int surface);
	void set_surface_cel(int surface, int cel);

	int slide_ceiling_sky(float u_speed, float v_speed);
	int slide_surface(int surface_id, const vector<3>& direction);

	int surface_light_anim(int surface_id, float start_light, float end_light, float change_time);

	static void register_verbs(cog::verbs::verb_table&, components&);
};

}
}
}
}
}
