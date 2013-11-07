#include "cog/verbs/table.h"
#include "game/components.h"
#include "key_presenter.h"
#include "key_model.h"
#include "game/world/level/level_model.h"
#include "content/manager.h"
#include "game/world/level/gameplay/character_controller.h"

gorc::game::world::level::keys::key_presenter::key_presenter(content::manager& contentmanager)
	: contentmanager(contentmanager), levelModel(nullptr), model(nullptr) {
	return;
}

void gorc::game::world::level::keys::key_presenter::start(level_model& levelModel, key_model& model) {
	this->levelModel = &levelModel;
	this->model = &model;
}

void gorc::game::world::level::keys::key_presenter::DispatchAllMarkers(int thing_id, const std::vector<std::tuple<double, flags::key_marker_type>>& markers,
		double begin, double end, bool wraps, double frame_ct) {
	if(wraps) {
		begin = std::fmod(begin, frame_ct);
		end = std::fmod(end, frame_ct);
	}

	if(wraps && begin > end) {
		for(const auto& marker : markers) {
			double ft = std::get<0>(marker);
			if(ft >= begin || ft < end) {
				DispatchMarker(thing_id, std::get<1>(marker));
			}
		}
	}
	else {
		for(const auto& marker : markers) {
			double ft = std::get<0>(marker);
			if(ft >= begin && ft < end) {
				DispatchMarker(thing_id, std::get<1>(marker));
			}
		}
	}
}

void gorc::game::world::level::keys::key_presenter::DispatchMarker(int thing_id, flags::key_marker_type marker) {
	auto& thing = levelModel->things[thing_id];
	thing.controller->handle_animation_marker(thing_id, marker);
}

int gorc::game::world::level::keys::key_presenter::GetThingMixId(int thing_id) {
	auto& thing = levelModel->things[thing_id];
	if(thing.attached_key_mix < 0) {
		auto& mix = model->mixes.emplace();
		mix.attached_thing = thing_id;
		thing.attached_key_mix = mix.get_id();
	}

	return thing.attached_key_mix;
}

void gorc::game::world::level::keys::key_presenter::update(double dt) {
	// Reset mix priorities.
	for(auto& mix : model->mixes) {
		mix.body.priority = mix.high.priority = mix.low.priority = std::numeric_limits<int>::lowest();
	}

	// update animation frames
	for(auto& key : model->keys) {
		auto& mix = model->mixes[key.mix_id];

		// update anim time and compute frame number
		double prev_anim_time = key.animation_time;
		key.animation_time += dt * key.speed;

		if(key.animation) {
			bool loops = false;
			const auto& anim = *key.animation;
			double prev_logical_frame = anim.framerate * prev_anim_time;
			double logical_frame = anim.framerate * key.animation_time;
			double frame = logical_frame;

			if(key.flags & flags::key_flag::PausesOnFirstFrame) {
				if(frame > anim.frame_count) {
					frame = 0.0;
				}
			}
			else if(key.flags & flags::key_flag::PausesOnLastFrame) {
				if(frame > anim.frame_count) {
					frame = anim.frame_count;
				}
			}
			else if(static_cast<int>(key.flags) == 0) {
				loops = true;
				frame = std::fmod(frame, anim.frame_count);
			}

			key.current_frame = frame;

			DispatchAllMarkers(mix.attached_thing, key.animation->markers, prev_logical_frame, logical_frame, loops, key.animation->frame_count);

			if((key.flags & flags::key_flag::EndSmoothly) && frame >= anim.frame_count) {
				// End smoothly, continue into next animation.
				model->keys.erase(key);
				continue;
			}
		}

		// Apply mix
		if(key.high_priority >= mix.high.priority) {
			mix.high.animation = key.animation;
			mix.high.frame = key.current_frame;
			mix.high.priority = key.high_priority;
		}

		if(key.low_priority >= mix.low.priority) {
			mix.low.animation = key.animation;
			mix.low.frame = key.current_frame;
			mix.low.priority = key.low_priority;
		}

		if(key.body_priority >= mix.body.priority) {
			mix.body.animation = key.animation;
			mix.body.frame = key.current_frame;
			mix.body.priority = key.body_priority;
		}
	}
}

std::tuple<gorc::vector<3>, gorc::vector<3>> gorc::game::world::level::keys::key_presenter::get_node_frame(int mix_id,
		int node_id, flag_set<flags::mesh_node_type> node_type) const {
	const key_mix& mix = model->mixes[mix_id];

	// get animation corresponding to node type
	const key_mix_level_state* mix_level;
	if(node_type & flags::mesh_node_type::UpperBody) {
		mix_level = &mix.high;
	}
	else if(node_type & flags::mesh_node_type::LowerBody) {
		mix_level = &mix.low;
	}
	else {
		mix_level = &mix.body;
	}

	if(!mix_level->animation || mix_level->animation->nodes.size() <= node_id ||
			mix_level->animation->nodes[node_id].frames.empty()) {
		// Abort if there are no frames to interpolate.
		return std::make_tuple(make_zero_vector<3, float>(), make_zero_vector<3, float>());
	}

	const auto& anim_node = mix_level->animation->nodes[node_id];

	int actual_frame = static_cast<int>(std::floor(mix_level->frame));

	// Convert anim_time into a frame number
	auto comp_fn = [](int tgt_fr, const content::assets::animation_frame& fr) {
		return fr.frame > tgt_fr;
	};

	// Find frame immediately after desired frame, then back off.
	auto it = std::upper_bound(anim_node.frames.begin(), anim_node.frames.end(), actual_frame, comp_fn);
	if(it == anim_node.frames.begin()) {
		it = anim_node.frames.end() - 1;
	}
	else {
		--it;
	}

	float remaining_frame_time = static_cast<float>(mix_level->frame) - static_cast<float>(it->frame);

	auto position = it->position + remaining_frame_time * it->delta_position;
	auto orientation = it->orientation + remaining_frame_time * it->delta_orientation;

	return std::make_tuple(position, orientation);
}

int gorc::game::world::level::keys::key_presenter::play_key(int thing_id, int key,
		int priority, flag_set<flags::key_flag> flags) {
	int mix_id = GetThingMixId(thing_id);

	auto& state = model->keys.emplace();

	state.animation = &contentmanager.get_asset<content::assets::animation>(key);
	state.high_priority = state.low_priority = state.high_priority = priority;
	state.animation_time = 0.0;
	state.current_frame = 0.0;
	state.mix_id = mix_id;
	state.flags = flags;
	state.speed = 1.0;

	return state.get_id();
}

int gorc::game::world::level::keys::key_presenter::play_puppet_key(int thing_id,
		flags::puppet_mode_type major_mode, flags::puppet_submode_type minor_mode) {
	int mix_id = GetThingMixId(thing_id);
	auto& thing = levelModel->things[thing_id];

	auto& state = model->keys.emplace();

	const content::assets::puppet_submode& submode = thing.puppet->get_mode(major_mode).get_submode(minor_mode);

	state.animation = submode.animation;
	state.high_priority = submode.hi_priority;
	state.low_priority = submode.lo_priority;
	state.body_priority = std::max(submode.hi_priority, submode.lo_priority);
	state.animation_time = 0.0;
	state.current_frame = 0.0;
	state.mix_id = mix_id;
	state.flags = submode.flags;
	state.speed = 1.0;

	return state.get_id();
}

void gorc::game::world::level::keys::key_presenter::register_verbs(cog::verbs::verb_table& verbTable, components& components) {
	verbTable.add_verb<int, 4>("playkey", [&components](int thing, int key, int priority, int flags) {
		return static_cast<int>(components.current_level_presenter->key_presenter.play_key(thing, key, priority, flag_set<flags::key_flag>(flags)));
	});
}
