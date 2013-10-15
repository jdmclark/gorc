#include "query.h"
#include "framework/math/util.h"

using namespace gorc::math;

bool gorc::game::world::level::physics::point_inside_surface(const vector<3>& sp, const content::assets::level& level,
		const content::assets::level_surface& surface) {
	for(int i = surface.vertices.size() - 1, j = 0; j < surface.vertices.size(); i = j++) {
		auto p0 = level.vertices[std::get<0>(surface.vertices[i])];
		auto edge = level.vertices[std::get<0>(surface.vertices[j])] - p0;
		auto edge_normal = cross(surface.normal, edge);
		if(dot(edge_normal, sp - p0) < 0.0f) {
			return false;
		}
	}

	return true;
}

bool gorc::game::world::level::physics::segment_surface_intersection(const segment& segment, const content::assets::level& level,
		const content::assets::level_surface& surface) {
	// Check for point
	auto p = level.vertices[std::get<0>(surface.vertices[0])];
	auto u = dot(surface.normal, p - std::get<0>(segment)) / dot(surface.normal, std::get<1>(segment) - std::get<0>(segment));
	if(u < 0.0f || u > 1.0f) {
		return false;
	}

	// Check for thing path passing through surface polygon.
	auto sp = lerp(std::get<0>(segment), std::get<1>(segment), u);
	return point_inside_surface(sp, level, surface);
}

gorc::vector<3> gorc::game::world::level::physics::closest_point_on_surface(const vector<3>& origin, const content::assets::level& level,
		const content::assets::level_surface& surface) {
	auto p = level.vertices[std::get<0>(surface.vertices[0])];
	auto v = origin - p;
	auto pp = (v - surface.normal * dot(surface.normal, v)) + p;
	if(point_inside_surface(pp, level, surface)) {
		return pp;
	}

	// Check edges
	float closest_dist = std::numeric_limits<float>::max();
	vector<3> closest_point;

	for(int p0 = surface.vertices.size() - 1, p1 = 0; p1 < surface.vertices.size(); p0 = p1++) {
		auto vp0 = level.vertices[std::get<0>(surface.vertices[p0])];
		auto vp1 = level.vertices[std::get<0>(surface.vertices[p1])];
		auto lv = vp1 - vp0;
		auto pv = origin - vp0;

		vector<3> candidate_point;
		auto alpha = dot(lv, pv) / length2(lv);
		if(alpha < 0.0f) {
			candidate_point = vp0;
		}
		else if(alpha > 1.0f) {
			candidate_point = vp1;
		}
		else {
			candidate_point = lerp(vp0, vp1, alpha);
		}

		auto cp_dist = length(candidate_point - origin);
		if(cp_dist < closest_dist) {
			closest_point = candidate_point;
			closest_dist = cp_dist;
		}
	}

	return closest_point;
}
