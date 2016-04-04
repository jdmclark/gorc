#include "sound_aspect.hpp"
#include "game/world/sounds/components/thing_sound.hpp"
#include "game/world/sounds/components/voice.hpp"
#include "game/world/sounds/components/foley.hpp"
#include "game/world/sounds/components/stop_when_destroyed.hpp"

gorc::game::world::sounds::aspects::sound_aspect::sound_aspect(component_system &cs,
                                                               level_model &model)
    : inner_join_aspect(cs)
    , model(model) {

    destroyed_delegate =
        cs.bus.add_handler<events::destroyed>([&](events::destroyed const &e) {
        for(auto &sound : cs.find_component<components::sound>(e.destroyed_entity)) {
            sound.second.internal_sound.stop();
        }

        cs.erase_components(cs.all_components<components::thing_sound>(),
            [&](components::thing_sound const &cmp) {
                return cmp.sound == e.destroyed_entity;
            });

        cs.erase_components(cs.all_components<components::voice>(),
            [&](components::voice const &voc) {
                return voc.sound == e.destroyed_entity;
            });

        cs.erase_components(cs.all_components<components::foley>(),
            [&](components::foley const &fol) {
                return fol.sound == e.destroyed_entity;
            });

        for(auto &fol : cs.find_component<components::foley>(e.destroyed_entity)) {
            for(auto &snd : cs.find_component<components::sound>(entity_id(fol.second.sound))) {
                snd.second.stop_delay = std::numeric_limits<float>::epsilon();
            }
        }

        for(auto &tsnd : cs.find_component<components::stop_when_destroyed>(e.destroyed_entity)) {
            for(auto &snd : cs.find_component<components::sound>(entity_id(tsnd.second.sound))) {
                snd.second.stop_delay = std::numeric_limits<float>::epsilon();
            }
        }
    });

    return;
}

void gorc::game::world::sounds::aspects::sound_aspect::update(gorc::time t,
                                                              entity_id id,
                                                              components::sound &s) {
    if(s.stop_delay > 0.0f) {
        s.stop_delay -= static_cast<float>(t.elapsed_as_seconds());

        if(s.stop_delay <= 0.0f) {
            s.internal_sound.stop();
        }
    }

    if(s.has_played && s.internal_sound.getStatus() == sf::Sound::Stopped) {
        cs.erase_entity(id);
        return;
    }

    s.volume.update(static_cast<float>(t.elapsed_as_seconds()));
    s.pitch.update(static_cast<float>(t.elapsed_as_seconds()));

    if(s.do_distance_attenuation) {
        auto sound_dist = length(model.camera_model.current_computed_state.position - s.position);

        float attenuation = 1.0f;
        if(s.minimum_attenuation_radius < s.maximum_attenuation_radius) {
            attenuation = ((sound_dist - s.maximum_attenuation_radius) /
                           (s.minimum_attenuation_radius - s.maximum_attenuation_radius));
        }

        attenuation = clamp(attenuation, 0.0f, 1.0f);

        s.internal_sound.setVolume(attenuation * s.volume * 100.0f);
        s.internal_sound.setPitch(s.pitch);
    }
    else {
        s.internal_sound.setVolume(s.volume * 100.0f);
        s.internal_sound.setPitch(s.pitch);
    }

    if(!s.has_played) {
        s.has_played = true;
        s.internal_sound.play();
    }
}
