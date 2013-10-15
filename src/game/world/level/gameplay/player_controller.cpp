#include "player_controller.h"
#include "game/world/level/level_presenter.h"
#include "game/world/level/level_model.h"
#include "game/world/level/physics/query.h"

void gorc::game::world::level::gameplay::player_controller::find_sector_resting_manifolds(const physics::sphere& sphere, int sector_id,
		std::set<int>& closed, std::vector<vector<3>>& manifolds) {
	if(closed.find(sector_id) != closed.end()) {
		return;
	}

	closed.insert(sector_id);

	const auto& sector = presenter.model->sectors[sector_id];
	for(int i = sector.first_surface; i < sector.first_surface + sector.surface_count; ++i) {
		const auto& surface = presenter.model->surfaces[i];

		auto surf_nearest_point = physics::closest_point_on_surface(std::get<0>(sphere), presenter.model->level, surface);
		auto surf_nearest_dist = math::length(std::get<0>(sphere) - surf_nearest_point);

		if(		// Passable:
				surf_nearest_dist <= std::get<1>(sphere) &&
				surface.adjoin >= 0 &&
				(presenter.model->adjoins[surface.adjoin].flags & flags::adjoin_flag::AllowMovement) &&
				!(presenter.model->adjoins[surface.adjoin].flags & flags::adjoin_flag::AllowAiOnly) &&
				!(surface.flags & flags::surface_flag::Impassable)) {
			// Recurse into adjoined sector.
			find_sector_resting_manifolds(sphere, surface.adjoined_sector, closed, manifolds);
		}
		else if(surf_nearest_dist <= std::get<1>(sphere)) {
			// sphere is resting on surface.
			manifolds.push_back((std::get<0>(sphere) - surf_nearest_point) / surf_nearest_dist);
		}
	}
}

void gorc::game::world::level::gameplay::player_controller::update(int thing_id, double dt) {
	auto& thing = presenter.model->things[thing_id];

	std::vector<vector<3>> resting_manifolds;
	std::set<int> closed_set;
	find_sector_resting_manifolds(physics::sphere(thing.position, thing.size), thing.sector, closed_set, resting_manifolds);

	vector<3> new_thing_vel = thing.thrust;

	// Reject manifold vectors from velocity.
	for(const auto& manifold : resting_manifolds) {
		auto vel_dot = math::dot(new_thing_vel, manifold);
		if(vel_dot < 0.0f) {
			// Reject manifold vector from velocity.
			new_thing_vel -= vel_dot * manifold;
		}
	}

	presenter.adjust_thing_pos(thing_id, thing.position + new_thing_vel * dt);
}
