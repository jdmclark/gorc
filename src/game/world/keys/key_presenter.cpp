#include "game/level_state.hpp"
#include "key_presenter.hpp"
#include "game/world/level_model.hpp"
#include "content/content_manager.hpp"
#include "game/world/level_presenter.hpp"
#include "game/world/events/animation_marker.hpp"
#include "game/world/components/puppet_animations.hpp"
#include "components/key_state.hpp"

gorc::game::world::keys::key_presenter::key_presenter(content_manager& contentmanager)
    : contentmanager(contentmanager), levelModel(nullptr) {
    return;
}

void gorc::game::world::keys::key_presenter::start(level_model& levelModel, event_bus& bus) {
    this->levelModel = &levelModel;
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

gorc::game::world::keys::key_mix& gorc::game::world::keys::key_presenter::get_thing_mix(
        thing_id tid) const
{
    for(auto &mix : levelModel->ecs.find_component<key_mix>(tid)) {
        return *mix.second;
    }

    return levelModel->ecs.emplace_component<key_mix>(tid);
}

void gorc::game::world::keys::key_presenter::update(const gorc::time& time) {
    double dt = time.elapsed_as_seconds();

    // Expunge expired keys.
    for(auto &key : levelModel->ecs.all_components<key_state>()) {
        if(key.second->expiration_time > 0.0f) {
            key.second->expiration_time -= static_cast<float>(dt);

            if(key.second->expiration_time <= 0.0f) {
                levelModel->ecs.erase_entity(key.first);
            }
        }
    }

    // Reset mix priorities and decrement prev-blend
    for(auto &mix : levelModel->ecs.all_components<key_mix>()) {
        mix.second->body.priority = mix.second->high.priority
                                  = mix.second->low.priority
                                  = std::numeric_limits<int>::lowest();

        mix.second->high.prev_frame_blend -= dt * 2.0;
        if(mix.second->high.prev_frame_blend <= 0.0) {
            mix.second->high.prev_animation = nothing;
        }

        mix.second->low.prev_frame_blend -= dt * 2.0;
        if(mix.second->low.prev_frame_blend <= 0.0) {
            mix.second->low.prev_animation = nothing;
        }

        mix.second->body.prev_frame_blend -= dt * 2.0;
        if(mix.second->body.prev_frame_blend <= 0.0) {
            mix.second->body.prev_animation = nothing;
        }
    }

    // update animation frames
    for(auto &key : levelModel->ecs.all_components<key_state>()) {
        for(auto &mix : levelModel->ecs.find_component<key_mix>(key.second->mix_id)) {
            // update anim time and compute frame number
            double prev_anim_time = key.second->animation_time;
            key.second->animation_time += dt * key.second->speed;

            if(key.second->animation.has_value()) {
                bool loops = false;
                const auto& anim = *key.second->animation.get_value();
                double prev_logical_frame = anim.framerate * prev_anim_time;
                double logical_frame = anim.framerate * key.second->animation_time;
                double frame = logical_frame;

                if(key.second->flags & flags::key_flag::PausesOnFirstFrame) {
                    if(frame > anim.frame_count) {
                        frame = 0.0;
                    }
                }
                else if(key.second->flags & flags::key_flag::PausesOnLastFrame) {
                    if(frame > anim.frame_count) {
                        frame = anim.frame_count;
                    }
                }
                else if(static_cast<int>(key.second->flags) == 0) {
                    loops = true;
                    frame = std::fmod(frame, anim.frame_count);
                }

                key.second->current_frame = frame;

                DispatchAllMarkers(mix.first, anim.markers, prev_logical_frame, logical_frame, loops, anim.frame_count);

                if((key.second->flags & flags::key_flag::EndSmoothly) && frame >= anim.frame_count) {
                    // End smoothly, continue into next animation.
                    levelModel->ecs.erase_entity(key.first);
                    continue;
                }
            }

            // Apply mix
            if(key.second->high_priority > mix.second->high.priority ||
               (key.second->high_priority == mix.second->high.priority &&
                key.second->creation_timestamp > mix.second->high.key_timestamp)) {
                if(mix.second->high.animation != key.second->animation) {
                    mix.second->high.prev_animation = mix.second->high.animation;
                    mix.second->high.prev_frame = mix.second->high.frame;
                    mix.second->high.prev_frame_blend = 0.5;
                }

                mix.second->high.animation = key.second->animation;
                mix.second->high.frame = key.second->current_frame;
                mix.second->high.priority = key.second->high_priority;
                mix.second->high.key_timestamp = key.second->creation_timestamp;
            }

            if(key.second->low_priority > mix.second->low.priority ||
               (key.second->low_priority == mix.second->low.priority &&
                key.second->creation_timestamp > mix.second->low.key_timestamp)) {
                if(mix.second->low.animation != key.second->animation) {
                    mix.second->low.prev_animation = mix.second->low.animation;
                    mix.second->low.prev_frame = mix.second->low.frame;
                    mix.second->low.prev_frame_blend = 0.5;
                }

                mix.second->low.animation = key.second->animation;
                mix.second->low.frame = key.second->current_frame;
                mix.second->low.priority = key.second->low_priority;
                mix.second->low.key_timestamp = key.second->creation_timestamp;
            }

            if(key.second->body_priority > mix.second->body.priority ||
               (key.second->body_priority == mix.second->body.priority &&
                key.second->creation_timestamp > mix.second->body.key_timestamp)) {
                if(mix.second->body.animation != key.second->animation) {
                    mix.second->body.prev_animation = mix.second->body.animation;
                    mix.second->body.prev_frame = mix.second->body.frame;
                    mix.second->body.prev_frame_blend = 0.5;
                }

                mix.second->body.animation = key.second->animation;
                mix.second->body.frame = key.second->current_frame;
                mix.second->body.priority = key.second->body_priority;
                mix.second->body.key_timestamp = key.second->creation_timestamp;
            }
        }
    }

    return;
}

void gorc::game::world::keys::key_presenter::expunge_thing_animations(thing_id tid) {
    levelModel->ecs.erase_component_if<key_state>([&](thing_id, key_state const &ks) {
            return ks.mix_id == tid;
        });
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

std::tuple<gorc::vector<3>, gorc::vector<3>> gorc::game::world::keys::key_presenter::get_node_frame(thing_id mix_id,
        int node_id, flag_set<flags::mesh_node_type> node_type) const {
    auto const &mix = get_thing_mix(mix_id);

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

gorc::thing_id gorc::game::world::keys::key_presenter::play_mix_key(thing_id mix_id, keyframe_id key,
        int priority, flag_set<flags::key_flag> flags) {
    thing_id new_key_id = levelModel->ecs.emplace_entity();

    auto &state = levelModel->ecs.emplace_component<key_state>(new_key_id);
    get_thing_mix(mix_id);

    state.animation = get_asset(contentmanager, key);
    state.high_priority = state.low_priority = state.body_priority = priority;
    state.animation_time = 0.0;
    state.current_frame = 0.0;
    state.mix_id = mix_id;
    state.flags = flags;
    state.speed = 1.0;

    return new_key_id;
}

gorc::thing_id gorc::game::world::keys::key_presenter::play_key(thing_id tid, keyframe_id key,
        int priority, flag_set<flags::key_flag> flags) {
    return play_mix_key(tid, key, priority, flags);
}

gorc::thing_id gorc::game::world::keys::key_presenter::play_mode(thing_id tid,
                                                      flags::puppet_submode_type minor_mode) {
    auto& thing = levelModel->get_thing(tid);
    if(!thing.pup.has_value()) {
        return invalid_id;
    }

    maybe<content::assets::puppet_submode const *> submode_ptr;
    for(auto const &tpup : levelModel->ecs.find_component<components::puppet_animations>(tid)) {
        submode_ptr = tpup.second->puppet->get_mode(tpup.second->puppet_mode_type).get_submode(minor_mode);
    }

    if(!submode_ptr.has_value()) {
        return invalid_id;
    }

    const auto& submode = *submode_ptr.get_value();
    get_thing_mix(tid);

    auto new_key_id = levelModel->ecs.emplace_entity();
    auto &state = levelModel->ecs.emplace_component<key_state>(new_key_id);

    state.animation = submode.anim;
    state.high_priority = submode.hi_priority;
    state.low_priority = submode.lo_priority;
    state.body_priority = std::max(submode.hi_priority, submode.lo_priority);
    state.animation_time = 0.0;
    state.current_frame = 0.0;
    state.mix_id = tid;
    state.flags = submode.flags;
    state.speed = 1.0;

    return new_key_id;
}

void gorc::game::world::keys::key_presenter::stop_key(thing_id, thing_id key_id, float delay) {
    if(delay <= 0.0f) {
        // Immediately remove.
        levelModel->ecs.erase_entity(key_id);
    }
    else {
        for(auto &key : levelModel->ecs.find_component<key_state>(key_id)) {
            key.second->expiration_time = delay;
        }
    }
}

void gorc::game::world::keys::key_presenter::stop_all_mix_keys(thing_id mix) {
    for(auto &key : levelModel->ecs.all_components<key_state>()) {
        if(key.second->mix_id == mix) {
            stop_key(invalid_id, key.first, 0.0f);
        }
    }
}

void gorc::game::world::keys::key_presenter::register_verbs(cog::verb_table &verbs, level_state &components) {
    verbs.add_safe_verb("getkeylen", 0.0f, [&components](keyframe_id key_id) {
        return components.current_level_presenter->key_presenter->get_key_len(key_id);
    });

    verbs.add_safe_verb("playkey", cog::value(), [&components](thing_id thing, keyframe_id key, int priority, int flags) {
        return components.current_level_presenter->key_presenter->play_key(thing, key, priority, flag_set<flags::key_flag>(flags));
    });

    verbs.add_safe_verb("playmode", cog::value(), [&components](thing_id tid, int submode) {
        return components.current_level_presenter->key_presenter->play_mode(tid, static_cast<flags::puppet_submode_type>(submode));
    });

    verbs.add_safe_verb("stopkey", cog::value(), [&components](thing_id thing, thing_id key, float delay) {
        components.current_level_presenter->key_presenter->stop_key(thing, key, delay);
    });
}
