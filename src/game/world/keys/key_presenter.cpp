#include "game/level_state.hpp"
#include "key_presenter.hpp"
#include "key_model.hpp"
#include "game/world/level_model.hpp"
#include "content/content_manager.hpp"
#include "game/world/level_presenter.hpp"
#include "game/world/events/animation_marker.hpp"
#include "game/world/components/puppet_animations.hpp"

gorc::game::world::keys::key_presenter::key_presenter(content_manager& contentmanager)
    : contentmanager(contentmanager), levelModel(nullptr), model(nullptr) {
    return;
}

void gorc::game::world::keys::key_presenter::start(level_model& levelModel, key_model& model, event_bus& bus) {
    this->levelModel = &levelModel;
    this->model = &model;
    this->bus = &bus;
}

void gorc::game::world::keys::key_presenter::DispatchAllMarkers(thing_id thing, const std::vector<std::tuple<double, flags::key_marker_type>>& markers,
        double begin, double end, bool wraps, double frame_ct) {
    if(wraps) {
        begin = std::fmod(begin, frame_ct);
        end = std::fmod(end, frame_ct);
    }

    if(wraps && begin > end) {
        for(const auto& marker : markers) {
            double ft = std::get<0>(marker);
            if(ft >= begin || ft < end) {
                DispatchMarker(thing, std::get<1>(marker));
            }
        }
    }
    else {
        for(const auto& marker : markers) {
            double ft = std::get<0>(marker);
            if(ft >= begin && ft < end) {
                DispatchMarker(thing, std::get<1>(marker));
            }
        }
    }
}

void gorc::game::world::keys::key_presenter::DispatchMarker(thing_id thing, flags::key_marker_type marker) {
    bus->fire_event(events::animation_marker(thing, marker));
}

int gorc::game::world::keys::key_presenter::GetThingMixId(thing_id tid) {
    auto& thing = levelModel->get_thing(tid);
    if(thing.attached_key_mix < 0) {
        auto& mix = model->mixes.emplace();
        mix.attached_thing = tid;
        thing.attached_key_mix = mix.get_id();
    }

    return thing.attached_key_mix;
}

void gorc::game::world::keys::key_presenter::update(const gorc::time& time) {
    double dt = time.elapsed_as_seconds();

    // Expunge expired keys.
    for(auto& key : model->keys) {
        if(key.expiration_time > 0.0f) {
            key.expiration_time -= static_cast<float>(dt);

            if(key.expiration_time <= 0.0f) {
                model->keys.erase(key);
            }
        }
    }

    // Reset mix priorities and decrement prev-blend
    for(auto& mix : model->mixes) {
        mix.body.priority = mix.high.priority = mix.low.priority = std::numeric_limits<int>::lowest();

        mix.high.prev_frame_blend -= dt * 2.0;
        if(mix.high.prev_frame_blend <= 0.0) {
            mix.high.prev_animation = nothing;
        }

        mix.low.prev_frame_blend -= dt * 2.0;
        if(mix.low.prev_frame_blend <= 0.0) {
            mix.low.prev_animation = nothing;
        }

        mix.body.prev_frame_blend -= dt * 2.0;
        if(mix.body.prev_frame_blend <= 0.0) {
            mix.body.prev_animation = nothing;
        }
    }

    // update animation frames
    for(auto& key : model->keys) {
        auto& mix = model->mixes[key.mix_id];

        // update anim time and compute frame number
        double prev_anim_time = key.animation_time;
        key.animation_time += dt * key.speed;

        if(key.animation.has_value()) {
            bool loops = false;
            const auto& anim = *key.animation.get_value();
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

            DispatchAllMarkers(mix.attached_thing, anim.markers, prev_logical_frame, logical_frame, loops, anim.frame_count);

            if((key.flags & flags::key_flag::EndSmoothly) && frame >= anim.frame_count) {
                // End smoothly, continue into next animation.
                model->keys.erase(key);
                continue;
            }
        }

        // Apply mix
        if(key.high_priority > mix.high.priority || (key.high_priority == mix.high.priority && key.creation_timestamp > mix.high.key_timestamp)) {
            if(mix.high.animation != key.animation) {
                mix.high.prev_animation = mix.high.animation;
                mix.high.prev_frame = mix.high.frame;
                mix.high.prev_frame_blend = 0.5;
            }

            mix.high.animation = key.animation;
            mix.high.frame = key.current_frame;
            mix.high.priority = key.high_priority;
            mix.high.key_timestamp = key.creation_timestamp;
        }

        if(key.low_priority > mix.low.priority || (key.low_priority == mix.low.priority && key.creation_timestamp > mix.low.key_timestamp)) {
            if(mix.low.animation != key.animation) {
                mix.low.prev_animation = mix.low.animation;
                mix.low.prev_frame = mix.low.frame;
                mix.low.prev_frame_blend = 0.5;
            }

            mix.low.animation = key.animation;
            mix.low.frame = key.current_frame;
            mix.low.priority = key.low_priority;
            mix.low.key_timestamp = key.creation_timestamp;
        }

        if(key.body_priority > mix.body.priority || (key.body_priority == mix.body.priority && key.creation_timestamp > mix.body.key_timestamp)) {
            if(mix.body.animation != key.animation) {
                mix.body.prev_animation = mix.body.animation;
                mix.body.prev_frame = mix.body.frame;
                mix.body.prev_frame_blend = 0.5;
            }

            mix.body.animation = key.animation;
            mix.body.frame = key.current_frame;
            mix.body.priority = key.body_priority;
            mix.body.key_timestamp = key.creation_timestamp;
        }
    }
}

void gorc::game::world::keys::key_presenter::expunge_thing_animations(thing_id tid) {
    auto& thing = levelModel->get_thing(tid);
    if(thing.attached_key_mix >= 0) {
        for(auto& key : model->keys) {
            if(key.mix_id == thing.attached_key_mix) {
                stop_key(tid, key.get_id(), 0.0f);
            }
        }

        // Remove key mix
        model->mixes.erase(thing.attached_key_mix);
    }
}

std::tuple<gorc::vector<3>, gorc::vector<3>> gorc::game::world::keys::key_presenter::get_animation_frame(asset_ref<content::assets::animation> anim,
        int node_id, double frame) const {
    if(anim->nodes.size() <= static_cast<size_t>(node_id) || anim->nodes[node_id].frames.empty()) {
        // Abort if there are no frames to interpolate.
        return std::make_tuple(make_zero_vector<3, float>(), make_zero_vector<3, float>());
    }

    const auto& anim_node = anim->nodes[node_id];

    int actual_frame = static_cast<int>(std::floor(frame));

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

    float remaining_frame_time = static_cast<float>(frame) - static_cast<float>(it->frame);

    auto position = it->position + remaining_frame_time * it->delta_position;
    auto orientation = it->orientation + remaining_frame_time * it->delta_orientation;

    return std::make_tuple(position, orientation);
}

std::tuple<gorc::vector<3>, gorc::vector<3>> gorc::game::world::keys::key_presenter::get_node_frame(int mix_id,
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

    if(!mix_level->animation.has_value()) {
        // Abort if there are no frames to interpolate.
        return std::make_tuple(make_zero_vector<3, float>(), make_zero_vector<3, float>());
    }

    auto frame = get_animation_frame(mix_level->animation.get_value(), node_id, mix_level->frame);

    // Mix in prev frame.
    if(mix_level->prev_animation.has_value()) {
        auto alpha = static_cast<float>(mix_level->prev_frame_blend);
        auto mix_frame = get_animation_frame(mix_level->prev_animation.get_value(), node_id, mix_level->prev_frame);
        auto fr_orient = get<1>(frame);
        auto mx_orient = get<1>(mix_frame);
        auto combined_orient = make_vector(clerp(get<0>(fr_orient), get<0>(mx_orient), alpha),
                clerp(get<1>(fr_orient), get<1>(mx_orient), alpha),
                clerp(get<2>(fr_orient), get<2>(mx_orient), alpha));
        return std::make_tuple(lerp(get<0>(frame), get<0>(mix_frame), alpha), combined_orient);
    }
    else {
        return frame;
    }
}

float gorc::game::world::keys::key_presenter::get_key_len(keyframe_id key_id) {
    auto key = get_asset(contentmanager, key_id);
    return static_cast<float>(key->frame_count) / static_cast<float>(key->framerate);
}

int gorc::game::world::keys::key_presenter::play_mix_key(int mix_id, keyframe_id key,
        int priority, flag_set<flags::key_flag> flags) {
    auto& state = model->keys.emplace();

    state.animation = get_asset(contentmanager, key);
    state.high_priority = state.low_priority = state.body_priority = priority;
    state.animation_time = 0.0;
    state.current_frame = 0.0;
    state.mix_id = mix_id;
    state.flags = flags;
    state.speed = 1.0;
    state.creation_timestamp = model->key_creation_counter++;

    return state.get_id();
}

int gorc::game::world::keys::key_presenter::play_key(thing_id tid, keyframe_id key,
        int priority, flag_set<flags::key_flag> flags) {
    return play_mix_key(GetThingMixId(tid), key, priority, flags);
}

int gorc::game::world::keys::key_presenter::play_mode(thing_id tid,
                                                      flags::puppet_submode_type minor_mode) {
    auto& thing = levelModel->get_thing(tid);
    if(!thing.pup.has_value()) {
        return -1;
    }

    maybe<content::assets::puppet_submode const *> submode_ptr;
    for(auto const &tpup : levelModel->ecs.find_component<components::puppet_animations>(tid)) {
        submode_ptr = tpup.second.puppet->get_mode(tpup.second.puppet_mode_type).get_submode(minor_mode);
    }

    if(!submode_ptr.has_value()) {
        return -1;
    }

    const auto& submode = *submode_ptr.get_value();
    int mix_id = GetThingMixId(tid);
    auto& state = model->keys.emplace();

    state.animation = submode.anim;
    state.high_priority = submode.hi_priority;
    state.low_priority = submode.lo_priority;
    state.body_priority = std::max(submode.hi_priority, submode.lo_priority);
    state.animation_time = 0.0;
    state.current_frame = 0.0;
    state.mix_id = mix_id;
    state.flags = submode.flags;
    state.speed = 1.0;
    state.creation_timestamp = model->key_creation_counter++;

    return state.get_id();
}

void gorc::game::world::keys::key_presenter::stop_key(thing_id, int key_id, float delay) {
    if(delay <= 0.0f) {
        // Immediately remove.
        model->keys.erase(key_id);
    }
    else {
        auto& key = model->keys[key_id];
        key.expiration_time = delay;
    }
}

void gorc::game::world::keys::key_presenter::stop_all_mix_keys(int mix) {
    for(auto& key : model->keys) {
        if(key.mix_id == mix) {
            stop_key(invalid_id, key.get_id(), 0.0f);
        }
    }
}

int gorc::game::world::keys::key_presenter::create_key_mix() {
    auto mix = model->mixes.emplace();
    return mix.get_id();
}

void gorc::game::world::keys::key_presenter::register_verbs(cog::verb_table &verbs, level_state &components) {
    verbs.add_safe_verb("getkeylen", 0.0f, [&components](keyframe_id key_id) {
        return components.current_level_presenter->key_presenter->get_key_len(key_id);
    });

    verbs.add_safe_verb("playkey", cog::value(), [&components](thing_id thing, keyframe_id key, int priority, int flags) {
        return components.current_level_presenter->key_presenter->play_key(thing, key, priority, flag_set<flags::key_flag>(flags));
    });

    verbs.add_verb("playmode", [&components](thing_id tid, int submode) {
        return components.current_level_presenter->key_presenter->play_mode(tid, static_cast<flags::puppet_submode_type>(submode));
    });

    verbs.add_verb("stopkey", [&components](thing_id thing, int key, float delay) {
        components.current_level_presenter->key_presenter->stop_key(thing, key, delay);
    });
}
