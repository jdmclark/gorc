#include "cog_controller.h"
#include "game/world/level/level_presenter.h"
#include "game/world/level/thing.h"
#include "game/world/level/level_model.h"
#include "game/constants.h"
#include "framework/math/util.h"
#include <iostream>

using namespace gorc::math;

void gorc::game::world::level::gameplay::cog_controller::update(int thing_id, double dt) {
	thing_controller::update(thing_id, dt);

	thing& thing = presenter.model->things[thing_id];
	if(thing.path_moving) {
		update_thing_path_moving(thing_id, thing, dt);
	}

	// update thing attachment motion
	if(thing.attach_flags & flags::attach_flag::AttachedToThing) {
		class thing& attached_thing = presenter.model->things[thing.attached_thing];
		thing.position = attached_thing.position - thing.prev_attached_thing_position + thing.position;
		thing.prev_attached_thing_position = attached_thing.position;
	}
}

void gorc::game::world::level::gameplay::cog_controller::update_thing_path_moving(int thing_id, thing& thing, double dt) {
	auto target_position_tuple = thing.frames[thing.next_frame];
	vector<3> targetPosition = std::get<0>(target_position_tuple);
	vector<3> targetOrientation = std::get<1>(target_position_tuple);

	vector<3> currentPosition = thing.position;
	vector<3> currentOrientation = thing.orientation;

	// PathMoveSpeed seems to be some factor of distance per frame, and Jedi has a different framerate.
	// Use a magic multiple to correct it.
	float dist_len = math::length(targetPosition - currentPosition);
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
		thing.position = math::lerp(thing.position, targetPosition, alpha);
		thing.orientation = math::lerp(thing.orientation, targetOrientation, alpha);
	}
}

void gorc::game::world::level::gameplay::cog_controller::create_controller_data(int thing_id) {
	return;
}
