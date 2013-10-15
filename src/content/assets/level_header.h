#pragma once

#include "framework/math/vector.h"
#include <tuple>

namespace gorc {
namespace content {
namespace assets {

class level_header {
public:
	int version = 1;
	float world_gravity = 4.0f;
	float ceiling_sky_z = 20.0f;
	float horizon_distance = 100.0f;
	float horizon_pixels_per_rev = 768.0f;
	vector<2> horizon_sky_offset = math::zero<2>();
	vector<2> ceiling_sky_offset = math::zero<2>();
	std::tuple<float, float, float, float> mipmap_distances = std::make_tuple(1.0f, 2.0f, 3.0f, 4.0f);
	std::tuple<float, float, float, float> lod_distances = std::make_tuple(0.3f, 0.6f, 0.9f, 1.2f);
	float perspective_distance;
	float gouraud_distance;
};

}
}
}
