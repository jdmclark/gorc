#include "sound_aspect.hpp"
#include "game/world/sounds/components/thing_sound.hpp"
#include "game/world/sounds/components/voice.hpp"
#include "game/world/sounds/components/foley.hpp"
#include "game/world/sounds/components/stop_when_destroyed.hpp"

gorc::game::world::sounds::aspects::sound_aspect::sound_aspect(entity_component_system<thing_id> &cs,
                                                               level_model &model)
    : inner_join_aspect(cs)
    , model(model) {

    destroyed_delegate =
        ecs.bus.add_handler<entity_destroyed<thing_id>>([&](auto const &e) {
        for(auto &sound : ecs.find_component<components::sound>(e.entity)) {
            sound.second->internal_sound.stop();
        }

        ecs.erase_component_if<components::thing_sound>([&](thing_id, auto const &cmp) {
                return cmp.sound == e.entity;
            });

        ecs.erase_component_if<components::voice>([&](thing_id, auto const &voc) {
                return voc.sound == e.entity;
            });

        ecs.erase_component_if<components::foley>([&](thing_id, auto const &fol) {
                return fol.sound == e.entity;
            });

        for(auto &fol : ecs.find_component<components::foley>(e.entity)) {
            for(auto &snd : ecs.find_component<components::sound>(fol.second->sound)) {
                snd.second->stop_delay = std::numeric_limits<float>::epsilon();
            }
        }

        for(auto &tsnd : ecs.find_component<components::stop_when_destroyed>(e.entity)) {
            for(auto &snd : cs.find_component<components::sound>(tsnd.second->sound)) {
                snd.second->stop_delay = std::numeric_limits<float>::epsilon();
            }
        }
    });

    return;
}

void gorc::game::world::sounds::aspects::sound_aspect::update(time_delta t,
                                                              thing_id id,
                                                              components::sound &s) {
    if(s.stop_delay > 0.0f) {
        s.stop_delay -= static_cast<float>(t.count());

        if(s.stop_delay <= 0.0f) {
            s.internal_sound.stop();
        }
    }

    if(s.has_played && s.internal_sound.getStatus() == sf::Sound::Stopped) {
        ecs.erase_entity(id);
        return;
    }

    s.volume.update(static_cast<float>(t.count()));
    s.pitch.update(static_cast<float>(t.count()));

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
