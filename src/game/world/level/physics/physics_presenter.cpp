#include "cog/verbs/table.h"
#include "game/components.h"
#include "physics_presenter.h"
#include "game/world/level/level_presenter.h"
#include "game/world/level/level_model.h"
#include "game/constants.h"
#include "framework/utility/pair_range.h"
#include "framework/math/matrix.h"
#include "query.h"

using namespace gorc::game::world::level::physics;

physics_presenter::physics_presenter(level_presenter& presenter)
	: presenter(presenter), model(nullptr), physics_anim_node_visitor(*this, physics_thing_resting_manifolds, physics_touched_thing_pairs) {
	return;
}

void physics_presenter::start(level_model& model) {
	this->model = &model;
}

bool physics_presenter::physics_surface_needs_collision_response(int moving_thing_id, int surface_id) {
	const auto& moving_thing = model->things[moving_thing_id];
	const auto& surface = model->surfaces[surface_id];

	if(surface.adjoin < 0) {
		return true;
	}

	const auto& adjoin = model->adjoins[surface.adjoin];

	bool movement_allowed = adjoin.flags & flags::adjoin_flag::AllowMovement;
	bool ai_and_blocked = moving_thing.type == flags::thing_type::Actor && (adjoin.flags & flags::adjoin_flag::AllowPlayerOnly);
	bool player_and_blocked = moving_thing.type == flags::thing_type::Player && (adjoin.flags & flags::adjoin_flag::AllowAiOnly);

	bool surface_ethereal = movement_allowed && !ai_and_blocked && !player_and_blocked;

	return !surface_ethereal;
}

bool physics_presenter::physics_thing_needs_collision_response(int moving_thing_id, int collision_thing_id) {
	const auto& moving_thing = model->things[moving_thing_id];
	const auto& collision_thing = model->things[collision_thing_id];

	auto mt_type = moving_thing.type;
	auto ct_type = collision_thing.type;

	return (mt_type == flags::thing_type::Actor ||
			mt_type == flags::thing_type::Weapon ||
			mt_type == flags::thing_type::Debris ||
			mt_type == flags::thing_type::cog ||
			mt_type == flags::thing_type::Player) &&
			(ct_type == flags::thing_type::Actor ||
			ct_type == flags::thing_type::Weapon ||
			ct_type == flags::thing_type::Debris ||
			ct_type == flags::thing_type::cog ||
			ct_type == flags::thing_type::Player);
}

physics_presenter::physics_node_visitor::physics_node_visitor(physics_presenter& presenter, std::vector<physics::contact>& resting_manifolds,
		std::set<std::tuple<int, int>>& physics_touched_thing_pairs)
	: presenter(presenter), resting_manifolds(resting_manifolds), physics_touched_thing_pairs(physics_touched_thing_pairs) {
	return;
}

void physics_presenter::physics_node_visitor::visit_mesh(const content::assets::model& model, int mesh_id) {
	const auto& mesh = model.geosets.front().meshes[mesh_id];

	for(const auto& face : mesh.faces) {
		auto maybe_face_nearest_point = physics::bounded_closest_point_on_surface(sphere.position, mesh, face, current_matrix, sphere.radius);

		vector<3> face_nearest_point;
		if(maybe_face_nearest_point >> face_nearest_point) {
			auto face_nearest_dist = length(sphere.position - face_nearest_point);
			if(face_nearest_dist <= sphere.radius) {
				if(needs_response) {
					vector<3> contact_pos_vel = presenter.get_thing_path_moving_point_velocity(visited_thing_id, face_nearest_point);
					resting_manifolds.emplace_back((sphere.position - face_nearest_point) / face_nearest_dist, contact_pos_vel, visited_thing_id);
				}
				physics_touched_thing_pairs.emplace(std::min(moving_thing_id, visited_thing_id), std::max(moving_thing_id, visited_thing_id));
			}
		}
	}
}

void physics_presenter::physics_calculate_broadphase(double dt) {
	physics_broadphase_thing_influence.clear();
	physics_broadphase_sector_things.clear();

	// Calculate influence AABBs and record overlapping sectors.
	for(const auto& thing : model->things) {
		auto thing_off_v = make_vector(1.0f, 1.0f, 1.0f) * (thing.move_size + length(thing.vel) * dt);
		auto thing_aabb = make_box(thing.position - thing_off_v, thing.position + thing_off_v);

		physics_thing_closed_set.clear();
		physics_thing_open_set.clear();

		physics_thing_open_set.push_back(thing.sector);

		while(!physics_thing_open_set.empty()) {
			int sector_id = physics_thing_open_set.back();
			physics_thing_open_set.pop_back();

			const auto& sector = model->sectors[sector_id];

			if(physics_thing_closed_set.find(sector_id) != physics_thing_closed_set.end()
					|| !thing_aabb.overlaps(sector.collide_box)) {
				// Thing does not influence sector.
				continue;
			}

			// Thing influences sector. Move to closed set.
			physics_thing_closed_set.emplace(sector_id);
			physics_broadphase_sector_things.emplace(sector_id, thing.get_id());
			physics_broadphase_thing_influence.emplace(thing.get_id(), sector_id);

			// Add adjoining sectors to open set.
			for(int i = sector.first_surface; i < sector.first_surface + sector.surface_count; ++i) {
				const auto& surf = model->surfaces[i];
				if(surf.adjoin >= 0) {
					physics_thing_open_set.push_back(surf.adjoined_sector);
				}
			}
		}
	}

	return;
}

void physics_presenter::physics_find_sector_resting_manifolds(const physics::sphere& sphere, int sector_id, const vector<3>& vel_dir,
		int current_thing_id) {
	// Get list of sectors within thing influence.
	auto thing_influence_range = physics_broadphase_thing_influence.equal_range(current_thing_id);
	for(auto it = std::get<0>(thing_influence_range); it != std::get<1>(thing_influence_range); ++it) {
		const auto& sector = model->sectors[it->second];

		for(int i = sector.first_surface; i < sector.first_surface + sector.surface_count; ++i) {
			const auto& surface = model->surfaces[i];

			if(!physics_surface_needs_collision_response(current_thing_id, i)) {
				continue;
			}

			auto maybe_surf_nearest_point = physics::bounded_closest_point_on_surface(sphere.position, model->level, surface, sphere.radius);
			vector<3> surf_nearest_point;
			if(maybe_surf_nearest_point >> surf_nearest_point) {
				auto surf_nearest_dist = length(sphere.position - surf_nearest_point);

				if(surf_nearest_dist <= sphere.radius) {
					physics_thing_resting_manifolds.emplace_back((sphere.position - surf_nearest_point) / surf_nearest_dist, make_zero_vector<3, float>());
					physics_touched_surface_pairs.emplace(current_thing_id, i);
				}
			}
		}
	}
}

void physics_presenter::physics_find_thing_resting_manifolds(const physics::sphere& sphere, const vector<3>& vel_dir, int current_thing_id) {
	// Get list of things within thing influence.
	physics_overlapping_things.clear();
	auto thing_influence_range = physics_broadphase_thing_influence.equal_range(current_thing_id);
	for(auto it = std::get<0>(thing_influence_range); it != std::get<1>(thing_influence_range); ++it) {
		auto influenced_thing_range = physics_broadphase_sector_things.equal_range(it->second);
		for(auto jt = std::get<0>(influenced_thing_range); jt != std::get<1>(influenced_thing_range); ++jt) {
			physics_overlapping_things.emplace(jt->second);
		}
	}

	for(auto col_thing_id : physics_overlapping_things) {
		auto& col_thing = model->things[col_thing_id];

		if(col_thing.get_id() == current_thing_id) {
			continue;
		}

		// Skip things too far away
		auto vec_to = sphere.position - col_thing.position;
		auto vec_to_len = length(vec_to);
		if(vec_to_len > col_thing.move_size + sphere.radius) {
			continue;
		}

		if(col_thing.collide == flags::collide_type::sphere) {
			if(vec_to_len <= col_thing.size + sphere.radius) {
				// Spheres colliding
				if(physics_thing_needs_collision_response(current_thing_id, col_thing_id)) {
					// Find contact point velocity:
					vector<3> contact_normal = vec_to / vec_to_len;
					vector<3> contact_point = col_thing.position + contact_normal * col_thing.size;

					vector<3> contact_point_vel = make_zero_vector<3, float>();
					if(col_thing.move == flags::move_type::physics) {
						contact_point_vel = col_thing.vel;
					}
					else if(col_thing.move == flags::move_type::Path) {
						contact_point_vel = get_thing_path_moving_point_velocity(col_thing_id, contact_point);
					}

					physics_thing_resting_manifolds.emplace_back(vec_to / vec_to_len, make_zero_vector<3, float>(), col_thing_id);
				}

				physics_touched_thing_pairs.emplace(std::min(current_thing_id, col_thing_id), std::max(current_thing_id, col_thing_id));
			}
		}
		else if(col_thing.collide == flags::collide_type::face) {
			if(!col_thing.model_3d) {
				continue;
			}

			physics_anim_node_visitor.needs_response = physics_thing_needs_collision_response(current_thing_id, col_thing_id);
			physics_anim_node_visitor.sphere = sphere;
			physics_anim_node_visitor.visited_thing_id = col_thing_id;
			physics_anim_node_visitor.moving_thing_id = current_thing_id;
			presenter.key_presenter.visit_mesh_hierarchy(physics_anim_node_visitor, col_thing);
		}
	}
}

void physics_presenter::physics_thing_step(int thing_id, thing& thing, double dt) {
	thing.orientation = thing.orientation + thing.ang_vel * static_cast<float>(dt);
	thing.vel = thing.vel + thing.thrust * static_cast<float>(dt);

	if(thing.physics_flags & flags::physics_flag::has_gravity) {
		thing.vel = thing.vel + make_vector(0.0f, 0.0f, -1.0f) * model->header.world_gravity * static_cast<float>(dt);
	}

	// Only perform collision detection for player, actor, and weapon types.
	if(thing.type != flags::thing_type::Actor && thing.type != flags::thing_type::Player && thing.type != flags::thing_type::Weapon) {
		return;
	}

	if(thing.collide != flags::collide_type::sphere && thing.collide != flags::collide_type::sphere_unknown) {
		// Do not need sphere collision.
		return;
	}

	double dt_remaining = dt;
	double dt_step = static_cast<double>(thing.move_size) / static_cast<double>(length(thing.vel));

	int loop_ct = 0;

	while(dt_remaining > 0.0) {
		++loop_ct;

		double this_step_dt = (dt_remaining < dt_step) ? dt_remaining : dt_step;

		// Do sphere collision:

		physics_thing_resting_manifolds.clear();
		physics_find_sector_resting_manifolds(physics::sphere(thing.position, thing.size), thing.sector, thing.vel, thing_id);
		physics_find_thing_resting_manifolds(physics::sphere(thing.position, thing.size), thing.vel, thing_id);

		vector<3> prev_thing_vel = thing.vel;

		// Add 'towards' velocities from resting contacts.
		for(const auto& manifold : physics_thing_resting_manifolds) {
			auto man_vel_len = length(manifold.velocity);

			if(man_vel_len == 0.0f || dot(manifold.normal, manifold.velocity) < 0.0f) {
				// Not contributing velocity.
				continue;
			}

			auto vel_dot = dot(prev_thing_vel, manifold.velocity);
			auto amt_in_man_vel = vel_dot / man_vel_len;

			if(amt_in_man_vel < man_vel_len) {
				prev_thing_vel += (man_vel_len - amt_in_man_vel) * (manifold.velocity / man_vel_len);
			}
		}

		bool reject_vel = true;

		// Solve LCP, 20 iterations
		for(int i = 0; i < 5; ++i) {
			vector<3> new_computed_vel = prev_thing_vel;
			for(const auto& manifold : physics_thing_resting_manifolds) {
				// Three cases:
				auto vel_dot = dot(new_computed_vel, manifold.normal);
				if(vel_dot < 0.0f) {
					// Reject manifold vector from velocity
					new_computed_vel -= vel_dot * manifold.normal;
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
			thing.vel = prev_thing_vel;
			presenter.adjust_thing_pos(thing_id, thing.position + prev_thing_vel * this_step_dt);
		}
		else {
			thing.vel = make_zero_vector<3, float>();
			break;
		}

		// Check vel to make sure all valid resting velocities are still applied.
		for(const auto& manifold : physics_thing_resting_manifolds) {
			auto man_vel_len = length(manifold.velocity);

			if(man_vel_len == 0.0f || dot(manifold.normal, manifold.velocity) < 0.0f) {
				// Not contributing velocity.
				continue;
			}

			auto vel_dot = dot(prev_thing_vel, manifold.velocity);
			auto amt_in_man_vel = vel_dot / man_vel_len;

			if(amt_in_man_vel < man_vel_len) {
				// Thing is blocked.
				manifold.contact_thing_id.if_set([this](int contact_thing_id) {
					auto& contact_thing = model->things[contact_thing_id];
					contact_thing.is_blocked = true;
				});
			}
		}

		dt_remaining -= dt_step;
	}
}

void physics_presenter::update_thing_path_moving(int thing_id, thing& thing, double dt) {
	if(thing.move != flags::move_type::Path || !thing.path_moving || thing.is_blocked) {
		return;
	}

	auto target_position_tuple = thing.frames[thing.next_frame];
	vector<3> targetPosition = std::get<0>(target_position_tuple);
	vector<3> targetOrientation = std::get<1>(target_position_tuple);

	vector<3> currentPosition = thing.position;
	vector<3> currentOrientation = thing.orientation;

	// PathMoveSpeed seems to be some factor of distance per frame, and Jedi has a different framerate.
	// Use a magic multiple to correct it.
	float dist_len = length(targetPosition - currentPosition);
	float alpha = rate_factor * dt * thing.path_move_speed / dist_len;
	if(alpha >= 1.0f || dist_len == 0.0f) {
		thing.position = targetPosition;
		thing.orientation = targetOrientation;

		// Arrived at next frame. Advance to next.
		thing.current_frame = thing.next_frame;
		if(thing.current_frame == thing.goal_frame) {
			thing.path_moving = false;
			thing.path_move_speed = 0.0f;
			presenter.sound_presenter.stop_foley_loop(thing_id);
			presenter.sound_presenter.play_sound_class(thing_id, flags::sound_subclass_type::StopMove);

			// Dispatch cog messages and resume cogs which are waiting for stop.
			presenter.script_presenter.send_message_to_linked(cog::message_id::arrived, static_cast<int>(thing_id), flags::message_type::thing);
			presenter.script_presenter.resume_wait_for_stop(thing_id);
		}
		else if(thing.current_frame < thing.goal_frame) {
			thing.next_frame = thing.current_frame + 1;
		}
		else {
			thing.next_frame = thing.current_frame - 1;
		}
	}
	else {
		thing.position = lerp(thing.position, targetPosition, alpha);
		thing.orientation = lerp(thing.orientation, targetOrientation, alpha);
	}
}

gorc::vector<3> physics_presenter::get_thing_path_moving_point_velocity(int thing_id, const vector<3>& rel_point) {
	auto& thing = model->things[thing_id];

	if(thing.move == flags::move_type::Path && thing.path_moving && !thing.is_blocked) {
		auto target_position_tuple = thing.frames[thing.next_frame];
		vector<3> targetPosition = std::get<0>(target_position_tuple);
		vector<3> targetOrientation = std::get<1>(target_position_tuple);

		vector<3> currentPosition = thing.position;
		vector<3> currentOrientation = thing.orientation;

		// PathMoveSpeed seems to be some factor of distance per frame, and Jedi has a different framerate.
		// Use a magic multiple to correct it.
		float dist_len = length(targetPosition - currentPosition);
		float alpha = rate_factor * thing.path_move_speed / dist_len;

		auto delta_position = lerp(thing.position, targetPosition, alpha);
		auto delta_orientation = lerp(thing.orientation, targetOrientation, alpha);

		// Rotate rel_point into object space
		auto rel_point_rotated = (
				make_rotation_matrix(get<2>(-currentOrientation), make_vector(0.0f, 1.0f, 0.0f))
				* make_rotation_matrix(get<0>(-currentOrientation), make_vector(1.0f, 0.0f, 0.0f))
				* make_rotation_matrix(get<1>(-currentOrientation), make_vector(0.0f, 0.0f, 1.0f))
				* make_translation_matrix(-currentPosition)).transform(rel_point);

		auto rel_v = (make_translation_matrix(delta_position)
			* make_rotation_matrix(get<1>(delta_orientation), make_vector(0.0f, 0.0f, 1.0f))
			* make_rotation_matrix(get<0>(delta_orientation), make_vector(1.0f, 0.0f, 0.0f))
			* make_rotation_matrix(get<2>(delta_orientation), make_vector(0.0f, 1.0f, 0.0f))
			).transform(rel_point_rotated) - rel_point;
		return rel_v;
	}

	return make_zero_vector<3, float>();
}

void physics_presenter::update(double dt) {
	physics_touched_thing_pairs.clear();
	physics_touched_surface_pairs.clear();

	// General approach:

	// - Clear blocked flag on all path things.
	for(auto& thing : model->things) {
		thing.is_blocked = false;
	}

	// - Calculate potentially overlapping pairs (broadphase)
	physics_calculate_broadphase(dt);

	// - Rectify physics thing position vs. velocity, resting contacts, etc.
	for(auto& thing : model->things) {
		if(thing.move == flags::move_type::physics) {
			physics_thing_step(thing.get_id(), thing, dt);
		}
	}

	// - Send blocked message to all blocked things; else, update path.
	for(auto& thing : model->things) {
		if(thing.is_blocked) {
			presenter.script_presenter.send_message_to_linked(cog::message_id::blocked, thing.get_id(), flags::message_type::thing,
					-1, flags::message_type::nothing);
		}
		else if(thing.path_moving) {
			update_thing_path_moving(thing.get_id(), thing, dt);
		}
	}

	// Dispatch touched messages
	for(const auto& touched_surface_pair : physics_touched_surface_pairs) {
		int thing_id, surf_id;
		std::tie(thing_id, surf_id) = touched_surface_pair;
		auto& thing = model->things[thing_id];
		thing.controller->touched_surface(thing_id, surf_id);
	}

	for(const auto& touched_thing_pair : physics_touched_thing_pairs) {
		int thing_a_id, thing_b_id;
		std::tie(thing_a_id, thing_b_id) = touched_thing_pair;

		model->things[thing_a_id].controller->touched_thing(thing_a_id, thing_b_id);
		model->things[thing_b_id].controller->touched_thing(thing_b_id, thing_a_id);
	}
}

void physics_presenter::register_verbs(cog::verbs::verb_table& verbTable, components& components) {
}
