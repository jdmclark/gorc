#include "character_controller.h"
#include "game/world/level/level_presenter.h"
#include "game/world/level/level_model.h"
#include "game/constants.h"

void gorc::game::world::level::gameplay::character_controller::play_running_animation(int thing_id, thing& thing, double speed) {
	if(thing.actor_walk_animation >= 0) {
		keys::key_state& keyState = presenter.model->key_model.keys[thing.actor_walk_animation];
		const content::assets::puppet_submode& submode = thing.puppet->get_mode(flags::puppet_mode_type::Default).get_submode(flags::puppet_submode_type::Run);
		if(keyState.animation != submode.animation) {
			keyState.animation_time = 0.0;
		}

		keyState.animation = submode.animation;
		keyState.high_priority = submode.hi_priority;
		keyState.low_priority = submode.lo_priority;
		keyState.flags = flag_set<flags::key_flag>();
		keyState.speed = speed;
	}
}

void gorc::game::world::level::gameplay::character_controller::play_standing_animation(int thing_id, thing& thing) {
	if(thing.actor_walk_animation >= 0) {
		keys::key_state& keyState = presenter.model->key_model.keys[thing.actor_walk_animation];
		const content::assets::puppet_submode& submode = thing.puppet->get_mode(flags::puppet_mode_type::Default).get_submode(flags::puppet_submode_type::Stand);
		keyState.speed = 1.0;
		keyState.animation = submode.animation;
		keyState.high_priority = submode.hi_priority;
		keyState.low_priority = submode.lo_priority;
		keyState.flags = flag_set<flags::key_flag>();
	}
}

gorc::game::world::level::gameplay::standing_material gorc::game::world::level::gameplay::character_controller::get_standing_material(thing& thing) {
	if(thing.attach_flags & flags::attach_flag::AttachedToThingFace) {
		auto& floor_thing = presenter.model->things[thing.attached_thing];
		if(floor_thing.flags & flags::thing_flag::Metal) {
			return standing_material::metal;
		}
		else if(floor_thing.flags & flags::thing_flag::Dirt) {
			return standing_material::dirt;
		}
		else {
			return standing_material::hard;
		}
	}
	else if(thing.attach_flags & flags::attach_flag::AttachedToWorldSurface) {
		auto& floor_surf = presenter.model->surfaces[thing.attached_surface];

		if(floor_surf.flags & flags::surface_flag::Metal) {
			return standing_material::metal;
		}
		else if(floor_surf.flags & flags::surface_flag::Dirt) {
			return standing_material::dirt;
		}
		else if(floor_surf.flags & flags::surface_flag::ShallowWater) {
			return standing_material::shallow_water;
		}
		else if(floor_surf.flags & flags::surface_flag::DeepWater) {
			return standing_material::deep_water;
		}
		else if(floor_surf.flags & flags::surface_flag::VeryDeepWater) {
			return standing_material::very_deep_water;
		}
		else {
			return standing_material::hard;
		}
	}
	else {
		return standing_material::none;
	}
}

void gorc::game::world::level::gameplay::character_controller::update(int thing_id, double dt) {
	thing_controller::update(thing_id, dt);
}

void gorc::game::world::level::gameplay::character_controller::create_controller_data(int thing_id) {
	auto& new_thing = presenter.model->things[thing_id];

	// HACK: Initialize actor walk animation
	if(new_thing.puppet) {
		new_thing.actor_walk_animation = presenter.key_presenter.play_puppet_key(thing_id, flags::puppet_mode_type::Default, flags::puppet_submode_type::Stand);
		keys::key_state& keyState = presenter.model->key_model.keys[new_thing.actor_walk_animation];
		keyState.flags = flag_set<flags::key_flag>();
	}
	else {
		new_thing.actor_walk_animation = -1;
	}
}

void gorc::game::world::level::gameplay::character_controller::handle_animation_marker(int thing_id, flags::key_marker_type marker) {
	switch(marker) {
	case flags::key_marker_type::LeftRunFootstep:
		play_left_run_footstep(thing_id);
		break;

	case flags::key_marker_type::RightRunFootstep:
		play_right_run_footstep(thing_id);
		break;
	}
}

void gorc::game::world::level::gameplay::character_controller::play_left_run_footstep(int thing_id) {
	auto& thing = presenter.model->things[thing_id];
	standing_material mat = get_standing_material(thing);

	switch(mat) {
	case standing_material::metal:
		presenter.sound_presenter.play_sound_class(thing_id, flags::sound_subclass_type::LRunMetal);
		break;

	case standing_material::dirt:
		presenter.sound_presenter.play_sound_class(thing_id, flags::sound_subclass_type::LRunEarth);
		break;

	case standing_material::shallow_water:
		presenter.sound_presenter.play_sound_class(thing_id, flags::sound_subclass_type::LRunPuddle);
		break;

	case standing_material::deep_water:
		presenter.sound_presenter.play_sound_class(thing_id, flags::sound_subclass_type::LRunWater);
		break;

	case standing_material::very_deep_water:
		presenter.sound_presenter.play_sound_class(thing_id, flags::sound_subclass_type::LRunWater);
		break;

	default:
	case standing_material::hard:
		presenter.sound_presenter.play_sound_class(thing_id, flags::sound_subclass_type::LRunHard);
		break;
	}
}

void gorc::game::world::level::gameplay::character_controller::play_right_run_footstep(int thing_id) {
	auto& thing = presenter.model->things[thing_id];
	standing_material mat = get_standing_material(thing);

	switch(mat) {
	case standing_material::metal:
		presenter.sound_presenter.play_sound_class(thing_id, flags::sound_subclass_type::RRunMetal);
		break;

	case standing_material::dirt:
		presenter.sound_presenter.play_sound_class(thing_id, flags::sound_subclass_type::RRunEarth);
		break;

	case standing_material::shallow_water:
		presenter.sound_presenter.play_sound_class(thing_id, flags::sound_subclass_type::RRunPuddle);
		break;

	case standing_material::deep_water:
		presenter.sound_presenter.play_sound_class(thing_id, flags::sound_subclass_type::RRunWater);
		break;

	case standing_material::very_deep_water:
		presenter.sound_presenter.play_sound_class(thing_id, flags::sound_subclass_type::RRunWater);
		break;

	default:
	case standing_material::hard:
		presenter.sound_presenter.play_sound_class(thing_id, flags::sound_subclass_type::RRunHard);
		break;
	}
}
