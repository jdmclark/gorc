#include "player_controller.h"
#include "game/world/level/level_presenter.h"
#include "game/world/level/level_model.h"
#include "game/world/level/physics/query.h"

gorc::game::world::level::gameplay::player_controller::player_controller(level_presenter& presenter)
	: character_controller(presenter), anim_node_visitor(resting_manifolds) {
	return;
}

gorc::game::world::level::gameplay::player_controller::node_visitor::node_visitor(std::vector<vector<3>>& resting_manifolds)
	: resting_manifolds(resting_manifolds) {
	return;
}

void gorc::game::world::level::gameplay::player_controller::node_visitor::visit_mesh(const content::assets::model& model, int mesh_id) {
	const auto& mesh = model.geosets.front().meshes[mesh_id];

	for(const auto& face : mesh.faces) {
		auto face_nearest_point = physics::closest_point_on_surface(std::get<0>(sphere), mesh, face, current_matrix);
		auto face_nearest_dist = length(std::get<0>(sphere) - face_nearest_point);

		if(face_nearest_dist <= std::get<1>(sphere)) {
			resting_manifolds.push_back((std::get<0>(sphere) - face_nearest_point) / face_nearest_dist);
		}
	}
}

void gorc::game::world::level::gameplay::player_controller::find_sector_resting_manifolds(const physics::sphere& sphere, int sector_id,
		std::set<int>& closed, const vector<3>& vel_dir) {
	if(closed.find(sector_id) != closed.end()) {
		return;
	}

	closed.insert(sector_id);

	const auto& sector = presenter.model->sectors[sector_id];
	for(int i = sector.first_surface; i < sector.first_surface + sector.surface_count; ++i) {
		const auto& surface = presenter.model->surfaces[i];

		auto surf_nearest_point = physics::closest_point_on_surface(std::get<0>(sphere), presenter.model->level, surface, make_identity_matrix<4>());
		auto surf_nearest_dist = length(std::get<0>(sphere) - surf_nearest_point);

		if(		// Passable:
				surf_nearest_dist <= std::get<1>(sphere) &&
				surface.adjoin >= 0 &&
				(presenter.model->adjoins[surface.adjoin].flags & flags::adjoin_flag::AllowMovement) &&
				!(presenter.model->adjoins[surface.adjoin].flags & flags::adjoin_flag::AllowAiOnly) &&
				!(surface.flags & flags::surface_flag::Impassable)) {
			// Recurse into adjoined sector.
			find_sector_resting_manifolds(sphere, surface.adjoined_sector, closed, vel_dir);
		}
		else if(surf_nearest_dist <= std::get<1>(sphere)) {
			// sphere is resting on surface.
			resting_manifolds.push_back((std::get<0>(sphere) - surf_nearest_point) / surf_nearest_dist);
		}
	}
}

void gorc::game::world::level::gameplay::player_controller::find_thing_resting_manifolds(const physics::sphere& sphere, std::set<int>& closed,
		const vector<3>& vel_dir, int current_thing_id) {
	for(auto& col_thing : presenter.model->things) {
		if(col_thing.get_id() == current_thing_id) {
			// TODO: Skip things in inaccessible sectors
			continue;
		}

		// Skip things too far away
		auto vec_to = std::get<0>(sphere) - col_thing.position;
		auto vec_to_len = length(vec_to);
		if(vec_to_len > col_thing.move_size + std::get<1>(sphere)) {
			continue;
		}

		if(col_thing.collide == flags::collide_type::sphere) {
			auto vec_to = std::get<0>(sphere) - col_thing.position;
			auto vec_to_len = length(vec_to);
			if(vec_to_len <= col_thing.size + std::get<1>(sphere)) {
				// Spheres colliding
				resting_manifolds.push_back(vec_to / vec_to_len);
			}
		}
		else if(col_thing.collide == flags::collide_type::face) {
			if(!col_thing.model_3d) {
				continue;
			}

			anim_node_visitor.sphere = sphere;
			presenter.key_presenter.visit_mesh_hierarchy(anim_node_visitor, col_thing);
		}
	}
}

void gorc::game::world::level::gameplay::player_controller::step_physics(int thing_id, double dt) {
	auto& thing = presenter.model->things[thing_id];

	auto thing_vel = thing.thrust;

	resting_manifolds.clear();
	closed_set.clear();
	find_sector_resting_manifolds(physics::sphere(thing.position, thing.size), thing.sector, closed_set, thing_vel);
	find_thing_resting_manifolds(physics::sphere(thing.position, thing.size), closed_set, thing_vel, thing_id);

	vector<3> prev_thing_vel = thing.thrust;
	bool reject_vel = true;

	// Solve LCP, 20 iterations
	for(int i = 0; i < 20; ++i) {
		vector<3> new_computed_vel = prev_thing_vel;
		for(const auto& manifold : resting_manifolds) {
			auto vel_dot = dot(new_computed_vel, manifold);
			if(vel_dot < 0.0f) {
				// Reject manifold vector from velocity
				new_computed_vel -= vel_dot * manifold;
			}
		}

		if(length_squared(new_computed_vel - prev_thing_vel) < 0.000001f) {
			// Solution has stabilized
			prev_thing_vel = new_computed_vel;
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

void gorc::game::world::level::gameplay::player_controller::update(int thing_id, double dt) {
	constexpr int step_ct = 20;
	auto dt_step = dt / static_cast<double>(step_ct);
	for(int i = 0; i < step_ct; ++i) {
		step_physics(thing_id, dt_step);
	}
}
