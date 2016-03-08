#include "sound_presenter.hpp"
#include "game/world/level_model.hpp"
#include "game/level_state.hpp"
#include "libold/base/content/content_manager.hpp"
#include "game/world/level_presenter.hpp"
#include "sound_model.hpp"
#include "game/world/sounds/components/sound.hpp"
#include "game/world/sounds/components/foley.hpp"
#include "game/world/sounds/components/thing_sound.hpp"
#include "game/world/sounds/components/voice.hpp"
#include "game/world/sounds/components/stop_when_destroyed.hpp"
#include "game/constants.hpp"

#include "game/world/sounds/aspects/thing_sound_aspect.hpp"
#include "game/world/sounds/aspects/sound_aspect.hpp"

using gorc::game::world::sounds::sound_presenter;

constexpr gorc::entity_id invalid_sound_id(-1);

sound_presenter::sound_presenter(content::content_manager& contentmanager)
    : contentmanager(contentmanager)
    , levelModel(nullptr)
    , model(nullptr) {
    return;
}

void sound_presenter::start(level_model& levelModel, sound_model& soundModel) {
    this->levelModel = &levelModel;
    this->model = &soundModel;

    levelModel.ecs.emplace_aspect<aspects::thing_sound_aspect>();
    levelModel.ecs.emplace_aspect<aspects::sound_aspect>(levelModel);
}

void sound_presenter::update(const gorc::time& time) {
    // update listener
    const auto& cam = levelModel->camera_model.current_computed_state;
    auto camera_position = cam.position;
    auto listener_target = cam.look;
    sf::Listener::setDirection(get<0>(listener_target), get<2>(listener_target), (1 - get<1>(listener_target)) / 2);
    sf::Listener::setPosition(get<0>(camera_position), get<2>(camera_position), -get<1>(camera_position));
    // TODO: Handle camera orientation (not properly implemented in SFML).
    const auto& player_sec = levelModel->sectors[cam.containing_sector];
    set_ambient_sound(player_sec.ambient_sound, player_sec.ambient_sound_volume);

    model->ambient_music.update(time.elapsed_as_seconds());
}

void sound_presenter::set_ambient_sound(content::assets::sound const* sound, float volume) {
    if(&sound->buffer == model->ambient_sound.getBuffer() &&
       model->ambient_sound.getStatus() != sf::Sound::Stopped) {
        model->ambient_sound.setVolume(volume * 100.0f);
    }
    else if(sound != nullptr) {
        model->ambient_sound.setBuffer(sound->buffer);
        model->ambient_sound.setLoop(true);
        model->ambient_sound.setVolume(volume * 100.0f);
        model->ambient_sound.setPosition(0,0,0);
        model->ambient_sound.setRelativeToListener(true);
        model->ambient_sound.setAttenuation(0.0f);
        model->ambient_sound.play();
    }
    else {
        model->ambient_sound.stop();
    }
}

// verbs:

void sound_presenter::change_sound_pitch(entity_id channel, float pitch, float delay) {
    for(auto &sound : levelModel->ecs.find_component<components::sound>(channel)) {
        sound.second.pitch.set(pitch, delay);
    }
}

void sound_presenter::change_sound_vol(entity_id channel, float volume, float delay) {
    for(auto &sound : levelModel->ecs.find_component<components::sound>(channel)) {
        sound.second.volume.set(volume, delay);
    }
}

void sound_presenter::play_song(int start, int end, int loopto) {
    model->ambient_music.play_song(start, end, loopto);
}

gorc::entity_id sound_presenter::play_sound_class(entity_id thing_id,
                                                  flags::sound_subclass_type subclass_type) {
    auto& referenced_thing = levelModel->get_thing(thing_id);
    if(referenced_thing.sound_class) {
        auto const &subclass = referenced_thing.sound_class->get(subclass_type);
        if(subclass.sound >= 0) {
            return play_sound_thing(subclass.sound,
                                    thing_id,
                                    subclass.max_volume,
                                    subclass.min_radius,
                                    subclass.max_radius,
                                    subclass.flags + flags::sound_flag::ThingOriginMovesWithThing);
        }
    }

    return invalid_sound_id;
}

void sound_presenter::play_foley_loop_class(entity_id thing_id,
                                            flags::sound_subclass_type subclass_type) {
    stop_foley_loop(thing_id);

    auto channel = play_sound_class(thing_id, subclass_type);
    levelModel->ecs.emplace_component<components::foley>(thing_id, channel);
}

void sound_presenter::stop_foley_loop(entity_id thing_id) {
    for(auto const &foley : levelModel->ecs.find_component<components::foley>(thing_id)) {
        stop_sound(foley.second.sound, 0.0f);
    }
}

gorc::entity_id sound_presenter::play_sound_local(int wav,
                                                  float volume,
                                                  float panning,
                                                  flag_set<flags::sound_flag> flags) {
    if(wav < 0) {
        return invalid_sound_id;
    }

    auto const &buffer = contentmanager.get_asset<content::assets::sound>(wav);

    entity_id snd_id = levelModel->ecs.make_entity();
    components::sound &snd = levelModel->ecs.emplace_component<components::sound>(snd_id);

    snd.internal_sound.setBuffer(buffer.buffer);
    snd.internal_sound.setPosition(panning, 0.0f, 0.0f);
    snd.internal_sound.setRelativeToListener(true);
    snd.internal_sound.setVolume(volume * 100.0f);
    snd.internal_sound.setLoop(flags & flags::sound_flag::Loops);
    snd.internal_sound.setPitch(1.0f);
    snd.internal_sound.setAttenuation(0.0f);
    snd.internal_sound.setMinDistance(1.0f);

    snd.do_distance_attenuation = false;

    snd.volume.set(volume, 0.0f);
    snd.pitch.set(1.0f, 0.0f);

    return snd_id;
}

gorc::entity_id sound_presenter::play_sound_pos(int wav,
                                                vector<3> pos,
                                                float volume,
                                                float minrad,
                                                float maxrad,
                                                flag_set<flags::sound_flag> flags) {
    if(wav < 0) {
        return invalid_sound_id;
    }

    auto const &buffer = contentmanager.get_asset<content::assets::sound>(wav);

    entity_id snd_id = levelModel->ecs.make_entity();
    components::sound &snd = levelModel->ecs.emplace_component<components::sound>(snd_id);

    snd.minimum_attenuation_radius = std::min(minrad, maxrad);
    snd.maximum_attenuation_radius = std::max(minrad, maxrad);

    if(snd.minimum_attenuation_radius < 0.0f) {
        snd.minimum_attenuation_radius = 5.0f / sound_attenuation_factor;
    }

    if(snd.maximum_attenuation_radius < 0.0f || (flags & flags::sound_flag::VolumeQuickFalloff)) {
        snd.maximum_attenuation_radius = 25.0f / sound_attenuation_factor;
    }

    snd.internal_sound.setBuffer(buffer.buffer);
    snd.internal_sound.setPosition(get<0>(pos), get<2>(pos), -get<1>(pos));
    snd.internal_sound.setRelativeToListener(false);
    snd.internal_sound.setVolume(volume * 100.0f);
    snd.internal_sound.setLoop(flags & flags::sound_flag::Loops);
    snd.internal_sound.setMinDistance(snd.minimum_attenuation_radius);
    snd.internal_sound.setAttenuation(0.0f);
    snd.internal_sound.setPitch(1.0f);

    snd.do_distance_attenuation = true;

    snd.volume.set(volume, 0.0f);
    snd.pitch.set(1.0f, 0.0f);

    snd.position = pos;

    return snd_id;
}

gorc::entity_id sound_presenter::play_sound_thing(int wav,
                                                  entity_id thing_id,
                                                  float volume,
                                                  float minrad,
                                                  float maxrad,
                                                  flag_set<flags::sound_flag> flags) {
    if(wav < 0) {
        return invalid_sound_id;
    }

    auto const &soundfile = contentmanager.get_asset<content::assets::sound>(wav);

    if(flags & flags::sound_flag::IgnoreIfSoundclassAlreadyPlaying) {
        // Thing can only play this sound once.
        for(auto &tsnd : levelModel->ecs.find_component<components::thing_sound>(thing_id)) {
            for(auto &snd : levelModel->ecs.find_component<components::sound>(tsnd.second.sound)) {
                if(snd.second.internal_sound.getBuffer() == &soundfile.buffer) {
                    return invalid_sound_id;
                }
            }
        }
    }

    if(flags & flags::sound_flag::Voice) {
        // Each thing can only play one voice at a time.
        for(auto &voc : levelModel->ecs.find_component<components::voice>(thing_id)) {
            stop_sound(voc.second.sound, 0.0f);
        }
    }

    auto &thing = levelModel->get_thing(thing_id);

    entity_id snd_id = play_sound_pos(wav, thing.position, volume, minrad, maxrad, flags);
    if(snd_id == invalid_sound_id) {
        return invalid_sound_id;
    }

    if(flags & flags::sound_flag::ThingOriginMovesWithThing) {
        levelModel->ecs.emplace_component<components::thing_sound>(thing_id, snd_id);
    }

    if(flags & flags::sound_flag::StopsWhenThingDestroyed) {
        levelModel->ecs.emplace_component<components::stop_when_destroyed>(thing_id, snd_id);
    }

    if(flags & flags::sound_flag::Voice) {
        levelModel->ecs.emplace_component<components::voice>(thing_id, snd_id);
    }

    return snd_id;
}

void gorc::game::world::sounds::sound_presenter::set_music_vol(float volume) {
    model->ambient_music.set_volume(volume);
}

void gorc::game::world::sounds::sound_presenter::stop_sound(entity_id channel, float delay) {
    if(delay <= 0.0f) {
        delay = std::numeric_limits<float>::epsilon();
    }

    for(auto &snd : levelModel->ecs.find_component<components::sound>(channel)) {
        snd.second.stop_delay = delay;
    }
}

void gorc::game::world::sounds::sound_presenter::register_verbs(cog::verbs::verb_table& verbTable, level_state& components) {
    verbTable.add_verb<void, 3>("changesoundpitch", [&components](entity_id channel, float pitch, float delay) {
        components.current_level_presenter->sound_presenter->change_sound_pitch(channel, pitch, delay);
    });

    verbTable.add_verb<void, 3>("changesoundvol", [&components](entity_id channel, float volume, float delay) {
        components.current_level_presenter->sound_presenter->change_sound_vol(channel, volume, delay);
    });

    verbTable.add_verb<void, 3>("playsong", [&components](int start, int end, int loopto) {
        components.current_level_presenter->sound_presenter->play_song(start, end, loopto);
    });

    verbTable.add_verb<entity_id, 4>("playsoundlocal", [&components](int wav, float volume, float panning, int flags) {
        return components.current_level_presenter->sound_presenter->play_sound_local(wav, volume, panning, flag_set<flags::sound_flag>(flags));
    });

    verbTable.add_verb<entity_id, 6>("playsoundpos", [&components](int wav, vector<3> pos, float volume, float min_rad, float max_rad, int flags) {
        return components.current_level_presenter->sound_presenter->play_sound_pos(wav, pos,
            volume, min_rad / sound_attenuation_factor, max_rad / sound_attenuation_factor,
            flag_set<flags::sound_flag>(flags));
    });

    verbTable.add_verb<entity_id, 6>("playsoundthing", [&components](int wav, entity_id thing, float volume, float min_rad, float max_rad, int flags) {
        return components.current_level_presenter->sound_presenter->play_sound_thing(wav, thing,
            volume, min_rad / sound_attenuation_factor, max_rad / sound_attenuation_factor,
            flag_set<flags::sound_flag>(flags));
    });

    verbTable.add_verb<void, 1>("setmusicvol", [&components](float vol) {
        components.current_level_presenter->sound_presenter->set_music_vol(vol);
    });

    verbTable.add_verb<void, 2>("stopsound", [&components](entity_id channel, float delay) {
        components.current_level_presenter->sound_presenter->stop_sound(channel, delay);
    });
}
