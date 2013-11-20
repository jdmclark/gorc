#include "cog/verbs/table.h"
#include "game/application.h"
#include "physics_presenter.h"
#include "game/world/level_presenter.h"
#include "game/world/level_model.h"
#include "game/constants.h"
#include "framework/utility/pair_range.h"
#include "framework/math/matrix.h"
#include "query.h"

using namespace gorc::game::world::physics;

physics_presenter::physics_presenter(level_presenter& presenter)
	: presenter(presenter), model(nullptr), physics_anim_node_visitor(*this, physics_thing_resting_manifolds, physics_touched_thing_pairs),
	  segment_query_anim_node_visitor(*this) {
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
			mt_type == flags::thing_type::Debris ||
			mt_type == flags::thing_type::cog ||
			mt_type == flags::thing_type::Player) &&
			(ct_type == flags::thing_type::Actor ||
			ct_type == flags::thing_type::Debris ||
			ct_type == flags::thing_type::cog ||
			ct_type == flags::thing_type::Player);
}

physics_presenter::physics_node_visitor::physics_node_visitor(physics_presenter& presenter, std::vector<physics::contact>& resting_manifolds,
		std::set<std::tuple<int, int>>& physics_touched_thing_pairs)
	: presenter(presenter), resting_manifolds(resting_manifolds), physics_touched_thing_pairs(physics_touched_thing_pairs) {
	return;
}

void physics_presenter::physics_node_visitor::visit_mesh(const content::assets::model& model, int mesh_id, int node_id) {
	const auto& mesh = model.geosets.front().meshes[mesh_id];

	for(const auto& face : mesh.faces) {
		auto maybe_face_nearest_point = physics::bounded_closest_point_on_surface(sphere.position, mesh, face, current_matrix, sphere.radius);

		vector<3> face_nearest_point;
		if(maybe_face_nearest_point >> face_nearest_point) {
			auto face_nearest_dist = length(sphere.position - face_nearest_point);
			if(face_nearest_dist <= sphere.radius) {
				if(needs_response) {
					vector<3> contact_pos_vel = presenter.get_thing_path_moving_point_velocity(visited_thing_id, face_nearest_point);
					vector<3> nrm = current_matrix.transform_normal(face.normal);
					vector<3> nrm_vel = nrm * ((sphere.radius / face_nearest_dist) - 1.0f);

					resting_manifolds.emplace_back(face_nearest_point, (sphere.position - face_nearest_point) / face_nearest_dist,
							contact_pos_vel + nrm_vel);
					resting_manifolds.back().contact_thing_id = make_maybe(visited_thing_id);
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
					physics_thing_resting_manifolds.emplace_back(surf_nearest_point, (sphere.position - surf_nearest_point) / surf_nearest_dist,
							surface.normal * ((sphere.radius / surf_nearest_dist) - 1.0f));
					physics_thing_resting_manifolds.back().contact_surface_id = make_maybe(i);
					physics_touched_surface_pairs.emplace(current_thing_id, i);
				}
			}
		}
	}
}

void physics_presenter::physics_find_thing_resting_manifolds(const physics::sphere& sphere, const vector<3>& vel_dir, int current_thing_id) {
	auto& current_thing = model->things[current_thing_id];

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
		if(vec_to_len > (col_thing.size + sphere.radius)) {
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

					physics_thing_resting_manifolds.emplace_back(contact_point, vec_to / vec_to_len, contact_point_vel);
					physics_thing_resting_manifolds.back().contact_thing_id = make_maybe(col_thing_id);
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
			presenter.key_presenter.visit_mesh_hierarchy(physics_anim_node_visitor, *col_thing.model_3d, col_thing.position, col_thing.orient, col_thing.attached_key_mix);
		}
	}
}

void physics_presenter::physics_thing_step(int thing_id, thing& thing, double dt) {
	// TODO: Move somewhere more appropriate.
	thing.orient = thing.orient + thing.ang_vel * static_cast<float>(dt);
	//thing.vel = thing.vel + thing.thrust * static_cast<float>(dt);

	// Clamp thrusts.
	thing.rot_thrust = clamp_length(thing.rot_thrust, 0.0f, thing.max_rot_thrust);
	//thing.thrust = clamp_length(thing.thrust, 0.0f, thing.max_thrust);

	// Apply thrusts.
	if(thing.physics_flags & flags::physics_flag::uses_angular_thrust_to_rotate) {
		thing.ang_vel = thing.ang_vel + thing.rot_thrust * static_cast<float>(dt);
	}

	if((thing.physics_flags & flags::physics_flag::has_gravity) && static_cast<int>(thing.attach_flags) == 0) {
		thing.vel = thing.vel + make_vector(0.0f, 0.0f, -1.0f) * model->header.world_gravity * static_cast<float>(dt);
	}

	// TODO: Linear thrust

	// Clamp vel.
	// TODO: Must be some unknown flag. Objects move way too slowly.
	//thing.vel = clamp_length(thing.vel, 0.0f, thing.max_vel);
	thing.ang_vel = clamp_length(thing.ang_vel, 0.0f, thing.max_rot_vel);

	// Apply vels.

	if(thing.physics_flags & flags::physics_flag::uses_rotational_velocity) {
		thing.orient = thing.orient + thing.ang_vel * static_cast<float>(dt);
	}

	auto this_frame_only_vel = make_zero_vector<3, float>();
	if((thing.attach_flags & flags::attach_flag::AttachedToThing) || (thing.attach_flags & flags::attach_flag::AttachedToThingFace)) {
		const auto& parent_thing = model->things[thing.attached_thing];
		this_frame_only_vel = (parent_thing.position - thing.prev_attached_thing_position) / dt;
		thing.prev_attached_thing_position = parent_thing.position;
	}

	thing.vel += this_frame_only_vel;

	// Only perform collision detection for player, actor, and weapon types.
	if(thing.type != flags::thing_type::Actor && thing.type != flags::thing_type::Player && thing.type != flags::thing_type::Weapon) {
		return;
	}

	if(thing.collide != flags::collide_type::sphere && thing.collide != flags::collide_type::sphere_unknown) {
		// Do not need sphere collision.
		return;
	}

	double dt_remaining = dt;
	double dt_step = 0.5 * static_cast<double>(thing.move_size) / static_cast<double>(length(thing.vel));

	int loop_ct = 0;

	while(dt_remaining > 0.0) {
		++loop_ct;

		double this_step_dt = (dt_remaining < dt_step) ? dt_remaining : dt_step;

		// Do sphere collision:

		physics_thing_resting_manifolds.clear();
		physics_find_sector_resting_manifolds(physics::sphere(thing.position, thing.size), thing.sector, thing.vel, thing_id);
		physics_find_thing_resting_manifolds(physics::sphere(thing.position, thing.size), thing.vel, thing_id);

		vector<3> prev_thing_vel = thing.vel;

		bool influenced_by_manifolds = false;

		// Add 'towards' velocities from resting contacts, projected into manifold direction.
		for(const auto& manifold : physics_thing_resting_manifolds) {
			auto man_vel_len = dot(manifold.velocity, manifold.normal);

			if(man_vel_len <= 0.0f) {
				// Not contributing velocity.
				continue;
			}

			auto man_vel = manifold.normal * man_vel_len;

			auto vel_dot = dot(prev_thing_vel, man_vel);
			auto amt_in_man_vel = vel_dot / man_vel_len;

			if(amt_in_man_vel < man_vel_len) {
				influenced_by_manifolds = true;
				prev_thing_vel += (man_vel_len - amt_in_man_vel) * (man_vel / man_vel_len);
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
					// Reject manifold vector from velocity.
					// But just enough to prevent drift.
					auto vel_man_vel_dot = dot(new_computed_vel, manifold.velocity);
					if(vel_man_vel_dot <= 0.0f) {
						// Object is being stopped.
						new_computed_vel -= vel_dot * manifold.normal;
						influenced_by_manifolds = true;
					}
					else {
						// Object is possibly being slowed.
						float manifold_vel_len = length(manifold.velocity);
						auto nrm_manifold_vel = manifold.velocity / manifold_vel_len;
						auto nrm_man_vel_dot = vel_man_vel_dot / manifold_vel_len;
						if(nrm_man_vel_dot > manifold_vel_len) {
							// Object is being slowed.
							new_computed_vel -= nrm_manifold_vel * (nrm_man_vel_dot - manifold_vel_len);
							influenced_by_manifolds = true;
						}
					}
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

		// HACK: if thing is projectile and there is a valid resting manifold,
		// stop at the current position and process collisions.
		if(thing.type == flags::thing_type::Weapon && (influenced_by_manifolds || reject_vel)) {
			break;
		}
		else {
			if(!reject_vel) {
				thing.vel = prev_thing_vel;
				presenter.adjust_thing_pos(thing_id, thing.position + prev_thing_vel * this_step_dt);
			}
			else {
				thing.vel = make_zero_vector<3, float>();
				break;
			}
		}

		// Check vel to make sure all valid resting velocities are still applied.
		for(const auto& manifold : physics_thing_resting_manifolds) {
			auto man_vel_len = dot(manifold.velocity, manifold.normal);

			if(man_vel_len <= 0.0f) {
				// Not contributing velocity.
				continue;
			}

			auto man_vel = manifold.normal * man_vel_len;

			auto vel_dot = dot(prev_thing_vel, man_vel);
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

	thing.vel -= this_frame_only_vel;
}

void physics_presenter::update_thing_path_moving(int thing_id, thing& thing, double dt) {
	if(thing.move != flags::move_type::Path || thing.is_blocked) {
		return;
	}

	if(thing.path_moving) {
		auto target_position_tuple = thing.frames[thing.next_frame];
		vector<3> targetPosition = std::get<0>(target_position_tuple);
		vector<3> targetOrientation = std::get<1>(target_position_tuple);

		vector<3> currentPosition = thing.position;
		vector<3> currentOrientation = thing.orient;

		// PathMoveSpeed seems to be some factor of distance per frame, and Jedi has a different framerate.
		// Use a magic multiple to correct it.
		float dist_len = length(targetPosition - currentPosition);
		float alpha = rate_factor * dt * thing.path_move_speed / dist_len;
		if(alpha >= 1.0f || dist_len == 0.0f) {
			presenter.adjust_thing_pos(thing_id, targetPosition);
			thing.orient = targetOrientation;

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
			presenter.adjust_thing_pos(thing_id, lerp(thing.position, targetPosition, alpha));
			thing.orient = lerp(thing.orient, targetOrientation, alpha);
		}
	}
	else if(thing.rotatepivot_moving) {
		float alpha = dt / thing.path_move_speed;

		if(thing.path_move_time >= thing.path_move_speed) {

			thing.rotatepivot_moving = false;

			vector<3> frame_pos, frame_orient;
			std::tie(frame_pos, frame_orient) = thing.frames[thing.goal_frame];

			if(thing.rotatepivot_longway) {
				frame_orient = -frame_orient;
			}

			auto angle = frame_orient * (thing.path_move_speed - thing.path_move_time + dt) / thing.path_move_speed;
			auto new_pos = orient_direction_vector(thing.position - frame_pos, angle) + frame_pos;

			thing.orient += angle;
			presenter.adjust_thing_pos(thing_id, new_pos);

			presenter.sound_presenter.stop_foley_loop(thing_id);
			presenter.sound_presenter.play_sound_class(thing_id, flags::sound_subclass_type::StopMove);

			// Dispatch cog messages and resume cogs which are waiting for stop.
			presenter.script_presenter.send_message_to_linked(cog::message_id::arrived, static_cast<int>(thing_id), flags::message_type::thing);
			presenter.script_presenter.resume_wait_for_stop(thing_id);
		}
		else {
			vector<3> frame_pos, frame_orient;
			std::tie(frame_pos, frame_orient) = thing.frames[thing.goal_frame];

			if(thing.rotatepivot_longway) {
				frame_orient = -frame_orient;
			}

			auto angle = frame_orient * alpha;

			auto new_pos = orient_direction_vector(thing.position - frame_pos, angle) + frame_pos;

			thing.orient += angle;
			presenter.adjust_thing_pos(thing_id, new_pos);

			thing.path_move_time += dt;
		}
	}
}

gorc::vector<3> physics_presenter::get_thing_path_moving_point_velocity(int thing_id, const vector<3>& rel_point) {
	auto& thing = model->things[thing_id];

	if(thing.move == flags::move_type::Path && thing.path_moving && !thing.is_blocked) {
		auto target_position_tuple = thing.frames[thing.next_frame];
		vector<3> targetPosition = std::get<0>(target_position_tuple);
		vector<3> targetOrientation = std::get<1>(target_position_tuple);

		vector<3> currentPosition = thing.position;
		vector<3> currentOrientation = thing.orient;

		// PathMoveSpeed seems to be some factor of distance per frame, and Jedi has a different framerate.
		// Use a magic multiple to correct it.
		float dist_len = length(targetPosition - currentPosition);
		float alpha = rate_factor * thing.path_move_speed / dist_len;

		auto delta_position = lerp(thing.position, targetPosition, alpha);
		auto delta_orientation = lerp(thing.orient, targetOrientation, alpha);

		// Rotate rel_point into object space
		auto rel_point_rotated = (
				make_rotation_matrix(get<1>(-currentOrientation), make_vector(0.0f, 0.0f, 1.0f))
				* make_rotation_matrix(get<0>(-currentOrientation), make_vector(1.0f, 0.0f, 0.0f))
				* make_rotation_matrix(get<2>(-currentOrientation), make_vector(0.0f, 1.0f, 0.0f))
				* make_translation_matrix(-currentPosition)).transform(rel_point);

		auto rel_v = (make_translation_matrix(delta_position)
			* make_rotation_matrix(get<1>(delta_orientation), make_vector(0.0f, 0.0f, 1.0f))
			* make_rotation_matrix(get<0>(delta_orientation), make_vector(1.0f, 0.0f, 0.0f))
			* make_rotation_matrix(get<2>(delta_orientation), make_vector(0.0f, 1.0f, 0.0f))
			).transform(rel_point_rotated) - rel_point;
		return rel_v;
	}
	else if(thing.move == flags::move_type::Path && thing.rotatepivot_moving && !thing.is_blocked) {
		vector<3> frame_pos, frame_orient;
		std::tie(frame_pos, frame_orient) = thing.frames[thing.goal_frame];
		auto angle = frame_orient / thing.path_move_speed;

		auto obj_space_rel_point = rel_point - thing.position;
		obj_space_rel_point = orient_direction_vector(obj_space_rel_point, angle);

		auto pivot_space_rel_point = (obj_space_rel_point + thing.position) - frame_pos;
		pivot_space_rel_point = orient_direction_vector(pivot_space_rel_point, angle);

		auto new_rel_point = pivot_space_rel_point + frame_pos;
		return new_rel_point - rel_point;
	}


	return make_zero_vector<3, float>();
}

void physics_presenter::update(const time& time) {
	double dt = time;

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
		if(thing.is_blocked && (thing.path_moving || thing.rotatepivot_moving)) {
			presenter.script_presenter.send_message_to_linked(cog::message_id::blocked, thing.get_id(), flags::message_type::thing,
					-1, flags::message_type::nothing);
		}
		else if(thing.path_moving || thing.rotatepivot_moving) {
			update_thing_path_moving(thing.get_id(), thing, dt);
		}
		else if(thing.attach_flags & flags::attach_flag::AttachedToThing) {
			const auto& parent_thing = model->things[thing.attached_thing];
			presenter.adjust_thing_pos(thing.get_id(), thing.position + (parent_thing.position - thing.prev_attached_thing_position));
			thing.prev_attached_thing_position = parent_thing.position;
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

physics_presenter::segment_query_node_visitor::segment_query_node_visitor(physics_presenter& presenter)
	: presenter(presenter) {
	return;
}

void physics_presenter::segment_query_node_visitor::visit_mesh(const content::assets::model& model, int mesh_id, int node_id) {
	const auto& mesh = model.geosets.front().meshes[mesh_id];

	for(const auto& face : mesh.faces) {
		auto maybe_nearest_point = physics::segment_surface_intersection_point(cam_segment, mesh, face, current_matrix);
		vector<3> nearest_point;
		if(maybe_nearest_point >> nearest_point) {
			auto dist = length(nearest_point - std::get<0>(cam_segment));
			if(dist < closest_contact_distance) {
				closest_contact_distance = dist;
				has_closest_contact = true;
				closest_contact = nearest_point;
				closest_contact_normal = current_matrix.transform_normal(face.normal);
			}
		}
	}
}

gorc::maybe<contact> physics_presenter::segment_query(const segment& cam_segment, int initial_sector, int ray_cast_thing, const maybe<contact>& prev_contact) {
	// Search for closest thing-ray intersection.
	float query_radius = length(std::get<1>(cam_segment) - std::get<0>(cam_segment));

	float closest_contact_distance = std::numeric_limits<float>::max();
	bool has_contact = false;
	int closest_contact_thing_id = -1;
	int closest_contact_surface_id = -1;
	vector<3> closest_contact_position;
	vector<3> closest_contact_normal;

	prev_contact.if_set([&](const contact& prev_ct) {
		closest_contact_distance = length(prev_ct.position - std::get<0>(cam_segment));
		has_contact = true;
		prev_ct.contact_thing_id >> closest_contact_thing_id;
		prev_ct.contact_surface_id >> closest_contact_surface_id;
		closest_contact_position = prev_ct.position;
		closest_contact_normal = prev_ct.normal;
	});

	// Find contact among sectors.
	// Get list of sectors within thing influence.
	segment_query_closed_sectors.clear();
	segment_query_open_sectors.clear();
	segment_query_open_sectors.emplace_back(initial_sector);
	while(!segment_query_open_sectors.empty()) {
		int current_sector = segment_query_open_sectors.back();
		segment_query_open_sectors.pop_back();

		if(segment_query_closed_sectors.find(current_sector) != segment_query_closed_sectors.end()) {
			continue;
		}

		segment_query_closed_sectors.emplace(current_sector);

		const auto& sector = model->sectors[current_sector];

		for(int i = sector.first_surface; i < sector.first_surface + sector.surface_count; ++i) {
			const auto& surface = model->surfaces[i];

			auto maybe_nearest_point = physics::segment_surface_intersection_point(cam_segment, model->level, surface);
			vector<3> nearest_point;
			if(maybe_nearest_point >> nearest_point) {
				if(physics_surface_needs_collision_response(ray_cast_thing, i)) {
					auto dist = length(nearest_point - std::get<0>(cam_segment));
					if(dist < closest_contact_distance) {
						closest_contact_distance = dist;
						has_contact = true;
						closest_contact_surface_id = i;
						closest_contact_position = nearest_point;
						closest_contact_normal = surface.normal;
					}
				}
				else if(surface.adjoin >= 0) {
					segment_query_open_sectors.push_back(surface.adjoined_sector);
				}
			}
		}
	}

	// Find contact among things.
	// Get list of things within thing influence.
	physics_overlapping_things.clear();
	for(auto sector_id : segment_query_closed_sectors) {
		auto influenced_thing_range = physics_broadphase_sector_things.equal_range(sector_id);
		for(auto jt = std::get<0>(influenced_thing_range); jt != std::get<1>(influenced_thing_range); ++jt) {
			physics_overlapping_things.emplace(jt->second);
		}
	}

	for(auto col_thing_id : physics_overlapping_things) {
		auto& col_thing = model->things[col_thing_id];

		if(col_thing.get_id() == ray_cast_thing || !physics_thing_needs_collision_response(ray_cast_thing, col_thing_id)) {
			continue;
		}

		if(col_thing.collide == flags::collide_type::sphere) {
			auto maybe_int = segment_sphere_intersection(cam_segment, sphere(col_thing.position, col_thing.size));
			vector<3> int_point;
			if(maybe_int >> int_point) {
				// Sphere intersected.
				float col_dist = length(int_point - std::get<0>(cam_segment));
				if(col_dist < closest_contact_distance) {
					closest_contact_distance = col_dist;
					has_contact = true;
					closest_contact_surface_id = -1;
					closest_contact_thing_id = col_thing_id;
					closest_contact_normal = normalize(int_point - col_thing.position);
					closest_contact_position = int_point;
				}
			}
		}
		else if(col_thing.collide == flags::collide_type::face) {
			if(!col_thing.model_3d) {
				continue;
			}

			segment_query_anim_node_visitor.cam_segment = cam_segment;
			segment_query_anim_node_visitor.closest_contact_distance = closest_contact_distance;
			segment_query_anim_node_visitor.has_closest_contact = false;
			presenter.key_presenter.visit_mesh_hierarchy(segment_query_anim_node_visitor, *col_thing.model_3d, col_thing.position,
					col_thing.orient, col_thing.attached_key_mix);

			if(segment_query_anim_node_visitor.has_closest_contact) {
				closest_contact_distance = segment_query_anim_node_visitor.closest_contact_distance;
				has_contact = true;
				closest_contact_surface_id = -1;
				closest_contact_thing_id = col_thing_id;
				closest_contact_normal = segment_query_anim_node_visitor.closest_contact_normal;
				closest_contact_position = segment_query_anim_node_visitor.closest_contact;
			}
		}
	}

	if(has_contact) {
		contact new_contact(closest_contact_position, closest_contact_normal, make_zero_vector<3, float>());
		if(closest_contact_surface_id >= 0) {
			new_contact.contact_surface_id = make_maybe(closest_contact_surface_id);
		}

		if(closest_contact_thing_id >= 0) {
			new_contact.contact_thing_id = make_maybe(closest_contact_thing_id);
		}

		return make_maybe(new_contact);
	}

	return maybe<contact>();
}

gorc::maybe<contact> physics_presenter::thing_segment_query(int current_thing_id, const vector<3>& direction, const maybe<contact>& prev_contact) {
	const auto& current_thing = model->things[current_thing_id];
	return segment_query(segment(current_thing.position, current_thing.position + direction), current_thing.sector, current_thing_id, prev_contact);
}

void physics_presenter::register_verbs(cog::verbs::verb_table& verbTable, application& components) {
}
