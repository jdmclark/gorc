#include "query.h"
#include "framework/math/util.h"

using namespace Gorc::Math;

bool Gorc::Game::World::Level::Physics::PointInsideSurface(const Vector<3>& sp, const Content::Assets::Level& level,
		const Content::Assets::LevelSurface& surface) {
	for(int i = surface.Vertices.size() - 1, j = 0; j < surface.Vertices.size(); i = j++) {
		auto p0 = level.Vertices[std::get<0>(surface.Vertices[i])];
		auto edge = level.Vertices[std::get<0>(surface.Vertices[j])] - p0;
		auto edge_normal = Cross(surface.Normal, edge);
		if(Dot(edge_normal, sp - p0) < 0.0f) {
			return false;
		}
	}

	return true;
}

bool Gorc::Game::World::Level::Physics::SegmentSurfaceIntersection(const Segment& segment, const Content::Assets::Level& level,
		const Content::Assets::LevelSurface& surface) {
	// Check for point
	auto p = level.Vertices[std::get<0>(surface.Vertices[0])];
	auto u = Dot(surface.Normal, p - std::get<0>(segment)) / Dot(surface.Normal, std::get<1>(segment) - std::get<0>(segment));
	if(u < 0.0f || u > 1.0f) {
		return false;
	}

	// Check for thing path passing through surface polygon.
	auto sp = Lerp(std::get<0>(segment), std::get<1>(segment), u);
	return PointInsideSurface(sp, level, surface);
}

Gorc::Math::Vector<3> Gorc::Game::World::Level::Physics::ClosestPointOnSurface(const Math::Vector<3>& origin, const Content::Assets::Level& level,
		const Content::Assets::LevelSurface& surface) {
	auto p = level.Vertices[std::get<0>(surface.Vertices[0])];
	auto v = origin - p;
	auto pp = (v - surface.Normal * Dot(surface.Normal, v)) + p;
	if(PointInsideSurface(pp, level, surface)) {
		return pp;
	}

	// Check edges
	float closest_dist = std::numeric_limits<float>::max();
	Math::Vector<3> closest_point;

	for(int p0 = surface.Vertices.size() - 1, p1 = 0; p1 < surface.Vertices.size(); p0 = p1++) {
		auto vp0 = level.Vertices[std::get<0>(surface.Vertices[p0])];
		auto vp1 = level.Vertices[std::get<0>(surface.Vertices[p1])];
		auto lv = vp1 - vp0;
		auto pv = origin - vp0;

		Math::Vector<3> candidate_point;
		auto alpha = Dot(lv, pv) / Length2(lv);
		if(alpha < 0.0f) {
			candidate_point = vp0;
		}
		else if(alpha > 1.0f) {
			candidate_point = vp1;
		}
		else {
			candidate_point = Lerp(vp0, vp1, alpha);
		}

		auto cp_dist = Length(candidate_point - origin);
		if(cp_dist < closest_dist) {
			closest_point = candidate_point;
			closest_dist = cp_dist;
		}
	}

	return closest_point;
}
