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

	vector<3> prev_thing_vel = thing.thrust;
	bool reject_vel = true;

	// Solve LCP, 10 iterations
	for(int i = 0; i < 10; ++i) {
		vector<3> new_computed_vel = prev_thing_vel;
		for(const auto& manifold : resting_manifolds) {
			auto vel_dot = math::dot(new_computed_vel, manifold);
			if(vel_dot < 0.0f) {
				// Reject manifold vector from velocity
				new_computed_vel -= vel_dot * manifold;
			}
		}

		if(length2(new_computed_vel - prev_thing_vel) < 0.000001f) {
			// Solution has stabilized
			reject_vel = false;
			break;
		}
		else {
			// Solution has not yet stabilized.
			prev_thing_vel = new_computed_vel;
		}
	}

	if(!reject_vel) {
		presenter.adjust_thing_pos(thing_id, thing.position + prev_thing_vel * dt);
	}
}
