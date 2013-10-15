#pragma once

#include "content/flags/surface_flag.h"
#include "content/flags/face_flag.h"
#include "content/flags/geometry_mode.h"
#include "content/flags/light_mode.h"
#include "content/flags/texture_mode.h"
#include "framework/math/vector.h"
#include "framework/flag_set.h"
#include <memory>
#include <vector>
#include <tuple>

namespace gorc {
namespace content {
namespace assets {

class level_surface {
public:
	int material;
	flag_set<flags::surface_flag> flags;
	flag_set<flags::face_flag> face_type_flags;
	flags::geometry_mode geometry_mode;
	flags::light_mode light_mode;
	flags::texture_mode texture_mode;
	int adjoin;
	int adjoined_sector;
	float extra_light;
	std::vector<std::tuple<int, int, float>> vertices;
	vector<3> normal;
	vector<2> texture_offset;
	vector<3> thrust;
};

}
}
}
