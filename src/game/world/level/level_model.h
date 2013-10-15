#pragma once

#include "content/assets/level.h"
#include "cog/instance.h"
#include "framework/pool.h"
#include "thing.h"
#include "surface.h"
#include "sounds/sound.h"
#include "game/world/level/animations/animation_model.h"
#include "game/world/level/scripts/script_model.h"
#include "game/world/level/sounds/sound_model.h"
#include "game/world/level/keys/key_model.h"
#include "game/world/level/gameplay/inventory_model.h"
#include <vector>

using namespace gorc::math;

namespace gorc {
namespace game {
namespace world {
namespace level {

class level_model {
public:
	const content::assets::level& level;
	content::assets::level_header header;
	std::vector<content::assets::level_adjoin> adjoins;
	std::vector<surface> surfaces;
	std::vector<content::assets::level_sector> sectors;

	animations::animation_model animation_model;
	scripts::script_model script_model;
	sounds::sound_model sound_model;
	keys::key_model key_model;
	gameplay::inventory_model inventory_model;

	pool<thing> things;

	int camera_thing_id;
	unsigned int camera_sector;
	vector<3> camera_position;
	vector<3> camera_look = make_vector(0.0f, 1.0f, 0.0f);
	vector<3> camera_up = make_vector(0.0f, 0.0f, 1.0f);
	vector<3> camera_velocity = zero<3>();

	std::vector<content::assets::thing_template const*> spawn_points;
	unsigned int current_spawn_point = 0;

	double level_time = 0.0;
	double game_time = 0.0;
	vector<3> dynamic_tint = math::zero<3>();

	level_model(content::manager& manager, cog::compiler& Cogcompiler, const content::assets::level& level, const content::assets::inventory& inv);
};

}
}
}
}
