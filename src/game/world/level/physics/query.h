#pragma once

#include "content/assets/level.h"
#include "game/world/level/surface.h"
#include "content/assets/model.h"
#include "shape.h"
#include "contact.h"
#include "framework/math/util.h"
#include "framework/math/matrix.h"
#include "game/world/level/level_model.h"
#include "framework/maybe.h"

namespace gorc {
namespace game {
namespace world {
namespace level {
namespace physics {

template <typename VertexProvider, typename EdgeProvider> bool point_inside_surface(const vector<3>& sp,
		const VertexProvider& level, const EdgeProvider& surface, const matrix<4>& trns) {
	for(int i = surface.vertices.size() - 1, j = 0; j < surface.vertices.size(); i = j++) {
		auto p0 = trns.transform(level.vertices[std::get<0>(surface.vertices[i])]);
		auto edge = trns.transform(level.vertices[std::get<0>(surface.vertices[j])]) - p0;
		auto edge_normal = cross(trns.transform_normal(surface.normal), edge);
		if(dot(edge_normal, sp - p0) < 0.0f) {
			return false;
		}
	}

	return true;
}

template <typename VertexProvider, typename EdgeProvider> bool point_inside_surface(const vector<3>& sp,
		const VertexProvider& level, const EdgeProvider& surface) {
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

template <typename VertexProvider, typename EdgeProvider> bool segment_surface_intersection(const segment& segment,
		const VertexProvider& level, const EdgeProvider& surface, const matrix<4>& trns) {
	// Check for point
	auto nrm = trns.transform_normal(surface.normal);
	auto p = trns.transform(level.vertices[std::get<0>(surface.vertices[0])]);
	auto u = dot(nrm, p - std::get<0>(segment)) / dot(nrm, std::get<1>(segment) - std::get<0>(segment));
	if(u < 0.0f || u > 1.0f) {
		return false;
	}

	// Check for thing path passing through surface polygon.
	auto sp = lerp(std::get<0>(segment), std::get<1>(segment), u);
	return point_inside_surface(sp, level, surface, trns);
}

template <typename VertexProvider, typename EdgeProvider> bool segment_surface_intersection(const segment& segment,
		const VertexProvider& level, const EdgeProvider& surface) {
	// Check for point
	auto nrm = surface.normal;
	auto p = level.vertices[std::get<0>(surface.vertices[0])];
	auto u = dot(nrm, p - std::get<0>(segment)) / dot(nrm, std::get<1>(segment) - std::get<0>(segment));
	if(u < 0.0f || u > 1.0f) {
		return false;
	}

	// Check for thing path passing through surface polygon.
	auto sp = lerp(std::get<0>(segment), std::get<1>(segment), u);
	return point_inside_surface(sp, level, surface);
}

template <typename VertexProvider, typename EdgeProvider> maybe<vector<3>> bounded_closest_point_on_surface(const vector<3>& origin,
		const VertexProvider& level, const EdgeProvider& surface, const matrix<4>& trns, float max_dist) {
	auto nrm = trns.transform_normal(surface.normal);
	auto p = trns.transform(level.vertices[std::get<0>(surface.vertices[0])]);
	auto v = origin - p;
	auto plane_dist = dot(nrm, v);
	if(plane_dist < 0.0f || plane_dist > max_dist) {
		return maybe<vector<3>>();
	}

	auto pp = (v - nrm * plane_dist) + p;
	if(point_inside_surface(pp, level, surface, trns)) {
		return make_maybe(pp);
	}

	// Check edges
	float closest_dist = std::numeric_limits<float>::max();
	vector<3> closest_point;

	for(int p0 = surface.vertices.size() - 1, p1 = 0; p1 < surface.vertices.size(); p0 = p1++) {
		auto vp0 = trns.transform(level.vertices[std::get<0>(surface.vertices[p0])]);
		auto vp1 = trns.transform(level.vertices[std::get<0>(surface.vertices[p1])]);
		auto lv = vp1 - vp0;
		auto pv = origin - vp0;

		vector<3> candidate_point;
		auto alpha = dot(lv, pv) / length_squared(lv);
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

	return make_maybe(closest_point);
}

template <typename VertexProvider, typename EdgeProvider> maybe<vector<3>> bounded_closest_point_on_surface(const vector<3>& origin,
		const VertexProvider& level, const EdgeProvider& surface, float max_dist) {
	auto nrm = surface.normal;
	auto p = level.vertices[std::get<0>(surface.vertices[0])];
	auto v = origin - p;
	auto plane_dist = dot(nrm, v);
	if(plane_dist < 0.0f || plane_dist > max_dist) {
		return maybe<vector<3>>();
	}

	auto pp = (v - nrm * plane_dist) + p;
	if(point_inside_surface(pp, level, surface)) {
		return make_maybe(pp);
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
		auto alpha = dot(lv, pv) / length_squared(lv);
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

	return make_maybe(closest_point);
}

bool point_inside_sector(const vector<3>& position, const level_model& level_model, const content::assets::level_sector& sec);

void segment_adjoin_path(const segment& segment, const level_model& level_model, const content::assets::level_sector& initial_sector,
		std::vector<std::tuple<unsigned int, unsigned int>>& path);

}
}
}
}
}
