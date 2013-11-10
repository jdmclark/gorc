#pragma once

#include "framework/math/vector.h"
#include "framework/utility/flag_set.h"
#include "framework/text/tokenizer.h"
#include "framework/utility/flag_set.h"
#include "cog/compiler.h"
#include "content/flags/thing_type.h"
#include "content/flags/move_type.h"
#include "content/flags/collide_type.h"
#include "content/flags/thing_flag.h"
#include "content/flags/attach_flag.h"
#include "content/flags/actor_flag.h"
#include "content/flags/physics_flag.h"
#include "model.h"
#include "soundclass.h"
#include "colormap.h"
#include "script.h"
#include "puppet.h"
#include "sprite.h"
#include <string>
#include <memory>

namespace gorc {
namespace content {

class manager;

namespace assets {

class thing_template {
public:
	// General fields
	unsigned int sector;
	vector<3> position;
	vector<3> orientation;
	vector<3> thrust;

	flag_set<flags::actor_flag> actor_flags;
	vector<3> ang_vel;
	flag_set<flags::attach_flag> attach_flags;
	script const* cog = nullptr;
	flags::collide_type collide = flags::collide_type::none;
	int create_thing = 0;
	int explode = 0;
	vector<3> eye_offset;
	flag_set<flags::thing_flag> flags;
	int flesh_hit = 0;
	std::vector<std::tuple<vector<3>, vector<3>>> frames;
	float health = 100.0f;
	float height = 0.18f;
	float light = 0.0f;
	float light_intensity = 0.0f;
	vector<3> light_offset;
	float mass = 2.0f;
	float max_health = 100.0f;
	float max_light = 1.0f;
	float max_rot_vel = 200.0f;
	float max_vel = 1.0f;
	model const* model_3d = nullptr;
	flags::move_type move = flags::move_type::none;
	float move_size = 0.05f;
	flag_set<flags::physics_flag> physics_flags;
	puppet const* puppet = nullptr;
	float size = 0.05f;
	soundclass const* sound_class = nullptr;
	sprite const* sprite = nullptr;
	float timer = 0.0f;
	flags::thing_type type = flags::thing_type::ghost;
	vector<3> vel;

	void parse_args(text::tokenizer& tok, manager& manager, const colormap& cmp, const cog::compiler& compiler,
			const std::unordered_map<std::string, int>& templates, diagnostics::report& report);
};

}
}
}
