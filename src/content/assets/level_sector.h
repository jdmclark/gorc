#pragma once

#include "framework/math/vector.h"
#include "framework/math/box.h"
#include "sound.h"
#include "content/flags/sector_flag.h"
#include "framework/utility/maybe.h"

namespace gorc {
namespace content {
namespace assets {

class colormap;

class level_sector {
public:
	size_t number;
	flag_set<flags::sector_flag> flags;
	float ambient_light;
	float extra_light;
	size_t colormap_id;
	maybe<colormap const*> colormap;
	vector<3> tint;
	box<3> bounding_box;
	box<3> collide_box;
	sound const* ambient_sound = nullptr;
	float ambient_sound_volume = 0.0f;
	vector<3> center;
	float radius;
	std::vector<size_t> vertices;
	size_t first_surface;
	size_t surface_count;
	vector<3> thrust;
};

}
}
}
